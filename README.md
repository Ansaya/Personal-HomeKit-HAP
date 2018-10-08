# HomeKit Accessory Protocol Library
Developing some domotic stuff? Want to open your garage door just yelling at Siri? With HAP library this is exactly what you can do!
If your device has support for C++11 and is connected to a wifi then it's ready to go.

HAP library enables you to advertise an HomeKit bridge accessory through the Bonjour multicast DNS APIs: the bridge acts as a network 
switch where multiple accessories can be attached and then controlled through the HomeKit app on your iPhone.

Any supported type of accessory can be implemented using libHAP, which will take care of all the backend stuff to correctly 
advertise the the accessory and trigger registerd callbacks when required from the HomeKit app.

## Quick start
To have a quick showcase of what are the possibilites with libHAP just run the install.sh script: it will install necessary dependencies, 
compile the library and a little showcase application. Now launch the showcase executable, pick your iPhone and connect to the bridge 
named "libHAP tester" (password 12345678) and enjoy some of the most common accessories you can expose through this library.

Now keep an eye on the command line while clicking around on accessory properties and you'll see every change triggers a callback
from the library: that's where your accessory will be attached to.

As you'll see from the initialization code in Example main, each accessory has its services and each service has its characteristics:
accessory's services are up to you, your accessory can have any service you want without restrictions; for services insted there are 
some constraints you can find in official Apple HomeKit documentation, where all available and supported services are described, with 
specifications on required and optional characteristics which should/can be added to a specific service.
In order to ease your job I've added some of the most common services as helper methods in the Accessory class, so that you don't
have to read any documentation, but if you need others you can just configure them through the libHAP adding needed characteristics 
to your service.

### Attach your implementation to an HomeKit Accessory
To expose your accessory to the HomeKit application you will need to create an Accessory object, add an information service to it 
(info service is required for all types of accessories by the HomeKit protocol) and any other service you need. While adding a new 
service you should attach relative callbacks to its characteristics to let your implementation know when to act and what to do 
(for example, light service has the on/off characteristic which should be connected to your light on/off method).

Now that the accessory is configured just add it to the global AccessorySet and call the HAPService setupAndListen method to 
advertise your accessories through Bonjour mDNS service.

That's it! As simple as attaching your callbacks and you're ready to say "Hei Siri, turn on Bedroom Light".

--------------------------------------------------

## Special Thanks
I really want to thank the owner of the project I've forked who took care of all the real work about studying the HAP protocol 
documentation and implementing it all in C: my work was only to add some glitter and some C++ memory management stuff.

### Legal Notice
This library is intended for private use only, for commercial implementation of the HAP protocol just refer to official Apple 
documentation. If in any case there are some violation please contact me immediately and I'll take care of fixing what is needed.
