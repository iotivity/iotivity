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

export function actionUpdateServerState (tf) {
    return {
        type: actions.UPDATE_SERVER,
        payload: {"enabled":tf}
    };
}

export function actionUpdateDiscoveredList (res) {
    if(!res.addr){
        setTimeout(function() {
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
    else{
        return {
            type: actions.UPDATE_DISCOVERED_RESOURCES,
            payload: res
        };
    }
}

export function actionResetDiscoveredList () {
    return {
        type: actions.RESET_DISCOVERED_RESOURCES,
    };
}

export function actionEnableServerResourceControlUI(){
    return {
        type: actions.SHOW_SERVER_RESOURCE_CONTROL_UI,
    };
}

export function actionDisableServerResourceControlUI(){
    return {
        type: actions.HIDE_SERVER_RESOURCE_CONTROL_UI,
    };
}

export function actionEnableClientResourceControlUI(){
    return {
        type: actions.SHOW_CLIENT_RESOURCE_CONTROL_UI,
    };
}

export function actionDisableClientResourceControlUI(){
    return {
        type: actions.HIDE_CLIENT_RESOURCE_CONTROL_UI,
    };
}

export function actionUpdateGetResourceResults(result){
    return {
        type: actions.UPDATE_GET_RESOURCE_RESULTS,
        payload: result
    };
}

export function actionUpdateObserveResourceResults(result){
    return {
        type: actions.UPDATE_OBSERVE_RESOURCE_RESULTS,
        payload: result
    };
}

export const createToast = params => {
    console.log('createToast');
    return new LS2Request().send({
        service: 'luna://com.webos.notification/',
        method: 'createToast',
        parameters:{noaction:true, message:params},
        onComplete: (res) => {
            console.log(res);
            return;
        }
    });
};

export const startServer = (dispatch) =>  {
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

export const stopServer = (dispatch) =>  {
    const ls = LS2RequestSingleton.instance('startServer');
    if (ls) {
        ls.cancel();
        dispatch(actionUpdateServerState(false));
        dispatch(actionResetDiscoveredList([]));
        LS2RequestSingleton.deleteInstance('startServer');
    }
};

export const discoverResources = (dispatch) =>  {
    const ls = LS2RequestSingleton.instance('discoverResources', true);
    if (ls) {
        ls.send({
            service: 'luna://com.example.service.iotivity.client/',
            method: 'discoverResources',
            parameters: {
                subscribe: true
            },
            onComplete: (res) => {
                dispatch(actionUpdateDiscoveredList(res.discoveryResponse));
            },
        });
    }
};

export const resetDiscoveredList = (dispatch) =>  {
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
        parameters:params,
        onComplete: (res) => {
            if(res.returnValue){
                createToast(params.uri +" resource is ready");
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
        parameters:params,
        onComplete: (res) => {
            if(res.returnValue){
                createToast(params.uri +" resource is deleted");
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
        parameters:params,
        onComplete: (res) => {
            console.log(res);
            if(res.response.payload){
                dispatch(actionUpdateGetResourceResults(res.response));
            }
            else{
                createToast("Fail to get resource");
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
        parameters:params,
        onComplete: (res) => {
            if(res.returnValue){
                createToast(params.uri +" resource is deleted");
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
                subscribe:true,
                uri:params.uri,
                destination:params.destination
            },
            onComplete: (res) => {
                console.log(res);
                if(res.response.payload){
                    dispatch(actionUpdateGetResourceResults(res.response));
                }
                else{
                    createToast("Fail to observe resource");
                    dispatch(actionEnableClientResourceControlUI());
                }
                return;
            }
        });
    }
};

export const stopObserveResource = () =>  {
    const ls = LS2RequestSingleton.instance('observeResource');
    if (ls) {
        ls.cancel();
        LS2RequestSingleton.deleteInstance('observeResource');
    }
};
