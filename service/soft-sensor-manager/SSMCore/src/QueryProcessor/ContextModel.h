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
#ifndef _ContextModel_H_
#define _ContextModel_H_

#include "SSMInterface/SSMCore.h"
#include "Common/PlatformLayer.h"
#include "Common/InternalInterface.h"

/**
* @class    CContextModel
* @brief    CContextModel Interface
*            This class represents Context Model
*
* @see
*/
class CContextModel :
    public CObjectRoot<CObjectMultiThreadModel>
    , public IContextModel
    , public IThreadClient
    , public IEvent
{
    private:
        CObjectPtr<ITasker>                     m_pTasker;
        CObjectPtr<IEvaluationEngine>           m_pEvaluationEngine;
        CObjectPtr<IContextModel>               m_pParentModel;
        ModelPropertyVec                        m_modelProperties;
        //std::vector<IConditionedModel*>           m_ConditionedModels;
        int                                     m_modelId;
        std::string                             m_modelName;
        IContextModelEvent                      *m_pContextModelEvent;
        CSimpleMutex                            m_mtxActivationCount;
        int                                     m_activationCount;

        std::map<std::string, std::pair<int, ISSMResource> >    m_mapSSM;
        std::map<int, IntVec>                    m_mapDeviceDataIds;

        //DeviceDataId, RefCount
        std::map<int, int>                      m_mapSubscribedDevice;
        std::map<int, int>                      m_mapGetDevice;

        ConstructionType                        m_constructionType;

        std::string                             m_secLifeTime;

        void registerSSMResource(ActivationType activationType, int targetDeviceDataId,
                                 ISSMResource *pSSMResource);

        void unregisterSSMResource(ActivationType activationType, int targetDeviceDataId,
                                   ISSMResource *pSSMResource);

    public:
        SSMRESULT finalConstruct();

        void finalRelease();

        /**
        * @fn     create
        * @brief Create new Context Model
        *
        * @param [in] ConstructionType constructionType - Construction Type that current model is created from external modules or internally created.
        * @param [in] IContextModel *pParentModel - Parent Context Model that related to Model hierarchy.
        * @param [in] std::string modelName - Represent model's name
        * @param [in] ModelPropertyVec *pModelProperties - Represent model's properties that exposed to CQL
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT create(ConstructionType constructionType, IContextModel *pParentModel,
                         std::string modelName, ModelPropertyVec *pModelProperties);

        /**
        * @fn     registerContextModelEvent
        * @brief Register ContextModel's events like Activate or Deactivate
        *
        * @param [in] IContextModelEvent *pContextModelEvent - Event interface.
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT registerContextModelEvent(IContextModelEvent *pContextModelEvent);

        void onExecute(void *pArg);

        void onTerminate(void *pArg);

        /**
        * @fn     onEvent
        * @brief Called when new context model data arrived
        *
        * @param [in] std::string deviceID - DeviceID that represents where the data comes.
        * @param [in] TypeofEvent callType - Represent received data's type, streamed or single
        * @param [in] std::vector<ContextData> ctxData - Context data
        *
        * @return int
        * @warning
        * @exception
        * @see
        */
        int onEvent(std::string deviceID, TypeofEvent callType, std::vector<ContextData> ctxData);

        SSMRESULT queryInterface(const OID &objectID, IBase **ppObject)
        {
            if (ppObject == NULL)
                return SSM_E_POINTER;

            if (IsEqualOID(objectID, OID_IContextModel))
            {
                IBase *pBase = (IContextModel *)this;
                pBase->addRef();
                *ppObject = pBase;
                return SSM_S_OK;
            }

            return SSM_E_NOINTERFACE;
        }

        /**
        * @fn     getModelId
        * @brief Get current model's id used for Evaluation Engine
        *
        * @param NONE
        *
        * @return int
        * @warning
        * @exception
        * @see
        */
        int getModelId();

        /**
        * @fn     getModelName
        * @brief Get current model's name
        *
        * @param NONE
        *
        * @return std::string
        * @warning
        * @exception
        * @see
        */
        std::string getModelName();

        /**
        * @fn     getConstructionType
        * @brief Get current model's constructionType defined when created
        *
        * @param NONE
        *
        * @return IContextModel::ConstructionType
        * @warning
        * @exception
        * @see
        */
        ConstructionType getConstructionType();

        /**
        * @fn     getParentDataId
        * @brief Get parent dataId related to model hierarchy
        *
        * @param [in] int dataId - DataId that current model's
        * @param [in] IContextModel *pParentModel - Designated parent Context Model
        * @param [out] int *pParentDataId - Designated parent Context Model's data's Id
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getParentDataId(int dataId, IContextModel *pParentModel, int *pParentDataId);

        /**
        * @fn     getChildDataId
        * @brief Get child dataId related to model hierarchy
        *
        * @param [in] int dataId - DataId that current model's
        * @param [in] IContextModel *pChildModel - Designated child Context Model
        * @param [out] int *pParentDataId - Designated child Context Model's data's Id
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getChildDataId(int dataId, IContextModel *pChildModel, IntVec *pChildDataIds);

        /**
        * @fn     activate
        * @brief Activate current model's working. Each call to activate will increase internal reference counter
        *
        * @param [in] ActivationType activationType - Specify current model's working type
        * @param [in] int targetDeviceDataId - Specify target device's dataId
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT activate(ActivationType activationType, int targetDeviceDataId);

        /**
        * @fn     deactivate
        * @brief Deactivate current model's working. Each call to deactivate will decrease internal reference counter\n
        *        and finally deactivated when counter is 0
        *
        * @param [in] ActivationType activationType - Specify current model's working type
        * @param [in] int targetDeviceDataId - Specify target device's dataId
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT deactivate(ActivationType activationType, int targetDeviceDataId);

        //SSMRESULT GetModelSchema(ModelPropertyVec *pModelProperties);

        /**
        * @fn     addModelData
        * @brief Add model data to Context Model
        *
        * @param [in] int parentDataId - Specify current model's parent Context Model's dataId
        * @param [in] ModelPropertyVec *pData - Packed data for adding
        * @param [out] int *pDataId - Returned dataId that newly added
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT addModelData(int parentDataId, ModelPropertyVec *pData, int *pDataId);

        /**
        * @fn     updateModelData
        * @brief Update model data to Context Model
        *
        * @param [in] int dataId - DataId that provided AddModelData's execution
        * @param [in] ModelPropertyVec *pData - Packed data for updating
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT updateModelData(int dataId, ModelPropertyVec *pData);

        /**
        * @fn     deleteModelData
        * @brief Delete model data using given index
        *
        * @param [in] int dataId - DataId that provided AddModelData's execution
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT deleteModelData(int dataId);

        /**
        * @fn     getModelData
        * @brief Get model data
        *
        * @param [in] int dataId - DataId that provided AddModelData's execution
        * @param [out] ModelPropertyVec *pData - Packed data
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getModelData(int dataId, ModelPropertyVec *pData);

        /**
        * @fn     getModelDataSet
        * @brief Get model data set
        *
        * @param [in] int startIndex - Starting index of model data
        * @param [in] int count - Number of data to retrieve
        * @param [out] vector<ModelPropertyVec> *pData - Packed data
        * @param [out] int *pLastIndex - Index of last data
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getModelDataSet(int startIndex, int count,
                                  std::vector<ModelPropertyVec> *pDataSet, int *pLastIndex);

        /**
        * @fn     createConditionedModel
        * @brief Create Conditioned Model for data searching or triggering
        *
        * @param [in] ModelConditionVec *pModelConditionVec - Conditions that searching or triggering
        * @param [out] IConditionedModel **ppConditionedModel - Created Conditioned Model interface
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT createConditionedModel(ModelConditionVec *pModelConditionVec,
                                         IConditionedModel **ppConditionedModel);

        //SSMRESULT CleanUpModelData();

        /**
        * @fn     addSSMResourceAndDeviceDataId
        * @brief Add SSM resources with other informations - Internal use only
        *
        * @param [in] std::string deviceId - deviceId that used D2D communication
        * @param [in] int deviceDataId - Device's dataId that where SSMResource come from
        * @param [in] ISSMResource *pSSMResource - SSMResource for adding
        *
        * @return NONE
        * @warning
        * @exception
        * @see
        */
        void addSSMResourceAndDeviceDataId(std::string deviceId, int deviceDataId,
                                           ISSMResource *pSSMResource);

        /**
        * @fn     SetLifeTime
        * @brief Sets current model data's lifeTime
        *
        * @param [in] int seconds - model data's lifeTime
        *
        * @return NONE
        * @warning
        * @exception
        * @see
        */
        void setLifeTime(std::string seconds);
};
#endif
