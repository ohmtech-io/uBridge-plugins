# ubridge InfluxDB2.0 plugin
Client applications that acts as plugins for the uBridge server, subscribing to data from the uThing sensor units and publishing the data into an InfluxDB instance.

__NOTE:__ The application uses the "wire" protocol from InfluxDB. The Influx instance can be running on the same host of the _ubridge_ application or on a remote server. For authentication, uses the org/token pair generated from the Influx admin panel.

## Dependencies - LibCurl 
(TODO: specific versions on Ubunty and RPi, find common?)
### in Ubuntu & Debian
```
sudo apt-get install libcurl-dev 
```

### in Raspberry Pi
```
sudo apt-get install libcurl4-openssl-dev
```

## Building
```
git clone --recursive https://github.com/damancusonqn/ubridge-plugins.git

cd ubridge-plugins/cpp/influxdb
mkdir build 

cd build && cmake ..
make
```
## Installing service
```
sudo make install
sudo systemctl enable ubridge-influx-plugin
```
## Starting the service
```
sudo systemctl start ubridge
```
## Monitoring
```
systemctl status ubridge-influx-plugin.service
```
### Log file
```
tail /tmp/ubridge-influx.log
```
