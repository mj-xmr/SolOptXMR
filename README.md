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
./util/prep-env.sh	# Prepare the environment - downloads example data and creates useful symlinks
./util/deps-pull.sh	# Download the maintaned dependencies
./util/deps-build.sh	# Build and install the unmanaged dependencies (uses sudo for installation)
```

## Building & running
```bash
./ci-default 	# Build and run the demo
```

# Screenshots

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
