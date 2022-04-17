#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import json
import sunrise_lib

PATH_THIS = os.path.dirname(os.path.realpath(__file__))

DIR_JSON = PATH_THIS + '/ui-menus/'
KEY_SUBMENUS = 'submenus'

def read_json(fpath):
    print('reading', fpath)
    content = sunrise_lib.read_file(fpath)
    return json.loads(content)
    #with open(fpath) as fin:
    #    json.load(fin)

def read_menu(name):
    menu = read_json(DIR_JSON + name + '.json')
    return menu
    
def read_menu_parse(name):
    menu = read_menu(name)
    if KEY_SUBMENUS in menu:
        print(menu[KEY_SUBMENUS])
        for submenu in menu[KEY_SUBMENUS]:
            print(name)
            submenu_data = read_menu(submenu)
            menu[submenu] = submenu_data
    return menu

def test_all_jsons():
    for jfile in sorted(os.listdir(DIR_JSON)):
        read_json(DIR_JSON + jfile)
    print("Test all concluded")

def read_main_menu():
    menu_main = read_menu_parse('main')
    print(menu_main)
    print(json.dumps(menu_main, indent=4, sort_keys=False))
    #parsed = json.loads(str(menu_main))
    #print(json.dumps(parsed, indent=4, sort_keys=True))
    return menu_main

def test():
    test_all_jsons()
    read_menu_parse('geo')
    read_menu_parse('main')
    read_main_menu()

if __name__ == "__main__":
    test()
