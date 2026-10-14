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
4. Each functio has two call to `<set_gpio>`. The instructions executed during benchmarks are instructions between the `<set_gpio>` calls.

## Software floating point library
The ESP32-H2-MINI-1-N4 doesn't have an FPU. Floating point operations must be emulated in hardware. GCC provides a [low-level runtime library](https://gcc.gnu.org/onlinedocs/gccint/Libgcc.html), `libgcc.a`, and generates call to the routines from this library automatically when compiling programs. `libgcc.a` provides the software floating point library (soft-float), allowing to emulate [arithmetic, conversion, and comparison operations](https://gcc.gnu.org/onlinedocs/gccint/Soft-float-library-routines.html) using floating points.

In the `<benchmark_mat_mul_float>` function, benchmark instructions (between `<set_gpio>` calls) includes two calls to the soft-float library: `<__mulsf3>` and `<__addsf3>`. The former is use to compute the product of two matrix cells of the input matrix. The latter is used to add the result to a cell of the output matrix. 

## Disassembly of libgcc.a
The `libgcc.a` library should be located where the ESP32 cross-compiler is installed `~/.espressif/tools/riscv32-esp-elf/esp-<version>_<date>/riscv32-esp-elf/lib/gcc/riscv32-esp-elf/16.1.0/libgcc.a`.

1. Disassemble the object file

    `riscv32-esp-elf-objdump -drwC ~/.espressif/tools/riscv32-esp-elf/esp-<version>_<date>/riscv32-esp-elf/lib/gcc/riscv32-esp-elf/16.1.0/libgcc.a > libgcc.a.dis`
    
    Add the `--visualize-jumps=color` option to visualise the jumps better.

    Locate the `<__mulsf3>` and `<__addsf3>` functions.

The `results/esp32/analysis/disassemblies/__mulsf3.dis` file contains the compact list of instructions contained in `<__mulsf3>`. There are 309 instructions in total, with 9 stores and 9 loads. This includes two calls to `<__clzsi2>` (also in `<libgcc.a.dis>`).

The `results/esp32/analysis/disassemblies/__clzsi2.dis` file contains the compact list of instructions contained in `<__clzsi2>`. There are 19 instructions in total in this file, including 1 load. 

 In total, there are 309+19*2 = 347 instructions in the `<__mulsf3.dis>` file, including 20 memory operations. This means that around 5.8% of instructions are operations requiring access to the memory. 
