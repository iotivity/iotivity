import React from 'react';
import {render} from 'react-dom';
import {createStore, applyMiddleware, compose} from 'redux';
import {Provider} from 'react-redux';
import reducer from './reducer/Reducer';
import thunkMiddleware from 'redux-thunk';

import App from './App';

const composeEnhancers = (typeof window === 'object' && window.__REDUX_DEVTOOLS_EXTENSION_COMPOSE__) || compose;

function configureStore (initialState) {
    const ret = createStore(
        reducer,
        initialState,
        composeEnhancers(
            applyMiddleware(thunkMiddleware)
        )
    );

    return ret;
}

const store = configureStore();

let appElement = (
    <Provider store={store}>
        <App />
    </Provider>
);

// In a browser environment, render instead of exporting
if (typeof window !== 'undefined') {
    render(
        appElement,
        document.getElementById('root')
    );
}

export default appElement;
