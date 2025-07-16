/*
**********************
* PINS               *
**********************
*/
#pragma once

namespace PinMap
{

    static constexpr int H_MISO = 12;
    static constexpr int H_MOSI = 13;
    static constexpr int H_SCLK = 14;

    static constexpr int MagCs = 15;
    static constexpr int ImuCs_1 = 16;
    static constexpr int ImuCs_2 = 17;

    // static constexpr int V_SCLK = 18;
    // static constexpr int V_MISO = 19;
    static constexpr int _SDA = 21;
    static constexpr int _SCL = 22;
    // static constexpr int V_MOSI = 23;

    static constexpr int V_SCLK = 37;
    static constexpr int V_MISO = 38;
    static constexpr int V_MOSI = 36;
    static constexpr int sdcs = 4;

    // static constexpr int SdCs_1 = 25;
    // static constexpr int SdCs_2 = 26;
    // static constexpr int Buzzer = 27;

    static constexpr int TxCan = 32;
    static constexpr int RxCan = 33;

    // AMP

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
    static constexpr int AMP_ANT_SEL = 35; // 0 is SMA, 1 is UFL
    static constexpr int AMP_CSD = 34;     // AMP off (sleeping) -> 0, AMP on -> 1
    static constexpr int AMP_CRX = 33;     // Enable RX
    static constexpr int AMP_CHL = 17;     // high power TX -> 1, Low power TX -> 0
    static constexpr int AMP_CTX = 13;     // Enable TX
    static constexpr int AMP_CPS = 14;     // Bypass mode

    // SX1280
    static constexpr int SPI_SX1280_CS = 1;
    static constexpr int SPI_SX1280_SCK = 2;
    static constexpr int SPI_SX1280_MOSI = 4;
    static constexpr int SPI_SX1280_MISO = 5;
    static constexpr int SX1280_DIO1 = 9; // txd led
    static constexpr int SX1280_DIO2 = 8; // rxd led
    static constexpr int SX1280_DIO3 = 7;
    static constexpr int SX1280_BUSY = 10;
    static constexpr int SX1280_RST = 11;

    // UART
    static constexpr int TXD_ESP_UART = 43;
    static constexpr int RXD_ESP_UART = 44;
    static constexpr int TXD_UART = 41;
    static constexpr int RXD_UART = 42;

    // Pickle
    static constexpr int LoraReset = 11;
    static constexpr int LoraCs = 1;
    // might switch to busy, who knows lol
    static constexpr int LoraInt = 9;
    // static constexpr int TxCan = 27;
    // static constexpr int RxCan = 14;

    // static constexpr int TxCan = 33;
    // static constexpr int RxCan = 32;
    // static constexpr int SdDet_1 = 34;
    // static constexpr int SdDet_2 = 35;
    // static constexpr int BattVolt = 36;
    // static constexpr int LoraInt = 39;
};
