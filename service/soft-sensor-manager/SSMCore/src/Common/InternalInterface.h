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
#ifndef _InternalInterface_H_
#define _InternalInterface_H_

#include "SSMInterface/SSMModelDefinition.h"
#include "Common/PlatformLayer.h"

class CallbackData
{
    public:
        TypeofEvent m_callType;
        std::string m_name;
        IEvent     *m_pCallbackEvent;
        CallbackData()
        {
            m_callType = SSM_ONCE;
            m_pCallbackEvent = NULL;
        }

        CallbackData(TypeofEvent type, std::string name, IEvent *pCallbackEvent)
        {
            m_name = name;
            m_pCallbackEvent = pCallbackEvent;
            m_callType = type;
        }
};

struct DictionaryData
{
    std::string rootName;
    std::vector<std::map<std::string, std::string> > attributeProperty;
    std::vector<std::map<std::string, std::string> > outputProperty;
    std::vector<std::string> appInputs;
    std::vector<std::string> inputs;
    std::string attributePropertyCount;
    std::string outputPropertyCount;
    std::string inputCount;
    std::string appInputCount;
    std::vector<std::vector<std::string> > enterConditions;
};

enum PARSE_TYPE {SSM_UNKNOWN, SSM_ROOT_NAME, SSM_HIGH_CONTEXT_NAME, SSM_ATTRIBUTE, SSM_OUTPUT, SSM_APP_INPUTS, SSM_INPUTS, SSM_INPUT, SSM_ENTER_CONDITIONS};
enum DICTIONARY_TYPE {LOW, HIGH, SENSOR, USER};

static const OID OID_IContextRepository = { 0x611144b0, 0xf036, 0x44a1, { 0xba, 0xe6, 0x6a, 0x92, 0xe1, 0xf0, 0xcc, 0x53 } };
/**
 * @class    IContextRepository
 * @brief    Interface for context manifest
 *           parse, get, set context manifest. provide device information
 *
 * @see
 */
class IContextRepository : public IBase
{
    public:
        /**
        * @fn           getSoftSensorList
        * @brief        Get high level context resource list
        *
        * @param        [out] std::vector<ISSMResource*> *pSoftSensorList - High level context list
        * @return       SSMRESULT
        *                   SSM_S_OK
        *                   , SSM_S_FALSE
        *                   , SSM_E_POINTER
        *                   , SSM_E_OUTOFMEMORY
        *                   , SSM_E_FAIL
        *                   , SSM_E_NOINTERFACE
        *                   , SSM_E_NOTIMPL
        *
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT getSoftSensorList(OUT std::vector<ISSMResource *> *pSoftSensorList) = 0 ;

        /**
        * @fn           getPrimitiveSensorList
        * @brief        Get low level context resource list
        *
        * @param        [out] std::vector<ISSMResource*> *pPrimitiveSensorList - Low level context list
        * @return       SSMRESULT
        *                   SSM_S_OK
        *                   , SSM_S_FALSE
        *                   , SSM_E_POINTER
        *                   , SSM_E_OUTOFMEMORY
        *                   , SSM_E_FAIL
        *                   , SSM_E_NOINTERFACE
        *                   , SSM_E_NOTIMPL
        *
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT getPrimitiveSensorList(OUT std::vector<ISSMResource *> *pPrimitiveSensorList) = 0
                ;

        /**
        * @fn           initRepository
        * @brief        initialize repository using given info
        *
        * @param        [in] std::string name - Device name
        * @param        [in] std::string type - Device Type
        * @param        [in] std::string pathSoftSensors - SoftSensors Repository path
        * @param        [in] std::string pathDescription - SoftSensors Description path
        * @return       SSMRESULT
        *
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT initRepository(IN std::string name, IN std::string type,
                                         IN std::string pathSoftSensors, IN std::string pathDescription) = 0;

        virtual SSMRESULT registerResourceFinderEvent(IN IResourceEvent *pResourceEvent) = 0;
        virtual SSMRESULT startResourceFinder() = 0;
        virtual SSMRESULT onResourceFound(IN ISSMResource *pSensor) = 0;
        virtual SSMRESULT onResourceLost(IN ISSMResource *pSensor) = 0;

        virtual SSMRESULT startObserveResource(IN ISSMResource *pSensor, IN IEvent *pEvent) = 0;
        virtual SSMRESULT stopObserveResource(IN ISSMResource *pSensor) = 0;

        virtual SSMRESULT loadSoftSensor(IN std::string softSensorName, IN ICtxDelegate *pDelegate,
                                         OUT void **hSoftSensor) = 0;
        virtual SSMRESULT unloadSoftSensor(IN void *hSoftSensor) = 0;
};

struct ModelProperty
{
    enum Type {TYPE_NUMERIC, TYPE_INTEGER, TYPE_REAL, TYPE_TEXT};
    std::string     propertyName;
    std::string     propertyValue;
    Type            propertyType;
};

typedef std::vector<ModelProperty> ModelPropertyVec;

struct ModelCondition
{
    enum Predicate {PREDICATE_EQ, PREDICATE_NEQ, PREDICATE_GT, PREDICATE_LT, PREDICATE_GTE, PREDICATE_LTE};
    ModelProperty   modelProperty;
    Predicate       predicate;
};

typedef std::vector<ModelCondition> ModelConditionVec;

/**
* @class    IEvaluationEngineEvent
* @brief    IEvaluationEngineEvent Interface
*            This class represents event interface that listening IEvaluationEngine's results
*
* @see
*/
class IEvaluationEngineEvent : public IBase
{
    public:
        /**
        * @fn     onWatchModelData
        * @brief Called when Model data meets given conditions.
        *
        * @param [in] int triggerId - Trigger id.
        * @param [in] int dataId - Affected data's Id
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT onWatchModelData(IN int triggerId, IN int dataId) = 0;
};

static const OID OID_IEvaluationEngine = { 0x5a59a9f3, 0x7506, 0x4a3a, { 0xb9, 0x34, 0xcb, 0x51, 0xd, 0xb4, 0xed, 0x53 } };
/**
* @class    IEvaluationEngine
* @brief    IEvaluationEngine Interface
*            This class represents Evaluation Engine
*
* @see
*/
class IEvaluationEngine : public IBase
{
    public:
        /**
        * @fn     initializeEngine
        * @brief Initialize EvaluationEngine with database
        *
        * @param NONE
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT initializeEngine() = 0;

        /**
        * @fn     terminateEngine
        * @brief Terminate engine and dump to file if required
        *
        * @param NONE
        *
        * @return void
        * @warning
        * @exception
        * @see
        */
        virtual void terminateEngine() = 0;

        /**
        * @fn     createModel
        * @brief Create new context model
        *
        * @param [in] int parentModelId - Parent Context Model id that related to Model hierarchy.
        * @param [in] const char *newModelName - Represent model's name
        * @param [in] ModelPropertyVec *pModelProperties - Represent model's properties that exposed to CQL
        * @param [out] int *pModelId - Created model's id
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT createModel(IN int parentModelId, IN const char *newModelName,
                                      IN ModelPropertyVec *pModelDescs, OUT int *pModelId) = 0;
        //virtual SSMRESULT GetModelSchema(IN int modelId, OUT ModelPropertyVec *pModelProperties) = 0;

        /**
        * @fn     addModelData
        * @brief Add model data to Context Model
        *
        * @param [in] int modelId - model id that created CreateModel's execution.
        * @param [in] int parentModelId - parent model id that created CreateModel's execution.
        * @param [in] int parentDataId - Specify current model's parent Context Model's dataId
        * @param [in] ModelPropertyVec *pData - Packed data for adding
        * @param [out] int *pDataId - Returned dataId that newly added
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT addModelData(IN int modelId, IN int parentModelId, IN int parentDataId,
                                       IN ModelPropertyVec *pModelValues, OUT int *pDataId) = 0;

        /**
        * @fn     updateModelData
        * @brief Update model data to Context Model
        *
        * @param [in] int modelId - model id that created CreateModel's execution.
        * @param [in] int dataId - DataId that provided AddModelData's execution
        * @param [in] ModelPropertyVec *pData - Packed data for updating
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT updateModelData(IN int modelId, IN int dataId,
                                          IN ModelPropertyVec *pModelValues) = 0;

        //virtual SSMRESULT DeleteModel(IN int modelId) = 0;

        /**
        * @fn     deleteModelData
        * @brief Delete model data using given index
        *
        * @param [in] int modelId - model id that created CreateModel's execution.
        * @param [in] int dataId - DataId that provided AddModelData's execution
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT deleteModelData(IN int modelId, IN int dataId) = 0;

        //virtual SSMRESULT GetModelId(IN const char *modelName, OUT int *pModelId) = 0;

        /**
        * @fn     getModelData
        * @brief Get model data
        *
        * @param [in] int modelId - model id that created CreateModel's execution.
        * @param [in] int dataId - DataId that provided AddModelData's execution
        * @param [out] ModelPropertyVec *pData - Packed data
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT getModelData(IN int modelId, IN int dataId,
                                       OUT ModelPropertyVec *pModelProperties) = 0;

        /**
        * @fn     getModelDataSet
        * @brief Get model data set
        *
        * @param [in] int modelId - model id that created CreateModel's execution.
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
        virtual SSMRESULT getModelDataSet(IN int modelId, IN int startIndex, IN int count,
                                          OUT std::vector<ModelPropertyVec> *pDataSet, OUT int *pLastIndex) = 0;

        /**
        * @fn     getConditionedModelData
        * @brief Get affected data from given conditions
        *
        * @param [in] int modelId - model id that created CreateModel's execution.
        * @param [in] ModelConditionVec *pModelConditions - Conditions for data retrieving
        * @param [out] IntVec   *pDataIds - Searched dataId sets.
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT getConditionedModelData(IN int modelId, IN ModelConditionVec *pModelConditions,
                OUT IntVec *pDataIds) = 0;

        /**
        * @fn     watchModelData
        * @brief Watching data from given conditions
        *
        * @param [in] int modelId - model id that created CreateModel's execution.
        * @param [in] ModelConditionVec *pModelConditions - Conditions for data triggering
        * @param [in] IEvaluationEngineEvent *pEvaluationEngineEvent - Event interface where triggered event listening
        * @param [out] int *pTriggerId - Identifier which trigger is activated
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT watchModelData(IN int modelId, IN ModelConditionVec *pModelConditions,
                                         IN IEvaluationEngineEvent *pEvaluationEngineEvent, OUT int *pTriggerId) = 0;

        /**
        * @fn     dropWatchModelData
        * @brief Stop watching data
        *
        * @param [in] int triggerId - Identifier which trigger to drop
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT dropWatchModelData(IN int triggerId) = 0;

        /**
        * @fn     getParentDataId
        * @brief Get parent dataId related to model hierarchy
        *
        * @param [in] int modelId - model id that created CreateModel's execution.
        * @param [in] int dataId - DataId that current model's
        * @param [in] int parentModelId - Designated parent Context Model's id
        * @param [out] int *pParentDataId - Designated parent Context Model's data's Id
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT getParentDataId(IN int modelId, IN int dataId, IN int parentModelId,
                                          OUT int *pParentDataId) = 0;

        /**
        * @fn     getChildDataId
        * @brief Get child dataId related to model hierarchy
        *
        * @param [in] int modelId - model id that created CreateModel's execution.
        * @param [in] int dataId - DataId that current model's
        * @param [in] int childModelId - Designated child Context Model's id
        * @param [out] IntVec *pChildDataIds - Designated child Context Model's data's Id
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT getChildDataId(IN int modelId, IN int dataId, IN int childModelId,
                                         OUT IntVec *pChildDataIds) = 0;

        //For decision tree
        //virtual SSMRESULT GetPathToRoot(IN int currentModelId, OUT IntVec *pPath) = 0;
        //virtual SSMRESULT GetModelHierarchy(IN int rootModelId, OUT StringVec *pPath) = 0;
        //virtual SSMRESULT GetDataHierarchy(IN int rootModelId, IN int dataId, OUT StringVec *pPath) = 0;
};

/**
* @class    IConditionedModelEvent
* @brief    IConditionedModelEvent Interface
*            This class represents event interface that listening IConditionedModel's results
*
* @see
*/
class IConditionedModelEvent
{
    public:
        /**
        * @fn     onConditionedModelTriggered
        * @brief Called when each Conditioned Model meets the conditions
        *
        * @param [in] int triggerId - trigger id that which Conditioned Model triggered
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT onConditionedModelTriggered(IN int triggerId) = 0;
        virtual ~IConditionedModelEvent() {};
};

class IContextModel;

static const OID OID_IConditionedModel = { 0xdc71abc7, 0x5a02, 0x418a, { 0xad, 0x1e, 0xc0, 0x1, 0x9, 0x6b, 0x32, 0x6d } };
/**
* @class    IConditionedModel
* @brief    IConditionedModel Interface
*            This class represents Context Model's data search from given conditions
*
* @see
*/
class IConditionedModel : public IBase
{
    public:
        /**
        * @fn     getBaseContextModel
        * @brief Get base Context Model interface
        *
        * @param [out] IContextModel **ppBaseContextModel - Interface pointer for retrieving base Context Model interface.
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT getBaseContextModel(OUT IContextModel **ppBaseContextModel) = 0;

        /**
        * @fn     registerConditionedModelEvent
        * @brief Register for receiving triggered event from given conditions
        *
        * @param [in] IConditionedModelEvent *pConditionedModelEvent - Interface for listening events.
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT registerConditionedModelEvent(IN IConditionedModelEvent *pConditionedModelEvent) =
            0;

        /**
        * @fn     activateTrigger
        * @brief Activate trigger using given conditions
        *
        * @param [out] int *pTriggerId - Activated trigger id.
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT activateTrigger(OUT int *pTriggerId) = 0;

        /**
        * @fn     deactivateTrigger
        * @brief Deactivate trigger that activated
        *
        * @param NONE
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT deactivateTrigger() = 0;

        /**
        * @fn     hasAffectedData
        * @brief Quries if current Conditioned Model has affected data
        *
        * @param NONE
        *
        * @return bool
        * @warning
        * @exception
        * @see
        */
        virtual bool hasAffectedData() = 0;

        /**
        * @fn     getAffectedData
        * @brief Get affected data from given conditions includes triggered data
        *
        * @param [out] IntVec   *pDataIds - Affected dataId sets.
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT getAffectedData(OUT IntVec *pDataIds) = 0;

        /**
        * @fn     getWatchCondition
        * @brief Get watching conditions created from Context Model
        *
        * @param [out] ModelConditionVec *pModelConditions - Affected dataId sets.
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT getWatchCondition(OUT ModelConditionVec *pModelConditions) = 0;
};

static const OID OID_IConditionedQueryResult = { 0x4a24e85b, 0x8903, 0x4cf5, { 0x88, 0x7b, 0x65, 0xc2, 0xc3, 0x9e, 0x42, 0xfd } };
/**
* @class    IConditionedQueryResult
* @brief    IConditionedQueryResult Interface
*            This class represents executed CQL's result data
*
* @see
*/
class IConditionedQueryResult : public IBase
{
    public:
        /**
        * @fn     getConditionedModelCount
        * @brief Get conditioned model count
        *
        * @param NONE
        *
        * @return unsigned int
        * @warning
        * @exception
        * @see
        */
        virtual unsigned int getConditionedModelCount() = 0;

        /**
        * @fn     getConditionedContextModel
        * @brief Get conditioned model by index
        *
        * @param [in] unsigned int conditionedModelIndex - Index of Conditioned Model interface for retrieving
        * @param [out] IConditionedModel *pConditionedModel - Conditioned Model interface that contains results
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT getConditionedContextModel(IN unsigned int conditionedModelIndex,
                OUT IConditionedModel **ppConditionedModel) = 0;
};

/**
* @class    IConditionedQueryEvent
* @brief    IConditionedQueryEvent Interface
*            This class represents event interface that listening IConditionedQuery's results
*
* @see
*/
class IConditionedQueryEvent
{
    public:
        /**
        * @fn onConditionedQueryEvent
        * @brief IConditionedQueryEvent interface's event callbacks for receiving query results
        *
        * @param [in] std::string ContextQuery - Entered ContetxQuery
        * @param [out] int* cqid - ID of ContextQuery
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT onConditionedQueryEvent(IN int userTriggerId,
                IN IConditionedQueryResult *pConditionedQueryResult) = 0;
        virtual ~IConditionedQueryEvent() {};
};

static const OID OID_IConditionedQuery = { 0xd61373d1, 0x9727, 0x4a84, { 0x98, 0x14, 0x4c, 0xf, 0xed, 0x25, 0x52, 0xe3 } };
/**
* @class    IConditionedQuery
* @brief    IConditionedQuery Interface
*            This class represents parsed CQL for data retrieving
*
* @see
*/
class IConditionedQuery : public IBase
{
    public:
        /**
        * @fn     registerConditionedModel
        * @brief Register Conditioned Model for tracking triggers
        *
        * @param [in] IConditionedModel *pConditionedModel - Interface of Conditioned Model
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT registerConditionedModel(IN IConditionedModel *pConditionedModel) = 0;

        /**
        * @fn     activateTriggers
        * @brief Activate triggers for data evaluating. This changed related ContextModel's status to running
        *
        * @param [in] int userTriggerId - Specify TriggerId for event listening
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT activateTriggers(IN int userTriggerId) = 0;

        /**
        * @fn     deactivateTriggers
        * @brief Deactivate triggers for data evaluating.
        *
        * @param [in] int userTriggerId - Specify TriggerId for event listening
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT deactivateTriggers() = 0;

        /**
        * @fn     getConditionedQueryResult
        * @brief Retrieves conditioned data from current cached data
        *
        * @param [out] IConditionedQueryResult **ppConditionedQueryResult - Interface of result reader
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT getConditionedQueryResult(OUT IConditionedQueryResult **ppConditionedQueryResult)
            = 0;

        /**
        * @fn     hasAllConditionedModels
        * @brief Retrieves current conditioned query has all Conditioned ContextModels for evaluating
        *
        * @param NONE
        *
        * @return bool
        * @warning
        * @exception
        * @see
        */
        virtual bool hasAllConditionedModels() = 0;
};

static const OID OID_IContextModel = { 0xcc56003c, 0xa036, 0x4f27, { 0x9c, 0xb3, 0xdc, 0xaf, 0x1f, 0xb0, 0x81, 0x58 } };
/**
* @class    IContextModel
* @brief    IContextModel Interface
*            This class represents Context Model
*
* @see
*/
class IContextModel : public IBase
{
    public:
        enum Status {STATUS_ACTIVATE, STATUS_DEACTIVATE, STATUS_START_READ_VALUE, STATUS_STOP_READ_VALUE};
        enum ActivationType {ACTIVATION_TYPE_GET, ACTIVATION_TYPE_SUBSCRIBE};
        enum ConstructionType {CONSTRUCTION_TYPE_INTERNAL, CONSTRUCTION_TYPE_EXTERNAL, CONSTRUCTION_TYPE_REMOTE};

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
        virtual int getModelId() = 0;

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
        virtual std::string getModelName() = 0;

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
        virtual ConstructionType getConstructionType() = 0;

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
        virtual SSMRESULT getParentDataId(IN int dataId, IN IContextModel *pParentModel,
                                          OUT int *pParentDataId) = 0;

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
        virtual SSMRESULT getChildDataId(IN int dataId, IN IContextModel *pChildModel,
                                         OUT IntVec *pPath) = 0;

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
        virtual SSMRESULT activate(IN ActivationType activationType, IN int targetDeviceDataId) = 0;

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
        virtual SSMRESULT deactivate(IN ActivationType activationType, IN int targetDeviceDataId) = 0;
        //virtual SSMRESULT GetModelSchema(OUT ModelPropertyVec *pModelProperties) = 0;

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
        virtual SSMRESULT addModelData(IN int parentDataId, IN ModelPropertyVec *pData,
                                       OUT int *pDataId) = 0;

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
        virtual SSMRESULT updateModelData(IN int dataId, IN ModelPropertyVec *pData) = 0;

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
        virtual SSMRESULT deleteModelData(IN int dataId) = 0;

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
        virtual SSMRESULT getModelData(IN int dataId, OUT ModelPropertyVec *pData) = 0;

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
        virtual SSMRESULT getModelDataSet(IN int startIndex, IN int count,
                                          OUT std::vector<ModelPropertyVec> *pDataSet, OUT int *pLastIndex) = 0;

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
        virtual SSMRESULT createConditionedModel(IN ModelConditionVec *pModelConditionVec,
                OUT IConditionedModel **ppConditionedModel) = 0;
        //virtual SSMRESULT CleanUpModelData() = 0;

        /**
        * @fn     setLifeTime
        * @brief Sets current model data's lifeTime
        *
        * @param [in] int seconds - model data's lifeTime
        *
        * @return NONE
        * @warning
        * @exception
        * @see
        */
        virtual void setLifeTime(std::string seconds) = 0;
};

/**
* @class    IContextModelEvent
* @brief    IContextModelEvent Interface
*            This class represents event interface for tracking IContextModel events
*
* @see
*
* Copyright 2013 by Samsung Electronics, Inc.,
*
* This software is the confidential and proprietary information
* of Samsung Electronics, Inc. ("Confidential Information").  You
* shall not disclose such Confidential Information and shall use
* it only in accordance with the terms of the license agreement
* you entered into with Samsung.
*/
class IContextModelEvent
{
    public:
        /**
        * @fn     onModelStatusChange
        * @brief Called when ContextModel wants to activate or deactivate
        *
        * @param [in] IContextModel::Status newStatus - Context Model's new status
        * @param [in] ISSMResource *pSSMResource - Instance of actual resource
        * @param [in] IContextModel *pModel - Affected Context Model
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT onModelStatusChange(IN IContextModel::Status newStatus,
                                              IN ISSMResource *pSSMResource, IN IContextModel *pModel) = 0;
        virtual ~IContextModelEvent() {}
};

/**
* @class    IContextModelAccessor
* @brief    IContextModelAccessor Interface
*            This class represents context model accessors
*
* @see
*/
class IContextModelAccessor
{
    public:
        /**
        * @fn     onQueryContextModel
        * @brief Called when ContextModelAccessor wants to query installed ContextModel
        *
        * @param [in] std::string modelName - ContextModel name to find out
        * @param [out] IContextModel **ppContextModel - Interface of quried ContextModel
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT onQueryContextModel(IN std::string modelName,
                                              OUT IContextModel **ppContextModel) = 0;
        virtual ~IContextModelAccessor() {}
};

static const OID OID_IContextDataReader = { 0x65ce481, 0xc5dd, 0x41dc, { 0xa3, 0xc1, 0xa4, 0x98, 0xc9, 0xa8, 0xcc, 0xd5 } };
/**
* @class    IContextDataReader
* @brief    IContextDataReader Interface
*            This class represents context model database reader for context model developers
*
* @see
*/
class IContextDataReader : public IBase
{
    public:
        /**
        * @fn     registerContextModelAccessor
        * @brief Called when ContextModelAccessor wants to query installed ContextModel
        *
        * @param [in] IContextModelAccessor *pContextModelAccessor - Instance of implemented IContextModelAccessor interface
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT registerContextModelAccessor(IN IContextModelAccessor *pContextModelAccessor) = 0;

        /**
        * @fn     getContextData
        * @brief Retrieve ContextData using given ContextModel name and dataId
        *
        * @param [in] std::string modelName - ContextModel name
        * @param [in] int startIndex - Starting index of model data
        * @param [in] int count - Number of data to retrieve
        * @param [out] std::vector<ContextData> *data - ContextData set
        * @param [out] int *pLastIndex - Index of last data
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT getContextData(IN std::string modelName, IN int startIndex, IN int count,
                                         OUT std::vector<ContextData> *data, OUT int *pLastIndex) = 0;
};

typedef std::vector<std::pair<std::string, ModelConditionVec> > QueryCondition;

static const OID OID_IPropagationEngine = { 0xeb7e3d6c, 0x2cae, 0x43b3, { 0xb9, 0xae, 0x1b, 0xa, 0x6c, 0xdd, 0x9, 0xeb } };
/**
* @class    IPropagationEngine
* @brief    IPropagationEngine Interface
*            This class represents propagation engine that manages ContextModel's automation
*
* @see
*/
class IPropagationEngine : public IBase
{
    public:
        /**
        * @fn     getContextModel
        * @brief Retrieve ContextModel interface specified name
        *
        * @param [in] std::string modelName - ContextModel's name that looking for
        * @param [out] IContextModel **ppContextModel - Interface pointer for retrieving
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT getContextModel(IN std:: string modelName,
                                          OUT IContextModel **ppContextModel) = 0;

        /**
        * @fn     createConditionedQuery
        * @brief Create Conditioned Query using conditions
        *
        * @param [in] IContextModel::ActivationType activationType - ActivationType for ContextModel's working strategy
        * @param [in] QueryCondition *pQueryCondition - Conditions for data retrieving
        * @param [in] IConditionedQueryEvent *pEvent - Event interface for Conditioned Result listening
        * @param [out] IConditionedQuery **ppConditionedQuery - Interface for retrieving Conditioned Query
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT createConditionedQuery(IN IContextModel::ActivationType activationType,
                IN QueryCondition *pQueryCondition,
                IN IConditionedQueryEvent *pEvent, OUT IConditionedQuery **ppConditionedQuery) = 0;
};

static const OID OID_IQueryEngineInternal = { 0x6eabf1a9, 0xe884, 0x4562, { 0x8b, 0x90, 0x1e, 0xe8, 0xba, 0xbf, 0xbe, 0x1a } };
/**
* @class    IQueryEngineInternal
* @brief    IQueryEngineInternal Interface
*           This class manages ContextQuery at backend side.
*
* @see
*           CObject<CQueryEngine>   *pQueryEngine;
*           CreateInstance(OID_IQueryEngineInternal, (IBase**)&pQueryEngine);
*           pQeuryEngine->ExecuteContextQuery(contextquery,cqid);
*/
class IQueryEngineInternal : public IBase
    , public IQueryEngine
{
};

static const OID OID_IContextExecutor = { 0x92dcc900, 0xdc8a, 0x474b, { 0x87, 0x45, 0x4d, 0x0, 0xe4, 0xd1, 0x52, 0x2b } };
/**
 * @class    IContextExecutor
 * @brief    Interface for execute context model.
 *           Delegate requested context to lower layer.
 *           Execute context model.
 *
 * @see
 */
class IContextExecutor : public IBase
    , public IEvent
    , public ICtxDelegate
{
    public:
        /**
        * @fn     registerContext
        * @brief  Register Context for receiving values
        *
        * @param [in] TypeofEvent callType - Call types of Context data request, stream or current
        * @param [in] ISSMResource *pSSMResouce - Requested Context model
        * @param [in] IEvent *pEvent - Event interface for receiving data
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual void registerContext(TypeofEvent callType, ISSMResource *pSSMResouce, IEvent *pEvent) = 0;

        /**
        * @fn     unregisterContext
        * @brief  Unregister Context for receiving values
        *
        * @param [in] TypeofEvent callType - Call types of Context data request, stream or current
        * @param [in] ISSMResource *pSSMResouce - Requested Context model
        * @param [in] IEvent *pEvent - Event interface for receiving data
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual void unregisterContext(TypeofEvent callType, ISSMResource *pSSMResource,
                                       IEvent *pEvent) = 0;
};

static const OID OID_ISensingEngine = { 0x3140a3dc, 0xf912, 0x4d88, { 0x97, 0x3c, 0x86, 0xe8, 0x35, 0x69, 0xa7, 0xf8 } };
/**
 * @class    ISensingEngine
 * @brief    Class for implementing main abstration of SensorProcessor
 *
 *
 * @see
 */
class ISensingEngine : public IBase
    , public IEvent
{
    public:
        /**
        * @fn     registerContext
        * @brief  Register Context for receiving values
        *
        * @param [in] TypeofEvent callType - Call types of Context data request, stream or current
        * @param [in] ISSMResource *pSSMResouce - Requested Context model
        * @param [in] IEvent *pEvent - Event interface for receiving data
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT registerContext(TypeofEvent callType, ISSMResource *pSSMResouce,
                                          IEvent *pEvent) = 0;

        /**
        * @fn     unregisterContext
        * @brief  Unregister Context for receiving values
        *
        * @param [in] TypeofEvent callType - Call types of Context data request, stream or current
        * @param [in] ISSMResource *pSSMResouce - Requested Context model
        * @param [in] IEvent *pEvent - Event interface for receiving data
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT unregisterContext(TypeofEvent callType, ISSMResource *pSSMResource,
                                            IEvent *pEvent) = 0;

        /**
        * @fn    getList
        * @brief Get all available context models
        *
        * @param [out] std::vector<ISSMResource*> *pList - List of available context model
        *
        * @return NONE
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT getList(std::vector<ISSMResource *> *pList) = 0;
};

class IResourceFinderEvent
{
    public:
        virtual SSMRESULT onResourceFound(IN ISSMResource *pSensor) = 0;
        virtual SSMRESULT onResourceLost(IN ISSMResource *pSensor) = 0;
        virtual ~IResourceFinderEvent() {}
};

static const OID OID_IResourceFinder = { 0x8f52a31f, 0x51d5, 0x462b, { 0xa9, 0x8d, 0x40, 0x70, 0xa3, 0xf6, 0xb5, 0x6f } };
class IResourceFinder : public IBase
{
    public:
        virtual SSMRESULT registerResourceFinderEvent(IN IResourceFinderEvent *pEvent) = 0;
        virtual SSMRESULT startResourceFinder() = 0;
        virtual SSMRESULT startObserveResource(IN ISSMResource *pSensor, IN IEvent *pEvent) = 0;
        virtual SSMRESULT stopObserveResource(IN ISSMResource *pSensor) = 0;
};

/**
* @class    ISoftSensorManager
* @brief    ISoftSensorManager Interface
*            This class represents main interface of SSM.
*
* @see
*           static ISoftSensorManager       *g_pSoftSensorManager = NULL;
*           CreateInstance(OID_ISoftSensorManager, (IBase**)&g_pSoftSensorManager);
*           g_pSoftSensorManager->TerminateCore();
*           g_pSoftSensorManager->Release();
*/
static const OID OID_ISoftSensorManager = { 0x6b9ce4cb, 0x31fe, 0x4b05, { 0xb4, 0xca, 0x10, 0x3d, 0x82, 0x59, 0x4f, 0x4d } };
class ISoftSensorManager : public IBase
{
    public:
        /**
        * @fn initializeCore
        * @brief Initialize core using given configuration information.
        *           <SSMCore>
        *               <Device>
        *                   <UDN>windows7-31f8-11b4-a222-08002b34c003</UDN>
        *                   <Name>MyPC</Name>
        *                   <Type>PC</Type>
        *               </Device>
        *           </SSMCore>
        *
        * @param [in] std::string xmlDescription - specification described in XML
        * @param [out] IBase** ppvObject - reference pointer to get instance pointer
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT initializeCore(IN std::string xmlDescription) = 0;

        /**
        * @fn startCore
        * @brief Start Core.
        *         This enables network capability that can discover and discovered.
        *
        * @param None
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT startCore() = 0;

        /**
        * @fn stopCore
        * @brief Stop Core.
        *         This disables network capability that can discover and discovered.
        *
        * @param None
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT stopCore() = 0;

        /**
        * @fn terminateCore
        * @brief Terminates Core.
        *         Free all allocated resources
        *
        * @param None
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT terminateCore(IN bool factoryResetFlag) = 0;

        /**
        * @fn createQueryEngine
        * @brief Create new Query Engine instance.
        *         Each engine instance has it's own event thread
        *         we recommend each application should creates only one instance.
        *
        * @param [out] IQueryEngine **ppQueryEngine - instance pointer of new Query Engine
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT createQueryEngine(OUT IQueryEngine **ppQueryEngine) = 0;

        /**
        * @fn releaseQueryEngine
        * @brief Release exists Query Engine instance.
        *
        * @param [in] IQueryEngine *pQueryEngine - instance pointer of exists Query Engine
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual unsigned long releaseQueryEngine(IN IQueryEngine *pQueryEngine) = 0;

        /**
        * @fn getInstalledModelList
        * @brief Gets all installed models from local and remote
        *
        * @param [out] std::vector<ISSMResource*> *pList - List of installed context model
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT getInstalledModelList(OUT std::vector<ISSMResource *> *pList) = 0;
};
#endif
