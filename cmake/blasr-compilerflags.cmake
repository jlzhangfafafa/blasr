
include(CheckCXXCompilerFlag)

# shared CXX flags for all source code & tests
set(BLASR_FLAGS "-pedantic -Wno-long-long -Wall -Wextra -Wno-return-type -Wno-overloaded-virtual -Wno-unused-parameter -Wno-div-by-zero -MMD -MP -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free -fno-omit-frame-pointer -Wno-unused-variable")
set(CMAKE_CXX_STANDARD          14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

# gperftools support
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND APPLE)
    set(BLASR_LINKER_FLAGS "${BLASR_LINKER_FLAGS} -Wl,-no_pie")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug" AND APPLE)

# static linking
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(BLASR_LINKER_FLAGS "${BLASR_LINKER_FLAGS} -static-libstdc++")
ENDIF()

# NOTE: quash clang warnings w/ Boost
check_cxx_compiler_flag("-Wno-unused-local-typedefs" HAS_NO_UNUSED_LOCAL_TYPEDEFS)
if(HAS_NO_UNUSED_LOCAL_TYPEDEFS)
    set(BLASR_FLAGS "${BLASR_FLAGS} -Wno-unused-local-typedefs")
endif()

# Cannot use this until pbbam complies
# if (CMAKE_COMPILER_IS_GNUCXX)
#     set(BLASR_FLAGS "${BLASR_FLAGS} -Werror=suggest-override")
# endif()

# Extra testing that will lead to longer compilation times!
if (SANITIZE)
    # AddressSanitizer is a fast memory error detector
    set(BLASR_SANITY_FLAGS "${BLASR_SANITY_FLAGS} -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls")

    # Clang Thread Safety Analysis is a C++ language extension which warns about
    # potential race conditions in code.
    set(BLASR_SANITY_FLAGS "${BLASR_SANITY_FLAGS} -Wthread-safety")

    # ThreadSanitizer is a tool that detects data races
    set(BLASR_SANITY_FLAGS "${BLASR_SANITY_FLAGS} -fsanitize=thread")

    # MemorySanitizer is a detector of uninitialized reads.
    set(BLASR_SANITY_FLAGS "${BLASR_SANITY_FLAGS} -fsanitize=memory")

    # UndefinedBehaviorSanitizer is a fast undefined behavior detector.
    set(BLASR_SANITY_FLAGS "${BLASR_SANITY_FLAGS} -fsanitize=undefined")
endif()

set(BLASR_FLAGS "${BLASR_FLAGS} -DUSE_PBBAM")
set(BLASR_FLAGS "${BLASR_FLAGS} -DCMAKE_BUILD")

if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(BLASR_LINKER_FLAGS "${BLASR_LINKER_FLAGS} -lrt")
endif()

# shared CXX flags for src & tests
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BLASR_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${BLASR_COV_FLAGS} ${BLASR_SANITY_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${BLASR_LINKER_FLAGS}")
