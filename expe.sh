set -e

# parameters
EXPE_NAME=stm32_v2
NB_EXPES=32 # nb_confs * nb_benchmarks
NB_ITERS=3
NB_BENCHMARKS=4
DEADLINE_ITERATION=3600
MAX_CURRENT=0.022
OFFSET=0
ITER_OFFSET=0

scp measurements.py raspberrypi:/root/dw_ina/measurements.py

if [[ "$1" != "nomon" ]]; then
	(ssh raspberrypi 'kill $(pgrep -f measurements.py)' || true)
	ssh raspberrypi "cd /root/dw_ina && source venv/bin/activate && nohup python -u measurements.py $NB_EXPES $NB_ITERS $OFFSET $ITER_OFFSET 0 $NB_BENCHMARKS $DEADLINE_ITERATION $MAX_CURRENT > measurements.log 2>&1 < /dev/null" < /dev/null &
	PID=$!
fi

if [[ "$1" != "nomon" ]]; then
	tail --pid=$PID -f /dev/null
	
	# retrieve results and create plot
	mkdir -p "results/$EXPE_NAME"
	scp raspberrypi:/root/dw_ina/results.csv "results/$EXPE_NAME/results.csv"
fi

