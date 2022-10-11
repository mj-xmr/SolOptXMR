# Use cases

## Intro
In this document, we'll describe some interesting use cases, that might come in handy, even for the situations now yet discovered by the authors. 

Throughout the document it's assumed, that the execution path is the `SolOptXMR` source directory. For consistency across the various documents, it shall be:

```bash
og@ghetto:~/devel/github/SolOptXMR
``` 

## Weather predictions only

It might be useful to skip the time consuming hashrate optimization step altogether, if all you need to know is how the weather shall be ahead of time, from the context of the application. 
In this case, this means, that except for the hashrate optimization, and the corresponding mining rigs' energy consumption, all the other influences and reactions shall still be taken into account across the declared horizon. 
These are:

- the energy input (solar, wind, etc.),
- the changes of the batteries' charge, 
- and even all the "static" consumers, as declared in the `habits.json` ( [see here](habits.md) ), as their calculations cost just a little CPU time. 

To trigger this behavior, execute:

```bash
./soloptxmr --no-computers
``` 

or: -cn

```bash
./soloptxmr -cn
# As in: "Computers: None" for consistency with other computer related options.
``` 

## Computer exclusion

You might want to exclude individual computers from the list of the computers to be taken into account for optimization, as defined in `computers.json`. 
The reasons for doing this might be:

- a given computer(s) might have been taken down temporarily for maintenance,
- the optimizations, that include the computer(s) take too much time, while you'd want to see any result soon enough
- you'd simply like to experiment a bit, without the need of altering your config file, that you assume to be rather fixed.


To trigger this behavior, execute:

```bash
./soloptxmr --ignore-computers host1,host2,host3 # or: -ci host1,host2,host3
```  

## Computer inclusion

You may also set a list of computers that should be the only ones to be taken into account. 
The reasons for this may be very similar to the _exclusion_ case, but might also include one additional reason: 

- you know that your batteries have an unusually low charge, because they were abused yesterday, against the yesterday's plan. In such case you already know that it's enough to start just one of the machines to balance out the battery charge. For instance this would be the computer that you plan to work on, while it mines in the background. 

To trigger this behavior, execute:

```bash
./soloptxmr --only-computers host1,host2 # or: -co host1,host2

``` 

## Offline mode force
Doesn't use any network connections and tries to use assumptions instead. 
This option is useful to simulate connection failures. 


```bash
./soloptxmr --offline-force # or: -of
``` 

## Offline mode try
TODO: Not fully implemented

 
Treats individual subsystems' connection problems as warnings and tries to move on with the according static calculations for the individual ones.


To trigger this behavior, execute:

```bash
./soloptxmr --offline-try # or: -ot
``` 

## Plot network difficulty only
Displays the current network difficulty only, without running any parts of the simulation. 
This might help you in planning quickly the opportunistic scooping of coins during reduced network difficulty periods.

```bash
./soloptxmr --net-diff # or: -n
``` 

## No Python plotting
Disables Python plotting to spare some time, even if Python is available.

```bash
./soloptxmr --no-plot # or: -np
``` 

## No Gnuplot plotting
Disables Gnuplot console plotting, as it consumes quite some vertical space. 
The default setting in this application can be controlled via `config-volatile.json`.

```bash
./soloptxmr --no-gnuplot # or: -ng
``` 

## No Gnuplot plotting
Disables the schedule console display, as it consumes quite some vertical space. 
The default setting in this application can be controlled via `config-volatile.json`.

```bash
./soloptxmr --no-schedule # or: -ns
```

