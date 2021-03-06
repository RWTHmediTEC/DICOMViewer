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

#pragma once

#include "Server.h"
#include <Carna/base/model/Object3D.h>
#include <QColor>

class PointCloud;



// ----------------------------------------------------------------------------------
// PointCloud3D
// ----------------------------------------------------------------------------------

/** \brief	Carna scene element representation of some PointCloud.
  *
  * \image  html    PointCloud3D.png
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 31.5.12
  */
class PointCloud3D : public Carna::base::model::Object3D
{

    Q_OBJECT

public:

    /** \brief	Instantiates.
      *
      * Performs a deep copy of the supplied point clouds points.
      */
    PointCloud3D( Record::Server&, const PointCloud&, QWidget* modalityParent = nullptr );

    /** \brief	Releases acquired resources.
      */
    virtual ~PointCloud3D();

    
    /** \brief  Draws this object.
      */
    virtual void paint( const Carna::base::view::Renderer& ) const;
    
    /** \brief  Draw this object using all-over the specified color.
      */
    virtual void paintFalseColor( const Carna::base::view::Renderer&, const Carna::base::Vector3ui& color ) const;


    double getPointSize() const;

    const QColor& getPointColor() const;

    bool modulatesBrightnessByDistance() const;

    bool drawsBoundingBox() const;

#ifndef NO_CRA

    bool appliesRegistration() const;

#endif


public slots:

    void setPointSize( double );

    void setPointColor( const QColor& );

    void setBrightnessByDistanceModulation( bool );

    void setBoundingBoxDrawing( bool );

#ifndef NO_CRA

    void setRegistrationApplication( bool );

#endif


signals:

    void pointSizeChanged( double );

    void pointColorChanged( const QColor& );

    void brightnessByDistanceModulationChanged( bool );

    void boundingBoxDrawingChanged( bool );

#ifndef NO_CRA

    void registrationApplicationChanged( bool );

#endif


private slots:

    void serviceProvided( const std::string& serviceID );

#ifndef NO_CRA

    void transformationChanged();

#endif


private:

    Record::Server& server;


    /** \brief	Defines list of points.
      */
    typedef std::deque< Carna::base::Vector > PointList;

    /** \brief	Holds the points within this point cloud in model units.
      */
    PointList points;

    /** \brief  Holds weather this object can be painted.
      */
    bool ready;


    /** \brief	Holds the lowest \em x value within \ref points.
      */
    double minX;

    /** \brief	Holds the highest \em x value within \ref points.
      */
    double maxX;

    /** \brief	Holds the lowest \em y value within \ref points.
      */
    double minY;

    /** \brief	Holds the highest \em y value within \ref points.
      */
    double maxY;

    /** \brief	Holds the lowest \em z value within \ref points.
      */
    double minZ;

    /** \brief	Holds the highest \em z value within \ref points.
      */
    double maxZ;


    double pointSize;

    QColor pointColor;

    bool modulateBrightnessByDistance;

    bool drawBoundingBox;

#ifndef NO_CRA

    bool applyRegistration;

#endif


    /** \brief  Draws the geometry.
      */
    void draw( const Carna::base::view::Renderer&, const Carna::base::Vector3ui& color, bool simplified ) const;

}; // PointCloud3D
