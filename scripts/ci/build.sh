#!/usr/bin/env bash
set -vex

#########
# BUILD #
#########

# configure
# '--wrap-mode nofallback' prevents meson from downloading
# stuff from the internet or using subprojects.
meson \
  --wrap-mode nofallback \
  --prefix "${PREFIX_ARG:-/usr/local}" \
  -Dtests="${ENABLED_TESTS:-false}" \
  "${CURRENT_BUILD_DIR:-build}" .

# build
ninja -C "${CURRENT_BUILD_DIR:-build}" -v
