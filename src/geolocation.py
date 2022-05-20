#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# https://stackoverflow.com/a/11236372
# apt install gfortran libffi-dev
# pip3 install pvlib ephem pytz beautifulsoup4 cairosvg wget requests Pillow


import os
from pytz import timezone
import sunrise_lib

import geocoder

def get_lat_lon():
    return geocoder.ip('me')

def main():
    test()
    path_local = sunrise_lib.get_config_path('geo')
    config_geo = sunrise_lib.config_geo
    print("Config city:", config_geo.geo.city)
    print("Config lat:", config_geo.geo.lat)
    print("Config lon:", config_geo.geo.lon)
    
    print()
    g = get_lat_lon()
    city = g.city.lower()
    print("Detected city:", city)
    print("Detected lat/lon:", g.latlng)
    #print("Detected country:", g.country)

    if config_geo.geo.city != city:
        print("Please update city")
    if config_geo.geo.lat != g.latlng[0]:
        print("Please update lat")
    if config_geo.geo.lon != g.latlng[1]:
        print("Please update lon")
    
    print("Your config file is stored at:")
    print(path_local)

def test():
    g = get_lat_lon()
    assert len(g.latlng) == 2
    assert g.latlng[0] != 0
    assert g.latlng[1] != 0
    print("Detected lat/lon:", get_lat_lon().latlng)
    #print("City:", g.city)
    #run_main(elev, show_plots)


if __name__ == "__main__":
    main()
