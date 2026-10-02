if [ $1 == "pico2" ]; then
	if [ -z $SERIAL_NUMBER ]; then
		echo "The SERIAL_NUMBER must be set. You can get it from the output of 'sudo lsusb -v | grep -A12 -E \"Pico|RP2350\"'."
		exit 1
	fi
	if [ command -v picotool &> /dev/null ]; then
		picotool load --ser $SERIAL_NUMBER -f -x pico2/build/experiments/benchmarks.uf2
	else
		echo "picotool must be installed to flash via USB."
		exit 1
	fi
elif [ $1 == "stm32" ]; then
	STM32_Programmer_CLI -c port=usb1 -w stm32/build/expes-power.bin 0x08000000 -v
elif [ $1 == "esp32" ]; then
	cd esp32/esp-idf && . ./export.sh && cd -
	cd esp32/cpufreq && idf.py flash && cd -
else
	echo "Unknown target: $1"
fi
