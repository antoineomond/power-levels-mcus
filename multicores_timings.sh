set -e

# parameters
EXPE_NAME=pico2_multicores
NB_CONFS=1
NB_ITERS=1
NB_BENCHMARKS=2
DEADLINE_ITERATION=3600
MAX_CURRENT_LOWER=0.004
MAX_CURRENT_UPPER=0.004
OFFSET=0
ITER_OFFSET=0
NUM_CONF_SWITCH=6 # At the END of this conf (i.e., last benchmark using this conf finished), switch calibration from lower to upper

scp measurements_multicores_timings.py raspberrypi:/root/dw_ina/measurements_multicores_timings.py

if [[ "$1" != "nomon" ]]; then
	(ssh raspberrypi 'kill $(pgrep -f measurements_multicores_timings.py)' || true)
	ssh raspberrypi "cd /root/dw_ina && source venv/bin/activate && nohup python -u measurements_multicores_timings.py $NB_CONFS $NB_ITERS $OFFSET $ITER_OFFSET 0 $NB_BENCHMARKS $DEADLINE_ITERATION $MAX_CURRENT_LOWER $MAX_CURRENT_UPPER $NUM_CONF_SWITCH > measurements_multicores.log 2>&1 < /dev/null" < /dev/null &
	PID=$!
fi

if [[ "$1" != "nomon" ]]; then
	tail --pid=$PID -f /dev/null
	
	# retrieve results and create plot
	mkdir -p "results/$EXPE_NAME"
	scp raspberrypi:/root/dw_ina/results.csv "results/$EXPE_NAME/results.csv"
fi

