import * as actions from './ActionTypes';
import LS2Request from '@enact/webos/LS2Request';

const ls2instances = {};

export const LS2RequestSingleton = {
    instance: function (skey, preventRepeat) {
        if (preventRepeat && ls2instances[skey]) {
            return false;
        }

        ls2instances[skey] = ls2instances[skey] || new LS2Request();
        return ls2instances[skey];
    },
    deleteInstance: function (skey) {
        ls2instances[skey] = null;
    }
};

export function actionUpdateServerState(tf) {
    return {
        type: actions.UPDATE_SERVER,
        payload: { "enabled": tf }
    };
}

export function actionUpdateResourceValue(result) {
    return {
        type: actions.RESOURCE_CONTROL_UI,
        payload: { "state": result.value }
    };
}

export function actionUpdateDiscoveredList(res) {
    if (!res) {
        setTimeout(function () {
            const ls = LS2RequestSingleton.instance('discoverResources');
            if (ls) {
                ls.cancel();
                LS2RequestSingleton.deleteInstance('discoverResources');
            }
        }, 5000, null)
        return {
            type: actions.SHOW_RESOURCES_LIST,
            payload: res
        };
    }
    else {
        return {
            type: actions.UPDATE_DISCOVERED_RESOURCES,
            payload: res
        };
    }
}

export function actionResetDiscoveredList() {
    return {
        type: actions.RESET_DISCOVERED_RESOURCES,
    };
}

export function actionEnableServerResourceControlUI() {
    return {
        type: actions.SHOW_SERVER_RESOURCE_CONTROL_UI,
    };
}

export function actionDisableServerResourceControlUI() {
    return {
        type: actions.HIDE_SERVER_RESOURCE_CONTROL_UI,
    };
}

export function actionEnableClientResourceControlUI() {
    return {
        type: actions.SHOW_CLIENT_RESOURCE_CONTROL_UI,
    };
}

export function actionDisableClientResourceControlUI() {
    return {
        type: actions.HIDE_CLIENT_RESOURCE_CONTROL_UI,
    };
}

export function actionUpdateGetResourceResults(result) {
    return {
        type: actions.UPDATE_GET_RESOURCE_RESULTS,
        payload: result
    };
}

export function actionUpdateObserveResourceResults(result) {
    return {
        type: actions.UPDATE_OBSERVE_RESOURCE_RESULTS,
        payload: result
    };
}

export function actionCancelObserveResourceResults() {
    return {
        type: actions.CANCEL_OBSERVE_RESOURCE,
    };
}

export function actionRestartServer(value) {
    return {
        type: actions.RESTART_SERVER,
        payload: { state: value }
    };
}


export function actionRestartClient(value) {
    return {
        type: actions.RESTART_CLIENT,
        payload: { state: value }
    };
}

export const createToast = params => {
    return new LS2Request().send({
        service: 'luna://com.webos.notification/',
        method: 'createToast',
        parameters: { noaction: true, message: params },
        onComplete: (res) => {
            console.log(res);
            return;
        }
    });
};

export const startServer = (dispatch) => {
    const ls = LS2RequestSingleton.instance('startServer', true);
    if (ls) {
        ls.send({
            service: 'luna://com.example.service.iotivity.server/',
            method: 'startServer',
            parameters: {
                subscribe: true
            },
            onComplete: (res) => {
                if (res.subscribed) {
                    dispatch(actionUpdateServerState(true));
                    return;
                }
            },
        });
    }
};

export const stopServer = (dispatch) => {
    const ls = LS2RequestSingleton.instance('startServer');
    if (ls) {
        ls.cancel();
        dispatch(actionUpdateServerState(false));
        let param = { "value": false };
        dispatch(actionUpdateResourceValue(param));
        LS2RequestSingleton.deleteInstance('startServer');
        createToast("Server stopped.");
    }
};

export const startClient = (dispatch) => {
    const ls = LS2RequestSingleton.instance('startClient', true);
    if (ls) {
        ls.send({
            service: 'luna://com.example.service.iotivity.client/',
            method: 'startClient',
            parameters: {
                subscribe: true
            },
            onComplete: (res) => {
                if (res.subscribed) {
                    dispatch(actionEnableClientResourceControlUI());
                    return;
                }
            },
        });
    }
};

export const stopClient = (dispatch) => {
    const ls = LS2RequestSingleton.instance('startClient');
    if (ls) {
        ls.cancel();
        LS2RequestSingleton.deleteInstance('startClient');
        createToast("Client stopped.");
    }
    dispatch(actionEnableClientResourceControlUI());
};

export const discoverResources = (dispatch) => {
    const ls = LS2RequestSingleton.instance('discoverResources', true);
    console.log('discoverResources');
    if (ls) {
        ls.send({
            service: 'luna://com.example.service.iotivity.client/',
            method: 'discoverResources',
            parameters: {},
            onComplete: (res) => {
                console.log(res.discoveryResponse);
                dispatch(actionUpdateDiscoveredList(res.discoveryResponse));
                if (res.discoveryResponse.length === 0) {
                    createToast("Resource not found.");
                }
            },
        });
    }
};

export const resetDiscoveredList = (dispatch) => {
    const ls = LS2RequestSingleton.instance('discoverResources');
    const ls2 = LS2RequestSingleton.instance('observeResource');
    if (ls) {
        ls.cancel();
        dispatch(actionResetDiscoveredList([]));
        LS2RequestSingleton.deleteInstance('discoverResources');
    }
    if (ls2) {
        ls2.cancel();
        dispatch(actionResetDiscoveredList([]));
        LS2RequestSingleton.deleteInstance('observeResource');
    }
};

export const createResource = params => dispatch => {
    console.log('createResource');
    return new LS2Request().send({
        service: 'luna://com.example.service.iotivity.server/',
        method: 'createResource',
        parameters: params,
        onComplete: (res) => {
            if (res.returnValue) {
                createToast(params.uri + " resource is ready");
                console.log(res);
                dispatch(actionEnableServerResourceControlUI());
                return;
            }
        }
    });
};

export const deleteResource = params => dispatch => {
    console.log('deleteServerResource');
    return new LS2Request().send({
        service: 'luna://com.example.service.iotivity.server/',
        method: 'deleteResource',
        parameters: params,
        onComplete: (res) => {
            if (res.returnValue) {
                createToast(params.uri + " resource is deleted");
                console.log(res);
                dispatch(actionEnableServerResourceControlUI());
                return;
            }
        }
    });
};

export const getResource = params => dispatch => {
    console.log('getResource');
    return new LS2Request().send({
        service: 'luna://com.example.service.iotivity.client/',
        method: 'getResource',
        parameters: params,
        onComplete: (res) => {
            console.log(res);
            if (res.response.payload) {
                createToast("Success to get resource");
                dispatch(actionUpdateGetResourceResults(res.response));
            }
            else {
                createToast("Fail to get resource");
                dispatch(actionEnableClientResourceControlUI());
            }
            return;
        }
    });
};

export const putResource = params => dispatch => {
    console.log('putResource');
    return new LS2Request().send({
        service: 'luna://com.example.service.iotivity.client/',
        method: 'putResource',
        parameters: params,
        onComplete: (res) => {
            console.log(res);
            if (res.returnValue && res.response.payload) {
                createToast("Success to put resource");
                dispatch(actionUpdateGetResourceResults(res.response));
            }
            else {
                createToast("Fail to put resource");
                dispatch(actionEnableClientResourceControlUI());
            }
            return;
        }
    });
};

export const postResource = params => dispatch => {
    console.log('postResource');
    return new LS2Request().send({
        service: 'luna://com.example.service.iotivity.client/',
        method: 'postResource',
        parameters: params,
        onComplete: (res) => {
            console.log(res);
            if (res.returnValue && res.response.payload) {
                createToast("Success to post resource");
                dispatch(actionUpdateGetResourceResults(res.response));
            }
            else {
                createToast("Fail to post resource");
                dispatch(actionEnableClientResourceControlUI());
            }
            return;
        }
    });
};

export const requestDeleteResource = params => dispatch => {
    console.log('requestDeleteResource');
    return new LS2Request().send({
        service: 'luna://com.example.service.iotivity.client/',
        method: 'deleteResource',
        parameters: params,
        onComplete: (res) => {
            if (res.returnValue) {
                createToast(params.uri + " resource is deleted");
                console.log(res);
                dispatch(actionDisableClientResourceControlUI());
                return;
            }
        }
    });
};

export const observeResource = params => dispatch => {
    const ls = LS2RequestSingleton.instance('observeResource', true);
    if (ls) {
        ls.send({
            service: 'luna://com.example.service.iotivity.client/',
            method: 'observeResource',
            parameters: {
                subscribe: true,
                uri: params.uri,
                eps: params.eps,
                destination: params.destination
            },
            onComplete: (res) => {
                console.log(res);
                if (res.response.payload) {
                    dispatch(actionUpdateObserveResourceResults(res.response));
                }
                else {
                    createToast("Fail to observe resource");
                    const ls2 = LS2RequestSingleton.instance('observeResource');
                    if (ls2) {
                        ls2.cancel();
                        LS2RequestSingleton.deleteInstance('observeResource');
                    }
                    dispatch(actionEnableClientResourceControlUI());
                }
                return;
            }
        });
    }
};

export const stopObserveResource = (dispatch) => {
    const ls = LS2RequestSingleton.instance('observeResource');
    if (ls) {
        ls.cancel();
        dispatch(actionCancelObserveResourceResults());
        createToast("Cancel observe resource");
        LS2RequestSingleton.deleteInstance('observeResource');
    }
};

export const setBinarySwitchValue = params => dispatch => {
    console.log('setBinarySwitchValue');
    createToast("/binarySwitch value changed to " + params.value);
    dispatch(actionUpdateResourceValue(params));
    return new LS2Request().send({
        service: 'luna://com.example.service.iotivity.server/',
        method: 'setBinarySwitchValue',
        parameters: {
            value: params.value
        },
        onComplete: (res) => {
            console.log(res);
            return;
        }
    });
};

export const observeBinarySwitchValue = (dispatch) => {
    console.log("observeBinarySwitchValue");
    const ls = LS2RequestSingleton.instance('observeBinarySwitchValue', true);
    if (ls) {
        ls.send({
            service: 'luna://com.example.service.iotivity.server/',
            method: 'observeBinarySwitchValue',
            parameters: {
                subscribe: true
            },
            onComplete: (res) => {
                if (res) {
                    dispatch(actionUpdateResourceValue(res.response));
                }
                return;
            }
        });
    }
};

export const stopObserveBinarySwitchValue = (dispatch) => {
    console.log("stopObserveBinarySwitchValue");
    const ls = LS2RequestSingleton.instance('observeBinarySwitchValue');
    if (ls) {
        ls.cancel();
        LS2RequestSingleton.deleteInstance('observeBinarySwitchValue');
        let param = { "value": false };
        dispatch(actionUpdateResourceValue(param));
    }
};

export const copyServerCBORFile = params => (dispatch) => {
    console.log("copyServerCBORFile");
    console.log(params);
    dispatch(actionRestartServer(false));
    return new LS2Request().send({
        service: 'luna://com.example.service.iotivity.server/',
        method: 'copyFile',
        parameters: params,
        onComplete: (res) => {
            console.log(res);
            if (res.returnValue === true) {
                createToast("File copy completed");
            }
            else {
                createToast("File copy failed");
            }
            dispatch(actionRestartServer(true));
            return;
        }
    });
};

export const copyClientCBORFile = params => (dispatch) => {
    console.log("copyClientCBORFile");
    console.log(params);
    dispatch(actionRestartClient(false));
    return new LS2Request().send({
        service: 'luna://com.example.service.iotivity.client/',
        method: 'copyFile',
        parameters: params,
        onComplete: (res) => {
            console.log(res);
            if (res.returnValue === true) {
                createToast("File copy completed");
            }
            else {
                createToast("File copy failed");
            }
            dispatch(actionRestartClient(true));
            return;
        }
    });
};