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
def get_weather(horizon=3):
    assert horizon > 0
    MIN_WEATHER = 0.1
    try:
        #print(config_geo.geo.country)
        #print(config_geo.geo.city)

        dnow = sunrise_lib.DATE_NOW.date().isoformat()
        dirr = sunrise_lib.DIR_TMP + '/weather-pred/{}/'.format(dnow)

        path_template = dirr + '/weather-pic-{}.png'
        if not os.path.isdir(dirr):
            download_weather(dirr, path_template, horizon)
            # TODO: On error: log error and delete the dirr

        #fname = fname_template.format(dnow)
        #return 0
        #png_file =
        #if not os.path.isfile(png_file):
        #    download_weather(png_file)
        #else:
        #    print("File " + png_file + " exists.") 

        ret = []
        for ires in range(0, horizon):
            png_file = path_template.format(get_date_from_now_iso(ires))
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
                #print(pix, dic[pix])

            max_yellow = 0.34 # based on '//c.tadst.com/gfx/w/svg/wt-1.svg'

            wh = width * height
            yellow_relat = pix_yellow / wh / max_yellow
            #print("Yellow = ", pix_yellow, yellow_relat * 100)

            #print(page.text)

            if yellow_relat < MIN_WEATHER:
                yellow_relat = MIN_WEATHER

            ret.append(round(yellow_relat, 3))
        print("Weather pred:", ret)
        return ret
    except Exception:
        print(traceback.format_exc())
        return [MIN_WEATHER] * horizon

# TODO: Create other, alternative implementations
def download_weather(dirr, path_template, horizon):
    url = "https://www.timeanddate.com/weather/{}/{}/ext".format(config_geo.geo.country, config_geo.geo.city)
    print("Calling:")
    print(url)
    page = requests.get(url)
    #print(page)
    soup = BeautifulSoup(page.content, "html.parser")
    results = soup.findAll(class_="mtt")
    #print(results.prettify())
    print("Results len =", len(results), "horizon =", horizon)
    assert horizon < len(results) 
    os.makedirs(dirr) # So far all succeeded. Mark this by creating the directory
    for ele in results:
        img_url = ele['src']
        print(img_url)
        img_fname = img_url.split('/')[-1]
        if os.path.isfile(sunrise_lib.DIR_TMP + '/' + img_fname):
            print("Already downloaded:", img_fname)
            continue
        #img_url = '//c.tadst.com/gfx/w/svg/wt-33.svg'
        #img_url = '//c.tadst.com/gfx/w/svg/wt-1.svg'
        img_url = img_url.replace('//', 'https://')
        filenamedload = wget.download(img_url, out=sunrise_lib.DIR_TMP)
        #filename = '/tmp/a/wt-33.svg'
        print(filenamedload)
        
    for ires in range(0, len(results)):
        png_file = path_template.format(get_date_from_now_iso(ires))
        img_url = results[ires]['src']
        filename = sunrise_lib.DIR_TMP + '/' + img_url.split('/')[-1]
        svg_code = sunrise_lib.read_file(filename)
        print("Writing to " + png_file)
        svg2png(bytestring=svg_code, write_to=png_file)

def get_date_from_now_iso(ihorizon):
    dpred = sunrise_lib.DATE_NOW.date() + datetime.timedelta(days=ihorizon)
    dpred_iso = dpred.isoformat()
    return dpred_iso

def test():
    get_weather()    

if __name__ == "__main__":
    test()
    
