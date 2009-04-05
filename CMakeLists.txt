#--------------------------------#
# Mods for Apple to use GCC 4.2  #
#--------------------------------#
#SET(CMAKE_CXX_COMPILER g++-4.2)
#SET(VTK_DIR /vtk)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

PROJECT(EBSD)

INCLUDE(FindPkgConfig)
#pkg_check_modules (GTKREQ gtkglext-1.0)
#LINK_LIBRARIES(${GTKREQ_LIBRARIES})
#INCLUDE_DIRECTORIES(${GTKREQ_INCLUDE_DIRS})
#INCLUDE("CMake/FindGTK2.cmake")
#LINK_LIBRARIES(${GTK2_LIBRARIES})
#INCLUDE_DIRECTORIES(${GTK2_INCLUDE_DIRS})

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

INCLUDE(FindwxWidgets)

#FIND_PACKAGE(wxWidgets COMPONENTS
#				string
#				bitmap
#				image
#				icon
#				menu
#				gdicmn
#				font
#				colour
#				settings
#				statusbr
#				toolbar
#				treectrl
#				sizer
#				frame)

# The following allows you to access wxGLCanvas for GTK
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

FIND_PACKAGE(wxWidgets COMPONENTS base core adv ${WXGLCANVASLIBS})

IF(wxWidgets_FOUND)
  INCLUDE( ${wxWidgets_USE_FILE} )
ENDIF(wxWidgets_FOUND)

LINK_LIBRARIES(${wxWidgets_LIBRARIES})
INCLUDE(${wxWidgets_USE_FILE})

# Include VTK
#------------------------------------------#

#INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)
#FIND_PACKAGE(VTK)
IF (USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
ENDIF (USE_VTK_FILE)

LINK_LIBRARIES(
vtkCommon
vtkGraphics
vtkIO
vtkRendering
#vtkCommonPythonD
vtkFiltering
vtkWidgets
)

# Build Newmat
#------------------------------------------#

ADD_SUBDIRECTORY(newmat10)

#INCLUDE_DIRECTORIES(vtkgtk)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/newmat10)
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/prob)

LINK_LIBRARIES(newmat) 
#LINK_LIBRARIES(prob) 

# Include Python and NumPy
#------------------------------------------#
#INCLUDE(FindPythonLibs)
#INCLUDE(FindPythonInterp)

# Make Python Extention
#------------------------------------------#
#IF(PYTHON_LIBRARIES) 

#INCLUDE("CMake/FindNumpy.cmake")

#INCLUDE_DIRECTORIES(${PYTHON_NUMARRAY_INCLUDE_DIR})
#INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

#INCLUDE_DIRECTORIES(${PYTHON_LIBRARIES})
#INCLUDE_DIRECTORIES(${PYTHON_NUMARRAY_INCLUDE_DIR})

#ADD_LIBRARY(ebsd MODULE 
#				 pymod.cpp
#				 pymod.h
#				 ebsd.h
#				 symmetry.h
#				 visualisation.h
#				 characterisation.cpp
#				 alignment.cpp
#				 visualisation.cpp
#				 processing.cpp
#				 file_access.cpp
#				 segmentation.cpp
#				 orientation.cpp
#				 boundaries.cpp
#				 dataset.cpp
#				 prob.C
#				 prob.H)

#SET_TARGET_PROPERTIES(ebsd PROPERTIES
#			   PREFIX ""
#			   COMPILE_FLAGS -DPYTHON_BINDINGS)
#
#TARGET_LINK_LIBRARIES(ebsd python2.5)

#INSTALL(DIRECTORY python 
#		DESTINATION ${CMAKE_BINARY_DIR}
#		PATTERN ".*" EXCLUDE)
#
#ENDIF(PYTHON_LIBRARIES) 


# Make standalone executable
#------------------------------------------#
ADD_EXECUTABLE(p ebsd.cpp
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

				 
				 wxInterface/wxVTKRenderWindowInteractor.cpp
				 wxInterface/wxVTKRenderWindowInteractor.h
				 
				 wxInterface/vtkFrame.cpp
				 wxInterface/dialogs.cpp
				 wxInterface/alignmentFrame.cpp
				
				 wxInterface/vtk3DEBSDWindow.cpp
				 wxInterface/vtk2DEBSDWindow.cpp
				 
				 wxInterface/wx-ebsd.cpp
				 )

#TARGET_LINK_LIBRARIES(p gtkglext-1.0)

#SET_TARGET_PROPERTIES(p PROPERTIES
#			COMPILE_FLAGS -fopenmp)	

MESSAGE("${wxWidgets_DEFINITIONS}")

SET_TARGET_PROPERTIES(p PROPERTIES
			COMPILE_FLAGS "${wxWidgets_CXX_FLAGS} ${wxWidgets_DEFINITIONS}")


# Make Documentation
#------------------------------------------#			
INCLUDE(FindDoxygen)
IF(${DOXYGEN_EXECUTABLE})
EXECUTE_PROCESS(${DOXYGEN_EXECUTABLE} doxygen.txt 		
				WORKING DIRECTORY ${CMAKE_SOURCE_DIRECTORY})

INSTALL(DIRECTORY docs 
		DESTINATION ${CMAKE_BINARY_DIR} )
ENDIF(${DOXYGEN_EXECUTABLE})
			
