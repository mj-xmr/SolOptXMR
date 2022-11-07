#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Nov 07 14:49:11 2022

@author: mj-xmr
"""

import os
import shutil
import datetime
from subprocess import PIPE, run
import argparse
import time

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
SCRIPT_CPU_FREQ_CHANGE = SCRIPT_DIR + "/cpu-freq.sh"
SCRIPT_CPU_FREQ_READ = SCRIPT_DIR + "/cpu-freq-read.sh"
DIR_TESTCASES= SCRIPT_DIR + "/tests/temperature/"

LOOP_REFRESH_SECONDS = 2
DEFAULT_START_DELAY_SECONDS = 35

MAX_TEMP_C = 65.0
COOLED_TEMP_C = 47.0
MAX_FAN_RPM = 1 # Should be 0. 1 = testing
KEY_FMIN = "fmin"
KEY_FMAX = "fmax"
KEY_FCUR = "fcur"
TESTING = True
TESTING = False

def get_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument('-l', '--looped',    default=not TESTING, action='store_true', help="Looped (default: OFF)")
    parser.add_argument('-t', '--test',      default=True, action='store_true', help="Test")
    parser.add_argument('-v', '--verbose',      default=True, action='store_true', help="Test (default: OFF)")
    parser.add_argument('-m', '--max-temp-c',   default=MAX_TEMP_C, type=float, help="Max temperature [°C]")
    parser.add_argument('-c', '--cooled-temp-c',   default=COOLED_TEMP_C, type=float, help="Cooled temperature [°C]")
    parser.add_argument('-p', '--max-fan-rot',   default=MAX_FAN_RPM, type=int, help="Max fan rotation [RPM]")
    parser.add_argument('-r', '--refresh',   default=LOOP_REFRESH_SECONDS, type=int, help="Update frequency [s]")
    return parser

class StatefulBool:
    def __init__(self, initial):
        self._prev = initial

    def update_is_changed(self, new):
        ret = new != self._prev
        self._prev = new
        return ret

    def get(self):
        return self._prev

def looped(args):
    first = True

    # Temp
    is_overheated = StatefulBool(False)
    is_cooled_state = StatefulBool(True)
    is_cooling_down = False

    # Freq
    cool_clock = None
    
    try:
        while True:
            if first:
                first = False
                # Needed for Ubuntu, where seemingly the measurements aren't available early
                # and the program quits.
                # TODO: Debug the situation under Ubuntu
                time.sleep(DEFAULT_START_DELAY_SECONDS)
            else:
                time.sleep(args.refresh)

            temp = get_temp()
            
            if is_overheated.update_is_changed(is_overheat(temp, args.max_temp_c)):
                if is_overheated.get():
                    print("Temp state changed to overheated")
                    cool_clock = downclock()
                    is_cooling_down = True
                    is_cooled_state.update_is_changed(False)
                    continue

            if is_cooling_down:
                if is_cooled_state.update_is_changed(is_cooled(temp, args.cooled_temp_c)):
                    if is_cooled_state.get():
                        print("Temp back to normal. Restoring clock")
                        restore_user_clock(cool_clock)
    
                        is_cooling_down = False
                        continue
            
            if is_cooling_down:
                continue
        
    except KeyboardInterrupt:
        print("KeyboardInterrupt. Loop exiting. Trying to clean up")
        if cool_clock:
            restore_user_clock(cool_clock)
        #print(traceback.format_exc())
        
class Freq:
    def __init__(self, line, name="Freq name unknown"):
        print("Freq line", line)
        val_unit = line.split('=')[-1].strip()
        self.val_orig, self.unit_orig = val_unit.split(' ', 2)
        GHz = "GHz" 
        if self.unit_orig == GHz:
            self.val  = self.val_orig 
            self.unit = self.unit_orig
        elif self.unit_orig == "MHz":
            self.val  = str(float(self.val_orig) / 1000.0) 
            self.unit = GHz
        else:
            raise NotImplemented("Unknown unit = " + self.unit_orig)
        self.name = name
        self.print_descr()
        
    def print_descr(self):
        print(self.name, self.val, self.unit)    

def downclock():
    print("Downclocking")
    
    freqs = get_freqs()
    fcur = freqs[KEY_FCUR]
    fmin = freqs[KEY_FMIN]
    print(fcur.val, fcur.unit)
    print_clock_change(fmin, "Downclocking frequency to:")
    change_clock(fmin)

    return fcur
    
def get_freqs():
    cmd = [SCRIPT_CPU_FREQ_READ]
    result = run_cmd(cmd)
    ret_dict = parse_freqs(result.stdout) 
    return ret_dict

def parse_freqs(result_stdout):
    ret_dict = {}
    lines = result_stdout.split('\n')
    ret_dict[KEY_FMIN] = Freq(lines[0], KEY_FMIN)
    ret_dict[KEY_FMAX] = Freq(lines[1], KEY_FMAX)
    ret_dict[KEY_FCUR] = Freq(lines[2], KEY_FCUR)

    #print(lines)
    return ret_dict

def restore_user_clock(user_clock):
    print_clock_change(user_clock, "Restoring frequency to:")
    change_clock(user_clock)

def print_clock_change(freq, message):
    print(message, freq.val, freq.unit)

def change_clock(freq):
    cmd = [SCRIPT_CPU_FREQ_CHANGE, freq.val, freq.unit]
    run_cmd(cmd)

#def downclock_on_overheat(temp, max_temp=MAX_TEMP_C):
    
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

def is_fan_overrotating(rpm, max_rmp=MAX_FAN_RPM):
    if rpm == None:
        return False
    if rpm == 0:
        return False
    if max_rmp == 0:
        return False
    return rmp >= max_rmp

def get_temp():
    cmd = ["sensors"]
    result = run_cmd(cmd)
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

def run_cmd(cmd):
    result = run(cmd, stdout=PIPE, stderr=PIPE, universal_newlines=True)
    #print(result.returncode, result.stdout, result.stderr)
    return result

def test():
    print("Testing")
    test_sensors()
    test_cpufreq()
    test_basic()
    get_temp()
    #get_freqs()
    #restore_user_clock()

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

def test_cpufreq():
    cases = get_cases_dict_content(DIR_TESTCASES + "/cpufreq-info")
    for case in cases.keys():
        print_case(case)
        content = cases[case]
        #print(content)
        freqs = parse_freqs(content)

        fmin = freqs[KEY_FMIN]
        fmax = freqs[KEY_FMAX]
        fcur = freqs[KEY_FCUR]

        assert fmin.val < fmax.val
        assert fcur.val < fmax.val
        assert fmin.val < fcur.val

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
    
def test_basic():
    test_stateful_bool()
    
    assert is_overheat(MAX_TEMP_C + 1, MAX_TEMP_C) == True
    assert is_overheat(MAX_TEMP_C,     MAX_TEMP_C) == True
    assert is_overheat(MAX_TEMP_C - 1, MAX_TEMP_C) == False

def print_test_warning():
    if TESTING:
        print("\n------------")
        print("TESTING is still ON!")
           
def main(args):
    if args.test:
        test()

    print("Max temp =", args.max_temp_c, ", cooled temp =", args.cooled_temp_c)

    print_test_warning()
    if args.looped:
        looped(args)
    else:
        status = is_overheat(get_temp(), args.max_temp_c)
        print("Overheat" if status else "Cool'nuff")
    print_test_warning()
        
if __name__ == "__main__":
    parser = get_parser()
    args = parser.parse_args()
    main(args)
