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
#include <QWidget>
#include <Carna/Carna.h>

#ifndef NO_CRA
#include <CRA/LinkedObject.h>
#include <CRA/LinkedCamera.h>
#endif

class VolumeView;
class ToolChooser;
class VolumeViewCameraController;

class QComboBox;
class QCheckBox;
class QDoubleSpinBox;
class QSpinBox;



// ----------------------------------------------------------------------------------
// VolumeController
// ----------------------------------------------------------------------------------

/** \brief  Provides \c Carna::base::model::VolumeController with some extra options.
  *
  * \see    VolumeView
  * \author Leonid Kostrykin
  * \date   17.7.12 - 24.4.13
  */
class VolumeController : public QWidget
{

    Q_OBJECT

public:

    enum PreferredCameraMode
    {
        perspectiveProjection = 0,
        orthogonalProjection = 1,
        undefinedPreferredProjection = 2
    };


    VolumeController( Carna::VolumeRenderings::VolumeVisualization& view, Record::Server& server, VolumeView& tool );

    virtual ~VolumeController();

    VolumeView& tool;


public slots:

#ifndef NO_CRA

    /** \brief
      * If \c CRA::LinkedCamera is currently being used,
      * it is replaced by new instance of \c Carna::base::view::DefaultCamera through \ref releaseLinkedCamera.
      * Otherwise the current camera is replaced by new instance of \c Carna::base::view::DefaultCamera through \ref setDefaultCamera.
      */
    void resetCamera();

#endif


private:

    Carna::VolumeRenderings::VolumeVisualization& view;

    Record::Server& server;

    VolumeViewCameraController* const cameraController;

#ifndef NO_CRA

    ToolChooser* const linkedCameraChooser;

    std::unique_ptr< CRA::LinkedCamera > linkedCamera;

#endif

    PreferredCameraMode preferredCameraMode;

    QCheckBox* const cbAutoRotate;

    QDoubleSpinBox* const sbSecondsPerRotation;

    QSpinBox* const sbTargetFramesPerSecond;

    
    QComboBox* const cbRenderMode;

    enum RenderMode
    {
        monoscopic = 0,
        philips = 1,
        zalman = 2
    };


    QPushButton* const buGulsun;


#ifndef NO_CRA

    /** \brief
      * Creates new \c Carna::base::view::DefaultCamera instance and utilizes it as current camera.
      */
    void setDefaultCamera();

    /** \brief
      * If \c CRA::LinkedCamera is currently being used, and \c CRA::Registration is provided,
      * the camera-link is removed from the registration. Otherwise, nothing happens.
      */
    void unlinkCamera();

#endif


private slots:

    void processAddedService( const std::string& serviceID );

    void processRemovedService( const std::string& serviceID );


#ifndef NO_CRA

    /** \brief
      * Creates new \c CRA::LinkedCamera and utilizes it as current camera.
      * The newly created camera is also linked with the supplied tool.
      * If no \c CRA::Registration is provided, nothing happens.
      */
    void setLinkedCamera( CRA::Tool& );

    /** \brief
      * If \c CRA::LinkedCamera is currently being used,
      * than first it is unlinked through \ref unlinkCamera,
      * second \c Carna::base::view::DefaultCamera is utilized through \ref setDefaultCamera
      * and finally the \c CRA::LinkedCamera is destroyed.
      */
    void releaseLinkedCamera();

#endif


    void setRenderMode( int );

    void setPreferredCameraMode( int );

    /** \brief
      * Updates the projection mode of the current camera if it is instance of \c Carna::base::view::DefaultCamera.
      * Otherwise, nothing happens.
      */
    void updateCamera();

    void openGulsun();

    void gulsunClosed();

}; // VolumeController
