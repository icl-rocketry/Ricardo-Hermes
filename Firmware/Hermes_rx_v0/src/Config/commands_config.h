#pragma once

#include <stdint.h>
#include <unordered_map>
#include <functional>
#include <initializer_list>

#include <libriccore/commands/commandhandler.h>
#include <librnp/rnp_packet.h>

#include "Config/forward_decl.h"
#include "Commands/commands.h"

namespace Commands
{
    enum class ID : uint8_t
    {
        NoCommand = 0,
        Get_RSSI = 5,
        Free_Ram = 250,
        Push_Settings = 25,
        test_CMD = 55
    };

    inline std::initializer_list<ID> defaultEnabledCommands = {ID::Free_Ram, ID::Get_RSSI, ID::Push_Settings, ID::test_CMD};

    inline std::unordered_map<ID, std::function<void(ForwardDecl_SystemClass &, const RnpPacketSerialized &)>> command_map{
        {ID::Free_Ram, FreeRamCommand}, {ID::Get_RSSI, getRSSI}, {ID::Push_Settings, pushSettings}, {ID::test_CMD, testCMD}};

};