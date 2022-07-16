#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Source: http://www.scubaengineer.com/documents/lead_acid_battery_charging_graphs.pdf

import os
import datetime
import time
import traceback
import voltage_lib

def plot():
    plot_one(False)
    plot_one(True)
    
def plot_one(charge):
    import matplotlib.pyplot as plt
    x = voltage_lib.get_x(charge)
    xx = voltage_lib.get_x_interp(charge)
    discharge_rates = []

    legend = []
    for disc_rate in reversed(voltage_lib.get_keys(charge)):
        y = voltage_lib.battery_y(disc_rate, charge)
        print(len(x), len(y))
        plt.plot(x, y, 'o')
        yy_c10 = voltage_lib.percentage_to_voltage(xx, disc_rate, charge)
        plt.plot(xx, yy_c10, '.')
        legend.append('c/{} meas.'.format(disc_rate))
        legend.append('c/{} interp.'.format(disc_rate))

    plt.title('Voltage to State of Charge (SoC) [%] at ' + ('charge' if charge else 'discharge'))
    plt.xlabel('State of Charge (SoC) [%]')
    plt.ylabel('Voltage [V]')
    plt.legend(legend)
    plt.grid()
    plt.show()

if __name__ == "__main__":
    voltage_lib.test()
    plot()
