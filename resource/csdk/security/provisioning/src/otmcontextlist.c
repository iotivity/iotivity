#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "octypes.h"
#include "ownershiptransfermanager.h"
#include "utlist.h"
#include "otmcontextlist.h"

#define TAG "OTM_CTX_LIST"

/**
 * List for saving the OTMContext to be used while ownership transfer.
 */
static OTMContextItem_t* g_otmCtxList = NULL;

void RemoveOTMContext(const char* addr, uint16_t port)
{
    OTMContext_t* retCtx = NULL;

    OIC_LOG(DEBUG, TAG, "IN RemoveOTMContext");

    if (NULL != addr && 0 != port)
    {
        OTMContextItem_t* item = NULL;
        OTMContextItem_t* temp = NULL;

        LL_FOREACH_SAFE(g_otmCtxList, item, temp)
        {
            if (strncmp(addr, item->endpoint.addr, sizeof(item->endpoint.addr)) == 0 &&
                port == item->endpoint.port)
            {
                OIC_LOG_V(DEBUG, TAG, "Remove [%s:%d]'s context from OTMContext list", addr, port);
                retCtx = item->otmCtx;
                item->otmCtx = NULL;
                LL_DELETE(g_otmCtxList, item);
                OICFree(item);
                break;
            }
        }
    }

    OIC_LOG(DEBUG, TAG, "OUT RemoveOTMContext");
}

OCStackResult AddOTMContext(OTMContext_t* ctx, const char* addr, uint16_t port)
{
    OTMContextItem_t* item = NULL;
    OTMContextItem_t* temp = NULL;
    OTMContextItem_t* newItem = NULL;

    OIC_LOG(DEBUG, TAG, "IN AddOTMContext");

    if (NULL == ctx || NULL == addr || 0 == strlen(addr) || 0 == port)
    {
        return OC_STACK_INVALID_PARAM;
    }

    LL_FOREACH_SAFE(g_otmCtxList, item, temp)
    {
            if (strncmp(addr, item->endpoint.addr, sizeof(item->endpoint.addr)) == 0 &&
                port == item->endpoint.port)
            {
                //if OTM Context already exists, just return OC_STACK_OK.
                OIC_LOG(DEBUG, TAG, "Same OTMContext already exists.");
                return OC_STACK_OK;
            }
    }

    newItem = (OTMContextItem_t*)OICCalloc(1, sizeof(OTMContextItem_t));
    if (NULL == newItem)
    {
        OIC_LOG(ERROR, TAG, "Failed to allocate memory.");
        return OC_STACK_NO_MEMORY;
    }

    OIC_LOG_V(DEBUG, TAG, "Add [%s:%d]'s context to OTMContext list", addr, port);

    newItem->otmCtx = ctx;
    OICStrcpy(newItem->endpoint.addr, sizeof(newItem->endpoint.addr), addr);
    newItem->endpoint.port = port;
    LL_APPEND(g_otmCtxList, newItem);

    OIC_LOG(DEBUG, TAG, "OUT AddOTMContext");

    return OC_STACK_OK;
}

const OTMContext_t* GetOTMContext(const char* addr, uint16_t port)
{
    OIC_LOG(DEBUG, TAG, "IN GetOTMContext");

    if (NULL != addr && 0 != port)
    {
        OTMContextItem_t* item = NULL;
        OTMContextItem_t* temp = NULL;

        LL_FOREACH_SAFE(g_otmCtxList, item, temp)
        {
            if (strncmp(addr, item->endpoint.addr, sizeof(item->endpoint.addr)) == 0 &&
               port == item->endpoint.port)
            {
                OIC_LOG_V(DEBUG, TAG, "Found the OTMContext for [%s:%d]", addr, port);
                return item->otmCtx;
            }
        }
    }

    OIC_LOG(DEBUG, TAG, "OUT GetOTMContext");

    return NULL;
}

