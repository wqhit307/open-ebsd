CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

PROJECT(OpenEBSD)

INCLUDE(FindPkgConfig)

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

# The following allows you to access wxGLCanvas for GTK
#------------------------------------------#

IF(WIN32)
  SET(GUI_EXECUTABLE WIN32)
ELSE(WIN32)
  IF(APPLE)
    SET(GUI_EXECUTABLE MACOSX_BUNDLE)
    IF(VTK_USE_COCOA)
      SET_SOURCE_FILES_PROPERTIES(
        src/wxVTKRenderWindowInteractor.cxx
        PROPERTIES COMPILE_FLAGS "-ObjC++")
    ENDIF(VTK_USE_COCOA)
  ELSE(APPLE)
    # Ok X11 for sure, but just check:
    IF(NOT VTK_USE_X)
      MESSAGE(FATAL_ERROR "You need to have VTK_USE_X")
    ENDIF(NOT VTK_USE_X)
    # CMake 2.6:
    # technically those packages are not required since one can still use the Motif/X11 version and not the gtk one:
    FIND_PACKAGE(PkgConfig)
    pkg_check_modules (GTK2 gtk+-2.0)
    #MESSAGE("${GTK2_INCLUDE_DIRS}")
    INCLUDE_DIRECTORIES(${GTK2_INCLUDE_DIRS})
    LINK_LIBRARIES(${GTK2_LIBRARIES})
    # Can I require all my user to have the gl lib on linux, even if they do not really need it...
    SET(WXGLCANVASLIBS "gl")
  ENDIF(APPLE)
ENDIF(WIN32)


# Include wxWidgets
#------------------------------------------#

FIND_PACKAGE(wxWidgets COMPONENTS base core adv ${WXGLCANVASLIBS})


IF(wxWidgets_FOUND)
	INCLUDE_DIRECTORIES(${wxWidgets_INCLUDE_DIRS})
	ADD_DEFINITIONS(${wxWidgets_DEFINITIONS})
	INCLUDE(${wxWidgets_USE_FILE})
	LINK_LIBRARIES(${wxWidgets_LIBRARIES})
	LINK_DIRECTORIES(${wxWidgets_LIB_DIR})
ENDIF(wxWidgets_FOUND)




# Include VTK
#------------------------------------------#

IF (USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
ENDIF (USE_VTK_FILE)

LINK_LIBRARIES(
vtkCommon
vtkGraphics
vtkIO
vtkRendering

vtkFiltering
vtkWidgets
)


# Build Newmat
#------------------------------------------#

ADD_SUBDIRECTORY(newmat10)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/newmat10)
LINK_LIBRARIES(newmat) 



# Make standalone executable
#------------------------------------------#
ADD_EXECUTABLE(OpenEBSD		 ebsd.cpp
				 ebsd.h
				 symmetry.h
				 visualisation.h
				 vtk.h
				 characterisation.cpp
				 alignment.cpp
				 visualisation.cpp
				 processing.cpp
				 file_access.cpp
				 segmentation.cpp
				 orientation.cpp
				 boundaries.cpp
				 dataset.cpp
				 prob.C
				 prob.H

				 
				 wxVTKRenderWindowInteractor.cpp
				 wxVTKRenderWindowInteractor.h
				 
				 dialogs.cpp
				 dialogs.h

				 alignmentFrame.cpp
				 alignmentFrame.h
				
				 vtk3DEBSDWindow.h
				 vtk2DEBSDWindow.h
				 vtk3DEBSDWindow.cpp
				 vtk2DEBSDWindow.cpp
				 
				 wxEBSD.cpp
				 wxEBSD.h

				 vtkFrame.h
				 vtkFrame.cpp
				 )

SET_TARGET_PROPERTIES(OpenEBSD PROPERTIES
			       COMPILE_FLAGS "${wxWidgets_CXX_FLAGS} ${wxWidgets_DEFINITIONS}"
			       LINK_FLAGS "/SUBSYSTEM:WINDOWS")


# Make Documentation
#------------------------------------------#			
INCLUDE(FindDoxygen)
IF(${DOXYGEN_EXECUTABLE})
EXECUTE_PROCESS(${DOXYGEN_EXECUTABLE} doxygen.txt 		
				WORKING DIRECTORY ${CMAKE_SOURCE_DIRECTORY})

INSTALL(DIRECTORY docs 
		DESTINATION ${CMAKE_BINARY_DIR} )
ENDIF(${DOXYGEN_EXECUTABLE})
			
