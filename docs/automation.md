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

First, create your ssh id, if you haven't already done so, with:

```bash
ssh-keygen  # Choose passwordless key, at least for the production machine. 
```

Next, copy (or rather: install) your ID to each of the machines, that are meant to take part in the process with:
(TODO: this can be easily automated by a Python sctript by scanning the `computers.json`)

```bash
ssh-copy-id $HOST
```

Now, test that the connection can be established passwordlessly by simply logging into that machine:

```bash
ssh $HOST
exit
```

## Putting computers to sleep

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

The machine should halt and after a few seconds, by pressing the power button, you may test if it wakes up properly and reacts to further commands. If it doesn't respond after waking up, you will be forced to issue the `poweroff` command rather than the `suspend` command.
This means, that the given rig may be only powered on, rather than woken up, so you may as well skip the further setup steps for this given machine.
You will however have to alter the given rig's configuration to be able to be switchable with the poweroff command via the `computers.json`. The relevant configuration setting under each such computer is:  

```json
"is_poweroff" : true
```

OTOH, if the machine does react after waking up, we may move on to testing the issuance of a remote, passwordless `suspend` command. 
Bear in mind that all these steps will have to be repeated on the production machine, that is meant to control the whole process in a automated way in the future. 
You may imagine a Mini-PC or any other SoC whose power consumption is low enough not to interfere with the balance too much.

If all went fine, you may perform the final test of being able to put a machine to sleep or powering it off, please execute one of the below, according commands:

```bash
# Call this to suspend a machine remotely:
ssh -n $HOST "systemctl suspend"
# Or the below one, if you know that the machine doesn't wake up properly:
ssh -n $HOST "systemctl poweroff"
```

## Wake on LAN (WOL)

### WOL Intro
WOL is a very important functionality, that will help you automate the process entirely. 
In a nutshell, if all the prerequisites are met, it allows to wake up a computer, that was earlier put into the *suspended* mode, via a signal sent through Ethernet cable from the low powered controlling machine.
One thing to keep in mind here is, that from my experience, the signal is best to be sent via a controller that's nearest to the suspended machine in terms of the network's topology.
This means that even though you could setup everything correctly on the target machine, the WOL might still not work, if the signal is sent via already a second network switch along the way.
Don't get discouraged by this and just move the contolling machine "closer" - under the same network switch.
 
### WOL Howto

TODO: STUB

This is an excerpt from [Debian Wiki](https://wiki.debian.org/WakeOnLan)


## Install automation software

Currently the program `at` is being used to schedule tasks on both the mining rigs, as well as the controlling computer.

```bash
sudo apt install at
ssh $HOST # For each host
sudo apt install at
exit
```
