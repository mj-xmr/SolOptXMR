# Economy of the system based on renewable energy sources

## Introduction 
This section briefly describes a couple of aspects that are more obvious for people with somewhat more experience in the financial markets as well as engineering/physics. 
Sometimes however experience in one of these two extremes isn't necessarly complimented with the useful experience from the other. 
We'll try to bring these together. 

## Supply & demand
A very important factor to consider is that the profile of renewable sources' energy production comes in peaks and troughs. 
This means that at a given location there will always be an oversupply and undersupply of energy produced by the same way.
In such circumstances, I can imagine only 4 scenarios sorted by the worst to best.
The peak overproduction can either:
- overload the grid and surprisingly lead to a blackout, although this is rather not an issue after year 2010.
- not be required during that time of day, due to general oversupply, so it will simply be dissipated. An oversupply of any good always reduces its price
- be stored in expensive storage facilities (like cryogenic). The positive here is, that your local community will be supplied with electicity long after the peak. However, since the storage facility was a large investment, don't expect the local gov't to still have money left from the same budget, to be able to give you a good price for the energy that they will store for you.
- be either used or stored by you.

We will obviously focus on the last scenario, since we believe, that it has the biggest probability of success in the long term, past all the gov't subsidies.

## Usage: mining crypto for profit
Whenever there's competition, any gains availale at a given moment will be diminishing as the time passes, because the competition starts to follow your steps.
When you mine crypto, you compete against global market, where in some places of the World the electricity can be so cheap (even if temporarly) or even free, if being stolen, that you'll only profit from mining, when you assume to sell in future for a given higher price (for example the last year's All Time High).
While this is not that much of a problem, since you can store infinite amounts of crypto in your wallet, not having to pay more to store more (as opposed to storing precious metals or even Fiat), you have to admit, that the real reward, that is worth the effort, will only be available at some point in the future.
There exists also always a risk, that even the best currency collapses, before your expected profit goals are reached. 
We are in a group, where everybody assumes Moon, but bear in mind, that for reasons that are currently not known yet, this might never happen.

## Usage: household needs
As opposed to mining or selling something in a Free Market in general, there's never any competition to hinder your profit when you spare something. 
This is a 100% guaranteed win, even though the win might be small.
As a general rule, typically the lower the risks, the lower the gains (see explanation at [Bond Markets](#bm)), but the Reward/Risk ratio in case of savings is unprecedented. 
The current electricity price may rise anytime, so be prepared to be able to switch from using the majority of electricity for mining to partly covering your household needs, by at least stashing 12V compatible hardware (water kettles, etc.), and make sure, that this backup system works at all times.
As long as you input the SolOptXMR system the current electricity price, or better yet: provide a Python script, that checks for the price automatically, the system will inform you quantiatively what the best course of action might be at a given moment. 


### Digression: Bond Markets
#### BM
A good explanation for the risk/reward ratio is delivered by the Bond Markets. 
The Bond Vigilanties will typically avoid any bonds, that yield more than 10% or 15% per year and prefer being holders of lower yielding bonds. 
Bonds yielding as high as 40% are considered laughable and not taken seriously at all. 
Why do they avoid them, when the promised gains overwhelm many traditional investments? 
Because there are enough data in the history (both old and recent), that confirm, that the higher the yield, the larger the risk (i.e.: of a default).
If there's anybody who takes care of **generational wealth** rather than chases after Teh Nex NFT to be Moon, it's the Bond Vigilanties.
My preference is to be somebody in-between, since neither do I own any substantial wealth ("nothing to loose"), nor I manage somebody else's generational wealth, losing of which would make look like a clown "at the banquets". 
Depending on your location though, at a given moment of time, the price for electricity, that you have to pay for household usage may be so low, that it will still be worth to mine (higher risk gains), not caring about covering the household usage for the time being (lower risk gains). 

## Usage: networked batch tasks
For some Users it may be benefitial to have a few low powered PCs running all the time, mining or not.
These PCs could either perform networked batch tasks for you also during the night, or if you're a C++ developer, could be taking part in networked compilation thanks to software such as [distcc](https://www.distcc.org/) or better yet: [icecc/icecream](https://github.com/icecc/icecream).
However, bear in mind that such parallel compilation can only be possible using a compatible CPU architecture.
What won't work for sure, is an ARM-based System on Chip together with Intel CPUs, etc. 
Hint: you migh buy similarly low powered and low cost, as the popular SoCs are, yet Intel-compatible **fanless mini PCs** with 8 GB RAM and internal SSDs that don't require much maintenance.

## Diversifying the inputs
It might be clear to you by now, that the simultaneous peak production is a source of much trouble.
Even if you own desktop PCs, able to scalp the peak production, so that you don't have to sell it to the grid for pennies, you have to keep in mind, that a very intense usage of such PCs will tend to overheat them, making you hit yet another ceiling.
There are however some alternative solutions available, except for the obvious solution of buying more batteries - production & storage type diversification.

### Orienting the solar panels in various directions
It is possible to balance the production across time by simply orienting the solar panels in slightly different directions. 
Imagine having a mini-farm that consists of only 3 solar panels. 
The 1st one could point to south-east, the 2nd one to south and the 3rd one to south-west.
Thus the peak production of each of the panels will be different and supplementing each other.
The picture below presents the output of a setup, where 2 directions are used:

![image](https://user-images.githubusercontent.com/63722585/164888181-a4d5a7f9-f3ea-46b6-b261-b2c736df4b70.png)

[Source](https://pvlib-python.readthedocs.io/en/stable/gallery/irradiance-transposition/plot_mixed_orientation.html#sphx-glr-gallery-irradiance-transposition-plot-mixed-orientation-py)

[Source](https://pvlib-python.readthedocs.io/en/stable/gallery/irradiance-transposition/plot_mixed_orientation.html#sphx-glr-gallery-irradiance-transposition-plot-mixed-orientation-py)

### Additional energy storage
You might simply use the overproduction to power a hydro pump, by lifting a certain volume of distilled water, that would later be released onto a hydro generator, in order to prolong the input of the electricity's flow.
Using the electricity during the night might offer special benefits to you, like the following:
- the production of electricity in the night is low, so the grid's buyback prices might be greater
- the coin network's hashrate might be smaller during this period, as many people will switch off their mining rigs due to the same reason as above
- your mining rig will be able to work with more threads and CPU frequency, since the room temperature drops 
If enough volume is pumped, the losses of this act aren't too large, the night is cold, you might discover interesting opportunities.   


### Wind generators
An easy, though not cheap way to fill up the nightly gap is via wind generators, since night is when the wind is typically the strongest. 
In specific locations, the output profile of a hybrid system might look as promising as the following one:

![image](https://user-images.githubusercontent.com/63722585/164888207-cb7c3315-1dfb-4137-bc0a-80783f31b53b.png)

[Source](https://www.researchgate.net/publication/319236214_Wind-Solar_Hybrid_System_-_An_Innovative_and_Smart_Approach_to_Augment_Renewable_Generation_and_Moderate_Variability_to_the_Grid)

Spare yourself the trouble of the traditional wind generators and buy a **vertical** generator.
They look like a twisted serpentine.
This geometric property allows them to turn without having to first point frontally to the stream of the wind.
They will also typically be equipped with an electromagnetic brake, that would smoothly prevent them from turning too fast during stronger winds.


### Common benefits of diversification
One common benefit for all of the diversification solutions is that PCs with low CPU cache will behave better, if they use fewer threads, that would otherwise compete for the same cache regions, thus reducing the output.
In other words, their output doesn't scale linearly with the linear increase of used threads.
OTOH by not using all the threads, they leave an energetic surplus, as opposed to the sutuation when they use all the threads.
Another limitation, that prevents you from using all the threads of all the computers at the same time of peak production, that may only be obvious to electrical engineers, results from the cables' internal resistance.
If, for practical reasons, you decided to join all, say 4, batteries with a single cable, reaching the inverter and the cable is too narrow, then it will limit the number of electrons (hence finally: available Watts) that can be transfered per second.
This essentially forces you to diversify the inputs, unless you're OK with investing more than enough into batteries to store the unused overproduction.
Whatever diversification scheme is on your mind, before making such an investment decision, you may simulate the potential outcome by changing the configuration settings and re-running the simulation.
