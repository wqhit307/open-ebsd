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

# Include VTK
#------------------------------------------#

INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)
FIND_PACKAGE(VTK)
IF (USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
ENDIF (USE_VTK_FILE)

LINK_LIBRARIES(
vtkCommon
vtkGraphics
vtkIO
vtkRendering
vtkCommonPythonD
vtkFiltering
)

# Build Newmat
#------------------------------------------#

ADD_SUBDIRECTORY(newmat10)
ADD_SUBDIRECTORY(prob)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/newmat10)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/prob)

LINK_LIBRARIES(newmat) 
LINK_LIBRARIES(prob) 

# Include Python and NumPy
#------------------------------------------#
INCLUDE(FindPythonLibs)
INCLUDE(FindPythonInterp)

# Make Python Extention
#------------------------------------------#
IF(PYTHON_LIBRARIES) 

INCLUDE("CMake/FindNumpy.cmake")

INCLUDE_DIRECTORIES(${PYTHON_NUMARRAY_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

INCLUDE_DIRECTORIES(${PYTHON_LIBRARIES})
INCLUDE_DIRECTORIES(${PYTHON_NUMARRAY_INCLUDE_DIR})

ADD_LIBRARY(ebsd MODULE 
				 pymod.cpp
				 pymod.h
				 ebsd.h
				 symmetry.h
				 visualisation.h
				 characterisation.cpp
				 alignment.cpp
				 visualisation.cpp
				 processing.cpp
				 file_access.cpp
				 segmentation.cpp
				 orientation.cpp
				 boundaries.cpp
				 dataset.cpp)

SET_TARGET_PROPERTIES(ebsd PROPERTIES
			   PREFIX ""
			   COMPILE_FLAGS -DPYTHON_BINDINGS)

TARGET_LINK_LIBRARIES(ebsd python2.5)

INSTALL(DIRECTORY python 
		DESTINATION ${CMAKE_BINARY_DIR}
		PATTERN ".*" EXCLUDE)

ENDIF(PYTHON_LIBRARIES) 


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
				 dataset.cpp)

SET_TARGET_PROPERTIES(p PROPERTIES
			COMPILE_FLAGS -fopenmp)
			


# Make Documentation
#------------------------------------------#			
INCLUDE(FindDoxygen)
IF(${DOXYGEN_EXECUTABLE})
EXECUTE_PROCESS(${DOXYGEN_EXECUTABLE} doxygen.txt 		
				WORKING DIRECTORY ${CMAKE_SOURCE_DIRECTORY}

INSTALL(DIRECTORY docs 
		DESTINATION ${CMAKE_BINARY_DIR} )
ENDIF(${DOXYGEN_EXECUTABLE})
			