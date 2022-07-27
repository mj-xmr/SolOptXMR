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
DISCHARGE_RATE_40  = 40
DISCHARGE_RATE_100 = 100

CHARGE_RATE_DICT = {}
DISCHARGE_RATE_DICT = {}

def percentage_to_voltage(percentage, charge_status, discharge_rate=DISCHARGE_RATE_10):
    x = get_x(charge_status)
    y = battery_y(discharge_rate, charge_status)
    f = get_fun_interp(x, y)
    #print(percentage, "%")
    voltage = f(percentage)
    print(voltage, 'V ', percentage, "%")
    return voltage

def voltage_to_percentage(voltage, charge_status, discharge_rate=DISCHARGE_RATE_10):
    x = get_x(charge_status)
    y = battery_y(discharge_rate, charge_status)

    if voltage < y[0]:
        return x[0]
    if voltage > y[-1]:
        return x[-1]
    
    f = get_fun_interp(y, x)
    print(voltage, 'V ')
    percentage = f(voltage)
    print(voltage, 'V ', percentage, "%")
    return percentage

def charge_status_str_to_bool(status_str):
    status_str = status_str.lower()
    char1 = status_str[0]
    if char1 == 'c':
        return True
    elif char1 == 'd':
        return False

    raise IOError("Invalid charge status:", status_str) 

def battery_y(discharge_rate, charge=False):
    d = CHARGE_RATE_DICT if charge else DISCHARGE_RATE_DICT
    return d[discharge_rate]()

def get_start_end_range(charge, interp):
    start = 10 if charge else 0
    if interp:
        end = 121 if charge else 101
    else:
        end = 130 if charge else 110

    return start, end

def get_x(charge):
    start, end = get_start_end_range(charge, False)  
    return list(range(start, end, 10))

def get_x_interp(charge):
    start, end = get_start_end_range(charge, True)
    return list(range(start, end, 2))        


def get_keys(charge):
    if charge:
        return [DISCHARGE_RATE_40,  DISCHARGE_RATE_20, DISCHARGE_RATE_10, DISCHARGE_RATE_5]
    else:
        return [DISCHARGE_RATE_100, DISCHARGE_RATE_20, DISCHARGE_RATE_10, DISCHARGE_RATE_5, DISCHARGE_RATE_3]

def get_y_c_by_100():
    y = []
    y.append(11.7 )  # 0
    y.append(11.9 )  # 10
    y.append(12.1 )  # 20
    y.append(12.25)  # 30
    y.append(12.4 )  # 40
    y.append(12.5 )  # 50
    y.append(12.6 )  # 60
    y.append(12.65)  # 70
    y.append(12.66)  # 80
    y.append(12.665) # 90
    y.append(12.66)  # 100

    return y

def get_y_c_by_40_charge():
    y = []
    
    y.append(11.8 ) # 10
    y.append(12.3 ) # 20
    y.append(12.6 ) # 30
    y.append(12.75) # 40
    y.append(12.85) # 50
    y.append(12.9 ) # 60
    y.append(12.95) # 70
    y.append(13.  ) # 80
    y.append(13.2 ) # 90
    y.append(13.50) # 100
    y.append(14.50) # 110
    y.append(15.20) # 120

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

def get_y_c_by_20_charge():
    y = []
    
    y.append(12.15) # 10
    y.append(12.4 ) # 20
    y.append(12.7 ) # 30
    y.append(12.90) # 40
    y.append(13.05) # 50
    y.append(13.15) # 60
    y.append(13.20) # 70
    y.append(13.35) # 80
    y.append(13.52) # 90
    y.append(14.15) # 100
    y.append(15.10) # 110
    y.append(15.55) # 120

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

def get_y_c_by_10_charge():
    y = []
    
    y.append(12.37) # 10
    y.append(12.55) # 20
    y.append(12.8 ) # 30
    y.append(13.1 ) # 40
    y.append(13.25) # 50
    y.append(13.35) # 60
    y.append(13.40) # 70
    y.append(13.70) # 80
    y.append(14.10) # 90
    y.append(15.25) # 100
    y.append(15.70) # 110
    y.append(16.00) # 120

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


def get_y_c_by_5_charge():
    y = []
    
    y.append(12.6 ) # 10
    y.append(12.75) # 20
    y.append(13.0 ) # 30
    y.append(13.2 ) # 40
    y.append(13.4 ) # 50
    y.append(13.51) # 60
    y.append(13.7 ) # 70
    y.append(14.0 ) # 80
    y.append(15.25) # 90
    y.append(15.95) # 100
    y.append(16.25) # 110
    y.append(16.4 ) # 120

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
DISCHARGE_RATE_DICT[DISCHARGE_RATE_40]  = get_y_c_by_100
DISCHARGE_RATE_DICT[DISCHARGE_RATE_100] = get_y_c_by_100

CHARGE_RATE_DICT[DISCHARGE_RATE_3]   = get_y_c_by_5_charge
CHARGE_RATE_DICT[DISCHARGE_RATE_5]   = get_y_c_by_5_charge
CHARGE_RATE_DICT[DISCHARGE_RATE_10]  = get_y_c_by_10_charge
CHARGE_RATE_DICT[DISCHARGE_RATE_20]  = get_y_c_by_20_charge
CHARGE_RATE_DICT[DISCHARGE_RATE_40]  = get_y_c_by_40_charge
CHARGE_RATE_DICT[DISCHARGE_RATE_100] = get_y_c_by_40_charge


def get_fun_interp(x, y):
    # TODO: Use linear regression of 2nd order?
    f = interpolate.interp1d(x, y)
    return f

"""
res = stats.linregress(x, y)
plt.plot(x, y, 'o', label='original data')
plt.plot(x, res.intercept + res.slope*x, 'r', label='fitted line')
"""

def test():
    print("voltage")
    assert True == charge_status_str_to_bool("cHarge") == charge_status_str_to_bool("CHarge") == charge_status_str_to_bool("c")
    assert False == charge_status_str_to_bool("dIsch") == charge_status_str_to_bool("DIScHARGE") == charge_status_str_to_bool("d")
    charge = False # TODO: Same for charge
    x = get_x(charge)
    y = get_y_c_by_10()
    assert len(x) == 11 # both 0 and 100, therefore 11, not just 10
    assert len(y) == len(x)
    x_interp = get_x_interp(charge)
    assert len(x_interp) > len(x)
    yy_c10 = percentage_to_voltage(x_interp, charge, DISCHARGE_RATE_10)
    assert len(x_interp) == len(yy_c10)

    # Overvoltage
    try:
        percentage_to_voltage(110)
        assert False
    except:
        assert True # Exception thrown

    assert 40 < voltage_to_percentage(12, charge) < 60 # happy

    assert voltage_to_percentage(9,  charge) == 0 # Corner low
    assert voltage_to_percentage(14, charge) == 100 # Corner high

def plot():
    import voltage_plot
    voltage_plot.plot()

if __name__ == "__main__":
    test()
    #plot()
