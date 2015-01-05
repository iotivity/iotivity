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

#ifndef GENERALDATA_H_
#define GENERALDATA_H_

#include <iostream>
#include <map>
#include <vector>

#define TYPE_NODEFINE   0
#define TYPE_STRING     1
#define TYPE_CHAR           2
#define TYPE_INT            3
#define TYPE_SHORT      4
#define TYPE_FLOAT      5
#define TYPE_DOUBLE     6

// Macro Function.
#define VOIDINIT_CHECK()                                            \
    if ( flag == false ) {                                          \
        printf("Error : Not yet set data.\n");      \
        return ;                                                                    \
    }

#define BOOLINIT_CHECK()                                            \
    if ( flag == false ) {                                          \
        printf("Error : Not yet set data.\n");      \
        return false;                                                           \
    }


class Virtual_data
{
    private :
        int dataType;

        void s2data( const char *value);

    public :

        Virtual_data( void );

        Virtual_data(int type);

        void put(std::string value);

        void put(char *value);

        int get( std::string *value);

        template <typename TYPE>
        int get( TYPE *value)
        {
            void *data = get_data();

            switch (dataType)
            {
                case TYPE_CHAR :
                case TYPE_INT :
                case TYPE_SHORT :
                    *value = (TYPE)(*(int *)data);
                    return 1;
                    break;
                case TYPE_FLOAT :
                    *value = (TYPE)(*(float *)data);
                    return 1;
                case TYPE_DOUBLE :
                    *value = (TYPE)(*(double *)data);
                    return 1;
                    break;
                case TYPE_NODEFINE :
                    printf("Error : Not yet defined with dataType variable.\n");
            }

            return 0;
        }

        // Virtual Function.
        virtual void put_data( void *value ) = 0;

        virtual void *get_data( void ) = 0;
};



class GeneralData
{
    private :
        bool flag;
        std::string Name;
        int dataType;
        class Virtual_data *pValue;

        void set_dataType(const char *type);

    public :
        GeneralData( void );

        GeneralData( std::string name, std::string type );

        GeneralData( std::string name, std::string type, std::string value );

        bool initial( std::string name, std::string type );

        bool get_Name( std::string &name );

        void put( std::string value );

        void put( const char *value );

        int get_DataType( void );

        bool get( std::string *data );

        template <typename TYPE>
        bool get( TYPE *data)
        {
            BOOLINIT_CHECK();

            if ( pValue->get(data) == NULL )
            {
                printf("Error : No data.\n");
                *data = 0;
                return false;
            }

            return true;
        }

};

void Conversion_DataFormat( std::vector < std::map< std::string, std::string > > lVector ,
                            std::vector< GeneralData > &gVector );

#endif /* GENERALDATA_H_ */
