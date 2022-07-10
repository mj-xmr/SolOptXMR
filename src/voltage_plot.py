#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Source: http://www.scubaengineer.com/documents/lead_acid_battery_charging_graphs.pdf

import os
import datetime
import time
import traceback
import voltage_lib

def plot():
    import matplotlib.pyplot as plt
    x = voltage_lib.get_x()
    xx = voltage_lib.get_x_interp()
    discharge_rates = []

    legend = []
    for disc_rate in reversed(voltage_lib.DISCHARGE_RATE_DICT.keys()):
        y = voltage_lib.DISCHARGE_RATE_DICT[disc_rate]()
        plt.plot(x, y, 'o')
        yy_c10 = voltage_lib.percentage_to_voltage(xx, disc_rate)
        plt.plot(xx, yy_c10, '.')
        legend.append('c/{} meas.'.format(disc_rate))
        legend.append('c/{} interp.'.format(disc_rate))

    plt.title('Voltage to State of Charge (SoC) [%] at discharge')
    plt.xlabel('State of Charge (SoC) [%]')
    plt.ylabel('Voltage [V]')
    plt.legend(legend)
    plt.grid()
    plt.show()

if __name__ == "__main__":
    voltage_lib.test()
    plot()
