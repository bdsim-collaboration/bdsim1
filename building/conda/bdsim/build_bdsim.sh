#!/usr/bin/env bash
set -eux

declare -a CMAKE_PLATFORM_FLAGS
if [[ ${HOST} =~ .*darwin.* ]]; then
  CMAKE_PLATFORM_FLAGS+=(-DCMAKE_OSX_SYSROOT="${CONDA_BUILD_SYSROOT}")
else
  CMAKE_PLATFORM_FLAGS+=(-DCMAKE_TOOLCHAIN_FILE="${RECIPE_DIR}/cross-linux.cmake")
fi


if [[ "${target_platform}" == "osx-64" ]]; then
    export CXXFLAGS="${CXXFLAGS} -D_LIBCPP_DISABLE_AVAILABILITY"
fi
if [[ ${DEBUG_C:-no} == yes ]]; then
  CMAKE_BUILD_TYPE=Debug
else
  CMAKE_BUILD_TYPE=Release
fi

# cmake_minimum_required(VERSION 3.1) is required to compile
# the examples to compile using conda's compiler packages
sed -r -i -E 's#cmake_minimum_required\(VERSION [0-9]\.[0-9]#cmake_minimum_required(VERSION 3.1#gI' \
  $(find examples -name 'CMakeLists.txt')

mkdir bdsim-build
cd bdsim-build

cmake                                                          \
      -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"                 \
      -DCMAKE_INSTALL_PREFIX="${PREFIX}"                       \
      -DUSE_HEPMC3=OFF                                         \
      -DUSE_AWAKE=ON                                           \
      -DUSE_GEANT4_EMD_ID=ON                                   \
      -DCMAKE_CXX_STANDARD=17                                 \
      "${CMAKE_PLATFORM_FLAGS[@]}"                             \
      "${SRC_DIR}"

make "-j${CPU_COUNT}"
make install "-j${CPU_COUNT}"

# Print the contents of geant4.sh in case of problems
echo "Contents of ${PREFIX}/bin/bdsim.sh is"
cat "${PREFIX}/bin/bdsim.sh"

# Remove the bdsim.(c)sh scripts and replace with a dummy version
for suffix in sh; do
  rm "${PREFIX}/bin/bdsim.${suffix}"
  cp "${RECIPE_DIR}/bdsim-setup" "${PREFIX}/bin/bdsim.${suffix}"
  chmod +x "${PREFIX}/bin/bdsim.${suffix}"
done
