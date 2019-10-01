import React from 'react';
import { connect } from 'react-redux';
import PropTypes from 'prop-types';

import { MarqueeText } from '@enact/moonstone/Marquee';
import Button from '@enact/moonstone/Button';
import Divider from '@enact/moonstone/Divider';

import * as ActionCreators from '../actions/ActionCreators';

import { Layout, Cell } from '@enact/ui/Layout';

class Server extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            resourceUri: "/binaryswitch",
            resourceType: "oic.r.switch.binary",
            resourceUpdateCompleted: true,
            resourceObservable: true,
            serverStarted:false,
        };

        this.onStartServerTapped = this.startServer.bind(this);
        this.onResourceValueTapped = this.setBinarySwitchValue.bind(this);
        this.onStopServerTapped = this.stopServer.bind(this);
        this.onRFOTMTapped = this.onRFOTMChanged.bind(this);
        this.onRFNOPTapped = this.onRFNOPChanged.bind(this);
    }
    componentWillUnmount() {
        this.props.stopObserveBinarySwitchValue();
        this.props.stopServer();
    }
    componentWillReceiveProps(props) {
        if (props.serverEnabled && !this.state.serverStarted)
        {
            this.setState({
                serverStarted:true,
            });
            let param = {
                uri: this.state.resourceUri,
                types: this.state.resourceType,
                observable: this.state.resourceObservable
            }
            this.props.createResource(param);
            this.setState({
                resourceUri: "/binaryswitch",
                resourceType: "oic.r.switch.binary",
                resourceObservable: true,
                resourceUpdateCompleted: true,
            });
            this.props.observeBinarySwitchValue();
        }
    }
    startServer() {
        this.props.actionDisableServerResourceControlUI();
        this.props.startServer();
    }
    setBinarySwitchValue() {
        let param = {
            value: !this.props.resourceValue,
        };
        this.props.setBinarySwitchValue(param);
    }
    stopServer() {
        this.props.stopObserveBinarySwitchValue();
        this.props.stopServer();
        this.setState({
            serverStarted:false,
        });
    }
    onRFOTMChanged() {
        let param = {
            mode: "RFOTM"
        }
        this.stopServer();
        this.props.copyServerCBORFile(param);
    }
    onRFNOPChanged() {
        let param = {
            mode: "RFNOP"
        }
        this.stopServer();
        this.props.copyServerCBORFile(param);
    }
    render() {
        const { serverEnabled, enableServerResourceControlUI, resourceValue, serverStartable } = this.props;
        return (
            <div>
                <div style={{ "paddingLeft": 20 + "px", "paddingTop": 9 + "px" }}>
                    <div style={{ "width": 600 + "px", "display": "inline-block", "verticalAlign": "middle" }}>
                        <p>Change mode</p>
                    </div>
                    <Button small onClick={this.onRFOTMTapped}>RFOTM</Button>
                    <Button small onClick={this.onRFNOPTapped}>RFNOP</Button>
                </div>
                <Layout align="start">
                    <Cell shrink>
                        {serverEnabled ?
                            <Button small onClick={this.onStopServerTapped}>stop server</Button>
                            : <Button disabled={!serverStartable} small onClick={this.onStartServerTapped}>start server</Button>
                        }
                    </Cell>
                    <Cell>
                        {serverEnabled ?
                            <div style={{ "lineHeight": 60 + "px" }}>
                                <MarqueeText marqueeOn="render">Server is running</MarqueeText>
                            </div>
                            : null
                        }
                    </Cell>
                </Layout>
                <Layout align="start">
                    <Cell>
                        <Divider />
                        {(serverEnabled && enableServerResourceControlUI) ?
                            <div>
                                <div style={{ "paddingLeft": 20 + "px" }}>
                                    <MarqueeText marqueeOn="render">Resource: binarySwitch</MarqueeText>
                                    <MarqueeText marqueeOn="render">Type: oic.r.switch.binary</MarqueeText>
                                    <MarqueeText marqueeOn="render">Observable: true</MarqueeText>
                                </div>
                                <div style={{ "paddingLeft": 20 + "px", "paddingTop": 9 + "px" }}>
                                    <div style={{ "width": 600 + "px", "display": "inline-block", "verticalAlign": "middle" }}>
                                        <MarqueeText marqueeOn="render">value</MarqueeText>
                                    </div>
                                    <Button small onClick={this.onResourceValueTapped}>{resourceValue ? "true" : "false"}</Button>
                                </div>
                            </div>
                            : null
                        }
                    </Cell>
                </Layout>
            </div>
        )
    }
}
Server.propTypes = {
    startServer: PropTypes.func,
    stopServer: PropTypes.func,
    createResource: PropTypes.func,
    actionDisableServerResourceControlUI: PropTypes.func,
    setBinarySwitchValue: PropTypes.func,
};
const mapDispatchToProps = (dispatch) => {
    return {
        startServer: () => ActionCreators.startServer(dispatch),
        stopServer: () => ActionCreators.stopServer(dispatch),
        createResource: (params) => dispatch(ActionCreators.createResource(params)),
        actionDisableServerResourceControlUI: () => dispatch(ActionCreators.actionDisableServerResourceControlUI()),
        setBinarySwitchValue: (params) => dispatch(ActionCreators.setBinarySwitchValue(params)),
        observeBinarySwitchValue: () => ActionCreators.observeBinarySwitchValue(dispatch),
        stopObserveBinarySwitchValue: () => ActionCreators.stopObserveBinarySwitchValue(dispatch),
        copyServerCBORFile: (params) => dispatch(ActionCreators.copyServerCBORFile(params))
    };
};
let mapStateToProps = (state) => {
    return {
        serverEnabled: state.serverEnabled,
        discoveredResources: state.discoveredResources,
        showDiscoveredResources: state.showDiscoveredResources,
        enableServerResourceControlUI: state.enableServerResourceControlUI,
        resourceValue: state.resourceValue,
        serverStartable: state.serverStartable
    };
};
export default connect(mapStateToProps, mapDispatchToProps)(Server);
