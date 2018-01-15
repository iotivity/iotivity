import * as actions from './../actions/ActionTypes';

const initialState = {
    serverEnabled:false,
    discoveredResources:null,
    showDiscoveredResources:false,
    enableServerResourceControlUI:true,
    enableClientResourceControlUI:true,
    detailResourceInfo:null,
};

let arr=[];

export function reducer (state = initialState, action) {
    switch (action.type) {
        case actions.UPDATE_SERVER:
            return Object.assign({}, state, {serverEnabled: action.payload.enabled});
        case actions.UPDATE_DISCOVERED_RESOURCES:
            if(action.payload.addr){
                arr.push(action.payload);
                return Object.assign({}, state, {discoveredResources: arr});
            }
            return state;
        case actions.SHOW_RESOURCES_LIST:
            return Object.assign({}, state, {showDiscoveredResources: true});
        case actions.RESET_DISCOVERED_RESOURCES:
            arr=[];
            return Object.assign({}, state, {showDiscoveredResources: false, discoveredResources: null, detailResourceInfo:null});
        case actions.HIDE_SERVER_RESOURCE_CONTROL_UI:
            return Object.assign({}, state, {enableServerResourceControlUI: false});
        case actions.SHOW_SERVER_RESOURCE_CONTROL_UI:
            return Object.assign({}, state, {enableServerResourceControlUI: true});
        case actions.HIDE_CLIENT_RESOURCE_CONTROL_UI:
            return Object.assign({}, state, {enableClientResourceControlUI: false});
        case actions.SHOW_CLIENT_RESOURCE_CONTROL_UI:
            return Object.assign({}, state, {enableClientResourceControlUI: true, detailResourceInfo:null});
        case actions.UPDATE_GET_RESOURCE_RESULTS:
        case actions.UPDATE_OBSERVE_RESOURCE_RESULTS:
            return Object.assign({}, state, {detailResourceInfo: action.payload.payload});
        default:
            return state;
    }
}

export default reducer;
