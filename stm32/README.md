# Build and run experiments
`mkdir -p build && cd build`

`cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/gcc-arm-none-eabi.cmake ..`

`make -j4`

