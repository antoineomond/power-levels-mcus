if [ $# -ne 1 ]; then
		echo "Requires exactly one argument"
    exit 1
fi

if [ $1 == "pico2" ]; then
	mkdir -p pico2/build
	cd pico2/build
	cmake -DPICO_BOARD=pico2 ..
	make -j4
	cd ..
fi
if [ $1 == "stm32" ]; then
	mkdir -p stm32/build
	cd stm32/build 
	cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/gcc-arm-none-eabi.cmake ..
	make -j4
	arm-none-eabi-objcopy -O binary expes-power.elf expes-power.bin
fi
if [ $1 == "esp32" ]; then
	cd $ESP_IDF_PATH && . ./export.sh && cd -
	cd esp32 && idf.py build && cd -
fi
