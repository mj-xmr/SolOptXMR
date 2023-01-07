#!/usr/bin/env python3
# -*- coding: utf-8 -*-


import matplotlib
matplotlib.use('Agg')
from matplotlib import pyplot as plt

import sys
import ui_lib
import sunrise_lib
import weather_lib
import voltage_lib
import profitability
import generator
import kraken
import geolocation
import arrays
import ocr_gas
import sensors

def test(only_net_independent=False):
    modules = []
    
    modules.append(ui_lib)
    modules.append(sunrise_lib)
    #modules.append(arrays) # Needs UI
    modules.append(voltage_lib)
    modules.append(ocr_gas)
    modules.append(sensors)
    # Need network:
    if not only_net_independent:
        modules.append(generator) # TODO: Make network independent or implementa a fallback
        modules.append(weather_lib)
        # modules.append(profitability) # TODO: Currently unmaintained
        modules.append(kraken)
        modules.append(geolocation)
    
    for module in modules:
        module.test()

    print("All tests ended")
    print("(except profitability)")
    
if __name__ == "__main__":
    if len(sys.argv) > 1:
        print("Testing only network independent modules")
        test(True)
    else: # Default
        print("Testing all")
        test()
