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
import pickle
import numpy as np

from pytz import timezone
from matplotlib import pyplot as plt

import sunrise_lib
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

def get_sun_positions(start_date, days_horizon=3, unpickle=True):
    a = datetime.datetime.now()
    path_positions_file_name = f"{PATH_POSITIONS_BASE}-{start_date.year}-{start_date.month}-{start_date.day}-{days_horizon}"
    path_positions = path_positions_file_name + ".dat"
    if unpickle and os.path.isfile(path_positions):
        print("Reading from:", path_positions)
        with open(path_positions, "rb") as handle:
            pos = pickle.load(handle)
            b = datetime.datetime.now()
    else:    
        dti = pd.date_range(start_date, periods=days_horizon * 24, freq="H")
        
        pos = pvlib.solarposition.get_solarposition(dti, sunrise_lib.LAT, sunrise_lib.LON)
        b = datetime.datetime.now()
        
        with open(path_positions, "wb") as handle:
            print("Dumping data to:", path_positions)
            pickle.dump(pos, handle, protocol=pickle.HIGHEST_PROTOCOL)

    c = b - a
    print("Prepared data in", c.total_seconds(), "s")
    return pos

def plot_sun(name, elev, bat, usage, show_plots):
    #print(elev)
    plt.title("Algo: " + name)
    plt.xlabel("Time")
    plt.ylabel("Energy")
    plt.plot(elev,  'y')
    plt.plot(bat,   'g')
    plt.plot(usage, 'b')
    plt.plot(list_to_pd([MAX_CAPACITY]   * len(elev), elev))
    plt.plot(list_to_pd([MAX_CAPACITY/2] * len(elev), elev))
    plt.plot(list_to_pd([MAX_USAGE]      * len(elev), elev))
    plt.grid()
    plt.legend(['sun input', 'bat capacity', 'power usage', 'max cap', 'half cap', 'max usage'])
    os.makedirs(BUILD_DIR, exist_ok=True)
    fout_rel_path = "{}/fig-{}.png".format(BUILD_DIR, name)
    print("Saving figure to:",fout_rel_path)
    plt.savefig(fout_rel_path)
    if show_plots:
        plt.show()

def proc_data(pos):
    pos = simul_weather(pos)
    pos = adj_losses(pos)

    print("Dumping data to:", path_positions_txt)
    np.savetxt(path_positions_txt, pos[ELEVATION_KEY])
    
    return pos

def extr_data(pos):
    elev = pos[ELEVATION_KEY]
    return elev

def adj_losses(pos):
    pos.loc[pos[ELEVATION_KEY] < MIN_POWER, [ELEVATION_KEY]] = MIN_POWER
    pos.loc[pos[ELEVATION_KEY] > MAX_POWER, [ELEVATION_KEY]] = MAX_POWER
    return pos

class Computer:
    def __init__(self):
        self.cores = 4
        self.WATT_PER_CORE = 30

    def get_hashrate(self, cores, freq_ghz=1):
        HASH_PER_CORE = 250 * freq_ghz
        SCALING_FACTOR = 0.85
        if cores > self.cores:
            raise Exception("cores, " + str(cores) + " > max cores, " + str(self.cores))
        more_cores = cores - 1
        val = HASH_PER_CORE * cores * (SCALING_FACTOR ** more_cores)
        return val

    def get_usage(self, cores, freq_ghz=1):
        return cores * freq_ghz * self.WATT_PER_CORE

    #def get_freq(self, cores, power):
     #   return cores * freq_ghz * self.WATT_PER_CORE

class BatterySimulator:
    def __init__(self):
        self.MAX_CAPACITY = MAX_CAPACITY
        self.MIN_LOAD = self.MAX_CAPACITY / 2 # Assuming a lead-acid
        self.load = 0
        self.DISCHARGE_PER_HOUR = 0.16
        self.initial_load = True
        self.num_undervolted = 0
        self.num_undervolted_initial = 0
        self.num_overvolted = 0
        self.num_overused = 0

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


    def get_load(self):
        return self.load

    def print_stats(self, relative=0):
        if relative < 1:
            relative = 1

        def print_relative(val, relative):
            return "{}%".format(round(val / relative * 100, 2))
        if self.num_undervolted_initial > 0:
            print("Undervolted initial = ", print_relative(self.num_undervolted_initial, relative))
        if self.num_undervolted > 0:
            print("Undervolted = ", print_relative(self.num_undervolted, relative))
        if self.num_overvolted > 0:
            print("Overvolted  = ", print_relative(self.num_overvolted, relative))
        if self.num_overused > 0:
            print("Overused    = ", print_relative(self.num_overused, relative))

def get_usage_endor_example(available):
    # TODO: use the available power wisely
    usage = []
    hashrates = []
    loads = []
    incomes = []
    costs = []
    effs = []
    coin_data = POW_Coin(kraken.coin.XMR)
    comp = Computer()
    bat_sim = BatterySimulator()
    for avail in available:
        freq = avail / 2
        cores = 2
        hashrate = comp.get_hashrate(cores, freq)  # H/s
        use = comp.get_usage(cores, freq)  # W
        # profitability_data = coin_data.profitability(kraken.fiat.USD, hashrate, use, ELECTRICITY_PRICE, POOL_FEE, TARGET_PRICE)
        profitability_data = coin_data.profitability(kraken.fiat.USD, hashrate, use, ELECTRICITY_PRICE, POOL_FEE, TARGET_PRICE, reward=1.8, difficulty=150e9) # Rough values in 2020
        energy_used = avail * T_DELTA_HOURS / 1000  # W * h / 1000 W/kW = kWh
        income = energy_used * profitability_data["expected_fiat_income_kwh"]  # kWh * $/kWh
        cost = energy_used * ELECTRICITY_PRICE  # kWh * $/kWh
        incomes.append(income)
        costs.append(cost)
        effs.append(hashrate / use if use != 0 else 0)  # H/Ws - not needed, only useful for simulation sanity check

        if bat_sim.get_load() < bat_sim.MIN_LOAD:
            use = 0
        
        if use > MAX_USAGE:
            hashrate *= use / MAX_USAGE
            use = MAX_USAGE

        load_i = bat_sim.iter_get_load(avail, use)
        loads.append(load_i)

        hashrates.append(hashrate)
        usage.append(use)

    return "Endor", hashrates, usage, loads, bat_sim, incomes, costs, effs

def get_usage_simple(available):
    usage_exp = [MAX_USAGE] * len(available)
    hashrates = [usage_exp[0] * 0.5] * len(available)
    loads = []
    usage = []
    incomes = [0]* len(available)
    costs = [0]* len(available)
    effs = [0]* len(available)
    bat_sim = BatterySimulator()
    for i, use_exp in enumerate(usage_exp):
        if bat_sim.get_load() > bat_sim.MIN_LOAD:
            use = use_exp
        else:
            use = 0
        usage.append(use)            
        load_i = bat_sim.iter_get_load(available[i], use)
        loads.append(load_i)

    return "Simple", hashrates, usage, loads, bat_sim, incomes, costs, effs

def print_hashes(hashrates):
    HASH_UNIT = 1e6
    print("Total hashes =", round(np.sum(hashrates) / HASH_UNIT, 2), "MH")

def print_profits(incomes, costs):
    income = round(np.nansum(incomes), 2)
    cost = round(np.nansum(costs), 2)
    profit = income - cost
    if profit > 0:
        profitability = 100 * profit / cost if cost != 0 else np.PINF
    elif profit < 0:
        profitability = 100 * profit / income if income != 0 else np.NINF
    elif profit == 0:
        profitability = 0
    else:
        raise Exception("Wait, what?")
    print(f"Total income = {income:.2f} USD")
    print(f"Total cost = {cost:.2f} USD")
    print(f"Total profit = {profit:.2f} USD")
    print(f"Profitability = {profitability:.2f} %")

def get_usage(available, algo):
    name, hashrates, usage, bat, bat_sim, incomes, costs, effs  = algo(available)
    print("\nAlgo name: ", name)
    bat_sim.print_stats(len(available))
    print_hashes(hashrates)
    print_profits(incomes, costs)
    #print(effs)  # Check if efficiency is reasonable
    # return name, list_to_pd(usage, available), list_to_pd(incomes, available), list_to_pd(costs, available), list_to_pd(bat, available)
    return name, list_to_pd(usage, available), list_to_pd(bat, available) 


def list_to_pd(listt, df):
    return pd.DataFrame(listt, index=df.index.copy())

def simul_weather(pos):
    pos.loc[::5, [ELEVATION_KEY]] *= 0.6
    pos.loc[::3, [ELEVATION_KEY]] *= 0.8
    pos.loc[::8, [ELEVATION_KEY]] *= 0.3

    return pos

def run_main(elev, show_plots):
    run_algo(elev, show_plots, get_usage_simple)
    run_algo(elev, show_plots, get_usage_endor_example)

def run_algo(elev, show_plots, algo):
    name, usage, bat = get_usage(elev, algo)
    plot_sun(name, elev, bat, usage, show_plots)


def test(show_plots=False):
    start_date = datetime.datetime(2020, 1, 1)
    days_horizon = 30 * 9
    pos = get_sun_positions(start_date, days_horizon)
    #print(pos)

    proc = proc_data(pos)
    elev = extr_data(proc)
    run_main(elev, show_plots)

if __name__ == "__main__":
    test(True)
