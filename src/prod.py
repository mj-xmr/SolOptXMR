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

DEFAULT_BATTERY_STATE = 0
config_system = sunrise_lib.config_system
FILE_HASHRATE_BONUS = "/hashrate_bonus_ma.dat"
FILE_HASHRATE_SEASONAL = "/seasonal.dat"
FILE_HASHRATE_BONUS_SINGLE = "/hashrate_bonus_ma_single.dat"

def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('-a', '--battery-charge-ah', default=DEFAULT_BATTERY_STATE, type=float, help="Initial battery charge [Ah] (default: {} which means: minimal charge)".format(DEFAULT_BATTERY_STATE))
    parser.add_argument('-v', '--battery-charge-v',  default=DEFAULT_BATTERY_STATE, type=float, help="Initial battery charge [V]  (default: {} which means: minimal charge) UNSUOPPORTED YET".format(DEFAULT_BATTERY_STATE))
    sunrise_lib.add_date_arguments_to_parser(parser)    
    parser.add_argument('-i', '--in-data',  default="", type=str, help="Input hashrate data (default: {})".format(""))
    parser.add_argument('-o', '--out-dir',  default="", type=str, help="Output dir to exchange with tsqsim (default: {})".format(""))
    #parser.add_argument('-v', '--verbose',      default=TESTING, action='store_true', help="Test (default: OFF)")
    return parser.parse_args()

def getInstallPath():
    dir1 = 'build/icecc-shared-release/bin'
    dir2 = 'build/icecc-static-release/bin'
    dir3 = 'build/default-static-release/bin'
    dir4 = 'build/default-shared-release/bin'
    if os.path.isdir(dir1):
        dirr = dir1
    elif os.path.isdir(dir2):
        dirr = dir2
    elif os.path.isdir(dir3):
        dirr = dir3
    elif os.path.isdir(dir4):
        dirr = dir4
    return dirr
        
class BatterySimulatorCpp(generator.BatterySimulator):
    def __init(self):
        pass
    
    def run(self, args, battery_charge, horizon):
        cwd = os.getcwd()
        install_path = getInstallPath()
        os.chdir(install_path)
        
        hashrate_bonus = 0
        if args.in_data and args.out_dir:
            cmd = "./tsqsim"
            cmd += " --data {}".format(args.in_data)
            cmd += " --out {}".format(args.out_dir)
            cmd += " --latest-date"
            cmd += " --per h1"

            result = sunrise_lib.run_cmd(cmd, True)
            if result.returncode != 0:
                raise RuntimeError("Failed to run tsqsim")

            hashrate_bonus = np.loadtxt(args.out_dir + FILE_HASHRATE_BONUS_SINGLE)
            print("Last bonus =", hashrate_bonus)

        #hashrate_bonus = -3.2 # For simulation only
        #hashrate_bonus =  5.2 # For simulation only
        cmd = "./opti"
        cmd += " --battery-charge {}".format(battery_charge)
        cmd += " --horizon-days {}".format(horizon)
        cmd += " --hashrate-bonus {}".format(hashrate_bonus)
        #cmd += " --system-type {}".format(config_system.type)
        #cmd += " --system-voltage {}".format(config_system.voltage)

        
        result = sunrise_lib.run_cmd(cmd, True)
        if result.returncode != 0:
            raise RuntimeError("Failed to run opti")

        basePathIn = "/tmp/soloptout-{}.txt"

        self.hashrates  = np.loadtxt(basePathIn.format('hashrates'))
        self.loads      = np.loadtxt(basePathIn.format('battery'))
        self.usage      = np.loadtxt(basePathIn.format('usage'))

        os.chdir(cwd)

def get_usage_prod(args, available, battery_charge, horizon):
    bat_sim = BatterySimulatorCpp()
    bat_sim.run(args, battery_charge, horizon)
    hashrates = bat_sim.hashrates
    loads = bat_sim.loads
    usage = bat_sim.usage
    incomes = [0]* len(available)
    costs = [0]* len(available)
    effs = [0]* len(available)
    
    return "Production", hashrates, usage, loads, bat_sim, incomes, costs, effs

def plot_single(ax, data, days):
    length = 24*days
    ax.plot(data[-length:])
    ax.plot([0] * length, color='y')
    for d in range(0, days):
        ax.axvline(x=d * 24, color='g')
    ax.grid()

def plot_hashrates():
    if args.in_data and args.out_dir:
        fig, (ax1, ax2) = plt.subplots(2, 1)
        #plt.gca().xaxis_date(sunrise_lib.tz) # Not a time series just yet
        
        bonusMA = np.loadtxt(args.out_dir + FILE_HASHRATE_BONUS)
        ax1.set_title("Network difficulty rel. to its moving average")
        ax1.set_xlabel("Time [h]")
        ax1.set_ylabel("Rel. network diff.")
        plot_single(ax1, bonusMA, 8)
        
        bonus = np.loadtxt(args.out_dir + FILE_HASHRATE_SEASONAL)
        ax2.set_title("Network difficulty seasonal")
        ax2.set_xlabel("Time [h]")
        ax2.set_ylabel("Network diff. seasonal")
        plot_single(ax2, bonus, 4)

        
        plt.show()
        

def run_main(args, elev, show_plots, battery_charge, horizon):
    generator.run_algo(args, elev, show_plots, get_usage_prod, battery_charge, horizon)
    #generator.run_algo(args, elev, show_plots, generator.get_usage_simple)
    plot_hashrates()

def main(args):
    if args.battery_charge_v:
        raise ValueError("Voltage input not supported yet.") # TODO
    start_date = dateutil.parser.parse(args.start_date)
    elev = generator.get_power(start_date, args.days_horizon, unpickle=False)
    #print(pos)
    show_plots = True
    #print('hori', args.days_horizon)
    elev = generator.proc_data(elev, False, args.days_horizon)
    #elev = generator.extr_data(proc)
    #print(elev)
    run_main(args, elev, show_plots, args.battery_charge_ah, args.days_horizon)

if __name__ == "__main__":
    args = get_args()
    main(args)
