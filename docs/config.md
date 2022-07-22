# Configuration
Listed below are the separate configuration files, generated from templates after running the `./util/config.sh` script.
The default configuration file location is `~/.config/solar/` and will be refered as the base dir for the below config files.

## Arrays arrays.json
Inside the `~/.config/solar/arrays.json` it's important to understand, that an array's parameter `surface_tilt` accepts degrees and 0 means laying flat horizontally on a roof, while 90 means hanging flat vertically on an external wall. 
`surface_azimuth` uses 0 as north, 90 as east, 180 as south and 270 as west.

## Geo geo.json
Is expected to be modified by you, according to the physical location of the solar farm, the northern hemisphere and eastern side of globe are represented with positive numbers of Lat / Lon in degrees. 
Southern hemisphere and western side of globe use negative numbers of Lat / Lon. 
The script `src/geolocation.py` will aid you in setting the right values. 

The variable `city` is meant to deliver weather reports.
It is possible, that your farm's location is so remote, that the city where it belongs to, isn't listed on the weather reports.
In such cases, please enter the nearest largest city, until the script stops complaining.

## Batteries batteries.json
Defines your set of batteries. Currently only one battery type is supported and only the first battery type from the list will be taken into account. 
Their count however, will have a multiplying effect on the storage. 
Below are the parameters and example values:

| Var. name  | Value  | Description |
| ------------- | -- | --- |
| `"name"` | "Ca-Ca" | An arbitrary name. In this case the abbreviation means "Calcium-Calcium" lead-acid battery. |
| `"count"` | 2 | The number of batteries of the same type. |  
| `"discharge_rate_c_by"` | 10 | The discharge rate of the battery at discharge. See [this paper](http://www.scubaengineer.com/documents/lead_acid_battery_charging_graphs.pdf) about lead-acid batteries. Available choices: 3, 5, 10, 20, 100 | 
| `"max_discharge_amp"` | 11 | The maximum amount of Amperes, that the battery can deliver. |
| `"max_capacity_amph"` | 60 | The maximum capacity of the battery in Ampere-hours, as provided by the manufacturer. |
| `"max_charge_v"` | 13.2 | The maximum charging voltage, that you set in the MPPT controller. |
| `"min_load_amph"` | 30 | The minimum load, that has to remain in the battery, not to damage it. For lead batteries, it will be half of the maximum. |
| `"discharge_per_hour_percent"` | 0.9 | The percentual self-discharge value per hour. |

## Computers computers.json
Defines mining rigs. 
Currently the system doesn't work very well with the number of mining machines larger than 2.
In case a single computer should be disabled, please set its `count` field to `0`.

## Habits habits.json
Allows you to declare your daily/weekly habits, that drain the solar power in a predictable way. 
The syntax used there for scheduling is better explained by [this documentation](https://github.com/mariusbancila/croncpp#cron-expressions).
A single example:

`0 15 10 * * ? *`	means: 10:15 AM every day

[Here's more interactive way](https://crontab.guru/) to understand the syntax. 
Note, that it doesn't use the required seconds part though.

As with the `computers.json`, a given habit may be disabled by setting its `count` field to `0`.

## System system.json
Defines the voltage of the system (12, 24 or 48 V) used for conversions.
Other 3 options include the ability to independently: generate, buy and/or sell the electricity. 
The combination of these options helps in deriving the optimal solution for your case, regarding the profitability of the operation.

## Volatile config-volatile.json
Contains various entries from across the project, which don't really deserve their own config files.

| Var. name  | Description |
| ------------- | --- |
| `glob` / `ARCHIVE_IMAGES` | Archive images for future finer analysis, like when your OCR module misinterprets the readout |
| `glob` / `TESTING` | Internal variable used for testing |
| `paths` / `HOME` | Home directory. Currently unused. |
| `paths` / `DIR_TMP` | Temp directory, where all the intermediate files should be stored. |
| `paths` / `DIR_ARCHIVE` | If there's a need to archive anything, this basepath shall be used. |
| `paths` / `DIR_OCR_SCRIPT` | Script path for the OCR module. |
| `paths` / `IMG_CAPTURE_PATH` | Path where the captured images shall be stored. |
| `hosts` / `IMG_CAPTURE_HOST` | Host that capture images for OCR. |


For OCR related entries, please have a look [here](ocr.md)

## Electricity price
TODO: enable scheduling the electricity buy/sell prices via cron, like in the Habits json.

