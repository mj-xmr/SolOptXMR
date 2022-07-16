#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Source: http://www.scubaengineer.com/documents/lead_acid_battery_charging_graphs.pdf

import os
import datetime
import time
import traceback

from scipy import interpolate

DISCHARGE_RATE_3   = 3
DISCHARGE_RATE_5   = 5
DISCHARGE_RATE_10  = 10
DISCHARGE_RATE_20  = 20
DISCHARGE_RATE_100 = 100

DISCHARGE_RATE_DICT = {}

def percentage_to_voltage(percentage, discharge_rate=DISCHARGE_RATE_10):
    x = get_x()
    y = battery_y(discharge_rate)
    f = get_fun_interp(x, y)
    #print(percentage, "%")
    voltage = f(percentage)
    print(voltage, 'V ', percentage, "%")
    return voltage

def voltage_to_percentage(voltage, discharge_rate=DISCHARGE_RATE_10):
    x = get_x()
    y = battery_y(discharge_rate)

    if voltage < y[0]:
        return x[0]
    if voltage > y[-1]:
        return x[-1]
    
    f = get_fun_interp(y, x)
    print(voltage, 'V ')
    percentage = f(voltage)
    print(voltage, 'V ', percentage, "%")
    return percentage

def get_x():
    return list(range(0, 110, 10))

def get_x_interp():
    return list(range(0, 101, 2))        

# TODO: Add other discharge rates, also into battery.json

def get_y_c_by_100():
    y = []
    y.append(11.7,  11.8 )  # 0
    y.append(11.9,  11.8 )  # 10
    y.append(12.1,  12.3 )  # 20
    y.append(12.25, 12.6 )  # 30
    y.append(12.4,  12.75)  # 40
    y.append(12.5,  12.85)  # 50
    y.append(12.6,  12.9 )  # 60
    y.append(12.65, 12.95) # 70
    y.append(12.66, ) # 80
    y.append(12.665)# 90
    y.append(12.66) # 100

    return y

def get_y_c_by_20(): # Could be measured better
    y = []
    y.append(11.45)  # 0
    y.append(11.65)  # 10
    y.append(11.9)   # 20
    y.append(12.11)  # 30
    y.append(12.25)  # 40
    y.append(12.32)  # 50
    y.append(12.42)  # 60
    y.append(12.5)   # 70
    y.append(12.6)   # 80
    y.append(12.63)  # 90
    y.append(12.64)  # 100

    return y

def get_y_c_by_10():
    y = []
    y.append(11)    # 0
    y.append(11.27) # 10
    y.append(11.5)  # 20
    y.append(11.7)  # 30
    y.append(11.9)  # 40
    y.append(12.0)  # 50
    y.append(12.15) # 60
    y.append(12.25) # 70
    y.append(12.38) # 80
    y.append(12.45) # 90
    y.append(12.5)  # 100

    return y

def get_y_c_by_5():
    y = []
    y.append(10.2)  # 0
    y.append(10.6)  # 10
    y.append(10.9)  # 20
    y.append(11.2)  # 30
    y.append(11.4)  # 40
    y.append(11.55) # 50
    y.append(11.7)  # 60
    y.append(11.8)  # 70
    y.append(11.9)  # 80
    y.append(12)    # 90
    y.append(12.1)  # 100

    return y

def get_y_c_by_3():
    y = []
    y.append(9.5)   # 0
    y.append(10)    # 10
    y.append(10.4)  # 20
    y.append(10.7)  # 30
    y.append(11)    # 40
    y.append(11.2)  # 50
    y.append(11.35) # 60
    y.append(11.5)  # 70
    y.append(11.65) # 80
    y.append(11.7)  # 90
    y.append(11.75) # 100

    return y

DISCHARGE_RATE_DICT[DISCHARGE_RATE_3]   = get_y_c_by_3
DISCHARGE_RATE_DICT[DISCHARGE_RATE_5]   = get_y_c_by_5
DISCHARGE_RATE_DICT[DISCHARGE_RATE_10]  = get_y_c_by_10
DISCHARGE_RATE_DICT[DISCHARGE_RATE_20]  = get_y_c_by_20
DISCHARGE_RATE_DICT[DISCHARGE_RATE_100] = get_y_c_by_100

def get_fun_interp(x, y):
    # TODO: Use linear regression of 2nd order?
    f = interpolate.interp1d(x, y)
    return f

"""
res = stats.linregress(x, y)
plt.plot(x, y, 'o', label='original data')
plt.plot(x, res.intercept + res.slope*x, 'r', label='fitted line')
"""

def battery_y(discharge_rate):
    return DISCHARGE_RATE_DICT[discharge_rate]() 

def test():
    print("voltage")
    x = get_x()
    y = get_y_c_by_10()
    assert len(x) == 11 # both 0 and 100, therefore 11, not just 10
    assert len(y) == len(x)
    x_interp = get_x_interp()
    assert len(x_interp) > len(x)
    yy_c10 = percentage_to_voltage(x_interp, DISCHARGE_RATE_10)
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

def plot():
    import voltage_plot
    voltage_plot.plot()

if __name__ == "__main__":
    test()
    #plot()
