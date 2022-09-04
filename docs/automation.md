# Automation
The automation of the system requires a couple of preparation steps. 
They're documented here and proven to work under Debian systems, although more subtle solutions may be possible. 

The automation consist of 4 phases and their according preparations:
- cyclically performing the calculation: a simple crontab entry (easy)
- compiling and setting up the mining software (medium-hard)
- using WakeOnLAN technology to start computers: enabling WOL for your network card (medium)
- putting computers to sleep before they drain the battery: allowing a user to perform system tasks (hard)

Each of the above phases shall be explained in their own paragraphs.


TODO: STUB


## System specifics

### Mac OSX
Under Mac OSX you need to enable the `at` scheduler manually. [Here's an example solution](https://unix.stackexchange.com/questions/478823/making-at-work-on-macos/478840#478840)

