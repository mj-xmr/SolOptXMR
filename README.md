# SolOptXMR
Solar Optimal mining of XMR.

![soloptxmr-logo](https://user-images.githubusercontent.com/63722585/163665392-cae163e6-a405-4692-8b05-b98480bc0911.svg)


The goal is to create open-source software that aids people mining Monero with excess solar power (as well as other supplementing renewables) in the most profitable way. 
It will accommodate for issues such as:

- Time of day
- Weather fluctuations
- Avoiding depleting batteries below a threshold, that would damage them
- Avoiding overheating of the mining rig
- Leaving enough power for your daily use

[Full description on Monero Community Crowdfunding System](https://ccs.getmonero.org/proposals/soloptxmr-mj-endor-2022.html)


# Quickstart
In case these instructions become outdated, please refer to the steps of the [CI](.github/workflows/linux.yml).

## Preparation
Please run the below scripts. They are meant to be non-interactive and will require root permissions (via `sudo`). 
When in doubt, please view their contents with `cat` for an objective assessment of their functionalities.
```bash
git clone --recursive https://github.com/mj-xmr/SolOptXMR.git # Clone this repo (assuming it's not a fork)
cd SolOptXMR		# Enter the cloned repo's dir
pip install -r requirements.txt # Install Python packages (either use this command or its fitting alternative) 
./util/prep-env.sh	# Prepare the environment - downloads example data and creates useful symlinks
./util/deps-pull.sh	# Download the maintaned dependencies
./util/deps-build.sh	# Build and install the unmanaged dependencies (uses sudo for installation)
./util/config.sh	# Configure your rig
```

## Building & running
```bash
./ci-default          # Build and run the tests
./soloptxmr.py        # Run the prediction with default parameters  
./soloptxmr.py --help # Print all the available options 
```
By default the script perfomrs the predictions 3 days ahead from now, assuming the battery charge at its reasonable minimum, set by the `battery.json`.

In order to alter the default behavior, the main script can be ran with the following example options:
```bash
./soloptxmr.py \
--days-horizon 5 \
--battery-charge 55.2 \
--start-date 2022-02-20T20:22
```
or equivalently:
```bash
./soloptxmr.py \
-d 5 \
-b 55.2 \
-s "2022-02-20 20:22"
```

## Configuration
After running the `./util/config.sh` script, you'll be presented with paths to configuration scripts, that have just been copied to your `~/.config` directory. You're meant to adjust them to your specifics.

### Arrays
Inside the `~/.config/solar/arrays.json` it's important to understand, that an array's parameter `surface_tilt` accepts degrees and 0 means laying flat horizontally on a roof, while 90 means hanging flat vertically on a wall. `surface_azimuth` uses 0 as north, 90 as east, 180 as south and 270 as west.

### Geo
Inside the `~/.config/solar/geo.json`, which is expected to be modified by you, according to the physical location of the solar farm, the northern hemisphere and eastern side of globe are represented with positive numbers of Lat / Lon in degrees. Southern hemisphere and western side of globe use negative numbers of Lat / Lon.

The variable `city` is meant to deliver weather reports.
It is possible, that your farm's location is so remote, that the city where it belongs, isn't listed on the weather reports.
In such cases, please enter the nearest largest city, until the script stops complaining.

### Computers
Currently the system doesn't work very well with the number of mining machines larger than 2.

# Screenshots

## First production model
A model with an integrated n-day weather prediction & multiple, user-defined mining rigs (here: 2).

![solopt-prod](https://user-images.githubusercontent.com/63722585/167363580-978d0835-9d6c-40bb-94dd-ffa955bade10.png)


## First model
A plot of the energetic balance, based on real astronomic data and contrived weather distortions, simulating the battery drainage with a horizon of 3 days:

![plot-1st](https://user-images.githubusercontent.com/63722585/163774847-7c3f522a-a6b9-43bf-b133-6ba0c6e007f8.png)


## Console interactive UI
Accessible via `ssh`/`putty`, based on `curses`.

![image](https://user-images.githubusercontent.com/63722585/163773221-52705e53-167b-4468-ad7e-2038c2f822a8.png)

![image](https://user-images.githubusercontent.com/63722585/163773424-64711673-ce88-4a49-a1fc-0c16526e623d.png)

![image](https://user-images.githubusercontent.com/63722585/163773989-c00c180a-43ab-408d-be97-5624c099d148.png)

## Profitability calculation

```
Algo name:  Endor
Undervolted initial =  0.64%
Undervolted =  4.32%
Overvolted  =  25.99%
Total hashes = 175.83 MH
Total income = 34.64 USD
Total cost = 6.29 USD
Total profit = 28.35 USD
Profitability = 450.72 %
Saving figure to: build//fig-Endor.png
```
