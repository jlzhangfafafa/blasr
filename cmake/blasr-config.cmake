# Config generation

find_git_sha1(BLASR_GIT_SHA1)

configure_file(
    ${BLASR_RootDir}/BlasrVersion.h.in
    ${CMAKE_BINARY_DIR}/generated/BlasrVersion.h
)
