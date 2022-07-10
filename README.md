# SolOptXMR
Solar Optimal mining of XMR.

![soloptxmr-logo](https://user-images.githubusercontent.com/63722585/163665392-cae163e6-a405-4692-8b05-b98480bc0911.svg)


The goal is to create open-source software that aids people mining Monero with excess solar power (as well as other supplementing renewables) in the most profitable way. 
It will accommodate for issues such as:

- Time of day
- Weather fluctuations
- Opportunistic scooping during network difficulty reductions
- Avoiding depleting batteries below a threshold, that would damage them
- Avoiding overheating of the mining rig
- Leaving enough power for your daily use

[Full description on Monero Community Crowdfunding System](https://ccs.getmonero.org/proposals/soloptxmr-mj-endor-2022.html)

# Requirements
- A POSIX system with a C++ compiler and Python3 interpreter. 
- Less than 1 GB of RAM.

Supported Operating Systems and features:

| OS  | Support   |
| ------------- | -- | 
| Debian stable | ✓  | 
| Debian buster | ✓  | 
| Ubuntu 21.04  | ✓  | 
| Ubuntu 20.04  | ✓  | 
| Mac OSX 12    | ?  | 
| Mac OSX 11    | ?  | 
| Mac OSX 10.15 | ?  | 
| Raspbian      | ?  | 
| Windows       | X  | 

Legend:
- ✓ : Full support or there are bugs that are fixable
- ? : Should work but some more testing is required
- X : No support yet, but is being planned

# Quickstart
In case these instructions become outdated, please refer to the steps of the [CI](.github/workflows/linux.yml).

## Preparation
Please run the below scripts. They are meant to be non-interactive and will require root permissions (via `sudo`). 
When in doubt, please view their contents with `cat` for an objective assessment of their functionalities.
```bash
git clone --recursive https://github.com/mj-xmr/SolOptXMR.git # Clone this repo (assuming it's not a fork)
cd SolOptXMR		# Enter the cloned repo's dir
./util/prep-env.sh	# Prepare the environment - downloads example data and creates useful symlinks
pip install -r requirements.txt # Install Python packages (either use this command or its fitting alternative) 
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
By default the script performs the predictions 3 days ahead from now, assuming the battery charge at its reasonable minimum, set by the `battery.json`.

### Fine configuration
In order to alter the default behavior, the main script can be ran with the following example options:
```bash
./soloptxmr.py \
--days-horizon 5 \
--battery-charge-percent 78 \
--start-date 2022-02-20T20:22
```
or equivalently:
```bash
./soloptxmr.py \
-d 5 \
-p 78 \
-s "2022-02-20 20:22"
```

### Simpler battery charge input via voltage and OCR
Setting the battery's voltage directly, also via OCR, rather than the Ah or % charge is a feature currently being worked on. What's missing are measurements of batteries of various types, like in [Example 1](http://www.scubaengineer.com/documents/lead_acid_battery_charging_graphs.pdf)

```bash
./soloptxmr.py --battery-charge-v	12.3	# Set the voltage of your battery to be converted to its capacity
./soloptxmr.py --battery-charge-ocr			# Use image recognition to read the current battery voltage
```

### OCR
TODO: Describe how to create own module for image recognition.

TODO: Add battery charge & discharge profiles to voltage_lib and to battery.json 

### Other use cases
```bash 
./soloptxmr.py --net-diff     			# Only test the current network difficulty, without performing the simulation
```

## Plotting the hashrate situation only
Because the optimization takes some time and you might be only interested in the hashrate situation alone, the main script has an option to ommit the optimization part.

```bash
./soloptxmr.py --net-diff
```
or equivalently:
```bash
./soloptxmr.py -n
```
... shows the network difficulty situation only. 
This ususeful, because when you know, that your battery is charged, you don't plan on using the accumulated electricity domestically and there's a dip in the hashrate at the same time, it's simply obvious that you should start mining without being told to do so by the optimizer.

## Configuration
After running the `./util/config.sh` script, you'll be presented with paths to configuration scripts, that have just been copied to your `~/.config` directory. 
You're meant to adjust them to match your specifics. 
[Please read this file](docs/config.md) for further explanations.

# Extra scripts

## Modeling solar arrays
The script `src/arrays.py` will help you understand how a mixture of various orientations of arrays, defined via `~/.config/solar/arrays.json` reduces the peak & trough problem. 
By properly diversifying the orientation, you'll be blessed with more stable input across the day. 
Depending on your location, this might increase your profits of the sold electricity, that you can't use domestically nor on mining, since the buyback prices might be a function of the time of day.

You might also discover, that although the recommended panel tilt of 45° does deliver more production in total across the whole year, it also creates as much of overproduction during the summer, that you'll have a hard time using due to high temperatures and low buyback prices, as low underproduction it delivers during winter, when you need the power the most. 
In order to balance this discrepancy, please try experimenting with more vertical tilts, like between 89° and 80°.
The script accepts an iso-formatted date as an input, allowing you to simulate extreme conditions - winter and summer. 
The title of the plot presents the sum of produced electricity in a given day. 
It makes sense to compare and sum up this values for the extreme conditions.

# Further documentation
- [safety](docs/safety.md): how to handle electrical systems safely. Tell me, that you "read and understood it", and I can sleep fine.
- [economy](docs/economy.md): my economy views and the resulting dynamics tailored to production of electricity.

# Screenshots

## First production model
A model with an integrated n-day weather prediction & multiple, user-defined mining rigs (here: 2).

![solopt-prod](https://user-images.githubusercontent.com/63722585/167363580-978d0835-9d6c-40bb-94dd-ffa955bade10.png)


## First model
A plot of the energetic balance, based on real astronomic data and contrived weather distortions, simulating the battery drainage with a horizon of 3 days:

![plot-1st](https://user-images.githubusercontent.com/63722585/163774847-7c3f522a-a6b9-43bf-b133-6ba0c6e007f8.png)

## Network difficulty
These data are taken into account when making decision about starting a rig or not. 
Sometimes it's worth to wait for a while until other miners switch off their rigs in order to scoop more coins for the same amount of used power. 
Obviously this assumes, that your stored electricity doesn't go to waste. 
Also, please note the differences in scales of the two plots. 
The daily seasonal swings (lower plot) are 100 times smaller than the mean reversion ones (upper plot).

![Network difficulty](https://user-images.githubusercontent.com/63722585/169827786-7dc548c1-6b46-49a9-a7ac-ca20605f1046.png)

On top of higher individual gains, collectively, scooping the hashes this way also improves and stabilizes the network's security.

## Solar array modeling
An example evaluation (via `src/arrays.py`) of 2 sets of panels - one set pointing to south-east and the other to south-west. 
Please note the prolonged production across the whole day, reflected by the inverter's wide output.

![array-modelling](https://user-images.githubusercontent.com/63722585/170349578-16f0965a-9c34-45ea-9d14-df740a562723.png)

## Lead-acid battery discharge profiles

![bat-discharge](https://user-images.githubusercontent.com/63722585/178121819-af159f30-8d41-444c-8bf6-71c84c7ec902.png)


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
