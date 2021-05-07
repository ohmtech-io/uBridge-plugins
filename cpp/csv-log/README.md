# ubridge CSV plugin
Client application that acts as plugin for the uBridge server, subscribing to data from the connected uThing sensor units and writting to disk on CSV format.

__NOTE__: the location of the logged files, and format configuration can be changed on the _csv-pluging-config.json_ file.
## Clone the repository
```
git clone --recursive https://github.com/ohmtech-io/uBridge-plugins.git
```
(or 'git submodule init' if you've cloned it without the --recursive)

## Building
```
cd ubridge-plugins/cpp/csv
mkdir build 

cd build && cmake ..
make
```
## Installing service
```
sudo make install
sudo systemctl enable ubridge-csv-plugin
```
## Starting the service
```
sudo systemctl start ubridge
```
## Monitoring
```
systemctl status ubridge-csv-plugin.service
```
### Log file
```
tail /tmp/ubridge-csv.log
```
