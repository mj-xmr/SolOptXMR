#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Helps in modelling arrays

import sunrise_lib

import os
import pvlib
import pandas as pd
import traceback

from matplotlib import pyplot as plt


def plot_arrays():
    system = sunrise_lib.get_pv_system()
    print(system.num_arrays)
    for array in system.arrays:
        print(array.mount)

    loc = pvlib.location.Location(sunrise_lib.LAT, sunrise_lib.LON)
    mc = pvlib.modelchain.ModelChain(system, loc, aoi_model='physical',
                           spectral_model='no_loss')

    dti = pd.date_range('2019-06-01 02:00', '2019-06-01 23:00', freq='5min', tz=sunrise_lib.tz)
    weather = loc.get_clearsky(dti)
    mc.run_model(weather)

    fig, ax = plt.subplots()
    
    plt.title("Output as a fraction of the maximal output")
    plt.gca().xaxis_date(sunrise_lib.tz)
    mc.results.ac.plot(label='Inverter')
    for array, pdc, array_internal in zip(system.arrays, mc.results.dc, sunrise_lib.get_arrays()):
        pdc.plot(label=f'{array.name}')      
    plt.ylabel('System Output')
    plt.grid()
    plt.xticks(rotation=25, ha='right')
    plt.legend()
    plt.show()    


if __name__ == "__main__":
    plot_arrays()
    
