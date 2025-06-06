#pragma once

#include "networkeddynamichandler.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <ArduinoJson.h>
#include <memory>
#include <functional>
#include <unordered_map>


#include <librnp/rnp_networkmanager.h>

#include <librrc/Helpers/rrclog.h>


/**
 * @brief Templated base class for handlers which handle flight components i.e deployment handler and engine handler
 * 
 * @tparam T 
 */

template<class Object,class Derived>
class FlightComponentHandler : public NetworkedDynamicHandler<Object,Derived>{
    public:
        FlightComponentHandler(RnpNetworkManager& networkmanager,uint8_t serviceID,LIBRRC::RRCLog::LogCallback_t logcb):
        NetworkedDynamicHandler<Object,Derived>(networkmanager,serviceID,logcb)
        {};

        uint8_t flightCheck(){
            return static_cast<Derived*>(this)->flightCheck_impl();
        }

        void armComponents(){
            static_cast<Derived*>(this)->armComponents_impl();
        }

        void disarmComponents(){
            static_cast<Derived*>(this)->disarmComponents_impl();
        }

        /**
         * @brief Requests a new state update from each component
         * 
         */
        void getCurrentStates(){
            for (auto& component : *this){
                component->updateState();
            }
        };

        /**
         * @brief Get the Action Func object called by event handler
         * 
         * @param componentID 
         * @return std::function<void(int32_t param)> 
         */
        std::function<void(int32_t param)> getActionFunc(uint8_t componentID)
        {
            if (componentID >= this->size())
            {
                #ifdef _RICDEBUG
                throw std::runtime_error("Component ID out of range: " + std::to_string(componentID));
                #endif
                this->_logcb("Component ID out of range: " + std::to_string(componentID));
                return {};
            }

            return [&componentID, this](int32_t param)
            { this->getObject(componentID)->execute(param); };
        };


};