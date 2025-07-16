#pragma once

#include <libriccore/riccoresystem.h>

#include <memory>

#include "Config/systemflags_config.h"
#include "Config/commands_config.h"
#include "Config/pinmap_config.h"

#include "Commands/commands.h"

#include "Storage/sdfat_store.h"
#include "Storage/sdfat_file.h"
#include <RadioLib.h>

#include "Network/Interfaces/radio.h"

#include <SPI.h>
class System : public RicCoreSystem<System, SYSTEM_FLAG, Commands::ID>
{
public:
    System();

    typedef enum
    {
        PA_MODE_NONE = 0,
        PA_MODE_TX,
        PA_MODE_RX
    } PaMode_t;

    void initAmpPins();

    void setAmpPaTX();

    void setAmpBypassTX();

    void setAmpLNARX();

    void setAmpBypassRX();

    void selectAmpAnt(int port);

    void systemSetup();

    void systemUpdate();

    String receivePacket();

    // int sendPacket(std::vector<uint8_t>);

    int sendPacket(std::vector<uint8_t> packetData);
    int setRadioSettings(double freq, double bw, int sf, int cr, int crc, bool highSens, int preLen);

    void debugPacketSniffer(uint8_t *buf, size_t len, size_t packetLen, int state);

    void ensurePaMode(PaMode_t want);

    std::vector<uint8_t> makePacket();

    float currRSSI;
    float currSNR;
    bool badCRC;

private:
    SPIClass customSPI;
    SPISettings settingsx1280;
    SX1280 radio = new Module(PinMap::SPI_SX1280_CS, PinMap::SX1280_DIO1, PinMap::SX1280_RST, PinMap::SX1280_BUSY, customSPI, settingsx1280);
    Radio link2;
};