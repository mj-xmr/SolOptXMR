#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# https://stackoverflow.com/a/11236372
# apt install gfortran libffi-dev
# pip3 install pvlib ephem pytz beautifulsoup4 cairosvg wget requests Pillow


import os
import pvlib
import datetime
import pandas as pd
import time
import traceback
from subprocess import PIPE, run
from pathlib import Path

from pytz import timezone
import requests

#from tzlocal import get_localzone # $ pip install tzlocal

HOME = str(Path.home())
FNAME = '/dev/shm/solar.txt'
FNAME_MASTER = HOME + '/.config/solar/master'
CPU_FREQ_APP = '/root/bin/cpu-freq.sh'
HOURS_DELTA = 2.5
MIN_WEATHER = 0.4
DIR_TMP='/tmp/'
DIR_XMRIG=HOME + '/progs/crypto/xmr/xmrig/b'

LAT=53
LON=10

TESTING = False
#TESTING = True

def write_file(fname, val):
    with open(fname, 'w') as fout:
        fout.write(val)

def read_file(fname):
    with open(fname) as fin:
        return fin.read()
        
def run_cmd(cmd):
    result = run(cmd, stdout=PIPE, stderr=PIPE, universal_newlines=True)
    #print(result.returncode, result.stdout, result.stderr)
    return result

# get local timezone    
#local_tz = get_localzone()

class StatefulBool:
    def __init__(self, initial):
        self._prev = initial

    def update_is_changed(self, new):
        ret = new != self._prev
        self._prev = new
        return ret

    def get(self):
        return self._prev

#LOCAL_TIMEZONE = datetime.datetime.now(datetime.timezone.utc).astimezone().tzname
#print(LOCAL_TIMEZONE)
