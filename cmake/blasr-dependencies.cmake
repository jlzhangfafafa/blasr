
# External libraries

# Boost
if(NOT Boost_INCLUDE_DIRS)
    find_package(Boost REQUIRED)
endif()

# Threads
if (NOT Threads)
    find_package(Threads REQUIRED)
endif()

# ZLIB
if (NOT ZLIB_INCLUDE_DIRS OR NOT ZLIB_LIBRARIES)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ZLIB zlib)
else()
    set(ZLIB_LDFLAGS ${ZLIB_LIBRARIES})
endif()

# HDF5
if (HDF5_INCLUDE_DIR)
    message(FATAL_ERROR "Please specify HDF5_INCLUDE_DIRS not HDF5_INCLUDE_DIR!")
endif()
if (HDF5_LIBRARY)
    message(FATAL_ERROR "Please specify HDF5_LIBRARIES not HDF5_LIBRARY!")
endif()

if (NOT HDF5_INCLUDE_DIRS OR NOT HDF5_LIBRARIES)
    find_package(HDF5 REQUIRED)
    if (HDF5_ROOT)
        SET_PROPERTY(GLOBAL PROPERTY HDF5_LINKER_FLAG_GLOBAL "-L${HDF5_ROOT}/lib")
    endif()
else()
    SET_PROPERTY(GLOBAL PROPERTY HDF5_LINKER_FLAG_GLOBAL "-L${HDF5_LIBRARIES}")
    find_library(HDF5_LIBRARIES_     hdf5    ${HDF5_LIBRARIES} NO_CMAKE_SYSTEM_PATH)
    find_library(HDF5_CPP_LIBRARIES hdf5_cpp ${HDF5_LIBRARIES} NO_CMAKE_SYSTEM_PATH)
endif()

# htslib
if(NOT HTSLIB_INCLUDE_DIRS OR NOT HTSLIB_LIBRARIES)
    find_package(HTSlib)
    set(hts_INCLUDE_DIRS ${HTSlib_INCLUDE_DIRS})
    set(hts_LIBRARIES    ${HTSlib_LIBRARIES})
else()
    set(hts_INCLUDE_DIRS ${HTSLIB_INCLUDE_DIRS})
    set(hts_LIBRARIES    ${HTSLIB_LIBRARIES})
endif()

# pbbam
if (NOT PacBioBAM_INCLUDE_DIRS OR
    NOT PacBioBAM_LIBRARIES)
    set(PacBioBAM_build_docs    OFF CACHE INTERNAL "" FORCE)
    set(PacBioBAM_build_tests   OFF CACHE INTERNAL "" FORCE)
    set(PacBioBAM_build_tools   OFF CACHE INTERNAL "" FORCE)
    add_subdirectory(${BLASR_RootDir}/pbbam external/pbbam/build)
endif()
