#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# https://stackoverflow.com/a/11236372
# apt install gfortran libffi-dev
# pip3 install pvlib ephem pytz beautifulsoup4 cairosvg wget requests Pillow


import os
import traceback
import cv2 
import pytesseract
import numpy as np
import matplotlib.pyplot as plt

# get grayscale image
def get_grayscale(image):
    return cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

# noise removal
def remove_noise(image):
    return cv2.medianBlur(image,5)
 
#thresholding
def thresholding(image):
    return cv2.threshold(image, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)[1]

#dilation
def dilate(image):
    kernel = np.ones((5,5),np.uint8)
    return cv2.dilate(image, kernel, iterations = 1)
    
#erosion
def erode(image):
    kernel = np.ones((5,5),np.uint8)
    return cv2.erode(image, kernel, iterations = 1)

#opening - erosion followed by dilation
def opening(image):
    kernel = np.ones((5,5),np.uint8)
    return cv2.morphologyEx(image, cv2.MORPH_OPEN, kernel)

#canny edge detection
def canny(image):
    return cv2.Canny(image, 100, 200)

#skew correction
def deskew(image):
    coords = np.column_stack(np.where(image > 0))
    angle = cv2.minAreaRect(coords)[-1]
    if angle < -45:
        angle = -(90 + angle)
    else:
        angle = -angle
    (h, w) = image.shape[:2]
    center = (w // 2, h // 2)
    M = cv2.getRotationMatrix2D(center, angle, 1.0)
    rotated = cv2.warpAffine(image, M, (w, h), flags=cv2.INTER_CUBIC, borderMode=cv2.BORDER_REPLICATE)
    return rotated

#template matching
def match_template(image, template):
    return cv2.matchTemplate(image, template, cv2.TM_CCOEFF_NORMED) 

def test():
    imfile = 'data/img/lcd-volt-tilted.jpg'
    imfile = 'data/img/lcd-volt-90.jpg'
    imfile = 'data/img/lcd-volt.jpg'
    if not os.path.isfile(imfile):
        raise IOError("File doesn't exist", imfile)
    img = cv2.imread(imfile)
    print(img)
    if len(img) == 0:
        raise IOError("Image empty", imfile)
    # Adding custom options
    #custom_config = r'--oem 3 --psm 6'
    #imgNew = erode(img)
    #print("after")
    #print(imgNew)
    ax = plt.figure()
    ax.imshow(img)
    #plt.imshow(imgNew)
    plt.show()
    #print("Detection:")
    #print(pytesseract.image_to_string(img, config=custom_config))


if __name__ == "__main__":
    test()
