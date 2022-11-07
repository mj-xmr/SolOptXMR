# Automation
The automation of the system requires a couple of preparation steps. 
They're documented here and proven to work under Debian systems, although more subtle solutions may be possible. 

The automation consist of 4 phases and their according preparations:
- cyclically performing the calculation: a simple crontab entry (easy)
- using WakeOnLAN technology to start computers: enabling WOL for your network card (medium)
- putting computers to sleep before they drain the battery: allowing a user to perform system tasks (hard)
- compiling and setting up the mining software (easy)

Each of the above phases shall be explained in their own paragraphs.

## Glossary
The following terms will be introduced:

- *Controlling computer*: The computer, which should ideally consume very little power (think of a Mini-PC or any SoC), as it should be running for most of the time - always when the AC inverter is switched on. It will be issuing commands via Ethernet cable to *Mining rigs / computers*. It will also host the `p2pool` software.
- *Mining rig / computer*: Any computer, whose sole purpose from the perspective of `SolOptXMR`, is to mine and dissipate energy overproduction, if so required. The rig is able to react on a *controlling computer's* commands, issued via Ethernet cable.
- *Node computer*: an optional part of the infrastructure, for as long as you decide to attach your `p2pool` to an **external** Monero node, that you can trust. Otherwise, this has to be a computer, that runs 24/7 and is equipped with a relatively large (between 500GB and 1TB) SSD drive to be able to host the blockchain. An HDD won't work well with Monero's blockchain unfortunately, due to many random searches, that the node needs to perform over the blockchain. I use a low powered Mini-PC for this purpose, that is connected to the grid.

## System specifics

### Mac OSX
Under Mac OSX you need to enable the `at` scheduler manually. [Here's an example solution](https://unix.stackexchange.com/questions/478823/making-at-work-on-macos/478840#478840)

## Cron automation
If you are confident enough to let the system schedule execution of the commands without your supervision, then this is what you'd enter to your *Controlling computer's* `crontab` to plan the day at 6:05 a.m., assuming that your `SolOptXMR` installation resides in your `home` directory and that you use the OCR script to automatically read the battery voltage or its State of Charge in %:

```bash
crontab # The command that lets you edit the schedule
5 6 * * *  cd /home/yoname/SolOptXMR && ./soloptxmr.py --battery-charge-ocr --np && /bin/sh /home/yoname/temp/solar/sol-cmds.sh
```

## Passwordless SSH access
For the *Controlling PC* to be able to control the mining rigs remotely, we need to generate `ssh` keys and install them 

First, create your `ssh` key, if you haven't already done so, with:

```bash
ssh-keygen  # Choose passwordless key, at least for the production machine. 
```

Next, copy (or rather: install) your ID to each of the machines, that are meant to take part in the mining process with:
(TODO: this can be easily automated by a Python script by scanning the `computers.json`)

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

- `id="org.freedesktop.login1.set-wall-message`
- `id="org.freedesktop.login1.suspend`
- `id="org.freedesktop.login1.power-off`

, as well as for the file `/usr/share/polkit-1/actions/org.freedesktop.systemd1.policy`:

- `id="org.freedesktop.systemd1.manage-units`

And for each one of them and change:

```
<allow_any>auth_admin_keep</allow_any>
```

to:

```
<allow_any>yes</allow_any>
```


- Verify if the update of the settings works by logging into the machine from the user, whose sudo rights we've altered (USR) and execute: 

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
# Alternatively the below one, if you know that the machine doesn't wake up properly:
ssh -n $HOST "systemctl poweroff"
```

## Install the automation software

Currently the program `at` is being used to schedule tasks on both the mining rigs, as well as the controlling computer.
Additional software is needed to perform the remote operations.
The list also contains requirements for other tools.

```bash
# On the controlling machine:
sudo apt install at ethtool git wakeonlan libuv1-dev libzmq3-dev libsodium-dev libpgm-dev libnorm-dev libgss-dev libcurlpp-dev

# For each mining rig:
ssh $HOST 
sudo apt install at ethtool git build-essential cmake libhwloc-dev libuv1-dev libssl-dev libreadline-dev
exit
```

## Wake on LAN (WOL)

### WOL Intro
WOL is a very important functionality, that will help you automate the process entirely. 
In a nutshell, if all the prerequisites are met, it allows to wake up a computer, that was earlier put into the *suspended* mode, via a packet sent through Ethernet cable from the low powered controlling machine.
One thing to keep in mind here is, that from my experience, the packet is best to be sent via a controller that's nearest to the suspended machine in terms of the network's topology, meaning under the same network switch.
This means that even though you could setup everything correctly on the target machine, the WOL might still not work, if the packet is sent via already a second network switch along the way.
Don't get discouraged by this and just move the controlling machine "closer" - under the same network switch.
 
### WOL HowTo

This is an excerpt from [Debian Wiki](https://wiki.debian.org/WakeOnLan)

Log in to the mining rig and check the connected interface:

```bash
ssh $HOST
ip a | grep ": " | grep " UP "
```

If the output of this command would be:

```bash
3: enp2s0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP group default qlen 1000
```

, then the network interface, that you're looking for is `enp2s0`.
In such case, in order to learn the status of the WOL for this interface, execute this:

```bash
sudo ethtool enp2s0 | grep Wake-on
```

If the the command prints:

```bash
  Supports Wake-on: pumbg
  Wake-on: g
```

, where the 2nd line describes the currently enabled modes, then the interface can right now be able to be used for WOL.
If the `g` is not on the list, then you need to enable it. 
Here's how I do it, although it's just one of the methods:

```bash
sudo nano /etc/rc.local && sudo chmod +x /etc/rc.local
```

inside the file enter the below line, again, assuming that `enp2s0` is your interface :

```bash
/sbin/ethtool -s enp2s0 wol g
# ... other commands...

exit 0 # Make sure that this is the last command of this file.
```

After a reboot of this machine, rerun the previous test:

```bash
sudo ethtool enp2s0 | grep Wake-on
```

Hopefully the 2nd line now reads:

```bash
  Wake-on: g
```

If it doesn't, then you need to spend some time on troubleshooting, by browsing online resources.
If you give up on this one, you'll need to settle with having to manually powering on the computers, according to (more or less) the schedule that's generated by `SolOptXMR`.

## Preparing the mining software

### XMRig and p2pool compilation

On the mining rigs checkout SolOptXMR and use a script to install [xmrig mining software](https://github.com/xmrig/xmrig):

```bash
git clone --recursive https://github.com/mj-xmr/SolOptXMR.git # Clone this repo (assuming it's not a fork)
cd SolOptXMR		# Enter the cloned repo's dir
git checkout $(git describe --tags $(git rev-list --tags --max-count=1)) # Checkout the latest tag (master is risky)
./util/build-xmrig.sh   # Build the mining software
```

On the *Controlling Computer*, repeat the above steps, but instead of compiling `xmrig`, compile `p2pool` instead:

```bash
./util/build-p2pool.sh  # Build the pool software 
```

If you'd like to run your own node, there's a relevant script for that too, meant to be ran from within the *Node Computer*:

```bash
./util/build-monero.sh  # Build the monero daemon 
```

In such case, it will be expected, that you run your `p2pool` from the *Node Computer*, rather than from the *Controlling Computer*, so you have to compile the pool here instead.

### Optional optimizations of XMRig

At least for for most of Intel CPUs, it's very beneficial to enable the [MSR](https://en.wikipedia.org/wiki/Model-specific_register) module, in order to boost the hashes calculated for the same power input.
This task is however cumbersome and varying across different CPUs that the full description should not be placed in this documentation in its fullest.
However, the minimalistic setup, that only allows to use the MSR module from a `root` account would be the following:

```bash
sudo apt install msr-tools
sudo nano /etc/default/grub
```

Change the `GRUB_CMDLINE_LINUX` variable to following:
```bash
GRUB_CMDLINE_LINUX="msr.allow_writes=on"
```

```bash
sudo reboot
```

Now allow the *XMRig* to do some fine-tuning:

```bash
sudo build/xmrig/scripts/randomx_boost.sh
```

```bash
sudo reboot
```


### XMRig autostart

If you enabled the MSR module, the easiest way to leverage it for mining is to run the mining software as `root`.
Obviously this is a security risk, so you might want to think twice about this and spend effort on making the MSR module available for the user(s).

In case you prefer to stick to the `root` option for simplicity, this is how you'd run the `xmrig` on startup of the machine:

```bash
sudo nano /etc/rc.local && sudo chmod +x /etc/rc.local 
```

enter there:

```bash
sleep 60; cd /home/USR/SolOptXMR && ./util/run-xmrig.sh $(nproc) &
```

before the `exit 0` line of course. You may freely choose the number of threads that you want to use by replacing the `$(nproc)` with a reasonable number.
This will run the `xmrig` miner as `root`.
In case that you've either made the effort to enable the MSR module for your user, not even no effort at all to enable the MSR module even for the `root`, then the safer alternative to the above would be:

```bash
su - USR -c "sleep 60; cd /home/USR/SolOptXMR && ./util/run-xmrig.sh $(nproc)" &`
```

### Temperature control (optional)

The temperature might be monitored and controlled by throttling down the CPU frequency, by having this script ran as `root` or alternatively as a user, after allowing the user to modify the CPU frequency (TODO) with:

```bash
sleep 5; cd /home/USR/SolOptXMR && ./util/temperature.py --max 70 --min 50 &
```

Where `--max 70` would consider the 70°C as overheat, and `--min 50` - 50°C as the target temperature while cooling down.

Notice, that although this will protect your computer from overheating, it will mess up some calculations but only to the extent, that the end results will indeed be different than expected, yet not totally erratic.
IMO, it's however better to preserve your hardware, rather than receiving a few picos more for having to settle with chips being molten down.

### p2pool autostart

As p2pool luckily doesn't require `root` privileges, you may run it from your user, or even from a special user, if you so desire. 
In this example we'll call this 2nd user `USR_P2POOL`, but it might remain the same `USR` as before.

```bash
sudo nano /etc/rc.local && sudo chmod +x /etc/rc.local 
```

and enter:

```bash
su - USR_P2POOL -c "sleep 40; cd /home/USR/SolOptXMR && ./util/run-p2pool.sh NODE_IP WALLET_ADDR" &
```

where the `NODE_IP` is the connection string for the Monero node, that has to be synced and running, while the `WALLET_ADDR` is the target address for payouts.

### Monero daemon autostart

If you decided to run your own node, then from within the computer, that is able to run 24/7, place the following line in its `/etc/rc.local`

```bash
su - USR -c "sleep 40; cd /home/USR/SolOptXMR && ./util/run-monero.sh" &
```

