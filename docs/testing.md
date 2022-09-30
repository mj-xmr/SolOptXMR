# Testing

## Intro
The below testing results are achieved via the following testing executables:

- `opti-system-min-test`
- `opti-system-long-test`


## Corner cases

In this section the corner cases are presented, that have been isolated so far, together with their short description. 
This should help the user assess the current robustness of the system.
The expectation of the system on corner cases is to at least exit cleanly, preferrably provide a solution, that minimizes the damage, that might be inevitable.

### Happy path

The Hello World of testing. 

![happy path](screenshots/testing/01-happy-path.png)

If anything is supposed to work, it's the happy path - the most optimistic setup, in which the system can operate. 
In our case this is achieved with all of the main components adjusted so, that they remain in such a balance, that no emergency handling needs to be enabled. 
The main components are:

- solar panels as input
- batteries as a buffer, large enough to store the input
- computers as the consumers

The Penalities/hashes progression depicts how these values decreased / increased accordingly, across the progressing iterations of the simulation. 
While the hashes value is self-explanatory, the penalities constitute of under- and overvoltage counting of the batteries. 
This translates to battery damage in both cases and wasted energy in the case of overvoltage alone.

### High input

![high-load](screenshots/testing/02-high-load.png)

The solar panels deliver so much energy, that neither the batteries can store it, nor the computers are able to take is away.
Still the computers wait some time before they start, not to undervolt the batteries, which damages them.

### High input noon

![high-load-midday](screenshots/testing/03-high-load-midday.png)

The same as above but the simulation starts from noon.
This allows to track if the computers started right at the beginning to reduce the inevitable overvoltage.

### Low input

![low-load](screenshots/testing/04-low-input.png)

With Wattage as low as 5W peak, it's interesting to see if the system is able to try to mine anything while keeping the batteries above the minimum.

### 2 computers on one day

![2 comp](screenshots/testing/05-2-computers-single-day.png)

Assuming enough wattage is being produced, here we try to run 2 computers simultaneously.
Below the individual computers' schedules are printed.

### Overcharged batteries

![overcharged](screenshots/testing/06-overcharged-batteries.png)

The simulation starts with severly overcharged batteries.
Instead of panicking, we try to dissipate the energy immedately and even spare some of it at the end of the simulation.

### Undercharged batteries

![undercharged](screenshots/testing/07-undercharged-batteries.png)

For the entire day the batteries were so undercharged, that the optimal solution doesn't include starting the computer for any period of time.
In the Penalities/hashes progression plot you may see, that one of the 1st solutions tried the mining was indeed considered, but ended up rejecting thes idea due to high penalities associated with even more undervoltage.

### Undercharged batteries but mining

![undercharged-mining](screenshots/testing/08-undercharged-but-loading.png)

The day starts with undercharged batteries, but as soon as enough energy is amassed, the computers are being started to turn the night into a day.

### No computers

![no-computers](screenshots/testing/09-no-computers.png)

A very special case, when the `--no-computers` or its shorter version: `-cn` option is selected. 
In such case, it's not even considered to run any computers for the mining, but taking the pre-defined habits into account only.
This option serves as a simple and fast weather prediction forecast as well, and the weather's impact on your batteries.

### 2 computers - 2 days

![2 comp](screenshots/testing/10-2-computers-long.png)

More complex, yet more common case, where 2 computers are being used and the balance between the under- and overcharge is kept.
Please note, that the more computers are used, the more combinations are possible and non linearly.


