## Compile BLASR binary using cmake

Make sure that you are using cmake >=3.7 and
always start from an empty build subdirectory!

    git clone git://github.com/PacificBiosciences/blasr.git && cd blasr
    git submodule update --init --remote
    mkdir build && cd build
    cmake .. && make

Where is the binary?

    blasr/build/blasr

Want to build MUCH faster? Install ninja and execute

    cmake -GNinja .. && ninja

Is your HDF5 in a custom location?

    cmake -GNinja -DHDF5_ROOT=/your/location/hdf-1.8.16 ..

Are HDF$ libraries and include folders in different locations?

    cmake -GNinja -DHDF5_LIBRARIES=/your/location/hdf-1.8.16/lib
                  -DHDF5_INCLUDE_DIRS=/other/location/hdf-1.8.16/include ..

Prefer a custom libz implementation?

    cmake -GNinja -DZLIB_INCLUDE_DIRS=/your/location/zlib/include \
                  -DZLIB_LIBRARIES=/your/location/zlib/libz.so ..

Are you on MacOS? Install homebrew and install following dependencies and
use the default instructions above:

    brew install ninja hdf5 cmake