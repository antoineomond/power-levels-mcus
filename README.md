# Power usage levels 
This repository contains the source code to reproduce the experiments for the SIGMETRICS27 submission. 

The source code of the firmware containing the different configurations to experiment and the benchmarks are in the pico2/, esp32/ and stm32/ folders.

A `target_configuration.h` header file is present in each folder to change the configuration of the MCU. 

The results/ folder contains the results for 20 iterations for each MCU and scripts to plot the graphs and tables.

# Launching experiments
The `expe.sh` script copies and launch the `measurements.py` file on a target monitoring node. In our experiments, we use the Raspberry Pi 4B. It is assumed that the setup presented in the paper is done prior to execute the script.

To build the firmware, use the `build.sh` script with the target MCU. For instance, to build the pico2, use `build.sh pico2`.

To flash the firmware, use `flash.sh` in the same manner. For instance, to flash the pico2, use `flash.sh pico2`. It is assumed that the board is connected in USB and ready to receive the firmware.
