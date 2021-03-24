# ubridge-plugins
Client applications that acts as plugins for the uBridge server

## Dependencies

sudo apt-get install libcurl-dev (TODO: specific versions on Ubunty and RPi, find common?)

## Building

mkdir build 

cd build && cmake ..
make

## Installing service

sudo make install
sudo systemctl enable ubridge-influx-plugin

## Starting the service

sudo systemctl start ubridge

## Monitoring
 systemctl status ubridge-influx-plugin.service

### Log file
 tail /tmp/ubridge-influx.log
