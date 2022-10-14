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

## Passwordless SSH access
For the controlling PC to be able to control the mining rigs, we need to generate keys and install them 

ssh-keygen??
ssh-install-id

## Putting computers to sleep
TODO: STUB

The task here is to allow a user to put a given mining rig to sleep or to power it off without the need of entering the sudo password.
This is especially needed, as the commands will be scheduled to be ran in the future.

- First make sure, that the user in question is added to the sudoers group on each mining rig:

```bash
su
/sbin/usermod -aG sudo $USER
```

TODO: Is being in the sudoers group actually needed?

- allow the user to perform the commands without the password by executing:

```bash
sudo nano /etc/sudoers.d/suspend-user && sudo chmod 0440 /etc/sudoers.d/suspend-user
```

Enter the following directives, where the USR is the same username, as the previously defined sudoer:

```bash 
USR localhost =NOPASSWD: /usr/bin/systemctl suspend, /usr/bin/systemctl poweroff
```

- Update freedesktop settings
Find the following entries in the `/usr/share/polkit-1/actions/org.freedesktop.login1.policy` file:

- id="org.freedesktop.login1.set-wall-message
- id="org.freedesktop.login1.suspend
- id="org.freedesktop.login1.power-off

, as well as for the file `/usr/share/polkit-1/actions/org.freedesktop.systemd1.policy`:

- id="org.freedesktop.systemd1.manage-units

And for each one of them and change:

```
<allow_any>auth_admin_keep</allow_any>
```

to:

```
<allow_any>yes</allow_any>
```


- Veryfy if the update of the settings works by logging into the machine from the user, whose sudo rights we've altered (USR) and execute: 

```
systemctl suspend
```

The machine should halt and after a few seconds, by pressing the power button, you may test if it wakes up properly and reacts to further commands. If not, you will be forced to issue the `poweroff` command rather than the `suspend` command.
This means, that the given rig may be only powered on, rather than woken up, so you may as well skip the further setup steps for this given machine.
You will however have to 

or

ssh -n $HOST "systemctl suspend"



## Wake on Lan
TODO: STUB

This is an excerpt from [Debian Wiki](https://wiki.debian.org/WakeOnLan)


ONLY WOL from the closest computer behind the same switch!!!!
