#include "SSMInterface.h"
#include "SSMInterface/SSMCore.h"
#include "Common/PlatformLayer.h"

class SSMCoreEventReceiver : public IQueryEngineEvent
{
    public:
        SSMCoreEventReceiver()
        {
        }

        SSMRESULT onQueryEngineEvent(IN int cqid, IN IDataReader *pResult)
        {
            SSMRESULT res = SSM_E_FAIL;

            m_mtxListener.lock();

            if (m_mapListener.find(cqid) == m_mapListener.end())
            {
                SSM_CLEANUP_ASSERT(res);
            }

            m_mapListener[cqid]->onQueryEngineEvent(cqid, pResult);

            res = SSM_S_OK;

CLEANUP:
            m_mtxListener.unlock();
            return res;
        }

        void lockListener()
        {
            m_mtxListener.lock();
        }

        void unlockListener()
        {
            m_mtxListener.unlock();
        }

        void addListener(IN int cqid, IN IQueryEngineEvent *pEngineEvent)
        {
            m_mapListener[cqid] = pEngineEvent;
        }

        void removeListener(IN int cqid)
        {
            m_mapListener.erase(cqid);
        }

    private:
        CSimpleMutex                        m_mtxListener;
        std::map<int, IQueryEngineEvent *>   m_mapListener;
};

IQueryEngine                        *g_pQueryEngineInstance = NULL;
SSMCoreEventReceiver                *g_pEventReceiver = NULL;

SSMInterface::SSMInterface()
{
    std::string xmlDescription = "<SSMCore>"
                                 "<Device>"
                                 "<UDN>abcde123-31f8-11b4-a222-08002b34c003</UDN>"
                                 "<Name>MyPC</Name>"
                                 "<Type>PC</Type>"
                                 "</Device>"
                                 "</SSMCore>";

    SSMRESULT res = SSM_E_FAIL;

    g_pEventReceiver = new SSMCoreEventReceiver();
    SSM_CLEANUP_NULL_ASSERT(g_pEventReceiver);
    SSM_CLEANUP_ASSERT(InitializeSSMCore(xmlDescription));
    SSM_CLEANUP_ASSERT(StartSSMCore());
    SSM_CLEANUP_ASSERT(CreateQueryEngine(&g_pQueryEngineInstance));
    SSM_CLEANUP_ASSERT(g_pQueryEngineInstance->registerQueryEvent(g_pEventReceiver));
CLEANUP:
    ;
}

SSMInterface::~SSMInterface()
{
    SSMRESULT res = SSM_E_FAIL;

    SSM_CLEANUP_ASSERT(g_pQueryEngineInstance->unregisterQueryEvent(g_pEventReceiver));
    ReleaseQueryEngine(g_pQueryEngineInstance);
    g_pQueryEngineInstance = NULL;
    SSM_CLEANUP_ASSERT(StopSSMCore());
    SSM_CLEANUP_ASSERT(TerminateSSMCore());

CLEANUP:
    SAFE_DELETE(g_pEventReceiver);
}

SSMRESULT SSMInterface::registerQuery(IN std::string queryString, IN IQueryEngineEvent *listener,
                                      IN int &cqid)
{
    SSMRESULT res = SSM_E_FAIL;

    g_pEventReceiver->lockListener();
    SSM_CLEANUP_ASSERT(g_pQueryEngineInstance->executeContextQuery(queryString, &cqid));
    g_pEventReceiver->addListener(cqid, listener);

CLEANUP:
    g_pEventReceiver->unlockListener();
    return res;
}

SSMRESULT SSMInterface::unregisterQuery(IN int cqid)
{
    SSMRESULT res = SSM_E_FAIL;

    g_pEventReceiver->lockListener();
    SSM_CLEANUP_ASSERT(g_pQueryEngineInstance->killContextQuery(cqid));
    g_pEventReceiver->removeListener(cqid);

CLEANUP:
    g_pEventReceiver->unlockListener();
    return res;
}