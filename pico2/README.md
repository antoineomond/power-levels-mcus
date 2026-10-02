# Raspberry Pi Pico 2 experiments 

This repository contains the source code of different configurations of the Raspberry Pi Pico 2 along with their current consumption and time performance measurements. 

The current consumption were measured using an [INA228](https://www.ti.com/lit/ds/symlink/ina228.pdf?ts=1745147460272). A Raspberry Pi 4B was used to collect the measurements from the INA and to measure time performances from the Pico 2 using a single GPIO. 

The source code for current and performance measurements is present in [measurements.py](current_measurement.py). 

## Launching the monitoring script on the Rpi4
The following has been done on a Raspberry Pi 4b (Raspberry Pi OS lite, Debian bookworm). Some changes may need to be done in case another board/OS is used.

The following packages must be installed:

`
sudo apt update
sudo apt -y install git i2c-tools virtualenv
`

The communication with the INA228 is done in i2c:

`
sudo raspi-config nonint do_i2c 0
`

Adafruit provides a python library to configure and extract measurements from the INA228. The following python packages are required to run the measurement script:

`
adafruit-circuitpython-ina228
pigpio
`

Then, copy the [measurements.py](current_measurement.py) script on the board, then launch it.

## Building the project
Run the following to build the project (assuming pico-sdk and pico-extras have been cloned and are at the same folder level as this repository):

`
export PICO_SDK_PATH=../../pico-sdk
export PICO_EXTRAS_PATH=../../pico-extras
mkdir -p build
cd build
cmake -DPICO_BOARD=pico2 ..
make -j4
cd ..
`

After building the project, the compiled firmwares for the different experiments are present in the build folder in different formats, including uf2 and elf.

## Re-building after changes
Whenever changes are made, re-build the project using the following:

`
cd build
make -j4
cd ..
`
## Deploying the firmware (using SWD debug probe)
[openocd](https://openocd.org/pages/getting-openocd.html)  can be used to deploy the desired compiled firmware in elf format:

`
sudo openocd -s tcl -f interface/cmsis-dap.cfg -f target/rp2350.cfg -c "adapter speed 5000" -c "program build/path/to/experiment.elf verify reset exit"
`
## Deploying the firmware (using USB)
[picotool](https://github.com/raspberrypi/picotool) can be used to deploy the desired compiled firmware in uf2 format. It is possible to use the serial number of a connected Pico 2 to uniquely identify a connected board and easily deploy the firmware on it:

`
picotool load --ser <serial_number> -f -x build/path/to/experiment.uf2
`

The following command can be used to find the serial number (assuming the board is USB-connected and detected):

`
sudo lsusb -v | grep -A12 -E "Pico|RP2350"
`
# Validation/context run
This run is used to:
- ensure the configuration gives valid results for all 4 benchmarks
- give the measured reference clock frequency 

The values are sent serially from the board using uart. [minicom](https://www.man7.org/linux/man-pages/man1/minicom.1.html) can be used to read data and write them on a file. Assuming the SWD debug probe is connected to the computer, the following command can be used:

`
sudo minicom -D /dev/ttyACM0 -C ./validation.csv
`

Then, append the lines from `validation.csv` to the `results.csv` obtained from `measurements.py`.
