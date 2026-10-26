# Comparing asm instructions between integer and floating point matrix multiplications 
## Compile cpufreq.c
1. Follow instructions of `esp32/README.md` to install and activate the esp-idf environment
2. Go to `esp32/` and build the project

    `idf.py build`

    The compiled files should be in `esp32/build`

## Identify the asm instructions 
1. With the esp-idf environment activated, the espressif toolchain and binaries should be accessible from the PATH (installed in `~/.espressif/tools/riscv32-esp-elf/esp-<version>_<date>/riscv32-esp-elf/bin`) 
2. Go to `esp32/build` then disassemble the .elf file

    `riscv32-esp-elf-objdump -drwC cpufreq.elf > cpufreq.dis`

    Add the `--visualize-jumps=color` option to visualise the loops better.

3. In the disassembly, locate the functions `<benchmark_mat_mul>` and `<benchmark_mat_mul_float>`
4. Each function has two call to `<set_gpio>`. The instructions executed during benchmarks are instructions between the `<set_gpio>` calls.

## Software floating point library
The ESP32-H2-MINI-1-N4 doesn't have an FPU. Floating point operations must be emulated in hardware. Floating point can be emulated using the software floating point library. GCC provides a [low-level runtime library](https://gcc.gnu.org/onlinedocs/gccint/Libgcc.html), `libgcc.a`, and generates call to the routines from this library automatically when compiling programs. `libgcc.a` provides the software floating point library (soft-float), allowing to emulate [arithmetic, conversion, and comparison operations](https://gcc.gnu.org/onlinedocs/gccint/Soft-float-library-routines.html) using floating points. However, the ESP32-H2-MINI-1-N4 doesn't use this library, but has these functions integrated directly in ROM.

In the `<benchmark_mat_mul_float>` function, benchmark instructions (between `<set_gpio>` calls) includes two calls to the soft-float library: `<__mulsf3>` and `<__addsf3>`. The former is use to compute the product of two matrix cells of the input matrix. The latter is used to add the result to a cell of the output matrix. 

Running `riscv32-esp-elf-nm build/mat_mul_analysis.elf | grep -w __mulsf` gives `40000940 A __mulsf3`. The `0x40000940` address belongs to the ROM section of the memory (cf **Memory Address Mapping** table of the datasheet).

## Disassembly of libgcc.a
The ROM ELF files should be installed alongside esp-idf. For the ESP32H2, there is a single elf file: `~/.espressif/tools/esp-rom-elfs/20260528/esp32h2_rev0_rom.elf`. 

Disassemble the file:

    `riscv32-esp-elf-objdump -d ~/.espressif/tools/esp-rom-elfs/<date>/esp32h2_rev0_rom.elf > rom_dump.dis`

Locate the `<__mulsf3>` and `<__addsf3>` functions.
