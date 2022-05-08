#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# https://stackoverflow.com/a/11236372
# apt install gfortran libffi-dev
# pip3 install pvlib ephem pytz beautifulsoup4 cairosvg wget requests Pillow

import sunrise_lib

import os
import pvlib
import datetime
import pandas as pd
import time
import traceback
from subprocess import PIPE, run
from pathlib import Path


from pytz import timezone
import requests
from bs4 import BeautifulSoup
import wget
from cairosvg import svg2png

from PIL import Image

#from tzlocal import get_localzone # $ pip install tzlocal

config_geo = sunrise_lib.config_geo

TESTING = False
#TESTING = True

# TODO: Extent the prediction to multiple days ahead
def get_weather(horizon=3):
    try:
        print(config_geo.geo.country)
        print(config_geo.geo.city)
        url = "https://www.timeanddate.com/weather/{}/{}/ext".format(config_geo.geo.country, config_geo.geo.city)
        page = requests.get(url)
        print(page)
        soup = BeautifulSoup(page.content, "html.parser")
        results = soup.find(class_="mtt")
        img_url = results['src']
        #print(results.prettify())
        print(img_url)

        #img_url = '//c.tadst.com/gfx/w/svg/wt-33.svg'
        #img_url = '//c.tadst.com/gfx/w/svg/wt-1.svg'
        img_url = img_url.replace('//', 'https://')
        filename = wget.download(img_url, out=sunrise_lib.DIR_TMP)
        #filename = '/tmp/a/wt-33.svg'
        print(filename)
        
        svg_code = sunrise_lib.read_file(filename)
        png_file = sunrise_lib.DIR_TMP + '/output.png'
        print("Writing to " + png_file)
        svg2png(bytestring=svg_code, write_to=png_file)

        im = Image.open(png_file, 'r')
        width, height = im.size
        pixel_values = list(im.getdata())

        dic = {}
        for pix in pixel_values:
            if pix[0] == 0 and pix[1] == 0 and pix[2] == 0:
                continue
            if pix[0] == 255 and pix[1] == 255 and pix[2] == 255:
                continue
            if pix in dic:
                dic[pix] += 1
            else:
                dic[pix] = 1
            #print(pix)

        pix_yellow = 0

        for pix in dic:
            val = dic[pix]
            if val < 20:
                continue
            if pix[0] == 255:
                pix_yellow = val
            print(pix, dic[pix])

        max_yellow = 0.34 # based on '//c.tadst.com/gfx/w/svg/wt-1.svg'

        wh = width * height
        yellow_relat = pix_yellow / wh / max_yellow
        print("Yellow = ", pix_yellow, yellow_relat * 100)

        #print(page.text)

        if yellow_relat == 0:
            yellow_relat = 0.1

        return round(yellow_relat, 4)
    except Exception:
        print(traceback.format_exc())
        return 0

def test():
    get_weather()    

if __name__ == "__main__":
    test()
    
