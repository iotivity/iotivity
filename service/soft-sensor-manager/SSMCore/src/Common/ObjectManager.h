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

#ifndef _ObjectManager_H_
#define _ObjectManager_H_

#include "PlatformLayer.h"
#include "SSMInterface/SSMCore.h"

/* Define NULL pointer value */
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

typedef struct _OID
{
    unsigned long  data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char  data4[8];
} OID;

const OID OID_IBase = {0x3b465976, 0x6486, 0x4c1f, {0x84, 0xb9, 0xeb, 0x80, 0x79, 0x78, 0x2b, 0x8}};

/**
* @fn    IsEqualOID
* @brief Compare both OID
*
* @param [in] SSMRESULT res - return code
*
* @return Return positive value if both equal
* @warning
* @exception
* @see
*/
inline int IsEqualOID(const OID &oid1, const OID &oid2)
{
    return (( oid1.data1 ==  oid2.data1) &&
            ( oid1.data2 ==  oid2.data2) &&
            ( oid1.data3 ==  oid2.data3) &&
            ( oid1.data4[0] ==  oid2.data4[0]) &&
            ( oid1.data4[1] ==  oid2.data4[1]) &&
            ( oid1.data4[2] ==  oid2.data4[2]) &&
            ( oid1.data4[3] ==  oid2.data4[3]) &&
            ( oid1.data4[4] ==  oid2.data4[4]) &&
            ( oid1.data4[5] ==  oid2.data4[5]) &&
            ( oid1.data4[6] ==  oid2.data4[6]) &&
            ( oid1.data4[7] ==  oid2.data4[7]));
}

/**
* @class    IBase
* @brief    IBase Interface
*            This class represents top interface of managed Object's interface\n
*            If any class wants to work as managed Object, must inherit this interface.
*
* @see
*/
class IBase
{
    public:
        /**
        * @fn     queryInterface
        * @brief Query if requested interface support calling instance
        *
        * @param [in] const OID& interfaceID - Interface Id looking for
        * @param [out] IBase** ppObject - Interface pointer holds the object
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT queryInterface(IN const OID &interfaceID, OUT IBase **ppObject) = 0;

        /**
        * @fn     addRef
        * @brief Add reference counter, returns current counter value
        *
        * @param NONE
        *
        * @return unsigned long
        * @warning
        * @exception
        * @see
        */
        virtual unsigned long addRef() = 0;

        /**
        * @fn     release
        * @brief Release reference counter, returns current counter value and self destroyed if zero
        *
        * @param NONE
        *
        * @return unsigned long
        * @warning
        * @exception
        * @see
        */
        virtual unsigned long release() = 0;
        virtual ~IBase() {};
};

class CObjectMultiThreadModel;
/**
* @class    CObjectRoot
* @brief    CObjectRoot Interface
*            This class represents top class of managed Object\n
*            If any class wants to work as managed Object, must inherit this class.
*
* @see
*/
template <class ThreadModel = CObjectMultiThreadModel>
class CObjectRoot
{
    private:
        volatile unsigned long m_dwRef;

    public:
        CObjectRoot()
        {
            m_dwRef = 0;
        }

        ~CObjectRoot()
        {
        }

        /**
        * @fn     internalAddRef
        * @brief Add reference counter, returns current counter value
        *
        * @param NONE
        *
        * @return unsigned long
        * @warning
        * @exception
        * @see
        */
        unsigned long internalAddRef()
        {
            return ThreadModel::increment(&m_dwRef);
        }

        /**
        * @fn     internalRelease
        * @brief Release reference counter, returns current counter value
        *
        * @param NONE
        *
        * @return unsigned long
        * @warning
        * @exception
        * @see
        */
        unsigned long internalRelease()
        {
            return ThreadModel::decrement(&m_dwRef);
        }
};

/**
* @class    CObject
* @brief    CObject Interface
*            This class provides a way to declare instance of managed Object\n
*            If user wants to declare instance of managed Object, must use this class.
*
* @see
*/
template <class Base>
class CObject :
    public Base
{
    public:
        CObject()
        {
        }

        virtual ~CObject()
        {
        }

        /**
        * @fn     addRef
        * @brief Add reference counter, returns current counter value
        *
        * @param NONE
        *
        * @return unsigned long
        * @warning
        * @exception
        * @see
        */
        unsigned long addRef()
        {
            return this->internalAddRef();
        }

        /**
        * @fn     release
        * @brief Release reference counter, returns current counter value and self destroyed if zero
        *
        * @param NONE
        *
        * @return unsigned long
        * @warning
        * @exception
        * @see
        */
        unsigned long release()
        {
            unsigned long ref = this->internalRelease();
            if (ref == 0)
            {
                this->finalRelease();
                delete this;
            }
            return ref;
        }

        /**
        * @fn createInstance
        * @brief Create instance of current template class
        *
        * @param [out] CObject<Base>** pp - reference pointer to get instance pointer
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        static SSMRESULT createInstance(CObject<Base> **pp)
        {
            SSMRESULT res = SSM_E_FAIL;

            if (pp == NULL)
            {
                return SSM_E_POINTER;
            }

            *pp = NULL;

            CObject<Base> *p = new CObject<Base>();

            if (p == NULL)
            {
                return SSM_E_OUTOFMEMORY;
            }

            res = p->finalConstruct();

            if (res != SSM_S_OK)
            {
                delete p;
                p = NULL;
                return res;
            }

            *pp = p;

            return SSM_S_OK;
        }
};


/**
* @class    _noAddRefReleaseOnCObjectPtr
* @brief    _noAddRefReleaseOnCObjectPtr Interface
*            This class used for CObject's Smart pointer mechanism. Internal use only
*
* @see
*/
template <class T>
class _noAddRefReleaseOnCObjectPtr :
    public T
{
    private:
        virtual unsigned long addRef() = 0;
        virtual unsigned long release() = 0;
};

/**
* @class    CObjectPtr
* @brief    CObjectPtr Interface
*            This class used for declaring CObject's interface to work as Smart Pointer
*
* @see
*/
template <class T>
class CObjectPtr
{
    private:
        T *p;
    public:
        CObjectPtr()
        {
            p = NULL;
        }

        ~CObjectPtr()
        {
            SAFE_RELEASE(p);
        }

        _noAddRefReleaseOnCObjectPtr<T> *operator->() const
        {
            return (_noAddRefReleaseOnCObjectPtr<T> *)p;
        }

        operator T *() const
        {
            return p;
        }

        T **operator&()
        {
            return &p;
        }
        /*
        CObjectPtr(T* lp)
        {
            p = lp;
            if(p != NULL)
            p->AddRef();
        }

        CObjectPtr(const CObjectPtr<T>& lp)
        :CObjectPtr(lp.p)
        {
        }

        T& operator*() const
        {
            return *p;
        }

        T* operator=(T* lp)
        {
            return *this;
        }

        T* operator=(CObjectPtr<T>& lp)
        {
            return *this;
        }

        bool operator!() const
        {
            return (p == NULL);
        }

        bool operator<(T* pT) const
        {
            return !operator==(pT);
        }

        bool operator==(T* pT) const
        {
            return p == pT;
        }
        */
};

/**
* @fn CreateNewObject
* @brief Create instance of declared template
*
* @param [in] const OID& interfaceID - Interface Id looking for
* @param [out] IBase** ppObject - reference pointer to get instance pointer
* @return SSMRESULT
* @warning
* @exception
* @see
*/
template <class T>
SSMRESULT CreateNewObject(IN const OID &objectID, OUT IBase **ppObject)
{
    SSMRESULT res = SSM_E_OUTOFMEMORY;

    CObject<T> *p = new CObject<T>();

    if (p == NULL)
    {
        goto END;
    }

    res = p->finalConstruct();

    if (res != SSM_S_OK)
    {
        delete p;
        p = NULL;
        goto END;
    }

    res = p->queryInterface(objectID, ppObject);

END:
    return res;
}

#endif
