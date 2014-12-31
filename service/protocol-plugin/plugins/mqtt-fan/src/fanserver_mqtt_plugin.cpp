#include "fanserver_mqtt_plugin.h"
#include "fanserver.h"
#include <cstdlib>
#include <pthread.h>                            // 2

//int start_fanserver(void*);                   // 1
void *start_fanserver(void *d);     // 2

typedef struct plugin_data_t plugin_data_t;

struct plugin_data_t
{
    cp_context_t *ctx;
    pthread_t m_thread;                                 // 2
    void *str;
    bool flag;
};


#ifdef __cplusplus
extern "C"
{
#endif

static void *create(cp_context_t *ctx)
{
    plugin_data_t *data = (plugin_data_t *)malloc(sizeof(plugin_data_t));

    if (data != NULL)
    {
        data->ctx = ctx;
        data->flag = true;
    }
    else
    {
        printf("[MQTT] ERROR: Plug-in Data - NULL\n");
    }

    return data;
}

static int start(void *d)
{
    plugin_data_t *data = (plugin_data_t *)d;

    //data->str = (void *)cp_resolve_symbol(data->ctx, "fanserver_mqtt_plugin", "START_ARGUMENT", NULL);

    //cp_run_function(data->ctx, (cp_run_func_t)start_fanserver);           // 1
    pthread_create(&(data->m_thread), NULL, start_fanserver, data);         // 2
    return 0;
}

static void stop(void *d)
{
    printf("function_stop\n");
    plugin_data_t *data = (plugin_data_t *)d;

    data->flag = false;
    //cp_release_symbol(data->ctx, data->str);
    pthread_join(data->m_thread, (void **)NULL);
}

static void destroy(void *d)
{
    printf("function_destroy\n");
    plugin_data_t *data = (plugin_data_t *)d;
    free(data);
}

CP_EXPORT cp_plugin_runtime_t mqtt_plugin_fanserver_funcs =
{
    create,
    start,
    stop,
    destroy
};

#ifdef __cplusplus
}
#endif
