# Installation

## Easy install using cmake and ninja

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

## Not-so-easy install using make

### See a step by step Blasr installation example on Blasr wiki page

        https://github.com/PacificBiosciences/blasr/wiki/Step-by-step-blasr-installation-example

### Download source code

* To pull this project from git hub to your local system:

        git clone git://github.com/PacificBiosciences/blasr.git blasr

* To sync your code with the latest git code base:

        git pull --rebase origin master && git submodule update --init

* To update the submodule:

        make update-submodule

### Requirements

* To configure:

        ./configure.py --shared --sub --no-pbbam

* or with HDF5 directories (and note that `HDF5_LIB` is a *directory* here):

        ./configure.py --shared --sub --no-pbbam HDF5_INCLUDE=... HDF5_LIB=...

To build BLASR, you must have hdf 1.8.12 or above installed and
  configured with c++ support (you should have the library
  libhdf5_cpp.a).  If you are intalling the entire PacBio secondary
  analysis software suite, appropriate hdf libraries are already
  distributed and no configuration is necessary.  Otherwise, it is
  necessary to point two environment variables:

  + **HDF5_INCLUDE**, which points to directory of the HDF5 headers
  (e.g., hdf5.h)

  + **HDF5_LIB**, which points to the HDF5 library directory (e.g., hdf5*.a,
  and hdf5*.so)

  You may pass arguments to `configure.py` as above, or you may export them from command line:

        export HDF5_INC=path_to_your_hdf5_include && export HDF5_LIB=path_to_your_hdf5_lib

* To configure submodule:

    make configure-submodule

### Build

* To make the 'libcpp' libraries:

        make build-submodule

* To make 'blasr' only:

        make blasr

* To compile all tools, including blasr, pls2fasta, loadPulses, sawriter:

        make

  * Frequently used executables will be under utils.

* To test (with **cram** installed):

        #make cramtests
        make cramfast
        ## Currently:
        ## Ran 22 tests, 0 skipped, 4 failed.

* To clean all compiled tools and lib:

        make cleanall

* To clean compiled tools without cleaning lib:

        make clean

        make blasr
        ./blasr

### CXXFLAGS

* For optimized builds:

    ./configure.py CXXFLAGS=-O3 ...

* For debug builds:

    ./configure.py CXXFLAGS=-g ...

## Other issues
### Static binaries
If you want static binaries, drop `--shared` when you run configure.py. In that case, you
might need to pass `-lsz` to make, if you built HDF5 with szlib support (`--with-szlib`).

        ./configure.py --with-szlib ...

See [our issues](https://github.com/PacificBiosciences/blasr/issues/113#issuecomment-143981496).

If you have macosx (Darwin), then you almost certainly want non-static binaries (--shared).

### blasr_libcpp
If you have built and installed blasr_libcpp elsewhere, then drop `--sub` and do not run `make build-submodule`.
