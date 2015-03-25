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

/*
 * Indoor Trajectory Soft Sensor
 */

#ifndef INDOORTRAJECTORYSENSOR_H_
#define INDOORTRAJECTORYSENSOR_H_

/**
 * This header file is included to define _EXPORT_.
 */
#include "SSMModelDefinition.h"
#include "Trajectory.h"

#define _EXPORT_

#ifdef __cplusplus
extern "C"
{
#endif
INTERFACE_DECLSPEC void InitializeContext(ICtxDelegate *pDelegate);
#ifdef __cplusplus

};
#endif

namespace ITSName
{
#define PHYSICAL_EA 1

    typedef struct _physicalInput_
    {
        char *m_thingName;
        int m_inputNum;
        void *m_pInputStruct;
    } physicalInput;

    typedef enum
    {
        SUCCESS = 0, ERROR, NO_ELEMENT
    } ITSResult;

    class SThing
    {
        public:
            std::string timestamp;
            std::string ID;
            float distance;
            int   proximity;
            float SD;

            SThing( void ) : timestamp(""), ID(""), distance(0.0), proximity(0), SD(0.0) {}
            ~SThing( void ) {}
    };

    class Things
    {
        public :
            SThing *things;
            int EA;

            Things(void) : EA(0), things(0) {}
            Things( int ea ) : EA(ea), things(0)
            {
                things = new SThing[EA]();
            }
            ~Things( void )
            {
                if (things)
                {
                    delete [] things;
                    things = 0;
                }
            }
    };

    class ITS: public ICtxEvent
    {
        private:
            static ITS *singleton;
            Things *m_TList;
            SThing *m_closestThing;

            int runLogic(std::vector< ContextData > &contextDataList);

        public:
            static physicalInput s_PHYSICAL_SENSORs[PHYSICAL_EA];

            static ITS *GetInstance( void );
            ITS();
            ~ITS();

            static void *SelfTrajectoryThread(void *param);
            void onCtxEvent(enum CTX_EVENT_TYPE eventType, std::vector< ContextData > contextDataList);

            ITSResult getInput(std::vector< ContextData > &contextDataList, CurrentService *CService );
            ITSResult searchValidService( Things *things , CurrentService *CService );
            Trajectory *makeTrajectory(CurrentService *CService);
            ITSResult setOutput( Trajectory *m_result, ContextData *out );

            friend void INTERFACE_DECLSPEC initializeContext(ICtxDelegate *pDelegate);
    };
}
;

#endif /* INDOORTRAJECTORYSENSOR_H_ */
