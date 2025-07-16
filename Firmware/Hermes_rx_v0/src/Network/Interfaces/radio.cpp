#include "radio.h"

#include <libriccore/riccorelogging.h>

#include "system.h"
// #include <RadioLib.h>

#include <Config/types.h>
#include <Config/systemflags_config.h>

#include <memory>
#include <vector>

#include <librnp/rnp_interface.h>
#include <librnp/rnp_packet.h>

#include "Config/pinmap_config.h"
System sm;
Radio::Radio(uint8_t id, std::string name) : RnpInterface(id, name) {
                                             };
void Radio::setup()
{
    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("[Radio Interface] setup called");
};

void Radio::sendLowLevelPacket(System &sm, std::vector<uint8_t> &packetData)
{
    sm.sendPacket(packetData);
}

void Radio::sendPacket(RnpPacket &data)
{
    std::vector<uint8_t> serializedPacket;
    data.serialize(serializedPacket);
    sendLowLevelPacket(sm, serializedPacket);
    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("[Radio Interface] sendPacket called");
}

void Radio::update()
{

    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("[Radio Interface] update called");
    delay(1000);
}

const RnpInterfaceInfo *Radio::getInfo()
{
    return NULL;
}

/*
// Radio::Radio(uint8_t id, std::string name) : RnpInterface(id, name);

void Radio::sendPacket(System &sm, RnpPacket &data)
{
    std::vector<uint8_t> serializedPacket;
    data.serialize(serializedPacket);
    sm.
}

size_t Radio::send(std::vector<uint8_t> &data)
{
}

*/