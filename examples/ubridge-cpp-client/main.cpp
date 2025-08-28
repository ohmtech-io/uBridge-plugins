#include "uBridgeConfig.h"
#include "ubridgeClient.h"
#include <iostream>

using json = nlohmann::json;

void handleMessage(ubridge::message& message) {
  std::cout << "Topic: " << message.topic << "\n" << message.data.dump() << std::endl;
}

int main() {
  ubridge::Config cfg;
  ReqRepClient client(cfg.configSockUrl, cfg.streamSockUrl.c_str());

  if (client.connect() != 0) {
    std::cerr << "uBridge server not reachable" << std::endl;
    return 1;
  }

  json devices;
  if (client.getDevices(devices) == 0) {
    std::cout << devices.dump(2) << std::endl;
  }

  client.subscribe("/sensors", handleMessage);
  return 0;
}
