#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import traceback
import datetime
import time
import shutil
import sunrise_lib

def capture(host, imfile):
    """
    Capture image in a simple mode.
    """
    print("Trying to save image file for OCR:", imfile)
    cmd = "fswebcam -r 640x480 --jpeg 85 --delay 1 " + imfile
    if host == 'localhost':
        sunrise_lib.run_cmd(cmd, True)
    else:
        results = sunrise_lib.run_ssh_cmd(host, cmd).stderr.read()
        print(results)

def test():
    capture('localhost', '/tmp/img.jpg')

if __name__ == "__main__":
    test()

