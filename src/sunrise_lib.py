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

from python_json_config import ConfigBuilder

config_builder = ConfigBuilder()
config = config_builder.parse_config('config.json')

PROJECT_NAME = "SolOptXMR"
PROJECT_SUB_NAME = "Solar Optimal mining of XMR"

HOME = str(Path.home()) # TODO: move this in config.json too?
FNAME = config.sunrise_lib.FNAME
FNAME_MASTER = HOME + config.sunrise_lib.FNAME_MASTER
CPU_FREQ_APP = config.sunrise_lib.CPU_FREQ_APP
HOURS_DELTA = config.sunrise_lib.HOURS_DELTA
MIN_WEATHER = config.sunrise_lib.MIN_WEATHER
DIR_TMP = config.sunrise_lib.DIR_TMP
DIR_XMRIG = HOME + config.sunrise_lib.DIR_XMRIG

DATE_NOW = datetime.datetime.now()
LAT = config.sunrise_lib.LAT
LON = config.sunrise_lib.LON

TESTING = config.sunrise_lib.TESTING

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

def test_stateful_bool():
    print("Test stateful bool")
    false = StatefulBool(False)
    assert false != False
    assert false != True

    assert false.get() == False
    assert false.get() != True

    assert false.update_is_changed(False) == False
    assert false.update_is_changed(False) == False
    assert false.update_is_changed(True) == True
    assert false.get() == True
    assert false.update_is_changed(True) == False
    assert false.get() == True

def test():
    test_stateful_bool()

#LOCAL_TIMEZONE = datetime.datetime.now(datetime.timezone.utc).astimezone().tzname
#print(LOCAL_TIMEZONE)
