cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0057 NEW)
if (CMAKE_VERSION VERSION_LESS 3.9.0)
  cmake_policy(SET CMP0042 OLD)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GenerateExportHeader)

project(bdsim)

# project version
set( BDSIM_MAJOR_VERSION 1 )
set( BDSIM_MINOR_VERSION 7 )
set( BDSIM_PATCH_LEVEL   7.develop )
set( BDSIM_VERSION ${BDSIM_MAJOR_VERSION}.${BDSIM_MINOR_VERSION}.${BDSIM_PATCH_LEVEL})

message(STATUS "Configuring BDSIM ${BDSIM_VERSION}")
message(STATUS "Installation prefix: ${CMAKE_INSTALL_PREFIX}")

# add source dir as a place for CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

# include BDSIM macros
include(bdsim_macros)

# With this true we automatically include current src/binary directory
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

# Default build type (defines different sets of compiler flags)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
       "Choose the type of build, options are: None Debug DebugOutput Release RelWithDebInfo MinSizeRel DebugProf DebugCoverage."
       FORCE)
endif()

message(STATUS "Build Type ${CMAKE_BUILD_TYPE}")

# Add compiler flags based on build type
include(cmake/CompilerBuildFlags.cmake)

# Check if compiler is compatible
include(cmake/CompilerCheck.cmake)

# Identify operating system
include(cmake/SystemCheck.cmake)

# Regression testing flag
set(BDSIM_GENERATE_REGRESSION_DATA OFF CACHE BOOL "Whether to generate regression testing data from tests.")
mark_as_advanced(BDSIM_GENERATE_REGRESSION_DATA)
# Default prefix is test as we expect to be testing. Change to "ref" for reference results.
set(BDSIM_REGRESSION_PREFIX "test" CACHE STRING "Prefix for regression file generation.")
mark_as_advanced(BDSIM_REGRESSION_PREFIX)

#################  
# Find Packages #
#################

# GIT - optional - for software version in output & copying only example source files
# minimum version 1.8.2.1 for git ls-files for example copying
find_package(Git 1.8.2.1 QUIET)
# turn off git support if there's no .git folder - ie it's a copy of the source
# but not a repository
if (NOT EXISTS ${CMAKE_SOURCE_DIR}/.git)
  set(GIT_FOUND FALSE)
endif()

# Boost - do before ROOT as may change the compilation flags and standard
include(cmake/Boost.cmake)

# CLHEP
include(cmake/CLHEP.cmake)

# ROOT must be compiled in c++14 to be compatible with Boost
include(cmake/ROOT.cmake)

# DCMTK
include(cmake/DCMTK.cmake)

# AWAKE
include(modules/AWAKE.cmake)

# GDML (on by default)
option( USE_GDML "Include GDML support."  ON )
if(USE_GDML)
  include(cmake/XercesC.cmake)
  add_definitions("-DUSE_GDML")
  message(STATUS "GDML support ON")
endif()

# HEPMC3
include(cmake/HepMC.cmake)

# Sixtrack linking bits of code that aren't needed otherwise
option( USE_SIXTRACKLINK "Include linking code for SixTrack." OFF )
if(USE_SIXTRACKLINK)
  add_definitions("-DUSE_SIXTRACKLINK")
  message(STATUS "SixTrack link ON")
endif()

# Find GEANT4
include(cmake/Geant4.cmake)

## set compiler flags
include(cmake/CompilerFlags.cmake)

# add a target to generate API documentation with Doxygen
include (cmake/Doxygen.cmake)

# enable Testing with ctest
enable_testing()
include(CTest)
include(bdsim_test_macros)
include(comparator_test_macros)
include(interpolator_test_macros)
include(rebdsim_test_macros)
include(root_test_macros)
include(gmad_test_macros)

# rpath for libraries to work from installation correctly
set(CMAKE_BUILD_RPATH   "${ROOT_LIBRARY_DIR}")
if (USE_HEPMC3)
  #TBC - need to test if we can just use this
  #set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)
  set(CMAKE_INSTALL_RPATH "${Geant4_LIBRARY_DIR};${CMAKE_INSTALL_PREFIX}/lib;${HEPMC3_ROOT_DIR}/lib;${ROOT_LIBRARY_DIR}")
  #message(STATUS "rpath = ${CMAKE_INSTALL_RPATH}")
else()
  set(CMAKE_INSTALL_RPATH "${Geant4_LIBRARY_DIR};${CMAKE_INSTALL_PREFIX}/lib;${ROOT_LIBRARY_DIR}")
  #message(STATUS "rpath = ${CMAKE_INSTALL_RPATH}")
endif()
if (USE_DICOM)
  set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_RPATH};${DCMTK_CMAKE_INSTALL_LIBDIR})
endif()
set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include")
set(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib")
set(SYSCONFIG_INSTALL_DIR etc/bdsim/)

# This module sets up installation:
include(bdsim_install)

# Before any linkages we define library names
set(BDSIM_LIB_NAME "bdsim")
set(BDSIM_LIB_NAME_STATIC "bdsim-static")
set(GMAD_LIB_NAME "gmad")
set(GMAD_LIB_NAME_STATIC "gmad-static")

# Configuration information - do this before collating sources as defines
# some cmake variables for file configuration including git version
add_subdirectory(configuration)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/configuration)

# Source files
string(TIMESTAMP CURRENT_YEAR %Y)
configure_file(${PROJECT_SOURCE_DIR}/bdsim.cc ${CMAKE_BINARY_DIR}/bdsim.cc @ONLY)
configure_file(${PROJECT_SOURCE_DIR}/src/BDSExecOptions.cc ${CMAKE_BINARY_DIR}/src/BDSExecOptions.cc @ONLY)
configure_file(${PROJECT_SOURCE_DIR}/src/BDSIMClass.cc ${CMAKE_BINARY_DIR}/src/BDSIMClass.cc @ONLY)
configure_file(${PROJECT_SOURCE_DIR}/src/BDSVisManager.cc ${CMAKE_BINARY_DIR}/src/BDSVisManager.cc @ONLY)

BDSIM_COLLATE_APPLICATION_SOURCES(bdsim_sources)
if(NOT USE_GDML)
  list(REMOVE_ITEM bdsim_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/BDSGeometryGDML.cc)
endif()
list(APPEND bdsim_sources ${CMAKE_CURRENT_BINARY_DIR}/src/BDSExecOptions.cc)
list(REMOVE_ITEM bdsim_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/BDSExecOptions.cc)
list(APPEND bdsim_sources ${CMAKE_CURRENT_BINARY_DIR}/src/BDSIMClass.cc)
list(REMOVE_ITEM bdsim_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/BDSIMClass.cc)
list(APPEND bdsim_sources ${CMAKE_CURRENT_BINARY_DIR}/src/BDSVisManager.cc)
list(REMOVE_ITEM bdsim_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/BDSVisManager.cc)

# Add Modules
add_subdirectory(modules)
if (USE_DICOM)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/modules/dicom/include)
  file(GLOB dicom_sources "${CMAKE_CURRENT_SOURCE_DIR}/modules/dicom/src/*.cc")
  list(APPEND bdsim_sources ${dicom_sources})
endif()
if (USE_AWAKE)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/modules/AWAKE/include)
  file(GLOB awake_sources "${CMAKE_CURRENT_SOURCE_DIR}/modules/AWAKE/src/*.cc")
  list(APPEND bdsim_sources ${awake_sources})
endif()

if($ENV{VERBOSE})
  message(STATUS "ROOT dictionaries ${root_dicts}")
  message(STATUS "BDSIM sources ${bdsim_sources}")
endif()

# Parser
add_subdirectory(parser)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/parser)

# TBC - don't think is needed...
# include current directory for parser includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# external sources
add_subdirectory(src-external)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/src-external)

# add label to original BDSIM files
set_source_files_properties(${bdsim_sources} ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hh PROPERTIES LABELS "BDSIM")

# BDSIM library and binaries
option( BDSIM_BUILD_STATIC_LIBS "Build static BDSIM library."  OFF )
if (BDSIM_BUILD_STATIC_LIBS)
  message(STATUS "Building BDSIM static libraries in addition to shared ones.")
  add_library(${BDSIM_LIB_NAME_STATIC} STATIC ${root_dicts} ${bdsim_sources})
  generate_export_header(${BDSIM_LIB_NAME_STATIC})
endif()
add_library(${BDSIM_LIB_NAME} SHARED ${root_dicts} ${bdsim_sources})
target_link_libraries(${BDSIM_LIB_NAME} ${CLHEP_LIBRARIES})
target_link_libraries(${BDSIM_LIB_NAME} ${Geant4_LIBRARIES})
if (USE_GDML)
  target_include_directories(${BDSIM_LIB_NAME} SYSTEM PUBLIC ${XercesC_INCLUDE_DIR})
  target_link_libraries(${BDSIM_LIB_NAME} ${XercesC_LIBRARY_RELEASE})
endif()
target_link_libraries(${BDSIM_LIB_NAME} gmad)
generate_export_header(${BDSIM_LIB_NAME})

add_executable(bdsimExec ${CMAKE_BINARY_DIR}/bdsim.cc)

set_target_properties(bdsimExec PROPERTIES OUTPUT_NAME "bdsim" VERSION ${BDSIM_VERSION})
target_link_libraries(bdsimExec ${BDSIM_LIB_NAME} gmad)

if (USE_GZSTREAM)
  #preprocessor flag
  add_definitions("-DUSE_GZSTREAM")
  target_link_libraries(${BDSIM_LIB_NAME} gzstream)
endif()
if (USE_HEPMC3)
  target_link_libraries(${BDSIM_LIB_NAME} ${HEPMC3_LIBRARIES})
endif()
if (USE_BOOST)
  target_include_directories(${BDSIM_LIB_NAME} SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})
  target_link_libraries(${BDSIM_LIB_NAME} ${Boost_LIBRARIES})
endif()
if (USE_DICOM)
  target_link_libraries(${BDSIM_LIB_NAME} DCMTK::DCMTK)
  #target_include_directories(${BDSIM_LIB_NAME} SYSTEM PUBLIC ${DCMTK_INCLUDE_DIRS})
endif()

# link against ROOT
target_link_libraries(${BDSIM_LIB_NAME} ${ROOT_LIBRARIES})

if(${CMAKE_BUILD_TYPE} STREQUAL "DebugCoverage")
    target_link_libraries(${BDSIM_LIB_NAME} gcov)
endif()

# set binary name for testing usage
get_target_property(binaryname bdsimExec OUTPUT_NAME)
set(bdsimBinary ${CMAKE_CURRENT_BINARY_DIR}/${binaryname})

# ROOT Event Library
set(bdsimRootEventSources ${root_dicts} ${root_files}
        include/BDSPolarizationState.hh)

# Add parser options
list(APPEND bdsimRootEventSources ${CMAKE_CURRENT_SOURCE_DIR}/parser/optionsBase.cc)
list(APPEND bdsimRootEventSources ${CMAKE_CURRENT_SOURCE_DIR}/parser/beamBase.cc)
if($ENV{VERBOSE})
  message(STATUS "bdsimRootEventSources ${bdsimRootEventSources}")
endif()

add_library(bdsimRootEvent SHARED ${bdsimRootEventSources})
target_compile_definitions(bdsimRootEvent PRIVATE "-D__ROOTBUILD__")
# link against ROOT
target_link_libraries(bdsimRootEvent ${ROOT_LIBRARIES})
target_include_directories(bdsimRootEvent PUBLIC ${ROOT_INCLUDES} ${Geant4_INCLUDE_DIRS})
if (USE_BOOST)
  target_link_libraries(bdsimRootEvent ${BOOST_LIB_NAME})
  target_include_directories(bdsimRootEvent PUBLIC ${Boost_INCLUDE_DIRS})
endif()
# Add dependency to make sure bdsimRootEvent is done first
# This is done because the ROOT dictionary generation needs to be done separately for each of them
# And even if the dictionaries are the same compiling with multiple threads will result in building them simultaneously
add_dependencies(${BDSIM_LIB_NAME} bdsimRootEvent)

# Install the targets:
set(executables bdsimExec)
if (BDSIM_BUILD_STATIC_LIBS)
  set(libraries ${BDSIM_LIB_NAME} ${BDSIM_LIB_NAME_STATIC} bdsimRootEvent)
else()
  set(libraries ${BDSIM_LIB_NAME} bdsimRootEvent)
endif()
set_target_properties(${executables} ${libraries} PROPERTIES VERSION ${BDSIM_VERSION})
bdsim_install_targets(${executables} ${libraries})
bdsim_install_libs(${root_pcms})
file(GLOB headers ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hh)
bdsim_install_headers(${headers} ${root_dicts_headers})

## Add subdirectories
# Visualisation directory
add_subdirectory(vis)

# For testing
add_subdirectory(examples)
# copy examples to build directory - macro defined in bdsim_macros.cmake
if (GIT_FOUND)
  copy_examples()
else()
  copy_examples_no_git()
endif()

# optional test programs
option(BDSIM_BUILD_TEST_PROGRAMS "Build test executables" OFF)
  
# Include directory for tests
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
if (BDSIM_BUILD_TEST_PROGRAMS)
  add_subdirectory(test)
endif()

# Include directory for analysis
add_subdirectory(analysis)

# Include directory for comparator
add_subdirectory(comparator)

# Include directory for conversion utilities
add_subdirectory(convert)

# Include directory for interpolator
add_subdirectory(interpolator)

# Manual
add_subdirectory(manual)

# Mark some variables as advanced to clean up GUI
if(APPLE)
  mark_as_advanced(CMAKE_OSX_ARCHITECTURES)
  mark_as_advanced(CMAKE_OSX_DEPLOYMENT_TARGET)
  mark_as_advanced(CMAKE_OSX_SYSROOT)
endif()



# CMake packaging so others can use BDSIM in their cmake applications.


## in development
include(CPack)

option(BUILD_MACOSX_APP "Build Mac OSX application" OFF)
mark_as_advanced(BUILD_MACOSX_APP)

if (BUILD_MACOSX_APP AND APPLE)
    # building a macos executable making a bundle of
    add_executable(bdsimapp MACOSX_BUNDLE ${CMAKE_BINARY_DIR}/bdsim.cc)
    set_target_properties(bdsimapp PROPERTIES OUTPUT_NAME "bdsim" VERSION ${BDSIM_VERSION})
    target_link_libraries(bdsimapp ${BDSIM_LIB_NAME} gmad)
    bdsim_install_bundle(bdsim)
endif()

# package source
# uses git but only for developers
add_custom_target(dist
    COMMAND git archive --prefix=${ARCHIVE_NAME}/ HEAD
        | bzip2 > ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.bz2
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# copy dependent libraries in deps directory
add_custom_target(copy-deps
  COMMAND ${CMAKE_SOURCE_DIR}/cmake/copyldd.sh bdsim ${CMAKE_BINARY_DIR}/deps
)
## end in development

# we use "bdsimcmake" for install cmake stuff as it's prepared in the build
# directory and this would clash with the main library name. the ConfigPackageLocation
# is different though so it's installed in a sensible place.
export(EXPORT BDSIMTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/bdsimcmake/BDSIMTargets.cmake"
)

include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/BDSIMConfig.cmake.in
			      ${CMAKE_CURRENT_BINARY_DIR}/bdsimcmake/BDSIMConfig.cmake
			      INSTALL_DESTINATION ${LIB_INSTALL_DIR}/bdsimcmake/cmake
			      PATH_VARS INCLUDE_INSTALL_DIR BIN_INSTALL_DIR SYSCONFIG_INSTALL_DIR)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/bdsimcmake/BDSIMConfigVersion.cmake
  VERSION ${BDSIM_VERSION}
  COMPATIBILITY AnyNewerVersion
)

set(ConfigPackageLocation lib/cmake/bdsim)
install(EXPORT BDSIMTargets
  FILE
    BDSIMTargets.cmake
  DESTINATION
    ${ConfigPackageLocation}
)
install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/bdsimcmake/BDSIMConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/bdsimcmake/BDSIMConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)
