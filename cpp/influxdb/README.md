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
git clone --recursive https://github.com/ohmtech-io/uBridge-plugins.git

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

### mosquito_sub client CLI
We can use the "mosquitto" client to connect to the broker and subscribe to the messages that the plugin is publishing...

```
sudo apt-get install mosquitto-clients
```

Then we can run it providing the url, port and we use the wildcard # to subscribe to those topics. The "-v" option shows the topic also. For example:

```
mosquitto_sub -h broker.hivemq.com -p 1883 -v -t "v1/uThing/#"
```
### Log file
```
tail /tmp/ubridge-influx.log
```
