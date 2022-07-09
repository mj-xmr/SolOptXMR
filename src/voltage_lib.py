#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import datetime
import time
import traceback

from scipy import interpolate

def get_x():
    return list(range(0, 110, 10))

def get_x_interp():
    return list(range(0, 101, 2))        

def get_y_c_by_3():
    # TODO: Add other discharge rates, also into battery.json
    y = []
    y.append(9.5)    # 0
    y.append(10) # 10
    y.append(10.4)  # 20
    y.append(10.7)  # 30
    y.append(11)  # 40
    y.append(11.2)  # 50
    y.append(11.35) # 60
    y.append(11.5) # 70
    y.append(11.65)  # 80
    y.append(11.7) # 90
    y.append(11.75)  # 100

    return y

def get_y_c_by_10():
    # TODO: Add other discharge rates, also into battery.json
    y = []
    y.append(11)    # 0
    y.append(11.27) # 10
    y.append(11.5)  # 20
    y.append(11.7)  # 30
    y.append(11.9)  # 40
    y.append(12.0)  # 50
    y.append(12.15) # 60
    y.append(12.25) # 70
    y.append(12.38)  # 80
    y.append(12.45) # 90
    y.append(12.5)  # 100

    return y

def get_y_c_by_100():
    y = []
    y.append(11.7)    # 0
    y.append(11.9) # 10
    y.append(12.15)  # 20
    y.append(12.25)  # 30
    y.append(12.4)  # 40
    y.append(12.5)  # 50
    y.append(12.6) # 60
    y.append(12.65) # 70
    y.append(12.66)  # 80
    y.append(12.665) # 90
    y.append(12.66)  # 100

    return y
def get_fun_interp(x, y):
    # TODO: Use linear regression of 2nd order?
    f = interpolate.interp1d(x, y)
    return f

"""
res = stats.linregress(x, y)
plt.plot(x, y, 'o', label='original data')
plt.plot(x, res.intercept + res.slope*x, 'r', label='fitted line')
"""

def percentage_to_voltage(percentage, battery_y=get_y_c_by_10):
    x = get_x()
    y = battery_y()
    f = get_fun_interp(x, y)
    #print(percentage, "%")
    voltage = f(percentage)
    print(voltage, 'V ', percentage, "%")
    return voltage

def voltage_to_percentage(voltage, battery_y=get_y_c_by_10):
    x = get_x()
    y = battery_y()

    if voltage < y[0]:
        return x[0]
    if voltage > y[-1]:
        return x[-1]
    
    f = get_fun_interp(y, x)
    print(voltage, 'V ')
    percentage = f(voltage)
    print(voltage, 'V ', percentage, "%")
    return percentage

def plot():
    import matplotlib.pyplot as plt
    x = get_x()
    xx = get_x_interp()
    discharge_rates = []
    
    discharge_rates.append((100, get_y_c_by_100))
    discharge_rates.append((10,  get_y_c_by_10))
    discharge_rates.append((3,   get_y_c_by_3))

    legend = []
    for disc_rate in discharge_rates:
        y = disc_rate[1]()
        plt.plot(x, y, 'o')
        yy_c10 = percentage_to_voltage(xx, disc_rate[1])
        plt.plot(xx, yy_c10, '.')
        legend.append('c/{} meas.'.format(disc_rate[0]))
        legend.append('c/{} interp.'.format(disc_rate[0]))

    plt.title('Voltage to State of Charge (SoC) [%] at discharge')
    plt.xlabel('State of Charge (SoC) [%]')
    plt.ylabel('Voltage [V]')
    plt.legend(legend)
    plt.grid()
    plt.show()

def test():
    print("voltage")
    x = get_x()
    y = get_y_c_by_10()
    assert len(x) == 11 # 0 and 100
    assert len(y) == len(x)
    x_interp = get_x_interp()
    assert len(x_interp) > len(x)
    yy_c10 = percentage_to_voltage(x_interp, get_y_c_by_10)
    assert len(x_interp) == len(yy_c10)

    # Overvoltage
    try:
        percentage_to_voltage(110)
        assert False
    except:
        assert True # Exception thrown

    assert 40 < voltage_to_percentage(12) < 60 # happy

    assert voltage_to_percentage(9) == 0 # Corner low
    assert voltage_to_percentage(14) == 100 # Corner high

if __name__ == "__main__":
    test()
    #plot()
