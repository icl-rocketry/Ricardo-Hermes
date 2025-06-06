#pragma once

#include "dynamichandlerbase.h"

#include <vector>
#include <string>
#include <memory>

#include <ArduinoJson.h>

#include <librrc/Helpers/rrclog.h>
#include <librrc/Helpers/jsonconfighelper.h>



template<class Object,class Derived>
class ConfigurableDynamicHandler : public DynamicHandlerBase<Object,Derived> {
    public:
        ConfigurableDynamicHandler(LIBRRC::RRCLog::LogCallback_t logcb):
        DynamicHandlerBase<Object,Derived>(logcb)
        {};

        /**
         * @brief Can be overriden by derived in special cases but should suffice for most 
         * handler implementations. setupIndividual_impl called by use of crtp to remove virtual
         * dispatch overhead.
         * 
         * @param handlerconfig json array containing the config of the obejcts
         */
        void setup(JsonArrayConst handlerconfig)
        {
            static_cast<Derived*>(this)->setup_impl(handlerconfig);
        };

    protected:

        void setup_impl(JsonArrayConst handlerconfig){
            for (JsonObjectConst config: handlerconfig){
                size_t id = this->size();
                LIBRRC::JsonConfigHelper::checkConfigId(id,config);
                static_cast<Derived*>(this)->setupIndividual_impl(id,config);
            }
        }

};