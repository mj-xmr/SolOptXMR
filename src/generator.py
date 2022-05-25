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
import math

from pytz import timezone
from matplotlib import pyplot as plt

import sunrise_lib
import weather_lib
import kraken
from profitability import POW_Coin

from python_json_config import ConfigBuilder

config = sunrise_lib.config
config_batteries = sunrise_lib.config_batteries
config_system = sunrise_lib.config_system
battery = config_batteries.batteries[0] # TODO: extend

ELEVATION_KEY = config.generator.ELEVATION_KEY
BUILD_DIR='build/'  # TODO: Config

MIN_POWER = 0
# Not changable: solar system params
MAX_POWER = config.generator.MAX_POWER
MAX_USAGE = battery['max_discharge_amp']
MIN_CAPACITY = battery['min_load_amph'] * battery['count']
MAX_CAPACITY = battery['max_capacity_amph'] * battery['count']
MUL_POWER_2_CAPACITY = config.generator.MUL_POWER_2_CAPACITY
T_DELTA_HOURS = config.generator.T_DELTA_HOURS
DATE_NOW = sunrise_lib.DATE_NOW
PATH_POSITIONS_BASE = config.sunrise_lib.DIR_TMP + config.generator.PATH_POSITIONS # TODO: Parametrize based on keys
ELECTRICITY_PRICE = config.generator.ELECTRICITY_PRICE
POOL_FEE = config.generator.POOL_FEE / 100  # Must be a float!
TARGET_PRICE = config.generator.TARGET_PRICE

path_positions_txt = f"{PATH_POSITIONS_BASE}.txt"

def get_date_range(start_date, days_horizon):
    tz = sunrise_lib.tz
    dti = pd.date_range(start_date, periods=days_horizon * 24, freq="H")
    #print(dti)
    return dti

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
        dti = get_date_range(start_date, days_horizon)
        
        pos = pvlib.solarposition.get_solarposition(dti, sunrise_lib.LAT, sunrise_lib.LON)
        b = datetime.datetime.now()
        
        with open(path_positions, "wb") as handle:
            print("Dumping data to:", path_positions)
            pickle.dump(pos, handle, protocol=pickle.HIGHEST_PROTOCOL)

    c = b - a
    print("Prepared data in", c.total_seconds(), "s")
    print(pos)
    return pos

def get_arrays():
    arrays = []
    for array in sunrise_lib.config_arrays.arrays:
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
    system = pvlib.pvsystem.PVSystem(arrays=arrays, inverter_parameters=dict(pdc0=3))
    return system

def get_power(start_date, days_horizon=3, unpickle=True):
    print("get_power()", ", horizon", days_horizon)
    system = get_pv_system()
    print(system.num_arrays)
    for array in system.arrays:
        print(array.mount)

    loc = pvlib.location.Location(sunrise_lib.LAT, sunrise_lib.LON)
    mc = pvlib.modelchain.ModelChain(system, loc, aoi_model='physical',
                           spectral_model='no_loss')


    dti = get_date_range(start_date, days_horizon)
    #times = pd.date_range('2019-06-01 03:00', '2019-06-01 20:00', freq='5min',
    #                  tz='Etc/GMT+1')
    weather = loc.get_clearsky(dti)
    mc.run_model(weather)

    watt_peak = get_arrays()[0]['watt_peak'] # TODO: assuming each panel uses the same Wp
    watt = mc.results.ac * watt_peak * 0.8 # TODO: damping the result by 20%
    #watt.columns = ['Watt']
    #print("Watt")
    #print(watt)

    #plt.plot(watt)
    #plt.show()
    
    plot = False
    if plot:
        fig, ax = plt.subplots()
        for array, pdc, array_internal in zip(system.arrays, mc.results.dc, get_arrays()):
            pdc.plot(label=f'{array.name}')            
        mc.results.ac.plot(label='Inverter')
        plt.ylabel('System Output')
        plt.grid()
        plt.legend()
        plt.show()

    return watt

    

def plot_sun(name, elev, bat, usage, show_plots):
    #print(elev)
    plt.gca().xaxis_date(sunrise_lib.tz)
    plt.title("Algo: " + name)
    plt.xlabel("Time")
    plt.xticks(rotation=25, ha='right')
    plt.ylabel("Power [W] [Wh] & capacity [Ah]")
    plt.plot(list_to_pd([MAX_CAPACITY]   * len(elev), elev))
    plt.plot(bat,   'g')
    plt.plot(list_to_pd([MIN_CAPACITY]   * len(elev), elev))
    plt.plot(elev,  'y')
    plt.plot(list_to_pd([MAX_USAGE]      * len(elev), elev), 'r')
    plt.plot(usage, 'b')
    plt.grid()
    plt.legend(['max bat charge', 'bat charge', 'min bat charge', 'sun input', 'max power usage', 'power usage'])
    os.makedirs(BUILD_DIR, exist_ok=True)
    fout_rel_path = "{}/fig-{}.png".format(BUILD_DIR, name)
    print("Saving figure to:",fout_rel_path)
    plt.savefig(fout_rel_path)
    if show_plots:
        plt.show()

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

def get_usage_endor_example(available, battery_charge, horizon):
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

def get_usage_simple(available, battery_charge, horizon):
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

def get_usage(args, available, algo, battery_charge=0, horizon=0):
    name, hashrates, usage, bat, bat_sim, incomes, costs, effs  = algo(args, available, battery_charge, horizon)
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
    pos[::5] *= 0.6
    pos[::3] *= 0.8
    pos[::8] *= 0.3

    return pos

def add_weather(elev, horizon):
    #elev = elev[0, :].to_list()
    print("hori", horizon)
    weather = weather_lib.get_weather(horizon)
    curr_hour = sunrise_lib.DATE_NOW.hour
    for i in range(0, len(elev)):
        day = math.floor((i+curr_hour) / 24)
        if day < len(weather):
            wday = weather[day]
        else:
            wday = weather[-1]
        elev.at[elev.index[i]] *= wday
        #elev.iloc[i,0] *= wday
        #print(elev.iloc[i,0])
    return elev

def proc_data(elev, is_simul_weather=False, horizon=0):
    if is_simul_weather:
        pos = simul_weather(elev)
    else:
        pos = add_weather(elev, horizon)
    pos = adj_losses(pos) # TODO: This will be a solar panel parameter

    print("Dumping data to:", path_positions_txt)
    np.savetxt(path_positions_txt, elev)
    
    return pos

def extr_data(pos):
    elev = pos[ELEVATION_KEY]
    return elev

def adj_losses(pos):
    # TODO: This will be a solar panel parameter
    #pos.loc[pos[ELEVATION_KEY] < MIN_POWER, [ELEVATION_KEY]] = MIN_POWER
    #pos.loc[pos[ELEVATION_KEY] > MAX_POWER, [ELEVATION_KEY]] = MAX_POWER
    #pos.loc[pos.iloc[0] < MIN_POWER, pos.iloc[0]] = MIN_POWER
    #pos.loc[pos[:] > MAX_POWER, 0] = MAX_POWER
    return pos

def run_main(args, elev, show_plots, battery_charge=0, horizon=0):
    run_algo(args, elev, show_plots, get_usage_simple, battery_charge, horizon)
    run_algo(args, elev, show_plots, get_usage_endor_example, battery_charge, horizon)

def run_algo(args, elev, show_plots, algo, battery_charge, horizon):
    name, usage, bat = get_usage(args, elev, algo, battery_charge, horizon)
    plot_sun(name, elev, bat, usage, show_plots)


def test(show_plots=False):
    start_date = datetime.datetime(2020, 1, 1)
    days_horizon = 30 * 9
    #pos = get_sun_positions(start_date, days_horizon)
    elev = get_power(start_date, days_horizon)
    #print(elev)

    #elev = extr_data(pos)
    
    #proc = proc_data(elev, True)
    proc = proc_data(elev, False, 3)
    #
    #run_main(proc, show_plots)

if __name__ == "__main__":
    test(True)
