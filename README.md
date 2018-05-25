This README documents how I was able to use wireshark to sniff Zigbee packets using a [FreakLabs FreakUSB 2.4 GHz dongle](https://freaklabsstore.com/index.php?main_page=product_info&cPath=22&products_id=215).

I mostly followed the directions on [this blog post](https://archive.freaklabs.org/index.php/tutorials/software/feeding-the-shark-turning-the-freakduino-into-a-realtime-wireless-protocol-analyzer-with-wireshark.html) but I had to make some changes.

Environment:
* Ubuntu 18.04 LTS
* Wireshark 2.4.5
* Arduino 1.8.5 (since that was the version used in the FreakLabs tutorial)
* FreakUSB 2.4 GHz dongle + 3 dBi Antenna (the Freakduino 128P 2.4 GHz board should work just as well).

I followed the [Installing the chibiArduino Library & FreakLabs Board Files](https://freaklabs.org/just-added-installing-the-freaklabs-board-support-files-chibiarduino-library-video/) video after installing Arduino 1.8.5.


I modified the arduino chibi_ex09_wsbridge slightly to add selecting a channel. This should match the channel chosen by the Zigbee controller.
You can find my modified version [here](https://github.com/dhylands/freaklabs/tree/master/chibi_ex09_wsbridge).

For the mozilla-iot project, the channel number is printed to the log file when the zigbee adapter is started. You should see something like the
following:
```
2018-05-25 12:35:06.190 zigbee:        Device Type: 0x3000c - XStick
2018-05-25 12:35:06.190 zigbee:    Network Address: 0013a20040a3b8a9 0000
2018-05-25 12:35:06.190 zigbee:    Node Identifier: Master
2018-05-25 12:35:06.190 zigbee:  Configured PAN Id: ce20cf885305ef53
2018-05-25 12:35:06.191 zigbee:   Operating PAN Id: ce20cf885305ef53 599d
2018-05-25 12:35:06.191 zigbee:  Operating Channel: 20
2018-05-25 12:35:06.191 zigbee:  Channel Scan Mask: 1ffe
2018-05-25 12:35:06.191 zigbee:          Join Time: 0
2018-05-25 12:35:06.191 zigbee: Remaining Children: 10
2018-05-25 12:35:06.191 zigbee:      Stack Profile: 2
2018-05-25 12:35:06.191 zigbee:        API Options: 1
2018-05-25 12:35:06.191 zigbee: Encryption Enabled: 1
2018-05-25 12:35:06.191 zigbee: Encryption Options: 2
```
The `Operating Channel:` (which is 20 in the example above) is the channel you need to set.

The [wsbridge program](https://archive.freaklabs.org/index.php/wsbridge.html) has a serious flaw in the linux version. It winds up stripping the high
bit off of every byte received over the serial port. I put a corrected version [here](https://github.com/dhylands/freaklabs/tree/master/wsbridge/Linux)

In order to decode the Zigbee packets, you need to set the `Trust Center Link` key in wireshark.
* Choose Edit->Preferences
* Click the tippy triangle beside `Protocols` and scroll down to `Zigbee` and click on it.
* Select `AES Encryption, 32-bit Integrity Protection` for Security Level (it was already selected for me).
* Click on `Edit` for the `Pre-configured keys`
* Click on `+` (lower left)
* Enter "ZigBeeAlliance09" (with the quotes) into the key field. Note the capital B in ZigBee.
* A byte order of `Normal` is fine.
* I entered `Trust Center Link Key` as the label (this is optional)
* Click `Ok`

Enable the Zigbee protocols.
* Choose Ananlyze->Enabled Protocols
* Click on `Disable All`
* Enable `IEEE 802.15.4`
* Enable all of the protocols which start with `ZCL`
* Enable all of the protocols which start with `ZigBee`
* Click `Ok`

For the version of wireshark I was using (2.4.5), adding the /tmp/wireshark pipe interface didn't match up with the screen shots. This is how I did it:
* Start wsbridge (this will create the /tmp/wireshark named pipe).
* Choose Capture->Options and click on the `Manage Interfaces` button (bottom right)
* Click on the `Pipes` tab.
* Click on the `+` in the bottom left
* Make sure that you've already started wsbridge and that the /tmp/wireshark named pipe exists, otherwise wireshark won't save anything.
* Double click on the `New Pipe` text and type in `/tmp/wireshark` and press RETURN.
* Click OK
* If you sroll to the bottom of the list in the `Input` tab you should see the /tmp/wireshark entry.
* select it and click `Ok`

In order to properly decode the packets, wireshark needs to see the key exchange which occurs when you pair a device. So if your device
already paired, then unpair it and repair it so that wireshark can pick up the key (which is different from the `ZigBeeAlliance09` key referred to above).
Here's a screenshot showing an exchange between the Mozilla IOT gateway and a phillips Hue bulb:
![screenshot](wireshark.png?raw=true)
