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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GeneralData.h"

//#define __INTER_DEBUG__
// Hidden Class define.
template <typename TYPE>
class AllFormat_data : public Virtual_data
{
        typedef Virtual_data base;
    private :
        TYPE data;

    public :
        AllFormat_data(int typenum) : base(typenum)
        {
#ifdef __INTER_DEBUG__
            printf("[DEBUG] Test_data Constructor().\n");
#endif
        }

        virtual void put_data( void *value)
        {
            data = *(TYPE *)value;
        }

        virtual void *get_data( void )
        {
            return (void *)&data;
        }
};



/*******************************************
 * Virtual_data class member function define.
 */
void Virtual_data::s2data( const char *value)
{
    switch (dataType)
    {
        case TYPE_CHAR :
            {
                char data = atoi(value);
                put_data( (void *)&data );
            }
        case TYPE_INT :
        case TYPE_SHORT :
            {
                int data = atoi(value);
                put_data( (void *)&data );
            }
            break;
        case TYPE_FLOAT :
            {
                float data = atof(value);
                put_data( (void *)&data );
            }
            break;
        case TYPE_DOUBLE :
            {
                double data = atof(value);
                put_data( (void *)&data );
            }
            break;
        case TYPE_NODEFINE :
            printf("Error: dataType must have defined.\n");
            break;
        default :
            printf("Error: Not yet supported type.\n");
            break;
    }
}

Virtual_data::Virtual_data( void )
{
    dataType = TYPE_NODEFINE;
}

Virtual_data::Virtual_data(int type)
{
#ifdef __INTER_DEBUG__
    printf("[DEBUG] Virtual_data Constructor().\n");
#endif
    dataType = type;
}

void Virtual_data::put(std::string value)
{
    if (dataType == TYPE_STRING )
        put_data( (void *)&value );
    else
        s2data( value.c_str() );
}

void Virtual_data::put(char *value)
{
    if (dataType == TYPE_STRING )
    {
        std::string temp = std::string(value);
        put_data( (void *)&temp );
    }
    else
        s2data( value );
}

int Virtual_data::get( std::string *value)
{
    if (dataType != TYPE_STRING)
    {
        printf("Error : dataType is Not string.\n");
        return 0;
    }

    void *data = get_data();
    *value = *(std::string *)data;
    return 1;
}

void Virtual_data::put_data( void *value )
{
    printf("Error: virtual function(put_data) called.\n");
}

void *Virtual_data::get_data( void )
{
    printf("Error: virtual function(get) called.\n");
    return NULL;
}






/*******************************************
 * GeneralData class member function define.
 */
void GeneralData::set_dataType(const char *type)
{
    if ( strstr(type, "string") != NULL )
    {
        dataType = TYPE_STRING;
#ifdef __INTER_DEBUG__
        printf("DataType is std::string.\n");
#endif
        return ;
    }
    if ( strstr(type, "char") != NULL )
    {
        dataType = TYPE_CHAR;
#ifdef __INTER_DEBUG__
        printf("DataType is Character.\n");
#endif
        return ;
    }
    if ( strstr(type, "int") != NULL )
    {
        dataType = TYPE_INT;
#ifdef __INTER_DEBUG__
        printf("DataType is Integer.\n");
#endif
        return ;
    }
    if ( strstr(type, "short") != NULL )
    {
        dataType = TYPE_SHORT;
#ifdef __INTER_DEBUG__
        printf("DataType is Short.\n");
#endif
        return ;
    }
    if ( strstr(type, "float") != NULL )
    {
        dataType = TYPE_FLOAT;
#ifdef __INTER_DEBUG__
        printf("DataType is Floating.\n");
#endif
        return ;
    }
    if ( strstr(type, "double") != NULL )
    {
        dataType = TYPE_DOUBLE;
#ifdef __INTER_DEBUG__
        printf("DataType is Double.\n");
#endif
        return ;
    }

    dataType = TYPE_NODEFINE;
}

GeneralData::GeneralData( void )
{
#ifdef __INTER_DEBUG__
    printf("[DEBUG] Virtual_Test Constructor().\n");
#endif
    flag = false;
    Name = "";
    dataType = TYPE_NODEFINE;
    pValue = 0;
}

GeneralData::GeneralData( std::string name, std::string type )
{
#ifdef __INTER_DEBUG__
    printf("[DEBUG] Data_Normal Constructor().\n");
#endif
    flag = false;
    Name = "";
    dataType = TYPE_NODEFINE;
    pValue = 0;

    flag = initial( name, type );
}

GeneralData::GeneralData( std::string name, std::string type, std::string value )
{
#ifdef __INTER_DEBUG__
    printf("[DEBUG] Data_Normal Constructor().\n");
#endif
    flag = false;
    Name = "";
    dataType = TYPE_NODEFINE;
    pValue = 0;

    flag = initial( name, type );
    put(value);
}

bool GeneralData::initial( std::string name, std::string type )
{
    Name = name;
    set_dataType( type.c_str() );

    switch (dataType)
    {
        case TYPE_STRING :
            pValue = new AllFormat_data<std::string>(dataType);
            break;
        case TYPE_CHAR :
            pValue = new AllFormat_data<char>(dataType);
            break;
        case TYPE_INT :
            pValue = new AllFormat_data<int>(dataType);
            break;
        case TYPE_SHORT :
            pValue = new AllFormat_data<short>(dataType);
            break;
        case TYPE_FLOAT :
            pValue = new AllFormat_data<float>(dataType);
            break;
        case TYPE_DOUBLE :
            pValue = new AllFormat_data<double>(dataType);
            break;
        case TYPE_NODEFINE :
            pValue = 0;
            printf("Error : set_dataType()function is returned TYPE_NODEFINE.\n");
            break;
    }

    return true;
}

bool GeneralData::get_Name( std::string &name )
{
    BOOLINIT_CHECK();

    name = Name;
    return true;
}

void GeneralData::put( std::string value )
{
    VOIDINIT_CHECK();
    pValue->put(value);
}

void GeneralData::put( const char *value )
{
    VOIDINIT_CHECK();
    pValue->put(value);
}

int GeneralData::get_DataType( void )
{
    BOOLINIT_CHECK();

    return dataType;
}

bool GeneralData::get( std::string *data )
{
    BOOLINIT_CHECK();

    if ( pValue->get(data) == NULL )
    {
        printf("Error : No data.\n");
        *data = "";
        return false;
    }

    return true;
}



void Conversion_DataFormat( std::vector < std::map< std::string, std::string > > lVector ,
                            std::vector< GeneralData > &gVector )
{
#ifdef __INTERNAL_DEBUG__
    std::cout << "[DEBUG] ITS::" << __func__ << " is called." << std::endl;
#endif

    std::string name;
    std::string type;
    std::string value;

    for (unsigned int j = 0; j < lVector.size(); j++)
    {
        name = lVector[j]["name"];
        type = lVector[j]["type"];
        value = lVector[j]["value"];

        GeneralData pValue(name, type, value);
        gVector.push_back(pValue);
    }

    /*************************************************
     * Debugging print ( GeneralData format confirm. )
     */
#ifdef __INTERNAL_DEBUG__
    for (unsigned int j = 0; j < gVector.size(); j++)
    {
        if ( gVector[j].get_Name(name) == false )
        {
            printf("Error : Not initialed.\n");
            return ;
        }

        int dataType = gVector[j].get_DataType();
        switch (dataType)
        {
            case TYPE_STRING :
                {
                    std::string data;
                    if ( gVector[j].get(&data) == false )
                    {
                        printf("Error : Not initialed.\n");
                        return ;
                    }
                    printf("name=%s , type=%d, value=%s\n", name.c_str(), dataType, data.c_str() );
                }
                break;
            case TYPE_CHAR :
            case TYPE_SHORT :
            case TYPE_INT :
                {
                    int data;
                    if ( gVector[j].get(&data) == false )
                    {
                        printf("Error : Not initialed.\n");
                        return ;
                    }
                    printf("name=%s , type=%d, value=%d\n", name.c_str(), dataType, data );
                }
                break;
            case TYPE_FLOAT :
            case TYPE_DOUBLE :
                {
                    float data;
                    if ( gVector[j].get(&data) == false )
                    {
                        printf("Error : Not initialed.\n");
                        return ;
                    }
                    printf("name=%s , type=%d, value=%f\n", name.c_str(), dataType, data );
                }
                break;
        }
    }
#endif

    printf("Conversion_DataFormat() is Successful.\n");
}
