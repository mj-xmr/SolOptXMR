# Safety
This section desribes safety considerations, subjectively sorted by the most important to the least important. Please read the section, even if most parts sound obvious to you.
Bear in mind, that I'm not an electrician and I can't take legal responsibility for injuries or financial damages, that I might cause by giving you wrong advice. 

## Short circuiting (SC)
SC means connecting (+) and (-) to each other while there's energy stored or being produced, that would cause the electricity to flow between them. 
If this doesn't cause death or injury, it could at least damage your hardware. 
A SC can be caused not only by connecting the nodes directly, but also indirectly, when you for example touch the (+) and the (-) with your hands simultaneously. 
In such case, the charge passes right through you and definitely causing harm.
Unfortunatly a similar situation might occur, if you touch the (+) and are standing on the floor, without shoes, that would otherwise serve as isolation. If the floor is wet, it worsens the situation. 

## Terminals: connecting and disconnecting
When connecting and disconnecting terminals the following rules apply, that minimize the risk of electro shock: 
- connecting: connect + first (red), then - (black)
- disconnecting: disconnect - first (black) then + (red)

Or as a rule of thumb easy to remember: the + (red) can always be connected alone.
The same rule applies to all the other bipolar connectors as well.

## Voltage of the system
The general rule is, that from the safety perspective's alone, the lower the voltage, the safer the system is.
While a 24V system delivers slightly more economic alternative (in an optimistic case) to the safer 12V system, it might not be worth it, especially if you have nasty kids running around.

## Fire extinguishers
We recommend keeping at least a small fire extinguisher (FE) near the batteries, MPPT controller and the inverter (in that order). 
Make sure that the FE is suitable for dealing with electrical devices. You can buy cheaply FE rated up to 1000V. 
Except for the inverter, which produces 230V AC, the remaining parts of the system are mere 24V DC at best, if not the recommended 12V DC.
In case of a fire event, make sure to keep as much distance to the device being extinguished as possible, as this reduces your risks.

## MPPT cutoff voltage
The MPPT controllers cutoff any input that would exceed the maximal voltage, that you may set to prevent your batteries from overloading.
Overloading of the batteries leads not only to a temporary shut down of the inverter, but to such an increase of the batteries' internal temperature, that their liquids start to boil. 
Obviously this accelerates corrosion but leads to other unwanted effects as well, like toxic and flammable gas emissions and consequently such an increase of the internal pressure, that they might leak or even explode.
I personally set my maximal voltage to 13.4V for a 12V system.

## Battery conservation - protective goggles
In the case of calcium batteries, their conservation involves refilling their cell(s) with a tiny dose of distilled water.
The reason for this requirement is that as the batteries' internal temperature rises, the electrolyte evaporates.
In order to refill the electrolyte, one of the screws on top of the battery has to be unscrewed. 
**Before staring to unscrew the screwes put on protective googles**.
The reason for this is, that depending on how much cumulative overheats the batteries had to experience, so will be the built up pressure inside. 
This pressure will find its way out as soon as you start unscrewing the battery.
At the same time the air will typically contain traces of the electrolyte, that can be dangerous to your health.
Don't look at the burst with your remaining eye.
Obviously protective gloves also wouldn't hurt.


## MPPT reaction time
Bear in mind, that they react with a delay, so if you set the voltage high but still, say, 1V below the maximal sane batteries' voltage (14V vs 15V), exceeding of which would temporarly shut down your inverter, the delay in reaction of the MPPT controller may still lead to overvoltage, as defined by the inverter. 
This is very apparent during cloudy days.
The reason for this confusion is, that as soon as the battery is >80% charged, the voltage starts raising much faster with the same energy input, therefore the previous assumptions of slower growth and adjusted reaction don't hold anymore.

Fingers crossed!
