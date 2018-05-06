#!/usr/bin/env bash
set -vex

#########
# BUILD #
#########

# in order to make shared libraries consumable
# by conda and other package managers
default_LDFLAGS="-static-libstdc++ -static-libgcc"

# configure
# '--wrap-mode nofallback' prevents meson from downloading
# stuff from the internet or using subprojects.
CPPFLAGS="${HDF5_CFLAGS}" \
LDFLAGS="${default_LDFLAGS} ${HDF5_LIBS}" \
  meson \
    --wrap-mode nofallback \
    -Dtests="${ENABLED_TESTS:-false}" \
    "${CURRENT_BUILD_DIR:-build}" .

# build
ninja -C "${CURRENT_BUILD_DIR:-build}" -v
