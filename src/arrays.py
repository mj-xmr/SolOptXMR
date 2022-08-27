#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Helps in modelling arrays

import sunrise_lib

import os
import pvlib
import pandas as pd
import numpy as np
import traceback
import argparse
import dateutil
import datetime

import matplotlib
#matplotlib.use('Agg')
from matplotlib import pyplot as plt

def get_args():
    parser = argparse.ArgumentParser()
    sunrise_lib.add_date_arguments_to_parser(parser, '2019-06-01', 1)
    return parser.parse_args()

def plot_arrays(args, show=True):
    system = sunrise_lib.get_pv_system()
    print(system.num_arrays)
    for array in system.arrays:
        print(array.mount)

    loc = pvlib.location.Location(sunrise_lib.LAT, sunrise_lib.LON)
    mc = pvlib.modelchain.ModelChain(system, loc, aoi_model='physical',
                           spectral_model='no_loss')

    start_date = dateutil.parser.parse(args.start_date) + datetime.timedelta(hours=1)
    end_date = start_date + datetime.timedelta(days=args.days_horizon)
    dti = pd.date_range(start_date, end_date, freq='5min', tz=sunrise_lib.tz)
    weather = loc.get_clearsky(dti)
    mc.run_model(weather)

    fig, ax = plt.subplots()
    
    
    plt.gca().xaxis_date(sunrise_lib.tz)
    mc.results.ac.plot(label='Inverter')
    for array, pdc, array_internal in zip(system.arrays, mc.results.dc, sunrise_lib.get_arrays()):
        pdc.plot(label=f'{array.name}')
    sum_power = int(np.round(np.sum(mc.results.dc)))
    plt.title("Output as a fraction of the maximal output. Sum = {}".format(sum_power))
    plt.ylabel('System Output')
    plt.grid()
    plt.xticks(rotation=25, ha='right')
    plt.legend()
    if show:
        plt.show()    

def test():
    args = get_args()
    plot_arrays(args, False)

if __name__ == "__main__":
    args = get_args()
    plot_arrays(args)
    
