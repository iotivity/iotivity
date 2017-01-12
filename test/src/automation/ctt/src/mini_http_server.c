/******************************************************************
 *
 * Copyright 2016 Granite River Labs All Rights Reserved.
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
#include <pthread.h>
#include <mini_http_server.h>
#include <libuv/uv.h>

static uv_loop_t s_loop;
static uv_async_t s_stop_async;
static uv_thread_t s_server_thread_id;
static char* s_addr;
static int s_port;
static HttpRequestCb s_on_http_request;
static AfterHttpRequestCb s_on_after_http_request;

struct req_data_s
{
    uv_tcp_t* tcp;
    uv_buf_t buf;
};

typedef struct req_data_s req_data_t;

static void OnAlloc(uv_handle_t* client, size_t suggested_size, uv_buf_t* buf)
{
    buf->base = (char*)malloc(suggested_size);
    buf->len = suggested_size;
}

static void FreeWorkReq(uv_work_t* req, int status)
{
    free(req);
}

static char* CreateResponse(char* body)
{
    char rsp_template[] = "HTTP/1.1 200 OK\r\n" \
                       "Content-Type: text/xml; charset=iso-8859-1\r\n" \
                       "Content-Length: %d\r\n" \
                       "Connection: close\r\n" \
                       "\r\n" \
                       "%s";
    char* rsp = 0;

    if (body)
    {
        rsp = (char*)malloc(sizeof(char) * (strlen(body) + strlen(rsp_template) + 16));
        sprintf(rsp, rsp_template, strlen(body), body);
    }
    else
    {
        rsp = (char*)malloc(sizeof(char) * (strlen("") + strlen(rsp_template) + 16));
        sprintf(rsp, rsp_template, strlen(""), "");
    }

    return rsp;
}

static char* GetMethod(uv_buf_t* req)
{
    char* end = strstr(req->base, " ");
    size_t size = end - req->base;
    char* ret = (char*)malloc(sizeof(char) * (size + 1));

    strncpy(ret, req->base, size);
    ret[size] = 0;

    return ret;
}

static int GetContentLength(uv_buf_t* req)
{
    int ret = -1;

    char* cl = strstr(req->base, "Content-Length:");
    char* rn = strstr(req->base, "\r\n\r\n");

    if (cl == 0) goto error;
    if (cl > rn) goto error;

    sscanf(cl, "Content-Length: %d", &ret);

    return ret;

error:
    return -1;
}

static int CalcContentLength(uv_buf_t* req)
{
    char* rn = strstr(req->base, "\r\n\r\n");
    int header_len = 0;

    if (rn == 0) goto error;

    header_len = rn - req->base + 4;

    return req->len - header_len;

error:
    return -1;
}

static char* GetUrl(uv_buf_t* req)
{
    char* begin = strstr(req->base, " ") + 1;
    char* end = strstr(begin, " ");
    size_t size = end - begin;
    char* ret = (char*)malloc(sizeof(char) * (size + 1));

    strncpy(ret, begin, size);
    ret[size] = 0;

    return ret;
}

static char* GetBody(uv_buf_t* req)
{
    char* begin = 0;
    char* end = 0;
    char* ret = 0;
    size_t size = 0;

    begin = strstr(req->base, "\r\n\r\n");
    if (!begin) goto error;
    begin += 4;

    end = req->base + req->len;
    size = end - begin;

    if (size > 0)
    {
        ret = (char*)malloc(sizeof(char) * (size + 1));

        strncpy(ret, begin, size);
        ret[size] = 0;
    }

    return ret;

error:
    free(ret);

    return 0;
}

void DeleteHandle(uv_handle_t* handle)
{
    free(handle);
}

void FinishRequest(uv_write_t* req, int status)
{
    req_data_t* req_data = (req_data_t*)req->data;

    uv_close((uv_handle_t*) req_data->tcp, DeleteHandle);

    free(req_data->buf.base);
    free(req_data);
    free(req);
}

static void ProcessRequest(uv_work_t* req)
{
    req_data_t* req_data = (req_data_t*) req->data;
    uv_write_t* write_req = 0;
    char* rsp_body = 0;
    char* rsp = 0;

    char* req_method = GetMethod(&req_data->buf);
    char* req_url = GetUrl(&req_data->buf);
    char* req_body = GetBody(&req_data->buf);

    if (s_on_http_request)
    {
        rsp_body = s_on_http_request(req_method, req_url, req_body);
    }

    rsp = CreateResponse(rsp_body);

    free(req_data->buf.base);

    req_data->buf.base = rsp;
    req_data->buf.len = strlen(rsp);

    write_req = (uv_write_t*)malloc(sizeof(uv_write_t));
    write_req->data = req_data;

    uv_write(write_req, (uv_stream_t*) req_data->tcp, &req_data->buf, 1, FinishRequest);

    if (s_on_after_http_request)
    {
        s_on_after_http_request(rsp_body);
    }

    free(req_body);
    free(req_url);
    free(req_method);
}

static int IsRequestComplete(uv_buf_t* buf)
{
    int gcl = GetContentLength(buf);
    int ccl = CalcContentLength(buf);
    char* rnrn = strstr(buf->base, "\r\n\r\n");
    int ret = 0;

    if (rnrn)
    {
        if (gcl < 0)
        {
            ret = 1;
        }
        else if (gcl == ccl)
        {
            ret = 1;
        }
    }

    return ret;
}

static char* ConcatString(char* str1, size_t len1, char* str2, size_t len2)
{
    char* ret = (char*)malloc(sizeof(char) * (len1 + len2 + 1));

    strncpy(ret, str1, len1);
    strncpy(ret + len1, str2, len2);
    ret[len1 + len2] = 0;

    return ret;
}

static void OnRead(uv_stream_t* tcp, ssize_t nread, const uv_buf_t* buf)
{
    if (nread >= 0)
    {
        uv_work_t* req;
        req_data_t* req_data;
        uv_buf_t* tcp_buf = 0;
        char* new_base = 0;
        int new_len = 0;

        if (tcp->data == 0)
        {
            tcp->data = malloc(sizeof(uv_buf_t));
            ((uv_buf_t*)tcp->data)->len = 0;
            ((uv_buf_t*)tcp->data)->base = 0;
        }

        tcp_buf = (uv_buf_t*)tcp->data;
        new_len = tcp_buf->len + nread;
        new_base = ConcatString(tcp_buf->base, tcp_buf->len, buf->base, nread);

        free(tcp_buf->base);
        tcp_buf->base = new_base;
        tcp_buf->len = new_len;

        if (IsRequestComplete(tcp_buf))
        {
            req = (uv_work_t*) malloc(sizeof(uv_work_t));
            req_data = (req_data_t*) malloc(sizeof(req_data_t));

            req_data->tcp = (uv_tcp_t*)tcp;
            req_data->buf.base = tcp_buf->base;
            req_data->buf.len = tcp_buf->len;
            req->data = (void*)req_data;

            free(tcp_buf);
            tcp->data = 0;

            uv_queue_work(&s_loop, req, ProcessRequest, FreeWorkReq);
        }
    }
    else if (nread != UV__EOF)
    {
        if (tcp->data)
        {
            free(((uv_buf_t*)tcp->data)->base);
            free(tcp->data);
        }

        uv_close((uv_handle_t*) tcp, DeleteHandle);
    }

    free(buf->base);
}

static void OnNewConnection(uv_stream_t *server, int status)
{
    uv_tcp_t *client;

    if (status == -1) {
        return;
    }

    client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    client->data = 0;
    uv_tcp_init(&s_loop, client);

    if (uv_accept(server, (uv_stream_t*) client) == 0)
    {
        uv_read_start((uv_stream_t*) client, OnAlloc, OnRead);
    }
    else
    {
        uv_close((uv_handle_t*) client, DeleteHandle);
    }
}

static void StopMiniHttpServerAsync(uv_async_t* handle)
{
    uv_stop(&s_loop);
}

static void MiniHttpServerThread(void *arg)
{
    uv_tcp_t server;
    struct sockaddr_in bind_addr;

    uv_loop_init(&s_loop);
    uv_tcp_init(&s_loop, &server);
    uv_async_init(&s_loop, &s_stop_async, StopMiniHttpServerAsync);

    uv_ip4_addr(s_addr, s_port, &bind_addr);
    uv_tcp_bind(&server, (const struct sockaddr*)&bind_addr, 0);

    uv_listen((uv_stream_t*) &server, 128, OnNewConnection);
    uv_run(&s_loop, UV_RUN_DEFAULT);

    uv_close((uv_handle_t*)&server, 0);
}

void StopMiniHttpServer()
{
    uv_async_send(&s_stop_async);
    uv_thread_join(&s_server_thread_id);

    free(s_addr);
    s_addr = 0;
    s_port = 0;
    s_on_http_request = 0;
}

void StartMiniHttpServer(const char* addr, int port, HttpRequestCb on_http_request, AfterHttpRequestCb on_after_http_request)
{
    int addr_len = strlen(addr);
    s_addr = (char*) malloc(sizeof(char) * (addr_len + 1));
    strcpy(s_addr, addr);
    s_addr[addr_len] = 0;
    s_port = port;
    s_on_http_request = on_http_request;
    s_on_after_http_request = on_after_http_request;

    uv_thread_create(&s_server_thread_id, MiniHttpServerThread, 0);
}
