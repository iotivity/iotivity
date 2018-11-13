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
        };

        this.onCreateResourceTapped = this.createResource.bind(this);
        this.onResourceValueTapped = this.setBinarySwitchValue.bind(this);
        this.onStopServerTapped = this.stopServer.bind(this);
    }
    componentWillUnmount() {
        this.props.stopServer();
    }
    createResource() {
        this.props.startServer();
        let param = {
            uri: this.state.resourceUri,
            types: this.state.resourceType,
            observable: this.state.resourceObservable
        }
        this.props.actionDisableServerResourceControlUI();
        this.props.createResource(param);
        this.setState({
            resourceUri: "/binaryswitch",
            resourceType: "oic.r.switch.binary",
            resourceObservable: true,
            resourceUpdateCompleted: true,
        });
    }
    deleteResource() {
        let param = {
            uri: this.state.resourceUri,
        }
        this.props.actionDisableServerResourceControlUI();
        this.props.deleteResource(param);
        this.setState({
            resourceUri: "/binaryswitch",
            resourceType: "oic.r.switch.binary",
            resourceObservable: true,
            resourceUpdateCompleted: true,
        });
    }
    setBinarySwitchValue() {
        let param = {
            value: !this.props.resourceValue,
        };
        this.props.setBinarySwitchValue(param);
    }
    stopServer() {
        this.props.stopServer();
    }
    render() {
        const { serverEnabled, enableServerResourceControlUI, resourceValue } = this.props;
        return (
            <div>
                <Layout align="start">
                    <Cell shrink>
                        {serverEnabled ?
                            <Button small onClick={this.onStopServerTapped}>stop server</Button>
                            : <Button small onClick={this.onCreateResourceTapped}>start server</Button>
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
                            : null}
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
    deleteResource: PropTypes.func,
    actionDisableServerResourceControlUI: PropTypes.func,
    setBinarySwitchValue: PropTypes.func,
};
const mapDispatchToProps = (dispatch) => {
    return {
        startServer: () => ActionCreators.startServer(dispatch),
        stopServer: () => ActionCreators.stopServer(dispatch),
        createResource: (params) => dispatch(ActionCreators.createResource(params)),
        deleteResource: (params) => dispatch(ActionCreators.deleteResource(params)),
        actionDisableServerResourceControlUI: () => dispatch(ActionCreators.actionDisableServerResourceControlUI()),
        setBinarySwitchValue: (params) => dispatch(ActionCreators.setBinarySwitchValue(params)),
    };
};
let mapStateToProps = (state) => {
    return {
        serverEnabled: state.serverEnabled,
        discoveredResources: state.discoveredResources,
        showDiscoveredResources: state.showDiscoveredResources,
        enableServerResourceControlUI: state.enableServerResourceControlUI,
        resourceValue: state.resourceValue
    };
};
export default connect(mapStateToProps, mapDispatchToProps)(Server);
