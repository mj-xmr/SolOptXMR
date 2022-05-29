# Configuration
Listed below are the separate configuration files, generated from templates after running the `./util/config.sh` script.

## Arrays
Inside the `~/.config/solar/arrays.json` it's important to understand, that an array's parameter `surface_tilt` accepts degrees and 0 means laying flat horizontally on a roof, while 90 means hanging flat vertically on an external wall. 
`surface_azimuth` uses 0 as north, 90 as east, 180 as south and 270 as west.

## Geo
Inside the `~/.config/solar/geo.json`, which is expected to be modified by you, according to the physical location of the solar farm, the northern hemisphere and eastern side of globe are represented with positive numbers of Lat / Lon in degrees. 
Southern hemisphere and western side of globe use negative numbers of Lat / Lon.

The variable `city` is meant to deliver weather reports.
It is possible, that your farm's location is so remote, that the city where it belongs to, isn't listed on the weather reports.
In such cases, please enter the nearest largest city, until the script stops complaining.

## Computers
The mining rig has to be defined in the `~/.config/solar/computers.json` file. 
Currently the system doesn't work very well with the number of mining machines larger than 2.
In case a single computer should be disabled, please set its `count` field to `0`.

## Habits
The `~/.config/solar/habits.json` config file allows you to declare your daily/weekly habits, that drain the solar power in a predictable way. 
The syntax used there for scheduling is better explained by [this documentation](https://github.com/mariusbancila/croncpp#cron-expressions).
A single example:

`0 15 10 * * ? *`	means: 10:15 AM every day

[Here's more interactive way](https://crontab.guru/) to understand the syntax. 
Note, that it doesn't use the required seconds part though.

As with the `computers.json`, a given habit may be disabled by setting its `count` field to `0`.

## System
The `~/.config/solar/system.json` config file defines the voltage of the system (12 or 24 V) used for conversions.
Other 3 options include the ability to independently: generate, buy and/or sell the electricity. 
The combination of these options helps in deriving the optimal solution for your case, regarding the profitability of the operation.

## Electricity price
TODO: enable scheduling the electricity buy/sell prices via cron, like in the Habits json.

