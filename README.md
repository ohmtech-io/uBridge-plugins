# ubridge-plugins
Client applications that acts as plugins for the uBridge server

![uBridge-plugins](/img/uThing-flow-temp-min.svg)

## uBridge

**uBridge** is a lightweight, free __Open Source__, modular C++ service that detects and manages _uThing_ devices, streams their measurements over NNG PUB/SUB, and exposes control and status via NNG REQ/REP. It’s designed to run very efficiently on Linux gateways and SBCs.

The system consists of a main service (`ubridge`) which manages connected uThing devices and publishes data to clients (`plugin` applications that can run on the same host or remotely).


# Plugins Installation
## 1. Clone the repository
```
git clone --recursive https://github.com/ohmtech-io/uBridge-plugins.git
```

## 2. Build and install the desired plugins
For building and installation instructions, check the README corresponding to each plugin.

More documentation on: https://docs.ohmtech.io 


## License
MIT. See `LICENSE`.
