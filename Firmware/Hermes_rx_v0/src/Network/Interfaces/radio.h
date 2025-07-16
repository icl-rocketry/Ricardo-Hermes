#pragma once
// c++ stl
#include <memory>
#include <vector>
#include <string>
#include <queue>
// arduino + esp32

#include <Arduino.h>
// #include <SPI.h>

#include "Config/forward_decl.h"
// Ric libraries

#include <libriccore/riccorelogging.h>

#include <librnp/rnp_interface.h>
#include <librnp/rnp_packet.h>
#include <Preferences.h>

#include <Config/types.h>
#include <Config/systemflags_config.h>

struct RadioInterfaceInfo : public RnpInterfaceInfo
{
    int rssi;
    int packet_rssi;
    float snr;
    float packet_snr;
    long freqError;
};

class Radio : public RnpInterface
{
public:
    Radio(uint8_t id = 2, std::string name = "Radio");
    void setup() override;

    void sendPacket(RnpPacket &data) override;
    void sendLowLevelPacket(System &system, std::vector<uint8_t> &packetData);
    void update() override;
    const RnpInterfaceInfo *getInfo() override;
    // void restart();

private:
    //  void getPacket();
    //  void checkSendBuffer();
    //  size_t send(std::vector<uint8_t> &data);
    //  void checkTx();
    //  void sendFromBuffer();
};

/*
enum class RADIO_MODE : uint8_t
{
    SIMPLE,
    TURN_TIMEOUT,
    SYNC
};

struct RadioInterfaceInfo : public RnpInterfaceInfo
{
    size_t sendBufferSize;
    bool sendBufferOverflow;

    RADIO_MODE mode;
    uint32_t prevTimeSent;
    uint32_t prevTimeReceived;

    int rssi;
    int packet_rssi;
    float snr;
    float packet_snr;
    long freqError;
};

struct RadioConfig
{
    long frequency; // in Hz
    uint8_t sync_byte;
    long bandwidth; // in Hz
    int spreading_factor;
    int txPower; // in dB
};

class Radio : public RnpInterface
{
public:
    Radio(uint8_t id = 2, std::string name = "Radio");
    void setup() override;
    void sendPacket(ForwardDecl_SystemClass &system, RnpPacket &data);
    size_t send(std::vector<uint8_t> &data);

private:
    Types::CoreTypes::SystemStatus_t &_systemstatus; // pointer to system status object
};

*/