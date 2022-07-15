#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import shutil
import datetime
from subprocess import PIPE, run
import argparse
import time
import platform
import sunrise_lib

MAX_TEMP_C = 45.0
COOLED_TEMP_C = 40.0

SCRIPT_DIR = 'data/'
DIR_TESTCASES = sunrise_lib.fix_path_src(SCRIPT_DIR + "/sensor-testcases/")

def is_cooled(temp, min_temp=COOLED_TEMP_C):
    if temp == None:
        # Have to assume, that we're overheated
        return False
    return temp <= min_temp        

def is_overheat(temp, max_temp=MAX_TEMP_C):
    if temp == None:
        # Have to assume, that we're overheated
        return True
    return temp >= max_temp

def get_temp():
    """
    Reads the general temperature from the lm-sensors output
    """
    if platform.system() == 'Darwin':
        cmd = "osx-cpu-temp"
    else:
        cmd = "sensors"
    result = sunrise_lib.run_cmd(cmd)
    temp = parse_temp(result.stdout)
    return temp

def parse_temp(sensors_output):
    temps = []
    lines = sensors_output.split('\n')
    for line in lines:
        line = line.strip()
        if not line:
            continue
        tokens = line.split()
        for token in tokens:
            token = token.strip()
            if not token:
                continue
            if "°F" in token:
                raise NotImplemented("Fahranheits!")
            unit = "°C"
            if not unit in token:
                continue
            if "+0.0" + unit == token:
                continue
             
            #print("Token = ", token)
            temp = token[0:-len(unit)]
            temps.append(float(temp))
            break
            

    if len(temps) == 0:
        return None
    temp_avg = sum(temps) / len(temps)
    #print("Avg temp = ", temp_avg)
    return temp_avg

def get_cases_dict_content(dirr):
    print("\nTesting directory:", dirr)
    cases = {}
    for case in sorted(os.listdir(dirr)):
        path = dirr + '/' + case
        if os.path.isdir(path):
            continue
        with open(path) as fpath:
            content = fpath.read()
        cases[path] = content
    return cases

def print_case(case):
    print("case:", case.split('/')[-1])

def test_sensors():
    cases = get_cases_dict_content(DIR_TESTCASES + "/sensors")
    for case in cases.keys():
        print_case(case)
        content = cases[case]
        temp = parse_temp(content)
        assert temp != 0
        assert 0 < temp < 60
    
def test_basic():
    assert is_overheat(MAX_TEMP_C + 1, MAX_TEMP_C) == True
    assert is_overheat(MAX_TEMP_C,     MAX_TEMP_C) == True
    assert is_overheat(MAX_TEMP_C - 1, MAX_TEMP_C) == False

def test_temp():
    print("Current temperature:", get_temp(), "°C")
    #assert get_temp() >= 0
    get_temp()

def test():
    print("Testing sensors:")
    test_basic()
    test_sensors()
    test_temp()
    #get_freqs()
    #restore_user_clock()
           
def main():
    test()
        
if __name__ == "__main__":
    main()
