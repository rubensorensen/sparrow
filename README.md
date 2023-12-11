# Sparrow

No text editors are perfect. So I am making one.

Let's see how long I will bother trying to make this be usable.

## Build status
![Test status](https://github.com/rubensorensen/sparrow/actions/workflows/tests.yml/badge.svg)
![Release status](https://github.com/rubensorensen/sparrow/actions/workflows/release.yml/badge.svg)

## Quick start

### Linux
#### Build

```bash
  git clone https://github.com/rubensorensen/sparrow.git
  cd sparrow
  mkdir build && cd build
  
  #configure build
  cmake ..
  
  # Build and run tests
  make -k -j`nproc` test
  ./test
  
  # Build and run sparrow
  make -k -j`nproc` sparrow
  cd ..
  ./build/sparrow
```

### Windows
Windows support is not a priority at the moment and won't be officially supported in the near future.

### MacOS
MacOS support is not a priority at the moment and won't be officially supported in the near future. Linux build guide may work.

## Contributing
The project is still in its early stages, and I am currently the sole contributor while I'm working on the initial development. In the future I may consider opening up for public contributions.

## Licenses
This project is licensed under the [MIT license](https://opensource.org/license/mit/).
Licenses from third-parties are listed in [LICENSES_THIRD_PARTY](LICENSES_THIRD_PARTY)
