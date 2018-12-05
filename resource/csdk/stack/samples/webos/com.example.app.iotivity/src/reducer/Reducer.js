import * as actions from './../actions/ActionTypes';

const initialState = {
    serverEnabled: false,
    discoveredResources: null,
    showDiscoveredResources: false,
    enableServerResourceControlUI: true,
    enableClientResourceControlUI: true,
    detailResourceInfo: null,
    resourceValue: false,
    isObservingResource: false,
    serverRestarted: false,
    clientRestarted: false,
};

let arr = [];

export function reducer(state = initialState, action) {
    switch (action.type) {
        case actions.UPDATE_SERVER:
            return Object.assign({}, state, { serverEnabled: action.payload.enabled });
        case actions.UPDATE_DISCOVERED_RESOURCES:
            console.log('arr.length: ' + arr.length);
            return Object.assign({}, state, { discoveredResources: action.payload, showDiscoveredResources: true });
        case actions.SHOW_RESOURCES_LIST:
            return Object.assign({}, state, { showDiscoveredResources: true });
        case actions.RESET_DISCOVERED_RESOURCES:
            arr = [];
            return Object.assign({}, state, { showDiscoveredResources: false, discoveredResources: null, detailResourceInfo: null });
        case actions.HIDE_SERVER_RESOURCE_CONTROL_UI:
            return Object.assign({}, state, { enableServerResourceControlUI: false });
        case actions.SHOW_SERVER_RESOURCE_CONTROL_UI:
            return Object.assign({}, state, { enableServerResourceControlUI: true });
        case actions.HIDE_CLIENT_RESOURCE_CONTROL_UI:
            return Object.assign({}, state, { enableClientResourceControlUI: false, discoveredResources: null });
        case actions.SHOW_CLIENT_RESOURCE_CONTROL_UI:
            return Object.assign({}, state, { enableClientResourceControlUI: true, detailResourceInfo: null });
        case actions.UPDATE_GET_RESOURCE_RESULTS:
            return Object.assign({}, state, { detailResourceInfo: action.payload.payload });
        case actions.UPDATE_OBSERVE_RESOURCE_RESULTS:
            return Object.assign({}, state, { detailResourceInfo: action.payload.payload, isObservingResource: true });
        case actions.CANCEL_OBSERVE_RESOURCE:
            return Object.assign({}, state, { isObservingResource: false });
        case actions.RESOURCE_CONTROL_UI:
            return Object.assign({}, state, { resourceValue: action.payload.state });
        case actions.RESTART_SERVER:
            return Object.assign({}, state, { serverRestarted: true });
        case actions.STOP_RESTART_SERVER:
            return Object.assign({}, state, { serverRestarted: false });
        case actions.RESTART_CLIENT:
            return Object.assign({}, state, { clientRestarted: true });
        case actions.STOP_RESTART_CLIENT:
            return Object.assign({}, state, { clientRestarted: false });
        default:
            return state;
    }
}

export default reducer;