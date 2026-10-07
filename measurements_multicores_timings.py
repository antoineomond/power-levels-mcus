# Must be running from the monitoring device 
import sys
import time
import board
import adafruit_ina228
import pigpio
from datetime import datetime
from statistics import StatisticsError, mean, stdev, median

EXPE_PIN = 27
EXPE_PIN_MULTICORES = 22
expe_num = 0
done = 0
started = 0
init = 0 # Necessary when expe_pin is initially at 1 and reset to 0
init_multicores = 0
s = 0
end_of_expe = False
start_time = time.time()
timing_samples = []

if(len(sys.argv) < 11):
    print("Missing args")
    exit()
nb_confs = int(sys.argv[1])
nb_iter = int(sys.argv[2])
offset = int(sys.argv[3]) # If there was other expes done before, just offset to correctly assign the new expes
iter_offset = int(sys.argv[4]) # If there was other expes done before, just offset to correctly assign the new expes
live = int(sys.argv[5])
NB_BENCHMARKS = int(sys.argv[6])
DEADLINE_ITERATION = int(sys.argv[7])
MAX_CURRENT_LOWER = float(sys.argv[8])
MAX_CURRENT_UPPER = float(sys.argv[9])
NUM_CONF_SWITCH = float(sys.argv[10])
nb_expes = nb_confs * NB_BENCHMARKS

state_expe_pin = 0
state_expe_pin_multicores = 0
compute_firstcore_done = 0

def next_expe(user_gpio, level, tick):
    global end_of_expe
    if(end_of_expe):
        return
    
    global init
    global expe_num
    global started
    global done
    global s
    global start_time
    global state_expe_pin
    global compute_firstcore_done
    if(level == 1):
        state_expe_pin = 1
        if(init == 0):
            start_time = time.time() 
            ina228.reset_accumulators()
        init = 1
        started = 1
        s = tick
    if(init == 1 and level == 0):
        state_expe_pin = 0
        compute_firstcore_done = 1
        t = tick-s
        # tick is 32 bit timer that wraps around from 4294967295 to 0 if overflow, the following condition handles that case 
        if(tick < s):
            t += 4294967295
        timing_samples.append(t)
        if state_expe_pin_multicores == 0 or expe_num == 1:
            started = 0
            expe_num += 1
            if(expe_num%NB_BENCHMARKS == 0):
                init = 0
            if(expe_num >= nb_expes*nb_iter):
                 done = 1
            print(expe_num)
            if (expe_num/NB_BENCHMARKS)%nb_confs == NUM_CONF_SWITCH:
                ina228.set_calibration(7.5, MAX_CURRENT_UPPER)
                print(f"MAX_CURRENT={MAX_CURRENT_UPPER}A")
            if (expe_num%nb_expes == 0):
                ina228.set_calibration(7.5, MAX_CURRENT_LOWER)
                print(f"MAX_CURRENT={MAX_CURRENT_LOWER}A")

def next_expe_multicores(user_gpio, level, tick):
    global init_multicores
    global state_expe_pin_multicores
    global expe_num
    global init
    global done
    if(level == 1):
        state_expe_pin_multicores = 1
        init_multicores = 1
    if(init_multicores == 1 and level == 0):
        state_expe_pin_multicores = 0
        if compute_firstcore_done == 1:
            expe_num += 1
            if(expe_num%NB_BENCHMARKS == 0):
                init = 0
            if(expe_num >= nb_expes*nb_iter):
                 done = 1
            print(expe_num)
            if (expe_num/NB_BENCHMARKS)%nb_confs == NUM_CONF_SWITCH:
                ina228.set_calibration(7.5, MAX_CURRENT_UPPER)
                print(f"MAX_CURRENT={MAX_CURRENT_UPPER}A")
            if (expe_num%nb_expes == 0):
                ina228.set_calibration(7.5, MAX_CURRENT_LOWER)
                print(f"MAX_CURRENT={MAX_CURRENT_LOWER}A")

i2c = board.I2C()
ina228 = adafruit_ina228.INA228(i2c)

print("INA calibration")

# The shunt resistor is 1 Ohm
ina228.set_calibration(7.5, MAX_CURRENT_LOWER)
print(f"MAX_CURRENT={MAX_CURRENT_LOWER}A")

# Configuration of the INA: trade-off longer conversion time for better accuracy
ina228.mode = adafruit_ina228.Mode.CONTINUOUS
ina228.bus_voltage_conv_time = adafruit_ina228.ConversionTime.TIME_1052_US
ina228.shunt_voltage_conv_time = adafruit_ina228.ConversionTime.TIME_1052_US
ina228.temp_conv_time = adafruit_ina228.ConversionTime.TIME_1052_US
ina228.averaging_count = adafruit_ina228.AveragingCount.COUNT_1
ina228.adc_range = 0
ina228.shunt_tempco = 25
ina228.temp_comp = 1

# Start measurements
pi = pigpio.pi()
if not pi.connected:
    print("pigpiod need to run in background")
    exit(0)
pi.callback(EXPE_PIN, pigpio.EITHER_EDGE, next_expe)
pi.callback(EXPE_PIN_MULTICORES, pigpio.EITHER_EDGE, next_expe_multicores)
current_samples = [[] for _ in range(nb_expes*nb_iter)]
live_samples = [[] for _ in range(nb_expes*nb_iter)]
start_date = datetime.now()
print(f"Sampling starts at {start_date}")
while not done:
    if started and expe_num < nb_expes*nb_iter:  # only measure current when expe starts 
        try:
            current_val = ina228.current*1000
            current_samples[expe_num].append((current_val, ina228.power*1000, ina228.energy*1000, ina228.shunt_voltage, ina228.bus_voltage, round(time.time()-start_time, 3), state_expe_pin, state_expe_pin_multicores, compute_firstcore_done))
        # Handles race condition on expe_num
        except IndexError:
            pass

    #time.sleep(0.004) # 250Hz sampling

# Write results
print("Sampling ends")
result_file = "results.csv"
with open(result_file, "w") as f:
    f.write("iteration_num,conf_num,expe_num,bench_num,current_sample,power_sample,energy_sample,shunt_voltage_sample,bus_voltage_sample,current_timestamp,state_expe_pin,state_expe_pin_multicores,compute_firstcore_done,timing_sample\n")
    for expe_num, samples in enumerate(current_samples):
        for current_sample in samples:
            current, power, energy, shunt_voltage, bus_voltage, timestamp, state_pin, state_pin_multi, compute_firstcore_done = current_sample
            f.write(f"{expe_num//nb_expes+iter_offset},{(expe_num%nb_expes)//NB_BENCHMARKS+offset},{expe_num%nb_expes+offset*NB_BENCHMARKS},{expe_num%NB_BENCHMARKS},{current},{power},{energy},{shunt_voltage},{bus_voltage},{timestamp},{state_pin},{state_pin_multi},{compute_firstcore_done},\n")
    for expe_num, timing_sample in enumerate(timing_samples):
        f.write(f"{expe_num//nb_expes+iter_offset},{(expe_num%nb_expes)//NB_BENCHMARKS+offset},{expe_num%nb_expes+offset*NB_BENCHMARKS},{expe_num%NB_BENCHMARKS},,,,,,,{timing_sample}\n")

print(f"Done at {datetime.now()} in {datetime.now() - start_date}s")
