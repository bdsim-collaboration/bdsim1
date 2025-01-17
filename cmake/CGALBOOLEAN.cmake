# awake experiment-specific classes
option(USE_GEANT4CGALBOOLEAN "Use GEANT4CGALBOOLEAN Module" OFF)
if (USE_GEANT4CGALBOOLEAN)
    message(STATUS "GEANT4CGALBOOLEAN support ON")
    add_definitions("-DUSE_GEANT4CGALBOOLEAN")
    find_package(geant4cgalboolean REQUIRED HINTS GEANT4CGALBOOLEAN_DIR)
    find_package(Eigen3 REQUIRED NO_MODULE)
    find_package(CGAL REQUIRED)
endif()