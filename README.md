# alarm-controller
A daemon to activate or deactivate alarm when a smartphone is on bluetooth range.

The daemon look for the target every 10s.

When it detect the entry of the device in the bluetooth reception area, it execute the ''entry'' script.

When it detect the device is leaving the area, it execute the ''leaving'' script.

To test the smartphone presence we send a SDP request to the bluetooth mac address of the device and request the description of the service '''UUID: Service Discovery Serve.. (00001000-0000-1000-8000-00805f9b34fb)'''

You can use bluetoothctl to check the mac adress of your device

Make your device detectable, then

```
$ bluetoothctl
[bluetooth]# scan on
Discovery started
[CHG] Controller 00:**:**:**:**:14 Discovering: yes
[CHG] Device 2C:**:**:**:**:18 RSSI: -37
[bluetooth]# devices
Device 2C:**:**:**:**:18 Nosferatu
[bluetooth]# info 2C:**:**:**:**:18
Device 2C:**:**:**:**:18
	Name: Nosferatu
	Alias: Nosferatu
	Class: 0x7a020c
	Icon: phone
	Paired: yes
	Trusted: yes
	Blocked: no
	Connected: no
	LegacyPairing: no
	UUID: Vendor specific           (00000000-deca-fade-deca-deafdecacafe)
	UUID: Service Discovery Serve.. (00001000-0000-1000-8000-00805f9b34fb)
	UUID: Audio Source              (0000110a-0000-1000-8000-00805f9b34fb)
	UUID: A/V Remote Control Target (0000110c-0000-1000-8000-00805f9b34fb)
	UUID: A/V Remote Control        (0000110e-0000-1000-8000-00805f9b34fb)
	UUID: NAP                       (00001116-0000-1000-8000-00805f9b34fb)
	UUID: Handsfree Audio Gateway   (0000111f-0000-1000-8000-00805f9b34fb)
	UUID: Phonebook Access Server   (0000112f-0000-1000-8000-00805f9b34fb)
	UUID: Message Access Server     (00001132-0000-1000-8000-00805f9b34fb)
	UUID: PnP Information           (00001200-0000-1000-8000-00805f9b34fb)
	Modalias: bluetooth:v004Cp7005d0A30
	RSSI: -37
```


```
make
sudo make install
```

The software will be installed in /opt/alarm-controller

The configuration file is /opt/alarm-controller/etc/alarm-controller.conf

Change the target value to the bluetooth mac address of your device


