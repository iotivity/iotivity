import React from 'react';
import kind from '@enact/core/kind';

import {Panel, Header} from '@enact/moonstone/Panels';
import Divider from '@enact/moonstone/Divider';

import Server from './Server';
import Client from './Client';

import {Layout, Cell} from '@enact/ui/Layout';

const MainPanel = kind({
    name: 'MainPanel',
    render: () => (
        <Panel>
            <Header title="iotivity sampler" type="compact" />
            <Layout align="start">
                <Cell>
                    <Divider casing="word">Server role</Divider>
                    <Server />
                </Cell>
                <Cell>
                    <Divider casing="word">Client role</Divider>
                    <Client />
                </Cell>
            </Layout>
        </Panel>
    )
});

export default MainPanel;
