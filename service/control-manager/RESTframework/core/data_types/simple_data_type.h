/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#ifndef WEBSERVICE_SIMPLE_DATA_TYPE_H_
#define WEBSERVICE_SIMPLE_DATA_TYPE_H_

#include <vector>
#include "data_types/primitive_data_type.h"

namespace webservice
{

// Following is default generated code.
    class AlarmCodeType : public _string
    {
        public:
            AlarmCodeType(void)
            {
                this->v_restriction.push_back("Normal");
                this->v_restriction.push_back("Device");
                this->v_restriction.push_back("Generic_Sensor");
                this->v_restriction.push_back("Motion");
                this->v_restriction.push_back("Sound");
            }
            ~AlarmCodeType(void) { this->v_restriction.clear(); }
    };

    class AudioCodecType : public _string
    {
        public:
            AudioCodecType(void)
            {
                this->v_restriction.push_back("G711");
                this->v_restriction.push_back("G726");
                this->v_restriction.push_back("AAC");
            }
            ~AudioCodecType(void) { this->v_restriction.clear(); }
    };

    class AuthenticationType : public _string
    {
        public:
            AuthenticationType(void)
            {
                this->v_restriction.push_back("Password");
                this->v_restriction.push_back("Card");
                this->v_restriction.push_back("Fingerprint");
                this->v_restriction.push_back("NFC");
                this->v_restriction.push_back("USB");
                this->v_restriction.push_back("Button");
            }
            ~AuthenticationType(void) { this->v_restriction.clear(); }
    };

    class ConnectivityType : public _string
    {
        public:
            ConnectivityType(void)
            {
                this->v_restriction.push_back("WiFi");
                this->v_restriction.push_back("Zigbee");
                this->v_restriction.push_back("Z-wave");
                this->v_restriction.push_back("Bluetooth");
                this->v_restriction.push_back("IR");
                this->v_restriction.push_back("Cellular");
            }
            ~ConnectivityType(void) { this->v_restriction.clear(); }
    };

    class CurrencyType : public _string
    {
        public:
            CurrencyType(void)
            {
                this->v_restriction.push_back("Unknown");
                this->v_restriction.push_back("Australian_Dollar");
                this->v_restriction.push_back("Canadian_Dollar");
                this->v_restriction.push_back("Korean_Won");
                this->v_restriction.push_back("US_Dollar");
                this->v_restriction.push_back("Euro");
            }
            ~CurrencyType(void) { this->v_restriction.clear(); }
    };

    class DateTimeType : public _dateTime
    {
    };

    class DayType : public _string
    {
        public:
            DayType(void)
            {
                this->v_restriction.push_back("Sun");
                this->v_restriction.push_back("Mon");
                this->v_restriction.push_back("Tue");
                this->v_restriction.push_back("Wed");
                this->v_restriction.push_back("Thu");
                this->v_restriction.push_back("Fri");
                this->v_restriction.push_back("Sat");
            }
            ~DayType(void) { this->v_restriction.clear(); }
    };

    class DeviceType : public _string
    {
        public:
            DeviceType(void)
            {
                this->v_restriction.push_back("Unknown");
                this->v_restriction.push_back("Gateway");
                this->v_restriction.push_back("Smartphone");
                this->v_restriction.push_back("Tablet");
                this->v_restriction.push_back("TV");
                this->v_restriction.push_back("AV");
                this->v_restriction.push_back("Refrigerator");
                this->v_restriction.push_back("Kimchi_Refrigerator");
                this->v_restriction.push_back("Washer");
                this->v_restriction.push_back("Dryer");
                this->v_restriction.push_back("Air_Conditioner");
                this->v_restriction.push_back("System_Air_Conditioner");
                this->v_restriction.push_back("Dishwasher");
                this->v_restriction.push_back("Oven");
                this->v_restriction.push_back("Microwave_Oven");
                this->v_restriction.push_back("Cooktop");
                this->v_restriction.push_back("Robot_Cleaner");
                this->v_restriction.push_back("Vacuum_Cleaner");
                this->v_restriction.push_back("Light");
                this->v_restriction.push_back("IP_Camera");
                this->v_restriction.push_back("Door_Lock");
                this->v_restriction.push_back("Door_Phone");
                this->v_restriction.push_back("Thermostat");
                this->v_restriction.push_back("Water_Heater");
                this->v_restriction.push_back("Smart_Plug");
                this->v_restriction.push_back("Generic_Sensor");
                this->v_restriction.push_back("Motion_Detector");
                this->v_restriction.push_back("Zigbee_Bridge");
                this->v_restriction.push_back("Zwave_Bridge");
                this->v_restriction.push_back("Coffee_Pot");
                this->v_restriction.push_back("Iron");
                this->v_restriction.push_back("Heater");
                this->v_restriction.push_back("Ventilator");
                this->v_restriction.push_back("Fan");
                this->v_restriction.push_back("Blind");
                this->v_restriction.push_back("Switch");
                this->v_restriction.push_back("Motor");
                this->v_restriction.push_back("Shutter");
            }
            ~DeviceType(void) { this->v_restriction.clear(); }
    };

    class DispenserShapeType : public _string
    {
        public:
            DispenserShapeType(void)
            {
                this->v_restriction.push_back("Water");
                this->v_restriction.push_back("Cubed");
                this->v_restriction.push_back("Crushed");
                this->v_restriction.push_back("None");
            }
            ~DispenserShapeType(void) { this->v_restriction.clear(); }
    };

    class DisplayType : public _string
    {
        public:
            DisplayType(void)
            {
                this->v_restriction.push_back("Not_Supported");
                this->v_restriction.push_back("LED");
                this->v_restriction.push_back("Segment");
                this->v_restriction.push_back("STN");
                this->v_restriction.push_back("LCD");
            }
            ~DisplayType(void) { this->v_restriction.clear(); }
    };

    class DryerType : public _string
    {
        public:
            DryerType(void)
            {
                this->v_restriction.push_back("Electricity");
                this->v_restriction.push_back("Gas");
                this->v_restriction.push_back("EPA");
            }
            ~DryerType(void) { this->v_restriction.clear(); }
    };

    class EventType : public _string
    {
        public:
            EventType(void)
            {
                this->v_restriction.push_back("Created");
                this->v_restriction.push_back("Received");
                this->v_restriction.push_back("Notified");
                this->v_restriction.push_back("Started");
                this->v_restriction.push_back("Finished");
                this->v_restriction.push_back("Deleted");
                this->v_restriction.push_back("Connected");
                this->v_restriction.push_back("Disconnected");
            }
            ~EventType(void) { this->v_restriction.clear(); }
    };

    class FlickerlessType : public _string
    {
        public:
            FlickerlessType(void)
            {
                this->v_restriction.push_back("F50");
                this->v_restriction.push_back("F60");
                this->v_restriction.push_back("Auto");
            }
            ~FlickerlessType(void) { this->v_restriction.clear(); }
    };

    class Float32 : public _float
    {
    };

    class HashType : public _string
    {
        public:
            HashType(void)
            {
                this->v_restriction.push_back("MD5");
                this->v_restriction.push_back("SHA-0");
                this->v_restriction.push_back("SHA-1");
                this->v_restriction.push_back("SHA-2");
                this->v_restriction.push_back("SHA-3");
            }
            ~HashType(void) { this->v_restriction.clear(); }
    };

    class HumidityUnitType : public _string
    {
        public:
            HumidityUnitType(void)
            {
                this->v_restriction.push_back("Percentage");
                this->v_restriction.push_back("Water_Vapor");
            }
            ~HumidityUnitType(void) { this->v_restriction.clear(); }
    };


    class ImageFormatType : public _string
    {
        public:
            ImageFormatType(void)
            {
                this->v_restriction.push_back("JPG");
                this->v_restriction.push_back("GIF");
                this->v_restriction.push_back("BMP");
                this->v_restriction.push_back("PNG");
            }
            ~ImageFormatType(void) { this->v_restriction.clear(); }
    };

    class Int8 : public _byte
    {
    };

    class Int16 : public _short
    {
    };

    class Int32 : public _int
    {
    };

    class Int64 : public _long
    {
    };

    class InternalLocationType : public _string
    {
        public:
            InternalLocationType(void)
            {
                this->v_restriction.push_back("Living_Room");
                this->v_restriction.push_back("Bed_Room");
                this->v_restriction.push_back("Study_Room");
                this->v_restriction.push_back("Dining_Room");
                this->v_restriction.push_back("Kitchen");
                this->v_restriction.push_back("Bathroom");
                this->v_restriction.push_back("Laundry_Room");
                this->v_restriction.push_back("Storage_Room");
                this->v_restriction.push_back("Balcony");
                this->v_restriction.push_back("Terrace");
                this->v_restriction.push_back("Garden");
                this->v_restriction.push_back("Parking_Garage");
                this->v_restriction.push_back("Lobby");
                this->v_restriction.push_back("Main_Entrance");
                this->v_restriction.push_back("Window");
                this->v_restriction.push_back("Door");
                this->v_restriction.push_back("Others");
            }
            ~InternalLocationType(void) { this->v_restriction.clear(); }
    };

    class LocationType : public _string
    {
        public:
            LocationType(void)
            {
                this->v_restriction.push_back("Unknown");
                this->v_restriction.push_back("World_Wide");
                this->v_restriction.push_back("Asia");
                this->v_restriction.push_back("North_America");
                this->v_restriction.push_back("South_America");
                this->v_restriction.push_back("Europe");
                this->v_restriction.push_back("Middle_East");
                this->v_restriction.push_back("Africa");
                this->v_restriction.push_back("Oceania");
            }
            ~LocationType(void) { this->v_restriction.clear(); }
    };

    class LockType : public _string
    {
        public:
            LockType(void)
            {
                this->v_restriction.push_back("Lock");
                this->v_restriction.push_back("Unlock");
            }
            ~LockType(void) { this->v_restriction.clear(); }
    };


    class MediaMode : public _string
    {
        public:
            MediaMode(void)
            {
                this->v_restriction.push_back("SendOnly");
                this->v_restriction.push_back("RecvOnly");
                this->v_restriction.push_back("SendRecv");
            }
            ~MediaMode(void) { this->v_restriction.clear(); }
    };

    class MediaType : public _string
    {
        public:
            MediaType(void)
            {
                this->v_restriction.push_back("Image");
                this->v_restriction.push_back("Video");
                this->v_restriction.push_back("Audio");
                this->v_restriction.push_back("Application");
                this->v_restriction.push_back("Data");
                this->v_restriction.push_back("Control");
            }
            ~MediaType(void) { this->v_restriction.clear(); }
    };

    class MessageType : public _string
    {
        public:
            MessageType(void)
            {
                this->v_restriction.push_back("Image");
                this->v_restriction.push_back("Text");
            }
            ~MessageType(void) { this->v_restriction.clear(); }
    };

    class MovementType : public _string
    {
        public:
            MovementType(void)
            {
                this->v_restriction.push_back("Stop");
                this->v_restriction.push_back("Forward");
                this->v_restriction.push_back("Backward");
                this->v_restriction.push_back("Left");
                this->v_restriction.push_back("Right");
            }
            ~MovementType(void) { this->v_restriction.clear(); }
    };

    class NotificationEventType : public _string
    {
        public:
            NotificationEventType(void)
            {
                this->v_restriction.push_back("Created");
                this->v_restriction.push_back("Notified");
                this->v_restriction.push_back("Deleted");
            }
            ~NotificationEventType(void) { this->v_restriction.clear(); }
    };

    class OnType : public _string
    {
        public:
            OnType(void)
            {
                this->v_restriction.push_back("On");
                this->v_restriction.push_back("Off");
                this->v_restriction.push_back("Not_Used");
                this->v_restriction.push_back("Not_Supported");
            }
            ~OnType(void) { this->v_restriction.clear(); }
    };

    class OpenType : public _string
    {
        public:
            OpenType(void)
            {
                this->v_restriction.push_back("Open");
                this->v_restriction.push_back("Close");
            }
            ~OpenType(void) { this->v_restriction.clear(); }
    };

    class OvenStateType : public _string
    {
        public:
            OvenStateType(void)
            {
                this->v_restriction.push_back("Unknown");
                this->v_restriction.push_back("Ready");
                this->v_restriction.push_back("Setting");
                this->v_restriction.push_back("Run");
                this->v_restriction.push_back("Pause");
                this->v_restriction.push_back("Finish");
            }
            ~OvenStateType(void) { this->v_restriction.clear(); }
    };

    class PeriodType : public _string
    {
        public:
            PeriodType(void)
            {
                this->v_restriction.push_back("Hourly");
                this->v_restriction.push_back("Daily");
                this->v_restriction.push_back("Weekly");
                this->v_restriction.push_back("Monthly");
                this->v_restriction.push_back("Yearly");
            }
            ~PeriodType(void) { this->v_restriction.clear(); }
    };

    class PowerUnitType : public _string
    {
        public:
            PowerUnitType(void)
            {
                this->v_restriction.push_back("Wh");
                this->v_restriction.push_back("kWh");
            }
            ~PowerUnitType(void) { this->v_restriction.clear(); }
    };

    class PTZValueType : public _string
    {
        public:
            PTZValueType(void)
            {
                this->v_restriction.push_back("Absolute");
                this->v_restriction.push_back("Relative");
            }
            ~PTZValueType(void) { this->v_restriction.clear(); }
    };

    class ResourceType : public _string
    {
        public:
            ResourceType(void)
            {
                this->v_restriction.push_back("Alarm");
                this->v_restriction.push_back("Audio");
                this->v_restriction.push_back("AVSource");
                this->v_restriction.push_back("Camera");
                this->v_restriction.push_back("Configuration");
                this->v_restriction.push_back("Detection");
                this->v_restriction.push_back("Door");
                this->v_restriction.push_back("EnergyConsumption");
                this->v_restriction.push_back("Fridge");
                this->v_restriction.push_back("Humidity");
                this->v_restriction.push_back("Information");
                this->v_restriction.push_back("Level");
                this->v_restriction.push_back("Light");
                this->v_restriction.push_back("Message");
                this->v_restriction.push_back("Mode");
                this->v_restriction.push_back("Movement");
                this->v_restriction.push_back("Operation");
                this->v_restriction.push_back("Oven");
                this->v_restriction.push_back("Temperature");
                this->v_restriction.push_back("Washer");
                this->v_restriction.push_back("Wind");
            }
            ~ResourceType(void) { this->v_restriction.clear(); }
    };

    class SoundType : public _string
    {
        public:
            SoundType(void)
            {
                this->v_restriction.push_back("Not_Supported");
                this->v_restriction.push_back("Beep");
                this->v_restriction.push_back("Audio_32Bit");
            }
            ~SoundType(void) { this->v_restriction.clear(); }
    };

    class StateType : public _string
    {
        public:
            StateType(void)
            {
                this->v_restriction.push_back("Unknown");
                this->v_restriction.push_back("Ready");
                this->v_restriction.push_back("Run");
                this->v_restriction.push_back("Pause");
            }
            ~StateType(void) { this->v_restriction.clear(); }
    };

    class StorageType : public _string
    {
        public:
            StorageType(void)
            {
                this->v_restriction.push_back("Fridge");
                this->v_restriction.push_back("Freezer");
                this->v_restriction.push_back("Pantry");
            }
            ~StorageType(void) { this->v_restriction.clear(); }
    };

    class String10 : public _string
    {
        public:
            virtual bool CheckRestriction(void *in_data)
            {
                std::string *str_data = reinterpret_cast<std::string *>(in_data);
                if (str_data->size() <= 10) return true;
                else return false;
            }
    };

    class String20 : public _string
    {
        public:
            virtual bool CheckRestriction(void *in_data)
            {
                std::string *str_data = reinterpret_cast<std::string *>(in_data);
                if (str_data->size() <= 20) return true;
                else return false;
            }
    };

    class String30 : public _string
    {
        public:
            virtual bool CheckRestriction(void *in_data)
            {
                std::string *str_data = reinterpret_cast<std::string *>(in_data);
                if (str_data->size() <= 30) return true;
                else return false;
            }
    };

    class String50 : public _string
    {
        public:
            String50(void) {}
            String50(std::string default_value) : _string(default_value) {}
            virtual bool CheckRestriction(void *in_data)
            {
                std::string *str_data = reinterpret_cast<std::string *>(in_data);
                if (str_data->size() <= 50) return true;
                else return false;
            }
    };

    class String100 : public _string
    {
        public:
            virtual bool CheckRestriction(void *in_data)
            {
                std::string *str_data = reinterpret_cast<std::string *>(in_data);
                if (str_data->size() <= 100) return true;
                else return false;
            }
    };

    class String200 : public _string
    {
        public:
            virtual bool CheckRestriction(void *in_data)
            {
                std::string *str_data = reinterpret_cast<std::string *>(in_data);
                if (str_data->size() <= 200) return true;
                else return false;
            }
    };

    class String500 : public _string
    {
        public:
            virtual bool CheckRestriction(void *in_data)
            {
                std::string *str_data = reinterpret_cast<std::string *>(in_data);
                if (str_data->size() <= 500) return true;
                else return false;
            }
    };

    class TemperatureUnitType : public _string
    {
        public:
            TemperatureUnitType(void)
            {
                this->v_restriction.push_back("Fahrenheit");
                this->v_restriction.push_back("Celsius");
            }
            ~TemperatureUnitType(void) { this->v_restriction.clear(); }
    };

    class TimeOffsetType : public _int
    {
    };

    class TimeType : public _time
    {
    };

    class TimeZoneType : public _string
    {
        public:
            TimeZoneType(void)
            {
                this->v_restriction.push_back("Africa/Abidjan");
                this->v_restriction.push_back("Africa/Accra");
                this->v_restriction.push_back("Africa/Addis_Ababa");
                this->v_restriction.push_back("Africa/Algiers");
                this->v_restriction.push_back("Africa/Asmara");
                this->v_restriction.push_back("Africa/Bamako");
                this->v_restriction.push_back("Africa/Bangui");
                this->v_restriction.push_back("Africa/Banjul");
                this->v_restriction.push_back("Africa/Bissau");
                this->v_restriction.push_back("Africa/Blantyre");
                this->v_restriction.push_back("Africa/Brazzaville");
                this->v_restriction.push_back("Africa/Bujumbura");
                this->v_restriction.push_back("Africa/Cairo");
                this->v_restriction.push_back("Africa/Casablanca");
                this->v_restriction.push_back("Africa/Ceuta");
                this->v_restriction.push_back("Africa/Conakry");
                this->v_restriction.push_back("Africa/Dakar");
                this->v_restriction.push_back("Africa/Dar_es_Salaam");
                this->v_restriction.push_back("Africa/Djibouti");
                this->v_restriction.push_back("Africa/Douala");
                this->v_restriction.push_back("Africa/El_Aaiun");
                this->v_restriction.push_back("Africa/Freetown");
                this->v_restriction.push_back("Africa/Gaborone");
                this->v_restriction.push_back("Africa/Harare");
                this->v_restriction.push_back("Africa/Johannesburg");
                this->v_restriction.push_back("Africa/Juba");
                this->v_restriction.push_back("Africa/Kampala");
                this->v_restriction.push_back("Africa/Khartoum");
                this->v_restriction.push_back("Africa/Kigali");
                this->v_restriction.push_back("Africa/Kinshasa");
                this->v_restriction.push_back("Africa/Lagos");
                this->v_restriction.push_back("Africa/Libreville");
                this->v_restriction.push_back("Africa/Lome");
                this->v_restriction.push_back("Africa/Luanda");
                this->v_restriction.push_back("Africa/Lubumbashi");
                this->v_restriction.push_back("Africa/Lusaka");
                this->v_restriction.push_back("Africa/Malabo");
                this->v_restriction.push_back("Africa/Maputo");
                this->v_restriction.push_back("Africa/Maseru");
                this->v_restriction.push_back("Africa/Mbabane");
                this->v_restriction.push_back("Africa/Mogadishu");
                this->v_restriction.push_back("Africa/Monrovia");
                this->v_restriction.push_back("Africa/Nairobi");
                this->v_restriction.push_back("Africa/Ndjamena");
                this->v_restriction.push_back("Africa/Niamey");
                this->v_restriction.push_back("Africa/Nouakchott");
                this->v_restriction.push_back("Africa/Ouagadougou");
                this->v_restriction.push_back("Africa/Sao_Tome");
                this->v_restriction.push_back("Africa/Tripoli");
                this->v_restriction.push_back("Africa/Tunis");
                this->v_restriction.push_back("Africa/Windhoek");
                this->v_restriction.push_back("America/Adak");
                this->v_restriction.push_back("America/Anchorage");
                this->v_restriction.push_back("America/Anguilla");
                this->v_restriction.push_back("America/Antigua");
                this->v_restriction.push_back("America/Araguaina");
                this->v_restriction.push_back("America/Argentina/Buenos_Aires");
                this->v_restriction.push_back("America/Argentina/Catamarca");
                this->v_restriction.push_back("America/Argentina/Cordoba");
                this->v_restriction.push_back("America/Argentina/Jujuy");
                this->v_restriction.push_back("America/Argentina/L_aRioja");
                this->v_restriction.push_back("America/Argentina/Mendoza");
                this->v_restriction.push_back("America/Argentina/Rio_Gallegos");
                this->v_restriction.push_back("America/Argentina/Salta");
                this->v_restriction.push_back("America/Argentina/San_Juan");
                this->v_restriction.push_back("America/Argentina/San_Luis");
                this->v_restriction.push_back("America/Argentina/Tucuman");
                this->v_restriction.push_back("America/Argentina/Ushuaia");
                this->v_restriction.push_back("America/Aruba");
                this->v_restriction.push_back("America/Asuncion");
                this->v_restriction.push_back("America/Atikokan");
                this->v_restriction.push_back("America/Bahi_aBanderas");
                this->v_restriction.push_back("America/Bahia");
                this->v_restriction.push_back("America/Barbados");
                this->v_restriction.push_back("America/Belem");
                this->v_restriction.push_back("America/Belize");
                this->v_restriction.push_back("America/Blanc-Sablon");
                this->v_restriction.push_back("America/Bo_aVista");
                this->v_restriction.push_back("America/Bogota");
                this->v_restriction.push_back("America/Boise");
                this->v_restriction.push_back("America/Cambridge_Bay");
                this->v_restriction.push_back("America/Campo_Grande");
                this->v_restriction.push_back("America/Cancun");
                this->v_restriction.push_back("America/Caracas");
                this->v_restriction.push_back("America/Cayenne");
                this->v_restriction.push_back("America/Cayman");
                this->v_restriction.push_back("America/Chicago");
                this->v_restriction.push_back("America/Chihuahua");
                this->v_restriction.push_back("America/Cost_aRica");
                this->v_restriction.push_back("America/Creston");
                this->v_restriction.push_back("America/Cuiaba");
                this->v_restriction.push_back("America/Curacao");
                this->v_restriction.push_back("America/Danmarkshavn");
                this->v_restriction.push_back("America/Dawson");
                this->v_restriction.push_back("America/Dawson_Creek");
                this->v_restriction.push_back("America/Denver");
                this->v_restriction.push_back("America/Detroit");
                this->v_restriction.push_back("America/Dominica");
                this->v_restriction.push_back("America/Edmonton");
                this->v_restriction.push_back("America/Eirunepe");
                this->v_restriction.push_back("America/El_Salvador");
                this->v_restriction.push_back("America/Fortaleza");
                this->v_restriction.push_back("America/Glace_Bay");
                this->v_restriction.push_back("America/Godthab");
                this->v_restriction.push_back("America/Goose_Bay");
                this->v_restriction.push_back("America/Grand_Turk");
                this->v_restriction.push_back("America/Grenada");
                this->v_restriction.push_back("America/Guadeloupe");
                this->v_restriction.push_back("America/Guatemala");
                this->v_restriction.push_back("America/Guayaquil");
                this->v_restriction.push_back("America/Guyana");
                this->v_restriction.push_back("America/Halifax");
                this->v_restriction.push_back("America/Havana");
                this->v_restriction.push_back("America/Hermosillo");
                this->v_restriction.push_back("America/Indiana/Indianapolis");
                this->v_restriction.push_back("America/Indiana/Knox");
                this->v_restriction.push_back("America/Indiana/Marengo");
                this->v_restriction.push_back("America/Indiana/Petersburg");
                this->v_restriction.push_back("America/Indiana/Tell_City");
                this->v_restriction.push_back("America/Indiana/Vevay");
                this->v_restriction.push_back("America/Indiana/Vincennes");
                this->v_restriction.push_back("America/Indiana/Winamac");
                this->v_restriction.push_back("America/Inuvik");
                this->v_restriction.push_back("America/Iqaluit");
                this->v_restriction.push_back("America/Jamaica");
                this->v_restriction.push_back("America/Juneau");
                this->v_restriction.push_back("America/Kentucky/Louisville");
                this->v_restriction.push_back("America/Kentucky/Monticello");
                this->v_restriction.push_back("America/Kralendijk");
                this->v_restriction.push_back("America/L_aPaz");
                this->v_restriction.push_back("America/Lima");
                this->v_restriction.push_back("America/Los_Angeles");
                this->v_restriction.push_back("America/Lower_Princes");
                this->v_restriction.push_back("America/Maceio");
                this->v_restriction.push_back("America/Managua");
                this->v_restriction.push_back("America/Manaus");
                this->v_restriction.push_back("America/Marigot");
                this->v_restriction.push_back("America/Martinique");
                this->v_restriction.push_back("America/Matamoros");
                this->v_restriction.push_back("America/Mazatlan");
                this->v_restriction.push_back("America/Menominee");
                this->v_restriction.push_back("America/Merida");
                this->v_restriction.push_back("America/Metlakatla");
                this->v_restriction.push_back("America/Mexico_City");
                this->v_restriction.push_back("America/Miquelon");
                this->v_restriction.push_back("America/Moncton");
                this->v_restriction.push_back("America/Monterrey");
                this->v_restriction.push_back("America/Montevideo");
                this->v_restriction.push_back("America/Montserrat");
                this->v_restriction.push_back("America/Nassau");
                this->v_restriction.push_back("America/New_York");
                this->v_restriction.push_back("America/Nipigon");
                this->v_restriction.push_back("America/Nome");
                this->v_restriction.push_back("America/Noronha");
                this->v_restriction.push_back("America/North_Dakota/Beulah");
                this->v_restriction.push_back("America/North_Dakota/Center");
                this->v_restriction.push_back("America/North_Dakota/New_Salem");
                this->v_restriction.push_back("America/Ojinaga");
                this->v_restriction.push_back("America/Panama");
                this->v_restriction.push_back("America/Pangnirtung");
                this->v_restriction.push_back("America/Paramaribo");
                this->v_restriction.push_back("America/Phoenix");
                this->v_restriction.push_back("America/Port-au-Prince");
                this->v_restriction.push_back("America/Port_of_Spain");
                this->v_restriction.push_back("America/Porto_Velho");
                this->v_restriction.push_back("America/Puerto_Rico");
                this->v_restriction.push_back("America/Rainy_River");
                this->v_restriction.push_back("America/Rankin_Inlet");
                this->v_restriction.push_back("America/Recife");
                this->v_restriction.push_back("America/Regina");
                this->v_restriction.push_back("America/Resolute");
                this->v_restriction.push_back("America/Rio_Branco");
                this->v_restriction.push_back("America/Sant_aIsabel");
                this->v_restriction.push_back("America/Santarem");
                this->v_restriction.push_back("America/Santiago");
                this->v_restriction.push_back("America/Santo_Domingo");
                this->v_restriction.push_back("America/Sao_Paulo");
                this->v_restriction.push_back("America/Scoresbysund");
                this->v_restriction.push_back("America/Sitka");
                this->v_restriction.push_back("America/St_Barthelemy");
                this->v_restriction.push_back("America/St_Johns");
                this->v_restriction.push_back("America/St_Kitts");
                this->v_restriction.push_back("America/St_Lucia");
                this->v_restriction.push_back("America/St_Thomas");
                this->v_restriction.push_back("America/St_Vincent");
                this->v_restriction.push_back("America/Swift_Current");
                this->v_restriction.push_back("America/Tegucigalpa");
                this->v_restriction.push_back("America/Thule");
                this->v_restriction.push_back("America/Thunder_Bay");
                this->v_restriction.push_back("America/Tijuana");
                this->v_restriction.push_back("America/Toronto");
                this->v_restriction.push_back("America/Tortola");
                this->v_restriction.push_back("America/Vancouver");
                this->v_restriction.push_back("America/Whitehorse");
                this->v_restriction.push_back("America/Winnipeg");
                this->v_restriction.push_back("America/Yakutat");
                this->v_restriction.push_back("America/Yellowknife");
                this->v_restriction.push_back("Antarctica/Casey");
                this->v_restriction.push_back("Antarctica/Davis");
                this->v_restriction.push_back("Antarctica/DumontDUrville");
                this->v_restriction.push_back("Antarctica/Macquarie");
                this->v_restriction.push_back("Antarctica/Mawson");
                this->v_restriction.push_back("Antarctica/McMurdo");
                this->v_restriction.push_back("Antarctica/Palmer");
                this->v_restriction.push_back("Antarctica/Rothera");
                this->v_restriction.push_back("Antarctica/Syowa");
                this->v_restriction.push_back("Antarctica/Troll");
                this->v_restriction.push_back("Antarctica/Vostok");
                this->v_restriction.push_back("Arctic/Longyearbyen");
                this->v_restriction.push_back("Asia/Aden");
                this->v_restriction.push_back("Asia/Almaty");
                this->v_restriction.push_back("Asia/Amman");
                this->v_restriction.push_back("Asia/Anadyr");
                this->v_restriction.push_back("Asia/Aqtau");
                this->v_restriction.push_back("Asia/Aqtobe");
                this->v_restriction.push_back("Asia/Ashgabat");
                this->v_restriction.push_back("Asia/Baghdad");
                this->v_restriction.push_back("Asia/Bahrain");
                this->v_restriction.push_back("Asia/Baku");
                this->v_restriction.push_back("Asia/Bangkok");
                this->v_restriction.push_back("Asia/Beirut");
                this->v_restriction.push_back("Asia/Bishkek");
                this->v_restriction.push_back("Asia/Brunei");
                this->v_restriction.push_back("Asia/Choibalsan");
                this->v_restriction.push_back("Asia/Chongqing");
                this->v_restriction.push_back("Asia/Colombo");
                this->v_restriction.push_back("Asia/Damascus");
                this->v_restriction.push_back("Asia/Dhaka");
                this->v_restriction.push_back("Asia/Dili");
                this->v_restriction.push_back("Asia/Dubai");
                this->v_restriction.push_back("Asia/Dushanbe");
                this->v_restriction.push_back("Asia/Gaza");
                this->v_restriction.push_back("Asia/Harbin");
                this->v_restriction.push_back("Asia/Hebron");
                this->v_restriction.push_back("Asia/Ho_Chi_Minh");
                this->v_restriction.push_back("Asia/Hong_Kong");
                this->v_restriction.push_back("Asia/Hovd");
                this->v_restriction.push_back("Asia/Irkutsk");
                this->v_restriction.push_back("Asia/Jakarta");
                this->v_restriction.push_back("Asia/Jayapura");
                this->v_restriction.push_back("Asia/Jerusalem");
                this->v_restriction.push_back("Asia/Kabul");
                this->v_restriction.push_back("Asia/Kamchatka");
                this->v_restriction.push_back("Asia/Karachi");
                this->v_restriction.push_back("Asia/Kashgar");
                this->v_restriction.push_back("Asia/Kathmandu");
                this->v_restriction.push_back("Asia/Khandyga");
                this->v_restriction.push_back("Asia/Kolkata");
                this->v_restriction.push_back("Asia/Krasnoyarsk");
                this->v_restriction.push_back("Asia/Kual_aLumpur");
                this->v_restriction.push_back("Asia/Kuching");
                this->v_restriction.push_back("Asia/Kuwait");
                this->v_restriction.push_back("Asia/Macau");
                this->v_restriction.push_back("Asia/Magadan");
                this->v_restriction.push_back("Asia/Makassar");
                this->v_restriction.push_back("Asia/Manila");
                this->v_restriction.push_back("Asia/Muscat");
                this->v_restriction.push_back("Asia/Nicosia");
                this->v_restriction.push_back("Asia/Novokuznetsk");
                this->v_restriction.push_back("Asia/Novosibirsk");
                this->v_restriction.push_back("Asia/Omsk");
                this->v_restriction.push_back("Asia/Oral");
                this->v_restriction.push_back("Asia/Phnom_Penh");
                this->v_restriction.push_back("Asia/Pontianak");
                this->v_restriction.push_back("Asia/Pyongyang");
                this->v_restriction.push_back("Asia/Qatar");
                this->v_restriction.push_back("Asia/Qyzylorda");
                this->v_restriction.push_back("Asia/Rangoon");
                this->v_restriction.push_back("Asia/Riyadh");
                this->v_restriction.push_back("Asia/Sakhalin");
                this->v_restriction.push_back("Asia/Samarkand");
                this->v_restriction.push_back("Asia/Seoul");
                this->v_restriction.push_back("Asia/Shanghai");
                this->v_restriction.push_back("Asia/Singapore");
                this->v_restriction.push_back("Asia/Taipei");
                this->v_restriction.push_back("Asia/Tashkent");
                this->v_restriction.push_back("Asia/Tbilisi");
                this->v_restriction.push_back("Asia/Tehran");
                this->v_restriction.push_back("Asia/Thimphu");
                this->v_restriction.push_back("Asia/Tokyo");
                this->v_restriction.push_back("Asia/Ulaanbaatar");
                this->v_restriction.push_back("Asia/Urumqi");
                this->v_restriction.push_back("Asia/Ust-Nera");
                this->v_restriction.push_back("Asia/Vientiane");
                this->v_restriction.push_back("Asia/Vladivostok");
                this->v_restriction.push_back("Asia/Yakutsk");
                this->v_restriction.push_back("Asia/Yekaterinburg");
                this->v_restriction.push_back("Asia/Yerevan");
                this->v_restriction.push_back("Atlantic/Azores");
                this->v_restriction.push_back("Atlantic/Bermuda");
                this->v_restriction.push_back("Atlantic/Canary");
                this->v_restriction.push_back("Atlantic/Cape_Verde");
                this->v_restriction.push_back("Atlantic/Faroe");
                this->v_restriction.push_back("Atlantic/Madeira");
                this->v_restriction.push_back("Atlantic/Reykjavik");
                this->v_restriction.push_back("Atlantic/South_Georgia");
                this->v_restriction.push_back("Atlantic/St_Helena");
                this->v_restriction.push_back("Atlantic/Stanley");
                this->v_restriction.push_back("Australia/Adelaide");
                this->v_restriction.push_back("Australia/Brisbane");
                this->v_restriction.push_back("Australia/Broken_Hill");
                this->v_restriction.push_back("Australia/Currie");
                this->v_restriction.push_back("Australia/Darwin");
                this->v_restriction.push_back("Australia/Eucla");
                this->v_restriction.push_back("Australia/Hobart");
                this->v_restriction.push_back("Australia/Lindeman");
                this->v_restriction.push_back("Australia/Lord_Howe");
                this->v_restriction.push_back("Australia/Melbourne");
                this->v_restriction.push_back("Australia/Perth");
                this->v_restriction.push_back("Australia/Sydney");
                this->v_restriction.push_back("Europe/Amsterdam");
                this->v_restriction.push_back("Europe/Andorra");
                this->v_restriction.push_back("Europe/Athens");
                this->v_restriction.push_back("Europe/Belgrade");
                this->v_restriction.push_back("Europe/Berlin");
                this->v_restriction.push_back("Europe/Bratislava");
                this->v_restriction.push_back("Europe/Brussels");
                this->v_restriction.push_back("Europe/Bucharest");
                this->v_restriction.push_back("Europe/Budapest");
                this->v_restriction.push_back("Europe/Busingen");
                this->v_restriction.push_back("Europe/Chisinau");
                this->v_restriction.push_back("Europe/Copenhagen");
                this->v_restriction.push_back("Europe/Dublin");
                this->v_restriction.push_back("Europe/Gibraltar");
                this->v_restriction.push_back("Europe/Guernsey");
                this->v_restriction.push_back("Europe/Helsinki");
                this->v_restriction.push_back("Europe/Isle_of_Man");
                this->v_restriction.push_back("Europe/Istanbul");
                this->v_restriction.push_back("Europe/Jersey");
                this->v_restriction.push_back("Europe/Kaliningrad");
                this->v_restriction.push_back("Europe/Kiev");
                this->v_restriction.push_back("Europe/Lisbon");
                this->v_restriction.push_back("Europe/Ljubljana");
                this->v_restriction.push_back("Europe/London");
                this->v_restriction.push_back("Europe/Luxembourg");
                this->v_restriction.push_back("Europe/Madrid");
                this->v_restriction.push_back("Europe/Malta");
                this->v_restriction.push_back("Europe/Mariehamn");
                this->v_restriction.push_back("Europe/Minsk");
                this->v_restriction.push_back("Europe/Monaco");
                this->v_restriction.push_back("Europe/Moscow");
                this->v_restriction.push_back("Europe/Oslo");
                this->v_restriction.push_back("Europe/Paris");
                this->v_restriction.push_back("Europe/Podgorica");
                this->v_restriction.push_back("Europe/Prague");
                this->v_restriction.push_back("Europe/Riga");
                this->v_restriction.push_back("Europe/Rome");
                this->v_restriction.push_back("Europe/Samara");
                this->v_restriction.push_back("Europe/San_Marino");
                this->v_restriction.push_back("Europe/Sarajevo");
                this->v_restriction.push_back("Europe/Simferopol");
                this->v_restriction.push_back("Europe/Skopje");
                this->v_restriction.push_back("Europe/Sofia");
                this->v_restriction.push_back("Europe/Stockholm");
                this->v_restriction.push_back("Europe/Tallinn");
                this->v_restriction.push_back("Europe/Tirane");
                this->v_restriction.push_back("Europe/Uzhgorod");
                this->v_restriction.push_back("Europe/Vaduz");
                this->v_restriction.push_back("Europe/Vatican");
                this->v_restriction.push_back("Europe/Vienna");
                this->v_restriction.push_back("Europe/Vilnius");
                this->v_restriction.push_back("Europe/Volgograd");
                this->v_restriction.push_back("Europe/Warsaw");
                this->v_restriction.push_back("Europe/Zagreb");
                this->v_restriction.push_back("Europe/Zaporozhye");
                this->v_restriction.push_back("Europe/Zurich");
                this->v_restriction.push_back("Indian/Antananarivo");
                this->v_restriction.push_back("Indian/Chagos");
                this->v_restriction.push_back("Indian/Christmas");
                this->v_restriction.push_back("Indian/Cocos");
                this->v_restriction.push_back("Indian/Comoro");
                this->v_restriction.push_back("Indian/Kerguelen");
                this->v_restriction.push_back("Indian/Mahe");
                this->v_restriction.push_back("Indian/Maldives");
                this->v_restriction.push_back("Indian/Mauritius");
                this->v_restriction.push_back("Indian/Mayotte");
                this->v_restriction.push_back("Indian/Reunion");
                this->v_restriction.push_back("Pacific/Apia");
                this->v_restriction.push_back("Pacific/Auckland");
                this->v_restriction.push_back("Pacific/Chatham");
                this->v_restriction.push_back("Pacific/Chuuk");
                this->v_restriction.push_back("Pacific/Easter");
                this->v_restriction.push_back("Pacific/Efate");
                this->v_restriction.push_back("Pacific/Enderbury");
                this->v_restriction.push_back("Pacific/Fakaofo");
                this->v_restriction.push_back("Pacific/Fiji");
                this->v_restriction.push_back("Pacific/Funafuti");
                this->v_restriction.push_back("Pacific/Galapagos");
                this->v_restriction.push_back("Pacific/Gambier");
                this->v_restriction.push_back("Pacific/Guadalcanal");
                this->v_restriction.push_back("Pacific/Guam");
                this->v_restriction.push_back("Pacific/Honolulu");
                this->v_restriction.push_back("Pacific/Johnston");
                this->v_restriction.push_back("Pacific/Kiritimati");
                this->v_restriction.push_back("Pacific/Kosrae");
                this->v_restriction.push_back("Pacific/Kwajalein");
                this->v_restriction.push_back("Pacific/Majuro");
                this->v_restriction.push_back("Pacific/Marquesas");
                this->v_restriction.push_back("Pacific/Midway");
                this->v_restriction.push_back("Pacific/Nauru");
                this->v_restriction.push_back("Pacific/Niue");
                this->v_restriction.push_back("Pacific/Norfolk");
                this->v_restriction.push_back("Pacific/Noumea");
                this->v_restriction.push_back("Pacific/Pago_Pago");
                this->v_restriction.push_back("Pacific/Palau");
                this->v_restriction.push_back("Pacific/Pitcairn");
                this->v_restriction.push_back("Pacific/Pohnpei");
                this->v_restriction.push_back("Pacific/Port_Moresby");
                this->v_restriction.push_back("Pacific/Rarotonga");
                this->v_restriction.push_back("Pacific/Saipan");
                this->v_restriction.push_back("Pacific/Tahiti");
                this->v_restriction.push_back("Pacific/Tarawa");
                this->v_restriction.push_back("Pacific/Tongatapu");
                this->v_restriction.push_back("Pacific/Wake");
                this->v_restriction.push_back("Pacific/Wallis");
            }
            ~TimeZoneType(void) { this->v_restriction.clear(); }
    };

    class TransportProtocolType : public _string
    {
        public:
            TransportProtocolType(void)
            {
                this->v_restriction.push_back("RTSP");
                this->v_restriction.push_back("HTTP");
            }
            ~TransportProtocolType(void) { this->v_restriction.clear(); }
    };

    class UInt8 : public _unsignedByte
    {
    };

    class UInt16 : public _unsignedShort
    {
    };

    class UInt32 : public _unsignedInt
    {
    };

    class UInt64 : public _unsignedLong
    {
    };

    class UuidType : public _string
    {
        public:
            virtual bool CheckRestriction(void *in_data)
            {
                std::string *str_data = reinterpret_cast<std::string *>(in_data);
                if (str_data->size() <= 36) return true;
                else return false;
            }
    };

    class VersionType : public _string
    {
        public:
            VersionType(void)
            {
                this->v_restriction.push_back("Hardware");
                this->v_restriction.push_back("Software");
                this->v_restriction.push_back("Firmware");
                this->v_restriction.push_back("Protocol");
                this->v_restriction.push_back("Manufacturer_Specific");
            }
            ~VersionType(void) { this->v_restriction.clear(); }
    };

    class VideoCodecType : public _string
    {
        public:
            VideoCodecType(void)
            {
                this->v_restriction.push_back("MJPEG");
                this->v_restriction.push_back("MPEG2");
                this->v_restriction.push_back("MPEG4");
                this->v_restriction.push_back("H264");
            }
            ~VideoCodecType(void) { this->v_restriction.clear(); }
    };

    class WindDirectionType : public _string
    {
        public:
            WindDirectionType(void)
            {
                this->v_restriction.push_back("Unknown");
                this->v_restriction.push_back("Auto");
                this->v_restriction.push_back("Off");
                this->v_restriction.push_back("Fix");
                this->v_restriction.push_back("All");
                this->v_restriction.push_back("Up_And_Low");
                this->v_restriction.push_back("Left_And_Right");
                this->v_restriction.push_back("Wide");
                this->v_restriction.push_back("Center");
                this->v_restriction.push_back("Left");
                this->v_restriction.push_back("Right");
                this->v_restriction.push_back("Direct");
                this->v_restriction.push_back("Indirect");
                this->v_restriction.push_back("Long");
            }
            ~WindDirectionType(void) { this->v_restriction.clear(); }
    };


    class WifiEncryptionType : public _string
    {
        public:
            WifiEncryptionType(void)
            {
                this->v_restriction.push_back("WEP-64");
                this->v_restriction.push_back("WEP-128");
                this->v_restriction.push_back("TKIP");
                this->v_restriction.push_back("AES");
                this->v_restriction.push_back("TKIP_AES");
            }
            ~WifiEncryptionType(void) { this->v_restriction.clear(); }
    };

    class WifiEventType : public _string
    {
        public:
            WifiEventType(void)
            {
                this->v_restriction.push_back("Started");
                this->v_restriction.push_back("Connected");
                this->v_restriction.push_back("Disconnected");
            }
            ~WifiEventType(void) { this->v_restriction.clear(); }
    };

    class WifiOperationType : public _string
    {
        public:
            WifiOperationType(void)
            {
                this->v_restriction.push_back("Infrastructure");
                this->v_restriction.push_back("Soft_AP");
                this->v_restriction.push_back("WiFi_Direct");
            }
            ~WifiOperationType(void) { this->v_restriction.clear(); }
    };

    class WifiSecurityType : public _string
    {
        public:
            WifiSecurityType(void)
            {
                this->v_restriction.push_back("None");
                this->v_restriction.push_back("WEP");
                this->v_restriction.push_back("WPA-PSK");
                this->v_restriction.push_back("WPA2-PSK");
                this->v_restriction.push_back("WPA_WPA2-PSK");
            }
            ~WifiSecurityType(void) { this->v_restriction.clear(); }
    };

    class ChannelType : public _string
    {
        public:
            ChannelType(void)
            {
                this->v_restriction.push_back("RTP");
                this->v_restriction.push_back("RTSP");
                this->v_restriction.push_back("RTCP");
            }
            ~ChannelType(void) { this->v_restriction.clear(); }
    };

    class ModesExecuteMothodType : public _string
    {
        public:
            ModesExecuteMothodType(void)
            {
                this->v_restriction.push_back("sequence");
                this->v_restriction.push_back("parallel");
            }
            ~ModesExecuteMothodType(void) { this->v_restriction.clear(); }
    };

    class ExecuteType : public _string
    {
        public:
            ExecuteType(void)
            {
                this->v_restriction.push_back("stop");
                this->v_restriction.push_back("pause");
                this->v_restriction.push_back("start");
            }
            ~ExecuteType(void) { this->v_restriction.clear(); }
    };

    class RepetitionType : public _string
    {
        public:
            RepetitionType(void)
            {
                this->v_restriction.push_back("OneTime");
                this->v_restriction.push_back("Daily");
                this->v_restriction.push_back("Weekly");
                this->v_restriction.push_back("Monthly");
                this->v_restriction.push_back("Yearly");
            }
            ~RepetitionType(void) { this->v_restriction.clear(); }
    };

    class WeeksType : public _string
    {
        public:
            WeeksType(void)
            {
                this->v_restriction.push_back("Mon");
                this->v_restriction.push_back("Tue");
                this->v_restriction.push_back("Wed");
                this->v_restriction.push_back("Thu");
                this->v_restriction.push_back("Fri");
                this->v_restriction.push_back("Sat");
                this->v_restriction.push_back("Sun");
            }
            ~WeeksType(void) { this->v_restriction.clear(); }
    };

    class ZoneDeviceType : public _string
    {
        public:
            ZoneDeviceType(void)
            {
                this->v_restriction.push_back("Unknown");
                this->v_restriction.push_back("Gateway");
                this->v_restriction.push_back("Smartphone");
                this->v_restriction.push_back("Tablet");
                this->v_restriction.push_back("TV");
                this->v_restriction.push_back("AV");
                this->v_restriction.push_back("Refrigerator");
                this->v_restriction.push_back("Kimchi_Refrigerator");
                this->v_restriction.push_back("Washer");
                this->v_restriction.push_back("Dryer");
                this->v_restriction.push_back("Air_Conditioner");
                this->v_restriction.push_back("Dishwasher");
                this->v_restriction.push_back("Oven");
                this->v_restriction.push_back("Microwave_Oven");
                this->v_restriction.push_back("Cooktop");
                this->v_restriction.push_back("Robot_Cleaner");
                this->v_restriction.push_back("Vacuum_Cleaner");
                this->v_restriction.push_back("Light");
                this->v_restriction.push_back("IP_Camera");
                this->v_restriction.push_back("Door_Lock");
                this->v_restriction.push_back("Door_Phone");
                this->v_restriction.push_back("Thermostat");
                this->v_restriction.push_back("Water_Heater");
                this->v_restriction.push_back("Smart_Plug");
                this->v_restriction.push_back("Generic_Sensor");
                this->v_restriction.push_back("Motion_Detector");
                this->v_restriction.push_back("Bridge");
                this->v_restriction.push_back("Zigbee_Bridge");
                this->v_restriction.push_back("Zwave_Bridge");
                this->v_restriction.push_back("Coffee_Pot");
                this->v_restriction.push_back("Iron");
                this->v_restriction.push_back("Heater");
                this->v_restriction.push_back("Ventilator");
                this->v_restriction.push_back("Fan");
                this->v_restriction.push_back("Blind");
                this->v_restriction.push_back("Switch");
                this->v_restriction.push_back("Motor");
                this->v_restriction.push_back("Shutter");
            }
            ~ZoneDeviceType(void) { this->v_restriction.clear(); }
    };

    class ModeStateType : public _string
    {
        public:
            ModeStateType(void)
            {
                this->v_restriction.push_back("enable");
                this->v_restriction.push_back("disable");
                this->v_restriction.push_back("schedule");
            }
            ~ModeStateType(void) { this->v_restriction.clear(); }
    };

    class AuthType : public _string
    {
        public:
            AuthType(void)
            {
                this->v_restriction.push_back("Password");
                this->v_restriction.push_back("Card");
                this->v_restriction.push_back("Fingerprint");
                this->v_restriction.push_back("NFC");
                this->v_restriction.push_back("USB");
                this->v_restriction.push_back("Button");
            }
            ~AuthType(void) { this->v_restriction.clear(); }
    };

    class AuthState : public _string
    {
        public:
            AuthState(void)
            {
                this->v_restriction.push_back("ready");
                this->v_restriction.push_back("connected");
                this->v_restriction.push_back("authentication_ready");
            }
            ~AuthState(void) { this->v_restriction.clear(); }
    };


}

#endif /* WEBSERVICE_SIMPLE_DATA_TYPE_H_ */

