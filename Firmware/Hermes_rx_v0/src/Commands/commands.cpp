/**
 * @file commands.cpp
 * @author Kiran de Silva (kd619@ic.ac.uk)
 * @brief Implementation of commands for system
 * @version 0.1
 * @date 2023-06-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "commands.h"

#include <librnp/rnp_packet.h>
#include <libriccore/commands/commandhandler.h>

#include "system.h"

#include "esp_log.h"

void Commands::FreeRamCommand(System &sm, const RnpPacketSerialized &packet)
{

	// ESP_LOGI("ch", "%s", "deserialize");

	SimpleCommandPacket commandpacket(packet);

	// uint32_t freeram = esp_get_free_heap_size();
	uint32_t freeram = static_cast<uint32_t>(std::abs(sm.currRSSI));
	// avliable in all states
	// returning as simple string packet for ease
	// currently only returning free ram
	if (commandpacket.arg == 0)
	{
		MessagePacket_Base<0, static_cast<uint8_t>(decltype(System::commandhandler)::PACKET_TYPES::MESSAGE_RESPONSE)> message("FreeRam: " + std::to_string(freeram));
		// this is not great as it assumes a single command handler with the same service ID
		// would be better if we could pass some context through the function paramters so it has an idea who has called it
		// or make it much clearer that only a single command handler should exist in the system
		message.header.source_service = sm.commandhandler.getServiceID();
		message.header.destination_service = packet.header.source_service;
		message.header.source = packet.header.destination;
		message.header.destination = packet.header.source;
		message.header.uid = packet.header.uid;
		sm.networkmanager.sendPacket(message);
	}
	else if (commandpacket.arg == 1)
	{
		BasicDataPacket<uint32_t, 0, 105> responsePacket(freeram);
		responsePacket.header.source_service = sm.commandhandler.getServiceID();
		responsePacket.header.destination_service = packet.header.source_service;
		responsePacket.header.source = packet.header.destination;
		responsePacket.header.destination = packet.header.source;
		responsePacket.header.uid = packet.header.uid;
		sm.networkmanager.sendPacket(responsePacket);
	}
}

void Commands::testCMD(System &sm, const RnpPacketSerialized &packet)
{

	// ESP_LOGI("ch", "%s", "deserialize");

	SimpleCommandPacket commandpacket(packet);

	uint32_t rssi = 1337420;
	// avliable in all states
	// returning as simple string packet for ease
	// currently only returning free ram
	BasicDataPacket<uint32_t, 3, 103> responsePacket(rssi);
	responsePacket.header.source_service = sm.commandhandler.getServiceID();
	responsePacket.header.destination_service = packet.header.source_service;
	responsePacket.header.source = packet.header.destination;
	responsePacket.header.destination = packet.header.source;
	responsePacket.header.uid = packet.header.uid;
	sm.networkmanager.sendPacket(responsePacket);
}

void Commands::getRSSI(System &sm, const RnpPacketSerialized &packet)
{

	// ESP_LOGI("ch", "%s", "deserialize");

	SimpleCommandPacket commandpacket(packet);

	uint32_t rssi = static_cast<uint32_t>(std::abs(sm.currRSSI));
	// avliable in all states
	// returning as simple string packet for ease
	// currently only returning free ram
	BasicDataPacket<uint32_t, 0, 105> responsePacket(rssi);
	responsePacket.header.source_service = sm.commandhandler.getServiceID();
	responsePacket.header.destination_service = packet.header.source_service;
	responsePacket.header.source = packet.header.destination;
	responsePacket.header.destination = packet.header.source;
	responsePacket.header.uid = packet.header.uid;
	sm.networkmanager.sendPacket(responsePacket);
}

void Commands::pushSettings(System &sm, const RnpPacketSerialized &packet)
{

	SimpleCommandPacket commandpacket(packet);
	if (commandpacket.arg == 0)
	{
		MessagePacket_Base<0, static_cast<uint8_t>(decltype(System::commandhandler)::PACKET_TYPES::MESSAGE_RESPONSE)> message("Changed Freq to 2495.0 Mhz");
		Serial.println(sm.setRadioSettings(2495.0, -1, -1, -1, -1, -1, -1));
		// this is not great as it assumes a single command handler with the same service ID
		// would be better if we could pass some context through the function paramters so it has an idea who has called it
		// or make it much clearer that only a single command handler should exist in the system
		message.header.source_service = sm.commandhandler.getServiceID();
		message.header.destination_service = packet.header.source_service;
		message.header.source = packet.header.destination;
		message.header.destination = packet.header.source;
		message.header.uid = packet.header.uid;
		sm.networkmanager.sendPacket(message);
	}
	if (commandpacket.arg == 1)
	{
		MessagePacket_Base<0, static_cast<uint8_t>(decltype(System::commandhandler)::PACKET_TYPES::MESSAGE_RESPONSE)> message("Changed Freq to 2415.0 Mhz");
		Serial.println(sm.setRadioSettings(2415.0, -1, -1, -1, -1, -1, -1));
		// this is not great as it assumes a single command handler with the same service ID
		// would be better if we could pass some context through the function paramters so it has an idea who has called it
		// or make it much clearer that only a single command handler should exist in the system
		message.header.source_service = sm.commandhandler.getServiceID();
		message.header.destination_service = packet.header.source_service;
		message.header.source = packet.header.destination;
		message.header.destination = packet.header.source;
		message.header.uid = packet.header.uid;
		sm.networkmanager.sendPacket(message);
	}
	if (commandpacket.arg == 2)
	{
		MessagePacket_Base<0, static_cast<uint8_t>(decltype(System::commandhandler)::PACKET_TYPES::MESSAGE_RESPONSE)> message("Enabled high sens mode");
		sm.setRadioSettings(-1, -1, -1, -1, -1, true, -1);
		// this is not great as it assumes a single command handler with the same service ID
		// would be better if we could pass some context through the function paramters so it has an idea who has called it
		// or make it much clearer that only a single command handler should exist in the system
		message.header.source_service = sm.commandhandler.getServiceID();
		message.header.destination_service = packet.header.source_service;
		message.header.source = packet.header.destination;
		message.header.destination = packet.header.source;
		message.header.uid = packet.header.uid;
		sm.networkmanager.sendPacket(message);
	}
	if (commandpacket.arg == 3)
	{
		MessagePacket_Base<0, static_cast<uint8_t>(decltype(System::commandhandler)::PACKET_TYPES::MESSAGE_RESPONSE)> message("Disabled high sens mode");
		sm.setRadioSettings(-1, -1, -1, -1, -1, false, -1);
		// this is not great as it assumes a single command handler with the same service ID
		// would be better if we could pass some context through the function paramters so it has an idea who has called it
		// or make it much clearer that only a single command handler should exist in the system
		message.header.source_service = sm.commandhandler.getServiceID();
		message.header.destination_service = packet.header.source_service;
		message.header.source = packet.header.destination;
		message.header.destination = packet.header.source;
		message.header.uid = packet.header.uid;
		sm.networkmanager.sendPacket(message);
	}
}