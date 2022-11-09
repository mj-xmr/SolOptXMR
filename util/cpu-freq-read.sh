#!/bin/bash

# sudo apt install cpufrequtils
# Read min/max freq:
# cpufreq-info
# Example /etc/rc.local:
# /root/bin/cpu-freq.sh 0.8
#
# exit 0

FILE_OUT_USR=/dev/shm/cpu-freq-user.txt
FILE_OUT_MIN=/dev/shm/cpu-freq-min.txt

LIMITS=$(cpufreq-info | grep limits | head -1)
MIN=$(echo $LIMITS | awk '{print $3}')
MAX=$(echo $LIMITS | awk '{print $6}')

MIN_UNIT=$(echo $LIMITS | awk '{print $4}')
MAX_UNIT=$(echo $LIMITS | awk '{print $7}')

echo "Min = $MIN $MIN_UNIT"
echo "Max = $MAX $MAX_UNIT"  


FREQ_NOW_TXT=$(cpufreq-info | grep "frequency should be within" | head -1)
FREQ_NOW=$(echo $FREQ_NOW_TXT | awk '{print $(NF-1)}')
FREQ_NOW_UNIT=$(echo $FREQ_NOW_TXT | awk '{print $(NF-0)}' | cut -c -3 ) # Cut the dot

echo "Curr = $FREQ_NOW $FREQ_NOW_UNIT"

echo "${MIN} ${MIN_UNIT}" > $FILE_OUT_MIN
echo "${FREQ_NOW} ${FREQ_NOW_UNIT}" > $FILE_OUT_USR

