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
import voltage_lib
import ocr_gas
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
    parser.add_argument('-r', '--battery-charge-ocr',      default=False, action='store_true', help="Initial battery charge OCR (default: OFF)")
    parser.add_argument('-p', '--battery-charge-percent',  default=DEFAULT_BATTERY_STATE, type=float, help="Initial battery charge [0-100]  (default: {} which means: minimal charge)".format(DEFAULT_BATTERY_STATE))
    parser.add_argument('-a', '--battery-charge-ah', default=DEFAULT_BATTERY_STATE, type=float, help="Initial battery charge [Ah] (default: {} which means: minimal charge)".format(DEFAULT_BATTERY_STATE))
    parser.add_argument('-v', '--battery-charge-v',  default=DEFAULT_BATTERY_STATE, type=float, help="Initial battery charge [V]  (default: {} which means: minimal charge)".format(DEFAULT_BATTERY_STATE))
    sunrise_lib.add_date_arguments_to_parser(parser)
    # TODO:
    # parser.add_argument('-f', '--file-image-ocr',  default="", type=str, help="Image path to OCR (default: {})".format(""))
    parser.add_argument('-i', '--in-data',  default="", type=str, help="Input hashrate data (default: {})".format(""))
    parser.add_argument('-o', '--out-dir',  default=sunrise_lib.DIR_TMP, type=str, help="Output dir to exchange with tsqsim (default: {})".format(""))
    parser.add_argument('-n', '--net-diff', default=False, action='store_true', help="Plot network difficulty only (default: OFF)")
    parser.add_argument('-m', '--sim',      default=False, action='store_true', help="Plot simulation only (default: OFF)")
    #parser.add_argument('-v', '--verbose',      default=TESTING, action='store_true', help="Test (default: OFF)")
    return parser.parse_args()

def getInstallPathPrefix(prefix=''):
    pref = prefix + 'build/'
    dir_candidates = []
    dir_candidates.append('icecc-shared-release')
    dir_candidates.append('icecc-static-release')
    dir_candidates.append('default-static-release')
    dir_candidates.append('default-shared-release')
    dir_candidates.append('clang-static-release')
    dir_candidates.append('clang-shared-release')
    dir_candidates.append('gcc-static-release')
    dir_candidates.append('gcc-shared-release')

    for d in dir_candidates:
        dir_test = pref + d + "/bin"
        if os.path.isdir(dir_test):
            return dir_test
    return None

def getInstallPath():
    dirr = getInstallPathPrefix()
    if dirr == None:
        dirr = getInstallPathPrefix('../')
    return dirr

def get_hashrate_bonus(out_dir):
    hashrate_bonus = 0
    try:
        a = POW_Coin(kraken.coin.XMR)
        min_data_points = 20000
        height_start = a.height - min_data_points - 1
        height_end = a.height - 1  # The current height is not "historical" yet. TODO: Document in the API
        print("Downloading height: start =", height_start, ", end =", height_end)
        last_diff = a.historical_diff_range("h", height_start, height_end)
        ts_diff = last_diff[['timestamp', 'difficulty']]
        print(ts_diff)
        diff_csv_path = sunrise_lib.DIR_TMP + "/diff_original.csv"
        ts_diff.to_csv(diff_csv_path, sep=',', index=False)

        cmd = "./tsqsim"
        cmd += " --data {}".format(diff_csv_path)
        cmd += " --out {}" .format(out_dir)
        cmd += " --latest-date"
        cmd += " --per h1"

        result = sunrise_lib.run_cmd(cmd, True)
        if result.returncode != 0:
            raise RuntimeError("Failed to run tsqsim")

        hashrate_bonus = np.loadtxt(out_dir + FILE_HASHRATE_BONUS_SINGLE)
        print("Hashrate bonus (MA) =", hashrate_bonus)
    except Exception as ex:
        print(traceback.format_exc())
        print("Failed to fetch hashrate")

    return hashrate_bonus
        
class BatterySimulatorCpp(generator.BatterySimulator):
    def __init(self):
        pass
    
    def run(self, args, battery_charge, horizon):
        cwd = os.getcwd()
        install_path = getInstallPath()
        os.chdir(install_path)

        hashrate_bonus = get_hashrate_bonus(args.out_dir)

        #hashrate_bonus = -3.2 # For simulation only
        #hashrate_bonus =  5.2 # For simulation only
        cmd = "./opti"
        cmd += " --battery-charge {}".format(battery_charge)
        cmd += " --battery-charge-max-percent {}".format(generator.MAX_CAPACITY_PERCENTAGE)        
        cmd += " --horizon-days {}".format(horizon)
        cmd += " --hashrate-bonus {}".format(hashrate_bonus)
        cmd += " --out {}" .format(args.out_dir)
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

        #if hashrate_bonus != 0:
        #    plot_hashrates()
            

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
    fig, (ax1, ax2) = plt.subplots(2, 1)
    #plt.gca().xaxis_date(sunrise_lib.tz) # Not a time series just yet

    if os.path.isfile(args.out_dir + FILE_HASHRATE_BONUS):
        bonusMA = np.loadtxt(args.out_dir + FILE_HASHRATE_BONUS)
        ax1.set_title("Network difficulty rel. to its moving average")
        ax1.set_xlabel("Time [h]")
        ax1.set_ylabel("Relat. network diff.")
        plot_single(ax1, bonusMA, 8)

    if os.path.isfile(args.out_dir + FILE_HASHRATE_SEASONAL):
        bonus = np.loadtxt(args.out_dir + FILE_HASHRATE_SEASONAL)
        ax2.set_title("Network difficulty: seasonal (TODO)")
        ax2.set_xlabel("Time [h]")
        ax2.set_ylabel("Seasonal network diff.")
        plot_single(ax2, bonus, 4)

    plt.subplots_adjust(hspace=0.5)

    plt.show()
        

def run_main(args, elev, show_plots, battery_charge, horizon):
    generator.run_algo(args, elev, show_plots, get_usage_prod, battery_charge, horizon)
    #generator.run_algo(args, elev, show_plots, generator.get_usage_simple)
    if not args.sim:
        plot_hashrates()

def main(args):
    if args.battery_charge_ocr:
        print("OCR input is still unstable.")
        #make_picture()
        args.battery_charge_v = ocr_gas.get_detection()
        # TODO:
        # make_picture()
        # args.battery_charge_v = ocr_gas.get_detection()
        
    if args.battery_charge_v and args.battery_charge_v != DEFAULT_BATTERY_STATE:
        print("Voltage input is still unstable.") # TODO: use various discharge rates
        print("Battery voltage readout:", args.battery_charge_v)
        args.battery_charge_percent = voltage_lib.voltage_to_percentage(args.battery_charge_v, sunrise_lib.BAT_DISCHARGE_RATE_C_BY)
        if args.battery_charge_percent == 0:
            args.battery_charge_percent = 1
    if args.battery_charge_percent:
        if args.battery_charge_percent < 1:
            raise ValueError("Percentage input must be > 1.")
        print("Battery percentage readout:", args.battery_charge_percent)
        args.battery_charge_ah = args.battery_charge_percent / 100.0 * generator.MAX_CAPACITY
        print("Battery apere-hours readout:", args.battery_charge_ah)

    if args.battery_charge_ah == 0:
        print("Battery apere-hours is 0. Trying to guess a reasonable value.")
    
    if args.net_diff:
        install_path = getInstallPath()
        os.chdir(install_path)
        hashrate_bonus = get_hashrate_bonus(args.out_dir)
        plot_hashrates()
    else:
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
