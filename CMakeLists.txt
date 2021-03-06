cmake_minimum_required(VERSION 3.0.2)
project( DICOMViewer )

set( MAJOR_VERSION	 3 )
set( MINOR_VERSION	 1 )
set( PATCH_VERSION	17 )

set( CMAKE_DEBUG_POSTFIX d )
set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin )
set( LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin )
set( FULL_VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION} )
set( TARGET_NAME ${PROJECT_NAME}-${FULL_VERSION} )

############################################
# Options
# Set default options for this build
############################################

option(BUILD_DOC	"Build and install the API documentation"	OFF)
option(BUILD_TEST	"Build the unit tests"						OFF)

############################################
# Locate Find<ModuleName>.cmake scripts
############################################

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/misc/CMake-Modules)

############################################
# Run MediTEC compatiblity script
############################################

add_subdirectory(misc/MediTEC)

############################################

configure_file(	${CMAKE_CURRENT_SOURCE_DIR}/misc/${PROJECT_NAME}.bat.in
				${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}.bat @ONLY )
				
configure_file(	${CMAKE_CURRENT_SOURCE_DIR}/src/doc/Doxyfile.in
				${CMAKE_CURRENT_SOURCE_DIR}/src/doc/Doxyfile @ONLY )

############################################
# Setup the visual studio project properly
############################################

set( VSXPROJ_USER ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.vcxproj.user )
if( NOT EXISTS ${VSXPROJ_USER} )
	string( REGEX REPLACE "/" "\\\\" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
	configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/misc/DICOMViewer.vcxproj.user.in ${VSXPROJ_USER} @ONLY )
endif()

############################################
# Find required dependencies
############################################

# Qt
set( QT_USE_QTMAIN		TRUE )
set( QT_USE_QTOPENGL	TRUE )
set( QT_USE_QTXML		TRUE )
find_package( Qt4 REQUIRED )
include( ${QT_USE_FILE} )
add_definitions( ${QT_DEFINITIONS} )

# GLEW
find_package( GLEW 1.7.0 REQUIRED )
include_directories( ${GLEW_INCLUDE_DIRS} )

# Carna
find_package( Carna REQUIRED )
include_directories( ${CARNA_INCLUDE_DIR} )

# Carna-DICOM
find_package( CarnaDICOM REQUIRED )
include_directories( ${CARNADICOM_INCLUDE_DIR} )

# CRA
find_package( CRA )
include_directories( ${CRA_INCLUDE_DIR} )

# Eigen
find_package( Eigen3 3.0.5 REQUIRED )
include_directories( ${EIGEN3_INCLUDE_DIR} )

# TRTK
find_package( TRTK 0.13.1 REQUIRED )
include_directories( ${TRTK_INCLUDE_DIR} )

# Flann
find_package( Flann 1.7.1 REQUIRED )
include_directories( ${FLANN_INCLUDE_DIR} )

############################################
# Project
############################################

include_directories(${CMAKE_PROJECT_DIR}src)
set( QOBJECT_HEADERS
		src/BinaryDumpExportDialog.h
		src/BinaryDumpImportDialog.h
		src/CarnaModelFactory.h
		src/Component.h
		src/ComponentEmbeddable.h
		src/ComponentLauncher.h
		src/EmbedArea.h
		src/EmbedAreaArray.h
		src/Embeddable.h
		src/FileChooser.h
		src/Gulsun.h
		src/GulsunComponent.h
		src/GulsunSegmentation.h
		src/Histogram.h
		src/HistogramController.h
		src/HistogramView.h
		src/Importer.h
		src/IntegerFormatChooser.h
		src/MainWindow.h
		src/MaskingDialog.h
		src/ModelInfo.h
		src/MPR.h
		src/NotificationsProvider.h
		src/Object3DEditorDetails.h
		src/ObjectsController.h
		src/ObjectsList.h
		src/ObjectsListItem.h
		src/ObjectsView.h
		src/PointCloud.h
		src/PointCloud3D.h
		src/PointCloudChooser.h
		src/PointCloudComposer.h
		src/PointCloudComposerSlot.h
		src/PointCloudsController.h
		src/PointCloudsProvider.h
		src/Server.h
		src/SlicePlane.h
		src/SurfaceExtractionDialog.h
		src/ViewWindow.h
		src/VolumeController.h
		src/VolumeNormalizer.h
		src/VolumeView.h
		src/VolumeViewCameraController.h
		src/WindowingController.h
)
if(CRA_FOUND)
	set( QOBJECT_HEADERS
			${QOBJECT_HEADERS}
			src/ICP.h
			src/LocalizerController.h
			src/LocalizerProvider.h
			src/Pointer3D.h
			src/Pointer3DCalibration.h
			src/Pointer3DEditor.h
			src/PointRecorder.h
			src/PointRecorderView.h
			src/RegistrationController.h
			src/RegistrationProvider.h
			src/ToolCalibrator.h
			src/ToolChooser.h
			src/ToolEditor.h
			src/ToolSetEditor.h
		)
endif(CRA_FOUND)
set( HEADERS
		${QOBJECT_HEADERS}
		src/BinaryDumpProcessor.h
		src/BinaryVolumeMask.h
		src/CarnaContext.h
		src/CarnaContextClient.h
		src/CarnaContextProvider.h
		src/ClippedVolumeMask.h
		src/Components.h
		src/ComponentsClient.h
		src/ComponentsProvider.h
		src/DataSize.h
		src/Differential.h
		src/Dijkstra.h
		src/EmbeddablePlacer.h
		src/EmbedManager.h
		src/Exporter.h
		src/ExportProcessor.h
		src/FlowLayout.h
		src/glew.h
		src/GulsunRadiusStore.h
		src/ImportProcessor.h
		src/LeafFinder.h
		src/Medialness.h
		src/MedialnessGraph.h
		src/MultiscaleDifferential.h
		src/NormalizedEdgeResponse.h
		src/Notifications.h
		src/NotificationsClient.h
		src/Object3DEditor.h
		src/Object3DEditorFactory.h
		src/ObjectsComponent.h
		src/OptimizedVolumeDecorator.h
		src/Point3DEditor.h
		src/PointCloud3DEditor.h
		src/PointClouds.h
		src/PointCloudsClient.h
		src/PointCloudsComponent.h
		src/RegistredComponent.h
		src/ScalarField3ui.h
		src/Segmentation.h
		src/SuccessiveMedialness.h
		src/SurfaceExtraction.h
		src/WindowingComponent.h
	)
if(CRA_FOUND)
	set( HEADERS
			${HEADERS}
			src/Localizer.h
			src/LocalizerClient.h
			src/LocalizerComponent.h
			src/PointToPointRegistration.h
			src/Registration.h
			src/RegistrationArguments.h
			src/RegistrationClient.h
			src/RegistrationComponent.h
			src/RegistrationOperation.h
			src/Validation.h
		)
endif(CRA_FOUND)
set( SRC
		src/BinaryDumpExportDialog.cpp
		src/BinaryDumpImportDialog.cpp
		src/BinaryDumpProcessor.cpp
		src/CarnaContextProvider.cpp
		src/CarnaModelFactory.cpp
		src/ClippedVolumeMask.cpp
		src/Component.cpp
		src/ComponentEmbeddable.cpp
		src/ComponentLauncher.cpp
		src/ComponentsProvider.cpp
		src/Differential.cpp
		src/EmbedArea.cpp
		src/EmbedAreaArray.cpp
		src/Embeddable.cpp
		src/EmbeddablePlacer.cpp
		src/EmbedManager.cpp
		src/Exporter.cpp
		src/FileChooser.cpp
		src/FlowLayout.cpp
		src/Gulsun.cpp
		src/GulsunComponent.cpp
		src/GulsunRadiusStore.cpp
		src/GulsunSegmentation.cpp
		src/Histogram.cpp
		src/HistogramController.cpp
		src/HistogramView.cpp
		src/Importer.cpp
		src/IntegerFormatChooser.cpp
		src/main.cpp
		src/MainWindow.cpp
		src/MaskingDialog.cpp
		src/Medialness.cpp
		src/MedialnessGraph.cpp
		src/ModelInfo.cpp
		src/MPR.cpp
		src/MultiscaleDifferential.cpp
		src/NormalizedEdgeResponse.cpp
		src/Object3DEditor.cpp
		src/Object3DEditorDetails.cpp
		src/Object3DEditorFactory.cpp
		src/ObjectsComponent.cpp
		src/ObjectsController.cpp
		src/ObjectsList.cpp
		src/ObjectsListItem.cpp
		src/ObjectsView.cpp
		src/OptimizedVolumeDecorator.cpp
		src/Point3DEditor.cpp
		src/PointCloud.cpp
		src/PointCloud3D.cpp
		src/PointCloud3DEditor.cpp
		src/PointCloudChooser.cpp
		src/PointCloudComposer.cpp
		src/PointCloudComposerSlot.cpp
		src/PointCloudsComponent.cpp
		src/PointCloudsController.cpp
		src/Segmentation.cpp
		src/Server.cpp
		src/SlicePlane.cpp
		src/SuccessiveMedialness.cpp
		src/SurfaceExtraction.cpp
		src/SurfaceExtractionDialog.cpp
		src/ViewWindow.cpp
		src/VolumeController.cpp
		src/VolumeNormalizer.cpp
		src/VolumeView.cpp
        src/VolumeViewCameraController.cpp
		src/WindowingComponent.cpp
		src/WindowingController.cpp
	)
if(CRA_FOUND)
	set( SRC
			${SRC}
			src/ICP.cpp
			src/LocalizerComponent.cpp
			src/LocalizerController.cpp
			src/Pointer3D.cpp
			src/Pointer3DCalibration.cpp
			src/Pointer3DEditor.cpp
			src/PointRecorder.cpp
			src/PointRecorderView.cpp
			src/PointToPointRegistration.cpp
			src/RegistrationComponent.cpp
			src/RegistrationController.cpp
			src/RegistrationOperation.cpp
			src/ToolCalibrator.cpp
			src/ToolChooser.cpp
			src/ToolEditor.cpp
			src/ToolSetEditor.cpp
			src/Validation.cpp
		)
endif(CRA_FOUND)
set( FORMS
		""
	)
set( RESOURCES
		src/ComponentIcons.qrc
		src/shaders.qrc
		src/Icons.qrc
	)

############################################

add_definitions( -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS )

if( MSVC )
   add_definitions( /arch:SSE2 )
endif( MSVC )

if( NOT CRA_FOUND )
	add_definitions( -DNO_CRA )
endif()

############################################

QT4_WRAP_CPP( HEADERS_MOC ${QOBJECT_HEADERS} )
QT4_WRAP_UI( FORMS_HEADERS ${FORMS} )
QT4_ADD_RESOURCES( RESOURCES_RCC ${RESOURCES} )
	
############################################
# Exclude all source files from build,
# since we are using unity build.
############################################

set_source_files_properties(	${SRC}
								${HEADERS_MOC}
								${FORMS_HEADERS}
	PROPERTIES
	HEADER_FILE_ONLY TRUE )

############################################
# Write unity build file
############################################

set( UNITY_BUILD_FILE	${CMAKE_CURRENT_BINARY_DIR}/unity_build.cpp )

file( REMOVE	${UNITY_BUILD_FILE} )
file( WRITE		${UNITY_BUILD_FILE} "// This file is automatically generated by CMake.\n\n" )
file( APPEND	${UNITY_BUILD_FILE}	"#include \"${CMAKE_CURRENT_SOURCE_DIR}/src/glew.h\"\n" )

foreach( SOURCE_FILE ${SRC} )
	file( APPEND	${UNITY_BUILD_FILE}	"#include \"${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}\"\n" )
endforeach( SOURCE_FILE )

foreach( QT_GENERATED_SOURCE_FILE ${HEADERS_MOC} ${FORMS_HEADERS} )
	file( APPEND	${UNITY_BUILD_FILE}	"#include \"${QT_GENERATED_SOURCE_FILE}\"\n" )
endforeach( QT_GENERATED_SOURCE_FILE )

############################################	
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
############################################

add_executable( ${TARGET_NAME} WIN32
			${SRC}
			${UNITY_BUILD_FILE}
			${HEADERS}
			${HEADERS_MOC}
			${FORMS_HEADERS}
			${RESOURCES_RCC}
		)

############################################
# Add dependencies to the linker
############################################

target_link_libraries( ${TARGET_NAME}
			opengl32
			glu32
			${GLEW_LIBRARIES}
			${QT_LIBRARIES}
			${TRTK_LIBRARIES}
			${CARNA_LIBRARIES}
			${CARNADICOM_LIBRARIES}
			${CRA_LIBRARIES}
			${FLANN_LIBRARIES}
		)

############################################
# Define installation routines
############################################

install(TARGETS	${TARGET_NAME}
		RUNTIME	DESTINATION	${CMAKE_CURRENT_SOURCE_DIR}/bin/debug	CONFIGURATIONS Debug)

install(TARGETS	${TARGET_NAME}
		RUNTIME	DESTINATION	${CMAKE_CURRENT_SOURCE_DIR}/bin/release	CONFIGURATIONS Release)
