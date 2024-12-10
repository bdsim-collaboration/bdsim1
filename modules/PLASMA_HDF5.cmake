# plasma hdf5-specific classes
option( USE_PLASMA_HDF5 "Include HDF5 support."  OFF )
if(USE_PLASMA_HDF5)
    add_definitions("-DUSE_PLASMA_HDF5")
    find_package(HDF5 REQUIRED COMPONENTS CXX)
    message(STATUS "PLASMA_HDF5 module ON")
endif()