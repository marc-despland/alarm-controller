# alarm-controller
A daemon to activate or deactivate alarm when a smartphone is on bluetooth range.

The daemon look for the target every 10s.
When it detect the entrance of the device, it execute the stop script.
When it detect the device it out of range, it execute the start script.


```
make
```

```
sudo make install
```


