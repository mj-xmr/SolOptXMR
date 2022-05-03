#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# https://stackoverflow.com/a/11236372
# apt install gfortran libffi-dev
# pip3 install pvlib ephem pytz beautifulsoup4 cairosvg wget requests Pillow


import os
import datetime
import pandas as pd
import time
import traceback
import argparse
import pickle
import numpy as np
import dateutil

from pytz import timezone
from matplotlib import pyplot as plt

import sunrise_lib
import generator
import kraken
from profitability import POW_Coin

from python_json_config import ConfigBuilder

config = sunrise_lib.config

ELEVATION_KEY = config.generator.ELEVATION_KEY
BUILD_DIR='build/'  # TODO: Config

# Not changable: system params
MIN_POWER = config.generator.MIN_POWER
MAX_POWER = config.generator.MAX_POWER
MAX_USAGE = config.generator.MAX_USAGE
#MIN_CAPACITY = config.generator.MIN_CAPACITY
MAX_CAPACITY = config.generator.MAX_CAPACITY
MUL_POWER_2_CAPACITY = config.generator.MUL_POWER_2_CAPACITY
T_DELTA_HOURS = config.generator.T_DELTA_HOURS
DATE_NOW = sunrise_lib.DATE_NOW
PATH_POSITIONS_BASE = config.sunrise_lib.DIR_TMP + config.generator.PATH_POSITIONS # TODO: Parametrize based on keys
ELECTRICITY_PRICE = config.generator.ELECTRICITY_PRICE
POOL_FEE = config.generator.POOL_FEE / 100  # Must be a float!
TARGET_PRICE = config.generator.TARGET_PRICE

path_positions_txt = f"{PATH_POSITIONS_BASE}.txt"

DATE_NOW_STR = sunrise_lib.DATE_NOW.isoformat()
DEFAULT_HORIZON_DAYS = 3

def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', '--start-date',   default=DATE_NOW_STR, type=str, help="Start date, ISO format (default: {})".format(DATE_NOW_STR))
    parser.add_argument('-d', '--days-horizon', default=DEFAULT_HORIZON_DAYS, type=int, help="Horizon in days (default: {})".format(DEFAULT_HORIZON_DAYS))
    #parser.add_argument('-v', '--verbose',      default=TESTING, action='store_true', help="Test (default: OFF)")
    return parser.parse_args()

        
class BatterySimulatorCpp(generator.BatterySimulator): 
    def iter_get_load(self, inp, out, hours=T_DELTA_HOURS):
        discharge = hours * self.DISCHARGE_PER_HOUR
        balance = inp - out - discharge
        change = balance * MUL_POWER_2_CAPACITY
        if change > MAX_USAGE:
        #if out > MAX_USAGE: # A valid possibility
            self.num_overused += 1
            change = MAX_USAGE
        #print(change)
        self.load += change

        if self.load > self.MAX_CAPACITY:
            self.load = self.MAX_CAPACITY
            self.num_overvolted += 1

        if self.load < self.MIN_LOAD:
            if self.initial_load:
                self.num_undervolted_initial += 1
            else:
                self.num_undervolted += 1
        if self.load < 0:
            self.load = 0
                
        if self.initial_load:
            if self.load > self.MIN_LOAD:
                self.initial_load = False

        return self.get_load()


def get_usage_prod(available):
    bat_sim = generator.BatterySimulatorCpp()
    hashrates = [usage_exp[0] * 0.5] * len(available)
    loads = []
    usage = []
    incomes = [0]* len(available)
    costs = [0]* len(available)
    effs = [0]* len(available)
    
    return "Production", hashrates, usage, loads, bat_sim, incomes, costs, effs

def run_main(elev, show_plots):
    generator.run_algo(elev, show_plots, get_usage_prod)
    generator.run_algo(elev, show_plots, generator.get_usage_simple)


def main(args):
    start_date = dateutil.parser.parse(args.start_date)
    pos = generator.get_sun_positions(start_date, args.days_horizon)
    #print(pos)
    show_plots = True
    proc = generator.proc_data(pos)
    elev = generator.extr_data(proc)
    print(elev)
    run_main(elev, show_plots)

if __name__ == "__main__":
    args = get_args()
    main(args)    
