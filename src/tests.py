#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import ui_lib
import sunrise_lib
import weather_lib
import profitability
import generator
import kraken

def test():
    modules = []
    
    modules.append(ui_lib)
    modules.append(sunrise_lib)
    # Needs network:
    modules.append(generator) # TODO: Make network independent or implementa a fallback
    modules.append(weather_lib)
    modules.append(profitability)
    modules.append(kraken)
    
    
    for module in modules:
        module.test()

if __name__ == "__main__":
    test()
