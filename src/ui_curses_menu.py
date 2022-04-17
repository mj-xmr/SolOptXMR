#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import ui_lib
from cursesmenu import *
from cursesmenu.items import *

def cmenu():
    # Create the menu
    main_menu = ui_lib.read_main_menu()
    
    menu = CursesMenu("SolOptXMR", "Main menu")

    # Create some items

    for sub_name in main_menu[ui_lib.KEY_SUBMENUS]:
        sub_menu = main_menu[sub_name]
        #if isinstance(sub_menu, list):
        #    print(sub_menu, "list")
        print(sub_name, sub_menu)
        if ui_lib.KEY_SUBMENUS in sub_menu:
            pass
            ssub_menu = sub_menu[ui_lib.KEY_SUBMENUS]
            sselection_menu = SelectionMenu(ssub_menu)
            submenu_item = SubmenuItem(sub_name, sselection_menu, menu)
            menu.items.append(submenu_item)
        else:
            if sub_name in sub_menu:
                # Nested - dirty
                sub_menu = sub_menu[sub_name]
            #else:
                
            
            selection_menu = SelectionMenu(sub_menu)
            submenu_item = SubmenuItem(sub_name, selection_menu, menu)
            menu.items.append(submenu_item)
        

    # MenuItem is the base class for all items, it doesn't do anything when selected
    for key in main_menu:
        val = main_menu[key]
        if isinstance(val, list):
            pass
            #submenu = CursesMenu("Sub", "Sum menu")
            
            #selection_menu = SelectionMenu(val)
            
            
            
            #submenu_item = SubmenuItem("Submenu item", selection_menu, menu)
            #menu.items.append(submenu_item)
        else:
            pass
            #menu_item = MenuItem(key + ": " + val)
            #menu.items.append(menu_item)
    

    # A FunctionItem runs a Python function when selected
    #function_item = FunctionItem("Call a Python function", input, ["Enter an input"])

    # A CommandItem runs a console command
    #command_item = CommandItem("Run a console command", "touch hello.txt")

    # A SelectionMenu constructs a menu from a list of strings
    #selection_menu = SelectionMenu(["item1", "item2", "item3"])

    # A SubmenuItem lets you add a menu (the selection_menu above, for example)
    # as a submenu of another menu
    #submenu_item = SubmenuItem("Submenu item", selection_menu, menu)

    # Once we're done creating them, we just add the items to the menu
    #menu.items.append(menu_item)
    #menu.items.append(function_item)
    #menu.items.append(command_item)
    #menu.items.append(submenu_item)

    # Finally, we call show to show the menu and allow the user to interact
    menu.show()


def demo_original():
    # Create the menu
    menu = CursesMenu("Title", "Subtitle")

    # Create some items

    # MenuItem is the base class for all items, it doesn't do anything when selected
    menu_item = MenuItem("Menu Item")

    # A FunctionItem runs a Python function when selected
    function_item = FunctionItem("Call a Python function", input, ["Enter an input"])

    # A CommandItem runs a console command
    command_item = CommandItem("Run a console command", "touch hello.txt")

    # A SelectionMenu constructs a menu from a list of strings
    selection_menu = SelectionMenu(["item1", "item2", "item3"])

    # A SubmenuItem lets you add a menu (the selection_menu above, for example)
    # as a submenu of another menu
    submenu_item = SubmenuItem("Submenu item", selection_menu, menu)

    # Once we're done creating them, we just add the items to the menu
    menu.items.append(menu_item)
    menu.items.append(function_item)
    menu.items.append(command_item)
    menu.items.append(submenu_item)

    # Finally, we call show to show the menu and allow the user to interact
    menu.show()

cmenu()
#demo_original

