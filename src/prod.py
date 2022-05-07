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

DATE_NOW_STR = sunrise_lib.DATE_NOW.isoformat()
DEFAULT_HORIZON_DAYS = 3

def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', '--start-date',   default=DATE_NOW_STR, type=str, help="Start date, ISO format (default: {})".format(DATE_NOW_STR))
    parser.add_argument('-d', '--days-horizon', default=DEFAULT_HORIZON_DAYS, type=int, help="Horizon in days (default: {})".format(DEFAULT_HORIZON_DAYS))
    #parser.add_argument('-v', '--verbose',      default=TESTING, action='store_true', help="Test (default: OFF)")
    return parser.parse_args()

        
class BatterySimulatorCpp(generator.BatterySimulator):
    def __init(self):
        pass
    
    def run(self):
        basePath = 'build/src/opti/opti' # TODO: Pass on days horizon
        path = basePath
        if not os.path.isfile(path):
            path = '../' + path    
        result = sunrise_lib.run_cmd(path, True)
        if result.returncode != 0:
            raise RuntimeError("Failed to run opti")

        basePathIn = "/tmp/soloptout-{}.txt"

        self.hashrates  = np.loadtxt(basePathIn.format('hashrates'))
        self.loads      = np.loadtxt(basePathIn.format('battery'))
        self.usage      = np.loadtxt(basePathIn.format('usage'))

def get_usage_prod(available):
    bat_sim = BatterySimulatorCpp()
    bat_sim.run()
    hashrates = bat_sim.hashrates
    loads = bat_sim.loads
    usage = bat_sim.usage
    incomes = [0]* len(available)
    costs = [0]* len(available)
    effs = [0]* len(available)
    
    return "Production", hashrates, usage, loads, bat_sim, incomes, costs, effs

def run_main(elev, show_plots):
    generator.run_algo(elev, show_plots, get_usage_prod)
    #generator.run_algo(elev, show_plots, generator.get_usage_simple)


def main(args):
    start_date = dateutil.parser.parse(args.start_date)
    pos = generator.get_sun_positions(start_date, args.days_horizon, unpickle=False)
    #print(pos)
    show_plots = True
    proc = generator.proc_data(pos)
    elev = generator.extr_data(proc)
    print(elev)
    run_main(elev, show_plots)

if __name__ == "__main__":
    args = get_args()
    main(args)    
