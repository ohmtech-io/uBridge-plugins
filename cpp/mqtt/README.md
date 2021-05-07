# ubridge MQTT plugin
Client application that acts as plugin for the uBridge server, subscribing to data from the connected uThing sensor units and publish the incoming data to an MQTT Broker

## Clone the repository
```
git clone --recursive https://github.com/ohmtech-io/uBridge-plugins.git
```

## Dependencies

We need to compile and install the Paho C++ library:
### SSL
```
sudo apt-get install libssl-dev
```
### Paho MQTT C++ library
We need to build first the Paho C library:
```
cd ubridge-plugins/cpp/3dparty/paho.mqtt.cpp

git clone https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c
git checkout v1.3.8

cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON \
-DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON
sudo cmake --build build/ --target install
sudo ldconfig
```

Now Paho C++:
```
cd ..
cmake -Bbuild -H.
sudo cmake --build build/ --target install
sudo ldconfig
```

## Building Plugin
```
cd ubridge-plugins/cpp/mqtt
mkdir build 

cd build && cmake ..
make
```
## Installing service
```
sudo make install
sudo systemctl enable ubridge-mqtt-plugin
```
## Starting the service
```
sudo systemctl start ubridge-mqtt-plugin
```
## Monitoring
```
systemctl status ubridge-mqtt-plugin.service
```
### Log file
```
tail /tmp/ubridge-mqtt.log
```
