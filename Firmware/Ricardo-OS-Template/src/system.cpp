#include "system.h"

#include <memory>
#include <string>

#include <libriccore/riccoresystem.h>

#include <HardwareSerial.h>

#include "Config/systemflags_config.h"
#include "Config/commands_config.h"
#include "Config/pinmap_config.h"
#include "Config/general_config.h"

#include "Commands/commands.h"
#include <librnp/default_packets/simplecommandpacket.h>

#include "States/idle.h"
#include <RadioLib.h>

#define SPI_CS 1
#define SPI_SCK 2
#define SPI_MOSI 4
#define SPI_MISO 5
#define SX1280_DIO1 9 // txd led
#define SX1280_DIO2 8 // rxd led
#define SX1280_DIO3 7
#define SX1280_BUSY 10
#define SX1280_RST 11
#define AMP_ANT_SEL 35 // 0 is SMA, 1 is UFL
#define AMP_CSD 34     // AMP off (sleeping) -> 0, AMP on -> 1
#define AMP_CRX 33     // Enable RX
#define AMP_CHL 17     // high power TX -> 1, Low power TX -> 0
#define AMP_CTX 13     // Enable TX
#define AMP_CPS 14     // Bypass mode

#define TXD_ESP_UART 43
#define RXD_ESP_UART 44
#define TXD_UART 41
#define RXD_UART 42

/*
Pin means amp pin itself
 Mode | Description               | CSD (Pin 3) | CPS (Pin 15) | CRX (Pin 2) | CTX (Pin 17) | CHL (Pin 19)
 -----+---------------------------+-------------+--------------+-------------+--------------+-------------
 0    | All off (sleep mode)      | 0           | X            | X           | X            | X
 1    | Receive LNA mode          | 1           | 0            | 1           | 0            | X
 2    | Transmit high-power mode  | 1           | 0            | X           | 1            | 1
 3    | Transmit low-power mode   | 1           | 0            | X           | 1            | 0
 4    | Receive bypass mode       | 1           | 1            | 1           | 0            | X
 5    | Transmit bypass mode      | 1           | 1            | X           | 1            | X
 6    | All off (sleep mode)      | 1           | X            | 0           | 0            | X
*/

static const uint16_t BLINK_INTERVAL = 500;

System::System() : RicCoreSystem(Commands::command_map, Commands::defaultEnabledCommands, Serial),
                   customSPI(1),
                   settingsx1280(2000000, MSBFIRST, SPI_MODE0) {};


void System::systemSetup()
{

  Serial.setRxBufferSize(GeneralConfig::SerialRxSize);
  Serial.begin(GeneralConfig::SerialBaud);

  delay(2000);

  //pinMode(SPI_CS, OUTPUT);
  //digitalWrite(SPI_CS, HIGH);

  // intialize rnp message logger
  loggerhandler.retrieve_logger<RicCoreLoggingConfig::LOGGERS::SYS>().initialize(networkmanager);

  // initialize statemachine with idle state
  statemachine.initalize(std::make_unique<Idle>(systemstatus, commandhandler));
  // any other setup goes here
  delay(200);
  customSPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_CS);
  delay(200);
  delay(200);

  pinMode(AMP_CSD, OUTPUT);
  pinMode(AMP_CPS, OUTPUT);
  pinMode(AMP_CRX, OUTPUT);
  pinMode(AMP_CTX, OUTPUT);
  pinMode(AMP_CHL, OUTPUT);
  digitalWrite(AMP_CSD, HIGH);
  digitalWrite(AMP_CPS, HIGH);
  digitalWrite(AMP_CRX, LOW);
  digitalWrite(AMP_CTX, HIGH);
  digitalWrite(AMP_CHL, LOW);
  //digitalWrite(RXD_UART, LOW);
  delay(200);
  //digitalWrite(RXD_UART, HIGH);


  Serial.print(F("[SX1280] Initializing ... "));
  // carrier frequency:           2400.0 MHz
  // bandwidth:                   812.5 kHz
  // spreading factor:            9
  // coding rate:                 7
  // output power:                10 dBm
  // preamble length:             12 symbols
  // CRC:                         enabled
  int state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true) { delay(10); }
  }


};
int count = 0;
void System::systemUpdate() {
  Serial.print(F("[SX1280] Transmitting packet ... "));

  // you can transmit C-string or Arduino string up to
  // 256 characters long
  String str = "Hello World! #" + String(count++);
  int state = radio.transmit(str);

  // you can also transmit byte array up to 256 bytes long
  /*
    byte byteArr[] = {0x01, 0x23, 0x45, 0x56, 0x78, 0xAB, 0xCD, 0xEF};
    int state = radio.transmit(byteArr, 8);
  */

  if (state == RADIOLIB_ERR_NONE) {
    // the packet was successfully transmitted
    Serial.println(F("success!"));

  } else if (state == RADIOLIB_ERR_PACKET_TOO_LONG) {
    // the supplied packet was longer than 256 bytes
    Serial.println(F("too long!"));

  } else {
    // some other error occurred
    Serial.print(F("failed, code "));
    Serial.println(state);

  }

  // wait for a second before transmitting again
  delay(1000);


};