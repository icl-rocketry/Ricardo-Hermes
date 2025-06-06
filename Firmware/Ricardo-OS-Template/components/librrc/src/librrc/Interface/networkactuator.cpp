#include <Arduino.h> // needed for millis()


#include "networkactuator.h"
#include <librnp/default_packets/simplecommandpacket.h>

#include <librrc/Packets/nrcpackets.h>
#include <librrc/componentstatusflags.h>

void NetworkActuator::execute(int32_t param){
    

    SimpleCommandPacket execute_packet(static_cast<uint8_t>(NRCPacket::NRC_COMMAND_ID::EXECUTE),param); 
    execute_packet.header.type = static_cast<uint8_t>(NRCPacket::TYPES::NRC_COMMAND);
    execute_packet.header.source = _networkmanager.getAddress();
    execute_packet.header.source_service = _sourceService;
    execute_packet.header.destination = _address;
    execute_packet.header.destination_service = _destinationService;

    _networkmanager.sendPacket(execute_packet);

};

void NetworkActuator::updateState(){
    SimpleCommandPacket getstate_packet(static_cast<uint8_t>(NRCPacket::NRC_COMMAND_ID::GETSTATE),0); 
    getstate_packet.header.type = static_cast<uint8_t>(NRCPacket::TYPES::NRC_COMMAND);
    getstate_packet.header.source = _networkmanager.getAddress();
    getstate_packet.header.source_service = _sourceService;
    getstate_packet.header.destination = _address;
    getstate_packet.header.destination_service = _destinationService;

    _networkmanager.sendPacket(getstate_packet);
    lastStateRequestTime = millis();
};

void NetworkActuator::networkCallback(packetptr_t packetptr){
    if (packetptr->header.source != _address){
        return; // packet had incorrect data
    }
    switch (packetptr->header.type){
        case(static_cast<uint8_t>(NRCPacket::TYPES::NRC_STATE)):
        {
            NRCStatePacket nrcstatepacket(*packetptr); //deserialize state packet
            lastStateUpdateTime = millis();
            currentValue = nrcstatepacket.value;
            _state.trackRemoteState(nrcstatepacket.state);
        }
    
    }

};

void NetworkActuator::arm(int32_t arg){
    SimpleCommandPacket arm_packet(static_cast<uint8_t>(NRCPacket::NRC_COMMAND_ID::ARM),arg);  // future implementation can use a magic value here effectivley authenitcating the arm command?
    arm_packet.header.type = static_cast<uint8_t>(NRCPacket::TYPES::NRC_COMMAND);
    arm_packet.header.source = _networkmanager.getAddress();
    arm_packet.header.source_service = _sourceService;
    arm_packet.header.destination = _address;
    arm_packet.header.destination_service = _destinationService;

    _networkmanager.sendPacket(arm_packet);
}


void NetworkActuator::disarm()
{
    SimpleCommandPacket disarm_packet(static_cast<uint8_t>(NRCPacket::NRC_COMMAND_ID::DISARM),0);  // future implementation can use a magic value here effectivley authenitcating the arm command?
    disarm_packet.header.type = static_cast<uint8_t>(NRCPacket::TYPES::NRC_COMMAND);
    disarm_packet.header.source = _networkmanager.getAddress();
    disarm_packet.header.source_service = _sourceService;
    disarm_packet.header.destination = _address;
    disarm_packet.header.destination_service = _destinationService;

    _networkmanager.sendPacket(disarm_packet);
}