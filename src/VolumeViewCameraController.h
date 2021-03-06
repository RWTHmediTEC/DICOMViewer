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

#include <Carna/base/noncopyable.h>
#include <Carna/VolumeRenderings/VolumeVisualizationCameraController.h>
#include <QTimer>
#include <QTime>



// ----------------------------------------------------------------------------------
// VolumeViewCameraController
// ----------------------------------------------------------------------------------

class VolumeViewCameraController : public Carna::VolumeRenderings::VolumeVisualizationCameraController
{

    Q_OBJECT

    NON_COPYABLE

public:

    explicit VolumeViewCameraController( Carna::VolumeRenderings::VolumeVisualization& view, QObject* parent = nullptr );

    virtual void event( Carna::base::Visualization& sourceModule, QEvent* event ) override;


    int getStepsPerSecond() const;

    bool hasAutoRotate() const;

    double getSecondsPerRotation() const;


public slots:

    void setStepsPerSecond( int );

    void setAutoRotate( bool );

    void setSecondsPerRotation( double );


private slots:

    void doRotationStep();

    void startAutoRotate();

    void stopAutoRotate();


private:

    QTimer autoRotationTimer;

    QTime frameTimer;

    int stepsPerSecond;

    bool autoRotate;

    double secondsPerRotation;

}; // VolumeViewCameraController
