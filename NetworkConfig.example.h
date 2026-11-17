#pragma once

// Example network configuration for the Arduino UDP sensor node.
//
// Setup:
// 1. Copy this file to NetworkConfig.h in the same directory.
// 2. Update the IP addresses, MAC address, and ports for your network.
// 3. NetworkConfig.h is excluded from Git via .gitignore.

#include <EthernetENC.h>

// Unique node identifier (last octet of the node's static IP).
const byte nodeIpLastOctet = 178;

// Unique MAC address for this node.
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE,
  (byte)(0xEE + nodeIpLastOctet - 177)
};

// Node network configuration.
IPAddress ip(192, 168, 1, nodeIpLastOctet);
IPAddress dnsServer(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

unsigned int localPort = 8888;

// UDP destination (Go edge gateway).
IPAddress targetIP(192, 168, 1, 100);
unsigned int targetPort = 8080;