set -e

# parameters
EXPE_NAME=pico2
NB_CONFS=14
NB_ITERS=20
NB_BENCHMARKS=5
DEADLINE_ITERATION=3600
MAX_CURRENT_LOWER=0.004
MAX_CURRENT_UPPER=0.022
OFFSET=0
ITER_OFFSET=0
NUM_CONF_SWITCH=8 # At the END of this conf (i.e., last benchmark using this conf finished), switch calibration from lower to upper

scp measurements.py raspberrypi:/root/dw_ina/measurements.py

if [[ "$1" != "nomon" ]]; then
	(ssh raspberrypi 'kill $(pgrep -f measurements.py)' || true)
	ssh raspberrypi "cd /root/dw_ina && source venv/bin/activate && nohup python -u measurements.py $NB_CONFS $NB_ITERS $OFFSET $ITER_OFFSET 0 $NB_BENCHMARKS $DEADLINE_ITERATION $MAX_CURRENT_LOWER $MAX_CURRENT_UPPER $NUM_CONF_SWITCH > measurements.log 2>&1 < /dev/null" < /dev/null &
	PID=$!
fi

if [[ "$1" != "nomon" ]]; then
	tail --pid=$PID -f /dev/null
	
	# retrieve results and create plot
	mkdir -p "results/$EXPE_NAME"
	scp raspberrypi:/root/dw_ina/results.csv "results/$EXPE_NAME/results.csv"
fi

