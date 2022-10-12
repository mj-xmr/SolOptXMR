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

## Cron automation
If you are confident enough to let the system schedule execution of the commands without your supervision, then this is what you'd enter to your `crontab` to plan the day at 6:05 a.m., assuming that your `SolOptXMR` installation resides in your `home` directory and that you use the OCR script to automatically read the battery voltage or its State of Charge in %:

```bash
crontab # The command that lets you edit the schedule
5 6 * * *  cd /home/yoname/SolOptXMR && ./soloptxmr.py --battery-charge-ocr --np && /bin/sh /home/yoname/temp/solar/sol-cmds.sh
```

## Compiling the mining software
TODO: STUB

## Wake on Lan
TODO: STUB

## Putting computers to sleep
TODO: STUB

