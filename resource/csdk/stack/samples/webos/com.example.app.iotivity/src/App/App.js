import kind from '@enact/core/kind';
import MoonstoneDecorator from '@enact/moonstone/MoonstoneDecorator';
import Panels from '@enact/moonstone/Panels';
import React from 'react';

import MainPanel from '../views/MainPanel';

import css from './App.less';

const App = kind({
    name: 'App',

    styles: {
        css,
        className: 'app'
    },

    render: (props) => (
        <div {...props}>
            <Panels>
                <MainPanel />
            </Panels>
        </div>
    )
});

export default MoonstoneDecorator(App);
