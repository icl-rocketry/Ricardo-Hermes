#include "system.h"

#include <memory>
#include <string>

#include <libriccore/riccoresystem.h>

#include <HardwareSerial.h>

#include "Network/Interfaces/radio.h"

#include "Config/systemflags_config.h"
#include "Config/commands_config.h"
#include "Config/pinmap_config.h"
#include "Config/general_config.h"

#include "Commands/commands.h"
#include <librnp/default_packets/simplecommandpacket.h>

#include "States/idle.h"
#include <RadioLib.h>

#include <librnp/rnp_packet.h>

#define ERR_NONE 0
#define TUILOG 0
#define DEBUG true
volatile bool settingSwitcher = false;
volatile bool receivedFlag = false;
volatile bool sentFlag = false;
std::vector<uint8_t> GlobalReceivedPacket;

// this function is called when a complete packet
// is received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
ICACHE_RAM_ATTR void setRxFlag(void)
{
  // we got a packet, set the flag
  receivedFlag = true;
}

ICACHE_RAM_ATTR void setTxFlag(void)
{
  // we got a packet, set the flag
  sentFlag = true;
}

#define FREQUENCY 2495.0 // MHz
#define BANDWIDTH 1625.0 // kHz
// #define SPREADING_FACTOR 6 // 5-12
#define SPREADING_FACTOR 7 // 5-12
#define CODING_RATE 7      // 5-8
// #define CODING_RATE 5      // 5-8
#define SYNC_WORD RADIOLIB_SX128X_SYNC_WORD_PRIVATE
#define TX_POWER -1        // dBm (for TX power reporting)
#define PREAMBLE_LENGTH 20 // symbols

int radiostate = 555;

enum PaMode_t
{
  PA_MODE_NONE = 0,
  PA_MODE_TX,
  PA_MODE_RX
};

void System::initAmpPins()
{
  pinMode(PinMap::AMP_CSD, OUTPUT);
  pinMode(PinMap::AMP_CPS, OUTPUT);
  pinMode(PinMap::AMP_CRX, OUTPUT);
  pinMode(PinMap::AMP_CTX, OUTPUT);
  pinMode(PinMap::AMP_CHL, OUTPUT);
  pinMode(PinMap::AMP_ANT_SEL, OUTPUT);
}

void System::selectAmpAnt(int port)
{
  if (port == 1)
  {
    digitalWrite(PinMap::AMP_ANT_SEL, HIGH);
  }
  else
  {
    digitalWrite(PinMap::AMP_ANT_SEL, LOW);
  }
}

void System::setAmpPaTX()
{
  digitalWrite(PinMap::AMP_CSD, HIGH);
  digitalWrite(PinMap::AMP_CPS, LOW);
  digitalWrite(PinMap::AMP_CRX, LOW);
  digitalWrite(PinMap::AMP_CTX, HIGH);
  digitalWrite(PinMap::AMP_CHL, HIGH);
  digitalWrite(PinMap::AMP_ANT_SEL, LOW);
}

void System::setAmpLNARX()
{
  digitalWrite(PinMap::AMP_CSD, HIGH);
  digitalWrite(PinMap::AMP_CPS, LOW);
  digitalWrite(PinMap::AMP_CRX, HIGH);
  digitalWrite(PinMap::AMP_CTX, LOW);
  digitalWrite(PinMap::AMP_CHL, LOW);
}

void System::setAmpBypassRX()
{
  digitalWrite(PinMap::AMP_CSD, HIGH);
  digitalWrite(PinMap::AMP_CPS, HIGH);
  digitalWrite(PinMap::AMP_CRX, HIGH);
  digitalWrite(PinMap::AMP_CTX, LOW);
  digitalWrite(PinMap::AMP_CHL, LOW);
}

void System::setAmpBypassTX()
{
  digitalWrite(PinMap::AMP_CSD, HIGH);
  digitalWrite(PinMap::AMP_CPS, HIGH);
  digitalWrite(PinMap::AMP_CRX, LOW);
  digitalWrite(PinMap::AMP_CTX, HIGH);
  digitalWrite(PinMap::AMP_CHL, LOW);
}

System::PaMode_t currentPaMode = System::PA_MODE_NONE;

void System::ensurePaMode(PaMode_t want)
{
  if (currentPaMode == want)
  {
    return;
  }
  switch (want)
  {
  case PA_MODE_TX:
    setAmpPaTX();
    break;
  case PA_MODE_RX:
    setAmpLNARX();
    break;
  default:
    setAmpBypassRX();
    break;
  }
  currentPaMode = want;
}

static const uint16_t BLINK_INTERVAL = 500;

System::System() : RicCoreSystem(Commands::command_map, Commands::defaultEnabledCommands, Serial),
                   customSPI(HSPI),
                   settingsx1280(2000000, MSBFIRST, SPI_MODE0),
                   link2(2)

{};

void System::systemSetup()
{
  setCpuFrequencyMhz(240);

  Serial.setRxBufferSize(GeneralConfig::SerialRxSize);
  Serial.begin(GeneralConfig::SerialBaud);

  delay(2000);

  // intialize rnp message logger
  loggerhandler.retrieve_logger<RicCoreLoggingConfig::LOGGERS::SYS>().initialize(networkmanager);

  // initialize statemachine with idle state
  statemachine.initalize(std::make_unique<Idle>(systemstatus, commandhandler));
  // any other setup goes here
  delay(200);

  initAmpPins();
  selectAmpAnt(0);

  delay(10);
  customSPI.begin(PinMap::SPI_SX1280_SCK, PinMap::SPI_SX1280_MISO, PinMap::SPI_SX1280_MOSI, PinMap::SPI_SX1280_CS);
  delay(1000);

  int state = radio.begin(FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE,
                          SYNC_WORD, TX_POWER, PREAMBLE_LENGTH);
  setRadioSettings(-1, -1, -1, -1, -1, true, -1);

  if (state == RADIOLIB_ERR_NONE)
  {
    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("SX1280 Init success");
  }
  else
  {

    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("SX1280 is fucked" + std::to_string(state));
  }
  radio.setPacketReceivedAction(setRxFlag);
  // radio.setPacketSentAction(setTxFlag);
  delay(200);
  networkmanager.addInterface(&link2);
  networkmanager.enableAutoRouteGen(true);
};

int System::setRadioSettings(double freq, double bw, int sf, int cr, int crc, bool highSens, int preLen)
{
  String rxShit;
  settingSwitcher = true;
  receivedFlag = false;
  radio.readData(rxShit);
  // These two don't seem to be needed (hopefully)
  //  radio.clearIrqFlags(RADIOLIB_SX128X_IRQ_TX_DONE);
  //  radio.clearIrqFlags(RADIOLIB_SX128X_IRQ_RX_DONE);
  radio.clearIrqFlags(RADIOLIB_SX128X_IRQ_ALL);
  int16_t err;

  // Carrier frequency: skip if freq == -1.0f
  if (freq != -1)
  {
    err = radio.setFrequency(freq);
    if (err != ERR_NONE)
    {
      RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Failed to set frequency");
      return -1;
    }
  }

  // Bandwidth: skip if bw == -1
  if (bw != -1)
  {
    err = radio.setBandwidth(bw);
    if (err != ERR_NONE)
    {
      RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Failed to set bandwith");
      return -1;
    }
  }

  // Spreading factor: skip if sf == -1
  if (sf != -1)
  {
    err = radio.setSpreadingFactor(sf);
    if (err != ERR_NONE)
    {
      RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Failed to set spreading factor");
      return -1;
    }
  }

  // Coding rate: skip if cr == -1
  if (cr != -1)
  {
    err = radio.setCodingRate(cr);
    if (err != ERR_NONE)
    {
      RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Failed to set coding rate");
      return -1;
    }
  }

  // CRC length: skip if crc == -1
  if (crc != -1)
  {
    err = radio.setCRC(crc);
    if (err != ERR_NONE)
    {
      RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Failed to set CRC");
      return -1;
    }
  }

  // High sensitivity mode: always call (bool can't be -1)
  err = radio.setHighSensitivityMode(highSens);
  if (err != ERR_NONE)
  {
    return -1;
    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Failed to set high sens RX mode");
  }

  // Preamble length: skip if preLen == -1
  if (preLen != -1)
  {
    err = radio.setPreambleLength(preLen);
    if (err != ERR_NONE)
    {
      RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Failed to set preamble");
      return -1;
    }
  }
  // reset flag
  settingSwitcher = false;

  return 0;
}

void System::debugPacketSniffer(uint8_t *buf, size_t len, size_t packetLen, int state)
{
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("[SX1280] Received packet!"));
    Serial.println();
    Serial.print(F("[SX1280] Packet length:\t"));
    Serial.println(packetLen);

    // 1) Dump raw bytes in hex, two digits each, separated by space
    Serial.print(F("[SX1280] Data bytes:\t"));
    for (size_t i = 0; i < len; ++i)
    {
      // ensure two‐digit hex
      if (buf[i] < 0x10)
        Serial.print('0');
      Serial.print(buf[i], HEX);
      Serial.print(' ');
    }
    Serial.println();

    // 2) Attempt to cast each byte to ASCII; non‐printables as '.'
    Serial.print(F("[SX1280] As ASCII:\t"));
    for (size_t i = 0; i < len; ++i)
    {
      char c = static_cast<char>(buf[i]);
      if (c >= 32 && c <= 126)
      {
        Serial.print(c);
      }
      else
      {
        Serial.print('.');
      }
    }
    Serial.println();

    // 3) Radio metrics
    Serial.print(F("[SX1280] RSSI:\t\t"));
    Serial.print(radio.getRSSI());
    Serial.println(F(" dBm"));

    Serial.print(F("[SX1280] SNR:\t\t"));
    Serial.print(radio.getSNR());
    Serial.println(F(" dB"));

    Serial.print(F("[SX1280] Frequency Error:\t"));
    Serial.print(radio.getFrequencyError());
    Serial.println(F(" Hz"));
  }
  else if (state == RADIOLIB_ERR_CRC_MISMATCH)
  {
    Serial.println(F("[SX1280] CRC error!"));
  }
  else
  {
    Serial.print(F("[SX1280] failed, code "));
    Serial.println(state);
  }
}

String System::receivePacket()
{
  ensurePaMode(System::PA_MODE_RX);
  badCRC = false;
  String receivedShit = "";
  std::vector<uint8_t> receivedPacketBuffer = {};

  if (!settingSwitcher)
  {
    radio.startReceive();
  }
  // check if the flag is set
  while (!receivedFlag && !settingSwitcher)
  {
    delay(2);
  }
  if (receivedFlag)
  {
    /*
    size_t len = radio.getPacketLength(true);
    receivedPacketBuffer.resize(len);
    int state = radio.readData(receivedPacketBuffer.data(), len);

    std::unique_ptr<RnpPacketSerialized> packet_ptr;
    packet_ptr = std::make_unique<RnpPacketSerialized>(receivedPacketBuffer);
    GlobalReceivedPacket = receivedPacketBuffer;
    */

    /*
      byte byteArr[8];
      int numBytes = radio.getPacketLength();
      int state = radio.readData(byteArr, numBytes);
    */

    // int len = 11;

    // reset flag
    receivedFlag = false;

    // read data into array
    int len = radio.getPacketLength();
    byte arr[len];
    int packetLen = len;
    int state = radio.readData(arr, len);

    // get link data
    currRSSI = radio.getRSSI();
    currSNR = radio.getSNR();
    if (state == RADIOLIB_ERR_CRC_MISMATCH)
    {
      badCRC = true;
    }

    // TUI data log mode for range test
    if (TUILOG == 1)
    {
      Serial.print("[SX1280] RSSI: ");
      Serial.println(currRSSI);
      Serial.print("[SX1280] SNR: ");
      Serial.println(currSNR);
      if (badCRC == 1)
      {
        Serial.println("[SX1280] BAD CRC");
      }
    }

    if (DEBUG == 1)
    {
      debugPacketSniffer(arr, len, len, state);
      /*
            if (state == RADIOLIB_ERR_NONE)
            {
              Serial.println(F("[SX1280] Received packet!"));
              Serial.println();
              Serial.print("[SX1280] packet length:\t\t");
              Serial.println(packetLen);

              // print data of the packet
              Serial.print(F("[SX1280] Data:\t\t"));
              for (size_t i = 0; i < len; ++i)
              {
                Serial.print(static_cast<char>(arr[i]));
              }
              Serial.println();
              // print RSSI (Received Signal Strength Indicator)
              Serial.print(F("[SX1280] RSSI:\t\t"));
              Serial.print(radio.getRSSI());
              Serial.println(F(" dBm"));

              // print SNR (Signal-to-Noise Ratio)
              Serial.print(F("[SX1280] SNR:\t\t"));
              Serial.print(radio.getSNR());
              Serial.println(F(" dB"));

              // print the Frequency Error
              // of the last received packet
              Serial.print(F("[SX1280] Frequency Error:\t"));
              Serial.print(radio.getFrequencyError());
              Serial.println(F(" Hz"));
            }
            else if (state == RADIOLIB_ERR_CRC_MISMATCH)
            {
              // packet was received, but is malformed
              Serial.println(F("CRC error!"));
            }
            else
            {
              // some other error occurred
              Serial.print(F("failed, code "));
              Serial.println(state);
            }
              */
    }
  }

  return "FixThisLater";
}

int System::sendPacket(std::vector<uint8_t> packetData)
{
  ensurePaMode(System::PA_MODE_TX);
  int txState1 = -1;
  const uint8_t *dataPointer = packetData.data();
  size_t dataLength = packetData.size();

  if (!settingSwitcher)
  {
    // transmit data
    txState1 = radio.transmit(dataPointer, dataLength, 0);
  }

  if (DEBUG == 1)
  {
    Serial.print("[SX1280] Transmit finished, state:\t\t");
    Serial.println(txState1);
  }

  return txState1;
}

std::vector<uint8_t> System::makePacket()
{
  RnpPacket syncPacket(RnpHeader{});
  // update start byte to refelct different protocol
  syncPacket.header.start_byte = 0xFC;

  std::vector<uint8_t> serializedSyncPacket;
  syncPacket.serialize(serializedSyncPacket);
  return serializedSyncPacket;
}

std::vector<uint8_t> dirtyPacket = {
    0x68, // 'h'
    0x65, // 'e'
    0x6C, // 'l'
    0x6C, // 'l'
    0x6F, // 'o'
    0x20, // ' ' (space)
    0x77, // 'w'
    0x6F, // 'o'
    0x72, // 'r'
    0x6C, // 'l'
    0x64  // 'd'
};
void System::systemUpdate() {
  // used for simple range test
  /*
  Serial.println("Transmitting packet, we have: " + sendPacket(makePacket()));
  delay(1000);
  Serial.print("Looping, state of radio: ");
  Serial.println(radiostate);
  */

  /*
  receivePacket();
  delay(100);
  */
  //  RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("I2C pwm expander failed to respond");
};