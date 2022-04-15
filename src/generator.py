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

from python_json_config import ConfigBuilder

config_builder = ConfigBuilder()
config = config_builder.parse_config('config.json')

ELEVATION_KEY = config.generator.ELEVATION_KEY

# Not changable: system params
MIN_POWER = config.generator.MIN_POWER
MAX_POWER = config.generator.MAX_POWER
MAX_USAGE = config.generator.MAX_USAGE
#MIN_CAPACITY = config.generator.MIN_CAPACITY
MAX_CAPACITY = config.generator.MAX_CAPACITY
MUL_POWER_2_CAPACITY = config.generator.MUL_POWER_2_CAPACITY
T_DELTA_HOURS = config.generator.T_DELTA_HOURS
PATH_POSITIONS = config.sunrise_lib.DIR_TMP + config.generator.PATH_POSITIONS # TODO: Parametrize based on keys


def get_sun_positions():
    a = datetime.datetime.now()
    if os.path.isfile(PATH_POSITIONS):
        print("Reading from", PATH_POSITIONS)
        with open(PATH_POSITIONS, "rb") as handle:
            pos = pickle.load(handle)
            b = datetime.datetime.now()
    else:
        start = datetime.datetime(2020, 1, 1)
        dti = pd.date_range(start, periods=30 * 8 * 24, freq="H")
        
        pos = pvlib.solarposition.get_solarposition(dti, sunrise_lib.LAT, sunrise_lib.LON)
        b = datetime.datetime.now()
        
        with open(PATH_POSITIONS, "wb") as handle:
            pickle.dump(pos, handle, protocol=pickle.HIGHEST_PROTOCOL)

    c = b - a
    print("Prepared data in", c.total_seconds(), "s")
    return pos

def plot_sun(name, elev, bat, usage):
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
    plt.show()

def proc_data(pos):
    pos = simul_weather(pos)
    pos = adj_losses(pos)

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
    comp = Computer()
    bat_sim = BatterySimulator()
    for avail in available:
        freq = avail / 2
        cores = 2
        hashrate = comp.get_hashrate(cores, freq)
        use = comp.get_usage(cores, freq)

        if bat_sim.get_load() < bat_sim.MIN_LOAD:
            use = 0
        
        if use > MAX_USAGE:
            hashrate *= use / MAX_USAGE
            use = MAX_USAGE

        load_i = bat_sim.iter_get_load(avail, use)
        loads.append(load_i)

        hashrates.append(hashrate)
        usage.append(use)

    return "Endor's", hashrates, usage, loads, bat_sim

def get_usage_simple(available):
    usage_exp = [MAX_USAGE] * len(available)
    hashrates = [usage_exp[0] * 0.5] * len(available)
    loads = []
    usage = []
    bat_sim = BatterySimulator()
    for i, use_exp in enumerate(usage_exp):
        if bat_sim.get_load() > bat_sim.MIN_LOAD:
            use = use_exp
        else:
            use = 0
        usage.append(use)            
        load_i = bat_sim.iter_get_load(available[i], use)
        loads.append(load_i)

    return "Simple", hashrates, usage, loads, bat_sim

def print_hashes(hashrates):
    HASH_UNIT = 1e6
    print("Total hashes =", round(np.sum(hashrates) / HASH_UNIT, 2), "MH")
    
def get_usage(available):
    name, hashrates, usage, bat, bat_sim = get_usage_simple(available)
    #name, hashrates, usage, bat, bat_sim = get_usage_endor_example(available)
    print("Algo name: ", name)
    bat_sim.print_stats(len(available))
    print_hashes(hashrates)
    return name, list_to_pd(usage, available), list_to_pd(bat, available)

def list_to_pd(listt, df):
    return pd.DataFrame(listt, index=df.index.copy())

def simul_weather(pos):
    pos.loc[::5, [ELEVATION_KEY]] *= 0.6
    pos.loc[::3, [ELEVATION_KEY]] *= 0.8
    pos.loc[::8, [ELEVATION_KEY]] *= 0.3

    return pos

pos = get_sun_positions()
#print(pos)

proc = proc_data(pos)
elev = extr_data(proc)
name, usage, bat = get_usage(elev)
plot_sun(name, elev, bat, usage)
