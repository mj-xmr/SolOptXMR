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
from subprocess import PIPE, run, Popen
from pathlib import Path

from pytz import timezone
import requests
import shutil

#from tzlocal import get_localzone # $ pip install tzlocal

from python_json_config import ConfigBuilder


HOME = str(Path.home()) + "/" # TODO: move this in config.json too?
DIR_THIS = os.path.basename(os.path.dirname(os.path.realpath(__file__)))

config_builder = ConfigBuilder()
config = config_builder.parse_config('config.json')

def get_config_path(name):
    os.makedirs(HOME + config.sunrise_lib.DIR_CFG, exist_ok=True)
    path_local = HOME + config.sunrise_lib.DIR_CFG + "/{}.json".format(name)
    if not os.path.isfile(path_local):
        cfg_template =  'system-cfg/{}-template.json'.format(name)
        if not os.path.isfile(cfg_template):
            cfg_template = "src/" + cfg_template
        print("Not found: " + path_local)
        print("Copying " + cfg_template + " to " + path_local)
        shutil.copy(cfg_template, path_local)
    return path_local

def get_config(name):
    path_local = get_config_path(name)
    config_local = config_builder.parse_config(path_local)    
    return config_local


config_volatile = get_config('config-volatile')
config_geo = get_config('geo')
config_batteries = get_config('batteries')
config_computers = get_config('computers')
config_arrays = get_config('arrays')
config_wind_turbines = get_config('wind')
config_system = get_config('system')
config_habits = get_config('habits')

PROJECT_NAME = "SolOptXMR"
PROJECT_SUB_NAME = "Solar Optimal mining of XMR"

BAT_DISCHARGE_RATE_C_BY = config_batteries.batteries[0]['discharge_rate_c_by'] # TODO: Consider other bats?
FNAME = config.sunrise_lib.FNAME
FNAME_MASTER = HOME + config.sunrise_lib.FNAME_MASTER
CPU_FREQ_APP = config.sunrise_lib.CPU_FREQ_APP
HOURS_DELTA = config.sunrise_lib.HOURS_DELTA
MIN_WEATHER = config.sunrise_lib.MIN_WEATHER
DIR_XMRIG = HOME + config.sunrise_lib.DIR_XMRIG
DIR_TMP = config_volatile.paths.DIR_TMP
PATH_OCR_IMAGE = DIR_TMP + '/ocr.jpg'

os.makedirs(DIR_TMP, exist_ok=True)
os.makedirs(config_volatile.paths.DIR_ARCHIVE, exist_ok=True)

tzstr = config_geo.geo.time_zone
tz = timezone(tzstr)
    
DATE_NOW = datetime.datetime.now(tz=tz)
DATE_NOW_STR = DATE_NOW.isoformat()
DEFAULT_HORIZON_DAYS = 3
LAT = config_geo.geo.lat
LON = config_geo.geo.lon

TESTING = config.sunrise_lib.TESTING

def add_date_arguments_to_parser(parser, date=DATE_NOW_STR, horizon=DEFAULT_HORIZON_DAYS):
    parser.add_argument('-s', '--start-date',    default=date, type=str, help="Start date, ISO format (parsed) (default: {})".format(date))
    parser.add_argument('-d', '--days-horizon',  default=horizon, type=int, help="Horizon in days (default: {})".format(horizon))
    parser.add_argument('-t', '--test-local',    default=False, action='store_true', help="Test only Internet independent modules")

def write_file(fname, val):
    with open(fname, 'w') as fout:
        fout.write(val)

def read_file(fname):
    with open(fname) as fin:
        return fin.read()
        
def run_cmd(cmd, print_result=True):
    print("Running command:\n" + cmd)
    timeout_minutes = 59
    timeout_s = timeout_minutes * 60
    result = run(cmd.split(), timeout=timeout_s, bufsize=0, stdout=PIPE, stderr=PIPE, universal_newlines=True)
    if print_result:
        print(result.returncode, result.stdout, result.stderr)
    return result

def run_ssh_cmd(host, cmd):
    print("Running command:\n" + cmd, "\nOn host:", host)
    cmds = ['ssh', '-n', host, cmd]
    return Popen(cmds, stdout=PIPE, stderr=PIPE, stdin=PIPE, universal_newlines=True)

def fix_path_src(pth, must_exist=True):
    if os.path.exists(pth):
        return pth
    alt = 'src/' + pth
    if must_exist:
        if not os.path.isdir(alt):
            if not os.path.isfile(alt):
                raise IOError("Path doesn't exist", pth)
    return alt

def km_per_hour_2_meter_per_second(kmpsval):
    return kmpsval * 1000 / 3600

def mile_per_hour_2_meter_per_second(mphval):
    return mphval * 0.44704

def get_number_from_val_unit(val_unit):
    return float(val_unit.split()[0])

def conv_speed_string_2_number(speed_str):
    if 'mph' in speed_str:
        return mile_per_hour_2_meter_per_second(get_number_from_val_unit(speed_str))
    for kmph in ['kmh', 'km/h']:
        if kmph in speed_str:
            return km_per_hour_2_meter_per_second(get_number_from_val_unit(speed_str))

    raise IOError("Unknown unit in " + str(speed_str))

def get_wind_power(watt_min, watt_max, wind_min, wind_max, wind):
    if wind > wind_max:
        wind = wind_max
    if wind < wind_min:
        return 0

    wind_work = wind - wind_min
    watt_mul = watt_max - watt_min

    watt = wind_work * watt_mul / (wind_max - wind_min)

    watt_final = watt + watt_min

    #print(watt_final)
    return watt_final

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

def get_arrays():
    arrays = []
    for array in config_arrays.arrays:
        #print("AR", array)
        for num in range(0, array['count']):
            arrays.append(array)
    return arrays

def get_pv_system():
    array_kwargs = dict(
        module_parameters=dict(pdc0=1, gamma_pdc=-0.004),
        temperature_model_parameters=dict(a=-3.56, b=-0.075, deltaT=3)
    )
    arrays = []
    for array in get_arrays():
        array_one = pvlib.pvsystem.Array(pvlib.pvsystem.FixedMount(surface_tilt=array['surface_tilt'], surface_azimuth=array['surface_azimuth']), name=array['name'],
                   **array_kwargs)
        arrays.append(array_one)
        pdc0 = config_system.inverter_max_input_dc
        if pdc0 == None:
            pdc0 = 40
    system = pvlib.pvsystem.PVSystem(arrays=arrays, inverter_parameters=dict(pdc0=pdc0))
    return system

def test_physical():
    assert km_per_hour_2_meter_per_second(36) == 10
    assert mile_per_hour_2_meter_per_second(100000) == 44704
    assert conv_speed_string_2_number("36 kmh") == 10
    assert conv_speed_string_2_number("100000 mph") == 44704

    assert get_wind_power(10, 100, 10, 100, 10) == 10
    assert get_wind_power(10, 100, 10, 100, 100) == 100
    assert get_wind_power(10, 150, 10, 100, 150) == 150
    assert get_wind_power(10, 100, 10, 100, 1) == 0
    assert get_wind_power(10, 100, 10, 100, 150) == 100

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
    test_physical()

if __name__ == "__main__":
    test()
    
#LOCAL_TIMEZONE = datetime.datetime.now(datetime.timezone.utc).astimezone().tzname
#print(LOCAL_TIMEZONE)
