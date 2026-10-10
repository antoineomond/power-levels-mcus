# Setup
Install the development framework
- `git clone --single-branch --recursive https://github.com/espressif/esp-idf.git`
- `cd esp-idf`
- `./install.sh esp32`

Enable the environment
- `. ./export.sh`

From the esp32 directory:
- `idf.py set-target esp32h2` (precise the board model)
- `idf.py build` (build the firmware)

Flash the firmware. The board must be connected in USB:
- `idf.py flash`
