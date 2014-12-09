#ifndef _SSMInterface_H_
#define _SSMInterface_H_

#include <string>
#include <vector>

enum SSMRESULT
{
    SSM_S_OK
    , SSM_S_FALSE
    , SSM_E_POINTER
    , SSM_E_OUTOFMEMORY
    , SSM_E_FAIL
    , SSM_E_NOINTERFACE
    , SSM_E_NOTIMPL
};

/**
* @class    IModelData
* @brief    IModelData Interface
*            This class represents context model data package
*
* @see
*/
class IModelData
{
    public:
        /**
        * @fn     getDataId
        * @brief Get affected DataId. ContextModel has plenty of data so \n
        *         returned data is matched from given condition
        *
        * @param None
        *
        * @return int
        * @warning
        * @exception
        * @see
        */
        virtual int getDataId() = 0;

        /**
        * @fn     GetPropertyCount
        * @brief ContextModel has at least one property that contains data \n
        *         property is described from its specification.
        *
        * @param None
        *
        * @return int
        * @warning
        * @exception
        * @see
        */
        virtual int getPropertyCount() = 0;

        /**
        * @fn     getPropertyName
        * @brief Retrieve propertyName
        *
        * @param [in] int propertyIndex - index of property to read
        *
        * @return std::string
        * @warning
        * @exception
        * @see
        */
        virtual std::string getPropertyName(int propertyIndex) = 0;

        /**
        * @fn     getPropertyValue
        * @brief Retrieve propertyValue
        *
        * @param [in] int propertyIndex - index of property to read
        *
        * @return std::string
        * @warning
        * @exception
        * @see
        */
        virtual std::string getPropertyValue(int propertyIndex) = 0;

        /**
        * @fn     getPropertyValueByName
        * @brief Retrieve propertyValue using given name
        *
        * @param [in] std::string propertyName - property name looking for
        *
        * @return std::string
        * @warning
        * @exception
        * @see
        */
        virtual std::string getPropertyValueByName(std::string propertyName) = 0;
    protected:
        virtual ~IModelData() {};
};

/**
* @class    IDataReader
* @brief    IDataReader Interface
*            This class represents context model data package's reader
*
* @see
*/
class IDataReader
{
    public:
        /**
        * @fn     getAffectedModels
        * @brief Get affected ContextModels. The CQL can specify multiple ContextModels for retrieving data.
        *
        * @param [out] std::vector<std::string> *pAffectedModels - affected ContextModel list
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT getAffectedModels(std::vector<std::string> *pAffectedModels) = 0;

        /**
        * @fn     getModelDataCount
        * @brief Get affected data count. There are multiple data can exist from given condition.
        *
        * @param [in] std::string modelName - affected ContextModel name
        *
        * @param [out] int *pDataCount - affected dataId count
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT getModelDataCount(std::string modelName, int *pDataCount) = 0;

        /**
        * @fn     getModelData
        * @brief Get actual Context Model data
        *
        * @param [in] std::string modelName - affected ContextModel name
        *
        *
        * @param [in] int dataIndex - affected dataId index
        *
        *
        * @param [out] IModelData **ppModelData - affected ContextModel data reader
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT getModelData(std::string modelName, int dataIndex, IModelData **ppModelData) = 0;
    protected:
        virtual ~IDataReader() {};
};

/**
* @class    IQueryEngineEvent
* @brief    IQueryEngineEvent Interface
*            This class represents Query Engine's event that contains results
*
* @see
*/
class IQueryEngineEvent
{
    public:
        /**
        * @fn     onQueryEngineEvent
        * @brief Transmit result of SSMCore to Application layer
        *
        * @param [in] int cqid - entered ContextQuery ID
        *
        * @param [in] IDataReader *pResult - result of SSMCore
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT onQueryEngineEvent(int cqid, IDataReader *pResult) = 0;
    protected:
        virtual ~IQueryEngineEvent() {};
};

class SSMInterface
{
    public:
        SSMInterface();
        ~SSMInterface();

        SSMRESULT registerQuery(std::string queryString, IQueryEngineEvent *listener, int &cqid);
        SSMRESULT unregisterQuery(int cqid);
};
#endif