#!/usr/bin/env bash
set -e

echo "#########"
echo "# BUILD #"
echo "#########"

# in order to make shared libraries consumable
# by conda and other package managers
default_LDFLAGS="-static-libstdc++ -static-libgcc"

CURRENT_BUILD_DIR="build"

# 1. configure
# '--wrap-mode nofallback' prevents meson from downloading
# stuff from the internet or using subprojects.
echo "## Configuring source (${CURRENT_BUILD_DIR})"
CPPFLAGS="${HDF5_CFLAGS}" \
LDFLAGS="${default_LDFLAGS} ${HDF5_LIBS}" \
  meson \
    --wrap-mode nofallback \
    -Dtests="${ENABLED_TESTS:-false}" \
    "${CURRENT_BUILD_DIR}" .

# 2. build
echo "## Building source (${CURRENT_BUILD_DIR})"
ninja -C "${CURRENT_BUILD_DIR}" -v

# 3. tests
echo "## Tests (${CURRENT_BUILD_DIR})"
ninja -C "${CURRENT_BUILD_DIR}" -v test
