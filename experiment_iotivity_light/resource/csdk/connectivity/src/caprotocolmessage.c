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

// Defining _BSD_SOURCE or _DEFAULT_SOURCE causes header files to expose
// definitions that may otherwise be skipped. Skipping can cause implicit
// declaration warnings and/or bugs and subtle problems in code execution.
// For glibc information on feature test macros,
// Refer http://www.gnu.org/software/libc/manual/html_node/Feature-Test-Macros.html
//
// This file requires #define use due to random() and srandom()
// For details on compatibility and glibc support,
// Refer http://www.gnu.org/software/libc/manual/html_node/BSD-Random.html
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include "oiclogger.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "caprotocolmessage.h"

// ARM GCC compiler doesnt define srandom function.
#if defined(ARDUINO) && !defined(ARDUINO_ARCH_SAM)
#define HAVE_SRANDOM 1
#endif

#define TAG "CA_PRTCL_MSG"

/**
 * @def VERIFY_NON_NULL_RET
 * @brief Macro to verify the validity of input argument
 */
#define VERIFY_NON_NULL_RET(arg, log_tag, log_message,ret) \
    if (NULL == arg ){ \
        OIC_LOG_V(ERROR, log_tag, "Invalid input:%s", log_message); \
        return ret; \
    }

#define CA_BUFSIZE (128)
#define CA_PDU_MIN_SIZE (4)
#define CA_PORT_BUFFER_SIZE (4)

static const char COAP_URI_HEADER[] = "coap://[::]/";

static unsigned int SEED = 0;

int CAGetOptionData(const uint8_t *data, int len, char *option, int buflen);
CAResult_t CAParseHeadOption(int code, const CAInfo_t *info, coap_list_t **optlist);

static coap_pdu_t *CAGeneratePDUImpl(code_t code, coap_list_t *options, const CAMessageInfo_t *messageInfo);

coap_pdu_t *CAGeneratePDU(const CAMessageInfo_t *messageInfo)
{
    CAResult_t res1, res2;
    coap_pdu_t *pdu = NULL;

    int code = messageInfo->info.coapCode;
    int coapType = messageInfo->info.coapType;

    // RESET have to use only 4byte (empty message)
    // and ACKNOWLEDGE can use empty message when code is empty.
    if (OC_MSG_RESET == coapType || (CA_EMPTY == code && OC_MSG_ACKNOWLEDGE == coapType))
    {
        return CAGeneratePDUImpl((code_t)code, NULL, messageInfo);
    }

    coap_list_t *optlist = NULL;

    if (OC_MSG_ACKNOWLEDGE != coapType)
    {
        res1 = CAParseURI(messageInfo->info.resourcePath, COAP_OPTION_URI_PATH, &optlist);
        res2 = CAParseURI(messageInfo->info.resourceQuery, COAP_OPTION_URI_QUERY, &optlist);
        if (CA_STATUS_OK != res1 || CA_STATUS_OK != res2)
        {
            goto error;
        }
    }
    // parsing options in HeadOption
    CAResult_t ret = CAParseHeadOption(code, &(messageInfo->info), &optlist);
    if (CA_STATUS_OK != ret)
    {
        goto error;
    }

    pdu = CAGeneratePDUImpl((code_t)code, optlist, messageInfo);
    if (NULL == pdu)
    {
        OIC_LOG(ERROR, TAG, "pdu NULL");
        goto error;
    }

error:
    coap_delete_list(optlist);

    return pdu;
}

coap_pdu_t *CAParsePDU(const char *data, int length)
{
    coap_pdu_t *outpdu = coap_new_pdu();
    if (NULL == outpdu)
    {
        OIC_LOG(ERROR, TAG, "outpdu is null");
        return NULL;
    }

    if (0 >= coap_pdu_parse((unsigned char *) data, length, outpdu))
    {
        OIC_LOG(ERROR, TAG, "pdu parse failed");
        coap_delete_pdu(outpdu);
        return NULL;
    }

    return outpdu;
}

coap_pdu_t *CAGeneratePDUImpl(code_t code, coap_list_t *options, const CAMessageInfo_t *messageInfo)
{
    const CAInfo_t *info = &(messageInfo->info);

    coap_pdu_t *pdu = coap_new_pdu();

    uint16_t message_id = info->messageId;
    OIC_LOG_V(DEBUG, TAG, "msgID is %d", message_id);
    if (!message_id)
    {
        prng((uint8_t * )&message_id, sizeof (message_id));
        OIC_LOG_V(DEBUG, TAG, "gen msg id=%d", message_id);
    }
    pdu->hdr->id = message_id;
    OIC_LOG_V(DEBUG, TAG, "messageId in pdu is %d, %d", message_id, pdu->hdr->id);

    pdu->hdr->type = info->coapType;
    pdu->hdr->code = COAP_RESPONSE_CODE(code);

    if (CA_EMPTY != code)
    {
        int len = info->token.size;
        OIC_LOG_V(DEBUG, TAG, "info token length: %d, token: %lx", len, info->token.t.value);
        int ret = coap_add_token(pdu, len, info->token.t.chars);
        if (0 == ret)
        {
            OIC_LOG(ERROR, TAG, "can't add token");
        }
    }

    if (options)
    {
        for (coap_list_t *opt = options; opt; opt = opt->next)
        {
            OIC_LOG_V(DEBUG, TAG, "[%s] opt will be added.",
                      COAP_OPTION_DATA(*(coap_option *) opt->data));
            coap_add_option(pdu, COAP_OPTION_KEY(*(coap_option *)opt->data),
                            COAP_OPTION_LENGTH(*(coap_option *)opt->data),
                            COAP_OPTION_DATA(*(coap_option *)opt->data));
        }
    }

    if (messageInfo->encodedSize && messageInfo->encodedPayload)
    {
        OIC_LOG(DEBUG, TAG, "payload is added");
        coap_add_data(pdu, messageInfo->encodedSize, messageInfo->encodedPayload);
    }

    return pdu;
}

CAResult_t CAParseURI(const char *uri, int target, coap_list_t **optlist)
{
    const char *c = uri;
    if (target == COAP_OPTION_URI_PATH && *c == '/') {
        c++;
    }
    const char *s = c;

    for (; *c != '\0'; c++) {
        if (target == COAP_OPTION_URI_PATH) {
            switch (*c) {
                case '/':
                case '?':
                case '\0':
                    break;
                default:
                    continue;
            }
        } else {  // target == COAP_OPTION_URI_QUERY
            switch (*c) {
                case '&':
                case ';':
                case '\0':
                    break;
                default:
                    continue;
            }
        }

        coap_list_t *node = CACreateNewOptionNode(target, c - s, (const uint8_t *)s);

        if (coap_insert(optlist, node, CAOrderOpts) <= 0)
            return CA_STATUS_INVALID_PARAM;

        s = c + 1;
    }

    return CA_STATUS_OK;
}

CAResult_t CAParseHeadOption(int code, const CAInfo_t *info, coap_list_t **optlist)
{
    (void)code;

    if (info->observationOption != OC_OBSERVE_NO_OPTION)
    {
        coap_list_t *node = CACreateNewOptionNode(COAP_OPTION_OBSERVE, sizeof (int32_t),
                                        (uint8_t *)&(info->observationOption));
        int ret = coap_insert(optlist, node, CAOrderOpts);
        if (ret <= 0)
        {
            return CA_STATUS_INVALID_PARAM;
        }
    }

    for (int i = 0; i < info->numOptions; i++)
    {
        const OCHeaderOption *ho = info->options + i;

        int id = ho->optionID;
        if (COAP_OPTION_URI_PATH == id || COAP_OPTION_URI_QUERY == id)
        {
            continue;
        }
        if (!ho->optionData || ho->optionLength <= 0)
        {
            continue;
        }
        OIC_LOG_V(DEBUG, TAG, "Head opt ID: %d", id);
        OIC_LOG_V(DEBUG, TAG, "Head opt data: %s", ho->optionData);
        OIC_LOG_V(DEBUG, TAG, "Head opt length: %d", ho->optionLength);

        coap_list_t *node = CACreateNewOptionNode(id, ho->optionLength, ho->optionData);
        int ret = coap_insert(optlist, node, CAOrderOpts);
        if (ret <= 0)
        {
            return CA_STATUS_INVALID_PARAM;
        }
    }

    return CA_STATUS_OK;
}

coap_list_t *CACreateNewOptionNode(uint16_t key, int length, const uint8_t *data)
{
    coap_option *option = coap_malloc(sizeof(coap_option) + length + 1);
    memset(option, 0, sizeof(coap_option) + length + 1);

    COAP_OPTION_KEY(*option) = key;
    COAP_OPTION_LENGTH(*option) = length;
    memcpy(COAP_OPTION_DATA(*option), data, length);

    /* we can pass NULL here as delete function since option is released automatically  */
    coap_list_t *node = coap_new_listnode(option, NULL);
    if (!node)
    {
        OIC_LOG(ERROR, TAG, "node is NULL");
        coap_free(option);
        return NULL;
    }

    return node;
}

int CAOrderOpts(void *a, void *b)
{
    if (COAP_OPTION_KEY(*(coap_option *) a) < COAP_OPTION_KEY(*(coap_option * ) b))
    {
        return -1;
    }

    return COAP_OPTION_KEY(*(coap_option *) a) == COAP_OPTION_KEY(*(coap_option * ) b);
}

CAResult_t CAGetMessageInfoFromPDU(const coap_pdu_t *pdu, CAMessageInfo_t *messageInfo)
{
    coap_opt_iterator_t opt_iter;
    coap_option_iterator_init((coap_pdu_t *)pdu, &opt_iter, COAP_OPT_ALL);

    CAInfo_t *info = &(messageInfo->info);

    info->numOptions = 0;
    info->coapType = pdu->hdr->type;
    info->messageId = pdu->hdr->id;
    info->observationOption = OC_OBSERVE_NO_OPTION;
    info->coapCode = CA_RESPONSE_CODE(pdu->hdr->code);

    coap_opt_t *option;
    char *path = info->resourcePath;
    char *query = info->resourceQuery;
    int idx = 0;
    size_t resultLength = 0;

    while ((option = coap_option_next(&opt_iter)))
    {
        int type = opt_iter.type;
        char *optBuf = (char *)COAP_OPT_VALUE(option);
        size_t optLength = COAP_OPT_LENGTH(option);

        switch (type)
        {
        case COAP_OPTION_URI_PATH:
            if ((resultLength + 1 + optLength) >= sizeof (info->resourcePath))
            {
                OIC_LOG(ERROR, TAG, "resource path too big");
                return CA_STATUS_FAILED;
            }

            *path++ = '/';

            memcpy(path, optBuf, optLength);
            path += optLength;
            *path = '\0'; // in case this is the last
            break;
        case COAP_OPTION_URI_QUERY:
            if ((resultLength + 1 + optLength) >= sizeof (info->resourceQuery))
            {
                OIC_LOG(ERROR, TAG, "resource query too big");
                return CA_STATUS_FAILED;
            }

            if (query != info->resourceQuery)
                *query++ = ';';

            memcpy(query, optBuf, optLength);
            query += optLength;
            *query = '\0'; // in case this is the last
            break;
        case COAP_OPTION_OBSERVE:
            info->observationOption = optBuf[0];
            break;
        case COAP_OPTION_BLOCK1:
        case COAP_OPTION_BLOCK2:
        case COAP_OPTION_SIZE1:
        case COAP_OPTION_SIZE2:
            OIC_LOG_V(DEBUG, TAG, "option[%d] is ignored", type);
            break;
        default:
            if (idx < MAX_HEADER_OPTIONS && optLength <= MAX_HEADER_OPTION_DATA_LENGTH)
            {
                info->options[idx].protocolID = OC_COAP_ID;
                info->options[idx].optionID = type;
                info->options[idx].optionLength = optLength;
                memcpy(info->options[idx++].optionData, optBuf, optLength);
            }
            break;
        }
    }

    memcpy(info->token.t.chars, pdu->hdr->token, pdu->hdr->token_length);
    info->token.size = pdu->hdr->token_length;

    // we point at the encoded payload in the pdu.  No alloc, no copy.
    coap_get_data(pdu, &(messageInfo->encodedSize), &(messageInfo->encodedPayload));
    return CA_STATUS_OK;
}

void CAGenerateToken(OCToken_t *token, int tokenLength)
{
    if (!caglobals.ca.randomSeeded)
    {
        caglobals.ca.randomSeeded = true;
#ifdef ARDUINO
        uint32_t seed = now();    
#else
        uint32_t seed = time(NULL);    
#endif
        if (seed == (uint32_t)((time_t)-1))
        {
            OIC_LOG(ERROR, TAG, "seed is not made");
        }
#if HAVE_SRANDOM
        srandom(SEED);
#else
        srand(SEED);
#endif
    }

    token->t.value = 0;
    for (int index = 0; index < tokenLength; index++)
    {
#ifdef ARDUINO
        token->t.chars[index] = rand() & 0x00FF;
#else
        token->t.chars[index] = random() & 0x00FF;
#endif
    }
    token->size = tokenLength;
}
