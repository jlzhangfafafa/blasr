
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
    find_package(ZLIB REQUIRED)
endif()

# pbbam
if (NOT PacBioBAM_INCLUDE_DIRS OR
    NOT PacBioBAM_LIBRARIES)
    set(PacBioBAM_build_docs    OFF CACHE INTERNAL "" FORCE)
    set(PacBioBAM_build_tests   OFF CACHE INTERNAL "" FORCE)
    set(PacBioBAM_build_tools   OFF CACHE INTERNAL "" FORCE)
    add_subdirectory(${BLASR_RootDir}/pbbam external/pbbam/build)
endif()