#!/bin/bash -e

# sudo apt install cpufrequtils
# Read min/max freq:
# cpufreq-info
# Example /etc/rc.local:
# /root/bin/cpu-freq.sh 0.8
#
# exit 0
FREQ=$1
FREQ_UNITS=$2

if [ -z $1 ]; then
	echo "Provide frequency in GHz"
	exit
fi

if [ -z $2 ]; then
	FREQ_UNITS=Ghz
fi

echo "Setting ${FREQ}${FREQ_UNITS}"

END=$(nproc)
for cpuID in $(seq 1 $END); do
	echo "Setting core $((cpuID-1)) to ${FREQ}${FREQ_UNITS}"
	cpufreq-set -c $((cpuID-1)) --related --max ${FREQ}${FREQ_UNITS}
done

