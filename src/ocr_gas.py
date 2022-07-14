#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# https://stackoverflow.com/a/11236372
# apt install gfortran libffi-dev
# pip3 install pvlib ephem pytz beautifulsoup4 cairosvg wget requests Pillow


import os
import sys
import traceback
import datetime
import time
import shutil
import sunrise_lib

sys.path.append('externals/GasPumpOCR-mj')
import headless

def test_single(image, script_dir, expected_val)
    detection = get_detection(imfile, script_dir)
    assert round(detection, 3) == expected_val
    print(image, ", Detection =", detection)

def test():
    script_dir_backlit  = 'externals/GasPumpOCR-mj/custom-scripts/lcd-glowing/'
    script_dir_gas_pump = 'externals/GasPumpOCR-mj/custom-scripts/gas-pump-dark/'
    test_single('data/img/lcd-glowing.jpg', script_dir_backlit, 11.7)
    test_single('data/img/gas-pump-13A95.jpg', script_dir_gas_pump, 13.95)
    test_single('data/img/gas-pump-49A95-mod-0.jpg', script_dir_gas_pump,  9.95)
    test_single('data/img/gas-pump-49A95-mod-1.jpg', script_dir_gas_pump, 19.95)

def get_detection(imfile=sunrise_lib.PATH_OCR_IMAGE, script_dir=sunrise_lib.config_volatile.paths.DIR_OCR_SCRIPT):
    if not os.path.isfile(imfile):
        imfile = "src/" + imfile
        if not os.path.isfile(imfile):
            raise IOError("File doesn't exist", imfile)
    print("Reading image file for OCR:", imfile)
    time_mod = os.path.getmtime(imfile)
    time_diff = time.time() - time_mod
    hour_diff = time_diff / 3600
    MAX_HOURS_DIFF = 0.5
    if hour_diff > MAX_HOURS_DIFF:
        print("WARNING! the OCR image file is older than the max of",
              MAX_HOURS_DIFF, "h, exactly:", round(hour_diff, 1), "h.",
              "You should capture a newer image.")
    
    if sunrise_lib.config_volatile.glob.ARCHIVE_IMAGES:
        fname = imfile.split('/')[-1] + str(round(time_mod)) + ".jpg"
        dirr = sunrise_lib.config_volatile.paths.DIR_ARCHIVE + "/ocr"
        os.makedirs(dirr, exist_ok=True)
        path_copy = dirr + "/" + fname
        if not os.path.isfile(path_copy):
            print("Archiving:", imfile, "to:", path_copy)
            shutil.copy(imfile, path_copy)
        else:
            print(path_copy, "already exists.")

    if not os.path.isdir(script_dir):
        script_dir = "src/" + script_dir
        if not os.path.isdir(script_dir):
            raise IOError("Dir doesn't exist", script_dir)
    print("Reading script dir for OCR:", script_dir)
    
    detection = headless.get_detection(imfile, script_dir)

    return detection

if __name__ == "__main__":
    test()

