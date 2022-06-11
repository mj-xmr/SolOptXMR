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

def get_y_c_by_10():
    # TODO: test by plotting
    y = []
    y.append(11)    # 0
    y.append(11.27) # 10
    y.append(11.5)  # 20
    y.append(11.7)  # 30
    y.append(11.9)  # 40
    y.append(12.0)  # 50
    y.append(12.15) # 60
    y.append(12.25) # 70
    y.append(12.3)  # 80
    y.append(12.43) # 90
    y.append(12.5)  # 100

    return y

def get_fun_interp(x, y):
    f = interpolate.interp1d(x, y)
    return f

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
    y = get_y_c_by_10()

    xx = get_x_interp()
    yy_c10 = percentage_to_voltage(xx, get_y_c_by_10)

    plt.plot(x, y, 'o', xx, yy_c10, '.')
    plt.title('Voltage to State of Charge (SoC) [%] at discharge')
    plt.xlabel('State of Charge (SoC) [%]')
    plt.ylabel('Voltage [V]')
    plt.legend(['c/10 meas.', 'c/10 inter.'])
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
