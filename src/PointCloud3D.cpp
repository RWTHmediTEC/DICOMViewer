/*
 *  Copyright (C) 2010 - 2013 Leonid Kostrykin
 *
 *  Chair of Medical Engineering (mediTEC)
 *  RWTH Aachen University
 *  Pauwelsstr. 20
 *  52074 Aachen
 *  Germany
 *
 */

#include "glew.h"
#include "PointCloud3D.h"
#include "PointCloud.h"
#include "CarnaContextClient.h"
#include "PointCloudsClient.h"
#include "NotificationsClient.h"
#include <Carna/base/view/ShaderBundle.h>
#include <Carna/base/view/ShaderProgram.h>
#include <Carna/base/model/Object3DEvent.h>
#include <Carna/base/view/Renderer.h>
#include <Carna/base/Transformation.h>
#include <climits>
#include <algorithm>
#include <QApplication>
#include <QProgressDialog>

#ifndef NO_CRA
#include "RegistrationClient.h"
#endif



// ----------------------------------------------------------------------------------
// Types & Globals
// ----------------------------------------------------------------------------------

const static std::string shaderId = "point-cloud";



// ----------------------------------------------------------------------------------
// PointCloud3D
// ----------------------------------------------------------------------------------

PointCloud3D::PointCloud3D( Record::Server& server, const PointCloud& cloud, QWidget* modalityParent )
    : Carna::base::model::Object3D( CarnaContextClient( server ).model(), cloud.getName() )
    , server( server )
    , points()
    , ready( false )
    , minX( +std::numeric_limits< double >::max() )
    , minY( +std::numeric_limits< double >::max() )
    , minZ( +std::numeric_limits< double >::max() )
    , maxX( -std::numeric_limits< double >::max() )
    , maxY( -std::numeric_limits< double >::max() )
    , maxZ( -std::numeric_limits< double >::max() )
    , pointSize( cloud.getList().size() > 20 ? 2.5 : 6.5 )
    , pointColor( 255, 255, 255 )
    , modulateBrightnessByDistance( cloud.getList().size() > 20 )
    , drawBoundingBox( true )
#ifndef NO_CRA
    , applyRegistration( cloud.source == PointCloud::trackingSide )
#endif
{
    QApplication::setOverrideCursor( Qt::WaitCursor );

    QProgressDialog progress( "Processing points...", "", 0, cloud.getList().size() - 1, modalityParent );
    progress.setWindowModality( Qt::WindowModal );
    progress.setCancelButton( nullptr );

    points.resize( cloud.getList().size() );

    for( PointCloud::PointList::const_iterator it  = cloud.getList().begin();
                                               it != cloud.getList().end();
                                             ++it )
    {
        const Carna::base::Vector& p = *it;

        std::unique_ptr< Carna::base::model::Position > pos;

        switch( cloud.getFormat() )
        {

        case PointCloud::millimeters:
            pos.reset( new Carna::base::model::Position( Carna::base::model::Position::fromMillimeters( model, p.x(), p.y(), p.z() ) ) );
            break;

        case PointCloud::volumeUnits:
            pos.reset( new Carna::base::model::Position( Carna::base::model::Position::fromVolumeUnits( model, p.x(), p.y(), p.z() ) ) );
            break;

        default:
            throw std::logic_error( "unknown PointCloud::Unit format" );

        }

        const Carna::base::Vector& modelSpace = pos->toVolumeUnits();

        points[ it - cloud.getList().begin() ] = modelSpace;

        minX = std::min( minX, modelSpace.x() );
        minY = std::min( minY, modelSpace.y() );
        minZ = std::min( minZ, modelSpace.z() );

        maxX = std::max( maxX, modelSpace.x() );
        maxY = std::max( maxY, modelSpace.y() );
        maxZ = std::max( maxZ, modelSpace.z() );

        progress.setValue( it - cloud.getList().begin() );
    }

    QApplication::restoreOverrideCursor();

 // ----------------------------------------------------------------------------------
    
    if( cloud.source == PointCloud::unknown )
    {
        const static unsigned int max_sample_count = 100;
        const static unsigned int step = std::max( points.size() / max_sample_count, unsigned( 1 ) );

        unsigned int points_within_data_space = 0;
        unsigned int sample_count = 0;
        for( unsigned int i = 0; i < points.size(); i += step )
        {
            const Carna::base::Vector& position = points[ i ];

            ++sample_count;

            if( position.x() >= 0. && position.x() <= 1.
             && position.y() >= 0. && position.y() <= 1.
             && position.z() >= 0. && position.z() <= 1. )
            {
                ++points_within_data_space;
            }
        }

        const double points_within_data_space_ratio = static_cast< double >( points_within_data_space ) / sample_count;

#ifndef NO_CRA
        if( points_within_data_space_ratio < 0.9 )
        {
            this->applyRegistration = true;
        }
#endif
    }

 // ----------------------------------------------------------------------------------

#ifndef NO_CRA

    NotificationsClient( server ).connectServiceProvided( this, SLOT( serviceProvided( const std::string& ) ) );
    if( server.hasService( Registration::serviceID ) )
    {
        serviceProvided( Registration::serviceID );
    }

#endif

 // ----------------------------------------------------------------------------------

    ready = true;
}


PointCloud3D::~PointCloud3D()
{
    ready = false;

    QApplication::setOverrideCursor( Qt::WaitCursor );

    points.clear();

    QApplication::restoreOverrideCursor();
}


void PointCloud3D::draw( const Carna::base::view::Renderer& renderer, const Carna::base::Vector3ui& color, bool simplified ) const
{
    if( !ready )
    {
        return;
    }

    glPushMatrix();

    const Carna::base::Vector& position = this->position().toVolumeUnits();
    glTranslatef( position.x(), position.y(), position.z() );

#ifndef NO_CRA

    if( applyRegistration )
    {
        try
        {
            using Carna::base::Transformation;
            using Carna::base::Vector;
            using Carna::base::model::Position;

            const Transformation trafo = RegistrationClient( server )->getTransformation();
            const Carna::base::model::Scene& model = CarnaContextClient( server ).model();

            glMultMatrix( Position::toVolumeUnitsTransformation( model ) );
            glMultMatrix( trafo );
            glMultMatrix( Position::toMillimetersTransformation( model ) );
        }
        catch( const Record::Server::Exception& )
        {
        }
    }

#endif

    glColor3ub( color.x, color.y, color.z );

    auto emitVertices = [&]()
    {
        glPointSize( static_cast< float >( pointSize ) );
        glBegin( GL_POINTS );

        for( auto it = points.begin(); it != points.end(); ++it )
        {
            const Carna::base::Vector& point = *it;

            glVertex3f( point.x(), point.y(), point.z() );
        }

        glEnd();
    };

    if( simplified || !modulateBrightnessByDistance )
    {
        emitVertices();
    }
    else
    {
        glPushAttrib( GL_ALL_ATTRIB_BITS );

        Carna::base::view::ShaderProgram::Binding shader( renderer.shader( shaderId ) );

        glDepthMask( GL_TRUE );
        glEnable( GL_DEPTH_TEST );
        glDepthFunc( GL_LEQUAL );

        emitVertices();

        glPopAttrib();
    }

    if( drawBoundingBox )
    {

        glLineWidth( 0.5f );
        glBegin( GL_LINE_LOOP );

        glVertex3f( minX, minY, minZ );
        glVertex3f( maxX, minY, minZ );
        glVertex3f( maxX, maxY, minZ );
        glVertex3f( minX, maxY, minZ );

        glEnd();
        glBegin( GL_LINE_LOOP );

        glVertex3f( minX, minY, maxZ );
        glVertex3f( maxX, minY, maxZ );
        glVertex3f( maxX, maxY, maxZ );
        glVertex3f( minX, maxY, maxZ );

        glEnd();
        glBegin( GL_LINES );

        glVertex3f( minX, minY, minZ );
        glVertex3f( minX, minY, maxZ );

        glVertex3f( maxX, minY, minZ );
        glVertex3f( maxX, minY, maxZ );

        glVertex3f( maxX, maxY, minZ );
        glVertex3f( maxX, maxY, maxZ );

        glVertex3f( minX, maxY, minZ );
        glVertex3f( minX, maxY, maxZ );

        glEnd();

    }

    glPopMatrix();
}


void PointCloud3D::paint( const Carna::base::view::Renderer& renderer ) const
{
    draw( renderer, Carna::base::Vector3ui
                                    ( pointColor.red()
                                    , pointColor.green()
                                    , pointColor.blue()
                    ), false );
}


void PointCloud3D::paintFalseColor( const Carna::base::view::Renderer& renderer, const Carna::base::Vector3ui& color ) const
{
    draw( renderer, color, true );
}


double PointCloud3D::getPointSize() const
{
    return pointSize;
}


const QColor& PointCloud3D::getPointColor() const
{
    return pointColor;
}


void PointCloud3D::setPointSize( double size )
{
    if( std::abs( this->pointSize - size ) > 1e-4 )
    {
        this->pointSize = size;

        invalidateObjects3D( Carna::base::model::Object3DEvent( Carna::base::model::Object3DEvent::shape ) );

        emit pointSizeChanged( this->pointSize );
    }
}


void PointCloud3D::setPointColor( const QColor& color )
{
    if( this->pointColor != color )
    {
        this->pointColor = color;

        invalidateObjects3D( Carna::base::model::Object3DEvent( Carna::base::model::Object3DEvent::shape ) );

        emit pointColorChanged( this->pointColor );
    }
}


bool PointCloud3D::modulatesBrightnessByDistance() const
{
    return modulateBrightnessByDistance;
}


void PointCloud3D::setBrightnessByDistanceModulation( bool modulateBrightnessByDistance )
{
    if( this->modulateBrightnessByDistance != modulateBrightnessByDistance )
    {
        this->modulateBrightnessByDistance = modulateBrightnessByDistance;

        invalidateObjects3D( Carna::base::model::Object3DEvent( Carna::base::model::Object3DEvent::shape ) );

        emit brightnessByDistanceModulationChanged( this->modulateBrightnessByDistance );
    }
}


bool PointCloud3D::drawsBoundingBox() const
{
    return drawBoundingBox;
}


void PointCloud3D::setBoundingBoxDrawing( bool drawBoundingBox )
{
    if( this->drawBoundingBox != drawBoundingBox )
    {
        this->drawBoundingBox = drawBoundingBox;

        invalidateObjects3D( Carna::base::model::Object3DEvent( Carna::base::model::Object3DEvent::shape ) );

        emit boundingBoxDrawingChanged( this->drawBoundingBox );
    }
}


void PointCloud3D::serviceProvided( const std::string& serviceID )
{
#ifndef NO_CRA
    if( serviceID == Registration::serviceID && this->applyRegistration )
    {
        RegistrationClient( server ).connectTransformationChanged( this, SLOT( transformationChanged() ) );
        transformationChanged();
    }
#endif
}


#ifndef NO_CRA

bool PointCloud3D::appliesRegistration() const
{
    return applyRegistration;
}


void PointCloud3D::setRegistrationApplication( bool applyRegistration )
{
    if( this->applyRegistration != applyRegistration )
    {
        this->applyRegistration = applyRegistration;

        if( server.hasService( Registration::serviceID ) )
        {
            invalidateObjects3D( Carna::base::model::Object3DEvent( Carna::base::model::Object3DEvent::position ) );
        }

        emit registrationApplicationChanged( this->applyRegistration );
    }
}


void PointCloud3D::transformationChanged()
{
    if( this->applyRegistration )
    {
        invalidateObjects3D( Carna::base::model::Object3DEvent( Carna::base::model::Object3DEvent::position ) );
    }
}

#endif
