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

#include <SPI.h>
class System : public RicCoreSystem<System,SYSTEM_FLAG,Commands::ID>
{
    public:

        System();
        
        void systemSetup();

        void systemUpdate();

    private:

        SPIClass vspi;
        SPIClass customSPI;
        SPISettings settingsx1280;
        SX1280 radio = new Module(1, 9, 11, 10, customSPI, settingsx1280);
        


};