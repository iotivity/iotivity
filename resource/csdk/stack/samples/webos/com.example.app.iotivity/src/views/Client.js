import React from 'react';
import { connect } from 'react-redux';
import PropTypes from 'prop-types';

import { MarqueeText } from '@enact/moonstone/Marquee';
import Button from '@enact/moonstone/Button';
import Divider from '@enact/moonstone/Divider';
import CheckboxItem from '@enact/moonstone/CheckboxItem';
import BodyText from '@enact/moonstone/BodyText';
import SpotlightContainerDecorator from '@enact/moonstone/Scroller';
import ExpandableInput from '@enact/moonstone/ExpandableInput';

import * as ActionCreators from '../actions/ActionCreators';

import Changeable from '@enact/ui/Changeable';
import { Group } from '@enact/ui/Group';
import { Layout } from '@enact/ui/Layout';

const SelectableGroup = Changeable({ mutable: true }, Group);
let discoveredItems = [];
let discoveredItemDatas = [];

class Client extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            selectedDiscoveredItemIndex: null,
            resourceUri: null,
            eps: null,
            resourceKey: null,
            resourceValue: null,
            requestResourceCompleted: true,
            isPostable: false,
        };
        this.onToggleHandler = this.onSelectedResource.bind(this);
        this.onToggleData = this.onSelectedData.bind(this);
        this.onResourceKeyChanged = this.resourceKeyChanged.bind(this);
        this.onResourceValueChanged = this.resourceValueChanged.bind(this);
        this.onGetResourceTapped = this.getResource.bind(this);
        this.onPostResourceTapped = this.postResource.bind(this);
        this.onDeleteResourceTapped = this.requestDeleteResource.bind(this);
        this.onObserveResourceTapped = this.requestObserveResource.bind(this);
        this.onCancelObserveResourceTapped = this.requestCancelObserveResource.bind(this);
        this.onEnablePostResourceTapped = this.setPostResourceButton.bind(this);
    }
    componentWillReceiveProps(props) {
        console.log(props);
        if (props.discoveredResources && props.discoveredResources.length > 0) {
            discoveredItems = [];
            for (let i = 0; i < props.discoveredResources.length; i++) {
                discoveredItems.push({
                    value: props.discoveredResources[i],
                    children: props.discoveredResources[i].addr.addr + " (port:" + props.discoveredResources[i].addr.port + ")",
                });
            }
        }
        else {
            discoveredItems = [];
            discoveredItemDatas = [];
            if (this.props.isObserving) {
                this.props.stopObserveResource();
            }
            this.setState({
                resourceUri: null,
                eps: null,
                resourceKey: null,
                resourceValue: null,
                resourceCompleted: true,
                selectedDiscoveredItemIndex: null,
                isPostable: false,
            });
        }
    }
    resourceUriChanged(ev) {
        this.setState({
            resourceUri: ev.value,
            eps: ev.eps,
            requestResourceCompleted: false,
        });
    }
    resourceKeyChanged(ev) {
        this.setState({
            resourceKey: ev.value,
            requestResourceCompleted: false,
        });
    }
    resourceValueChanged(ev) {
        this.setState({
            resourceValue: ev.value,
            requestResourceCompleted: false,
        });
    }
    getResource() {
        let param = {
            uri: this.state.resourceUri,
            eps: this.state.eps,
            destination: this.state.selectedDiscoveredItemIndex.addr
        }
        this.props.getResource(param);
        this.setState({
            requestResourceCompleted: true
        });
    }
    postResource() {
        let tempValue = (this.state.resourceValue === "true" || this.state.resourceValue === "false") ?
            this.state.resourceValue === "true" : this.state.resourceValue;

        let param = {
            uri: this.state.resourceUri,
            eps: this.state.eps,
            destination: this.state.selectedDiscoveredItemIndex.addr,
            key: this.state.resourceKey,
            value: tempValue
        }
        this.props.postResource(param);
        this.setState({
            requestResourceCompleted: true,
            isPostable: false
        });
    }
    requestDeleteResource() {
        let param = {
            uri: this.state.resourceUri,
            eps: this.state.eps,
            destination: this.state.selectedDiscoveredItemIndex.addr
        }
        this.props.requestDeleteResource(param);
        this.setState({
            resourceUri: null,
            resourceKey: null,
            resourceValue: null,
            resourceCompleted: true,
        });
        this.props.actionDisableClientResourceControlUI();
    }
    requestObserveResource() {
        let param = {
            uri: this.state.resourceUri,
            eps: this.state.eps,
            destination: this.state.selectedDiscoveredItemIndex.addr,
        }
        this.props.requestObserveResource(param);
        this.setState({
            resourceCompleted: true,
        });
    }
    requestCancelObserveResource() {
        this.props.stopObserveResource();
        this.setState({
            resourceCompleted: true,
        });
    }
    onSelectedResource(ev) {
        console.log(ev);
        discoveredItemDatas = [];
        for (let j = 0; j < this.props.discoveredResources[ev.selected].payload.resources.length; j++) {
            discoveredItemDatas.push({
                value: this.props.discoveredResources[ev.selected].payload.resources[j].uri,
                eps: this.props.discoveredResources[ev.selected].payload.resources[j].eps,
                children: "uri: " + this.props.discoveredResources[ev.selected].payload.resources[j].uri +
                    " (if:" + this.props.discoveredResources[ev.selected].payload.resources[j].interfaces + ")",
            });
        }
        this.setState({
            selectedDiscoveredItemIndex: this.props.discoveredResources[ev.selected],
        });
        this.props.actionEnableClientResourceControlUI();
    }
    onSelectedData(ev) {
        this.setState({
            resourceUri: discoveredItemDatas[ev.selected].value,
            eps: discoveredItemDatas[ev.selected].eps,
            requestResourceCompleted: false,
        });
    }
    setPostResourceButton() {
        this.setState((prevState) => ({
            isPostable: !prevState.isPostable,
        }));
    }
    render() {
        const { showDiscoveredResources, detailResourceInfo, isObserving } = this.props;
        return (
            <div>
                <Layout align="start">
                    {showDiscoveredResources ?
                        <SpotlightContainerDecorator focusableScrollbar style={{ "height": 210 + "px" }} direction="both" horizontalScrollbar="auto" verticalScrollbar="auto">
                            <SelectableGroup
                                childComponent={CheckboxItem}
                                onSelect={this.onToggleHandler}
                                selectedProp="selected"
                                select="radio">
                                {discoveredItems}
                            </SelectableGroup>
                            <div style={{ "paddingTop": 9 + "px" }} >
                                <Button small onClick={this.props.resetDiscoveredList}>OK</Button>
                            </div>

                        </SpotlightContainerDecorator>
                        : <Button small onClick={this.props.discoverResources}>Discover Resources</Button>
                    }
                </Layout>
                <Divider />
                <Layout align="start">
                    <SpotlightContainerDecorator focusableScrollbar style={{ "height": 200 + "px" }} direction="both" horizontalScrollbar="auto" verticalScrollbar="auto">
                        {discoveredItemDatas ?
                            <SelectableGroup
                                childComponent={CheckboxItem}
                                onSelect={this.onToggleData}
                                selectedProp="selected"
                                select="radio">
                                {discoveredItemDatas}
                            </SelectableGroup>
                            : null}
                    </SpotlightContainerDecorator>
                </Layout>
                <Layout align="start">
                    <SpotlightContainerDecorator focusableScrollbar style={{ "height": 400 + "px" }} direction="both" horizontalScrollbar="auto" verticalScrollbar="auto">
                        <div style={{ "paddingTop": 9 + "px" }}>
                            <Button style={{ lineHeight: 25 + "px" }} disabled={!this.state.resourceUri} small onClick={this.onGetResourceTapped}>get<br />resource</Button>
                            <Button style={{ lineHeight: 25 + "px" }} disabled={!this.state.resourceUri} small onClick={this.onDeleteResourceTapped}>delete<br />resource</Button>
                            {isObserving ?
                                <Button style={{ lineHeight: 25 + "px" }} disabled={!this.state.resourceUri} small onClick={this.onCancelObserveResourceTapped}>cancel<br />observe</Button> :
                                <Button style={{ lineHeight: 25 + "px" }} disabled={!this.state.resourceUri} small onClick={this.onObserveResourceTapped}>observe<br />resource</Button>
                            }
                        </div>
                        <div style={{ "paddingTop": 9 + "px" }}>
                            <Button style={{ lineHeight: 25 + "px" }} disabled={!this.state.resourceUri} small onClick={this.onEnablePostResourceTapped}>post<br />resource</Button>
                        </div>
                        {this.state.isPostable ?
                            <div style={{ "paddingTop": 9 + "px" }}>
                                <ExpandableInput title="key" onChange={this.onResourceKeyChanged} />
                                <ExpandableInput title="value" onChange={this.onResourceValueChanged} />
                                <Button style={{ lineHeight: 25 + "px" }} disabled={!this.state.resourceUri || !this.state.resourceKey || !this.state.resourceValue} small onClick={this.onPostResourceTapped}>send</Button>
                            </div> : null}
                        {detailResourceInfo ?
                            <div>
                                <MarqueeText marqueeOn="render">return</MarqueeText>
                                <SpotlightContainerDecorator focusableScrollbar style={{ "height": 400 + "px" }} direction="both" horizontalScrollbar="auto" verticalScrollbar="auto">
                                    <BodyText>{JSON.stringify(detailResourceInfo, null, 4)}</BodyText>
                                </SpotlightContainerDecorator>
                            </div> : null}
                    </SpotlightContainerDecorator>
                </Layout>
            </div>
        )
    }
}
Client.propTypes = {
    getResource: PropTypes.func,
    requestDeleteResource: PropTypes.func,
    requestObserveResource: PropTypes.func,
    stopObserveResource: PropTypes.func,
    discoverResources: PropTypes.func,
    resetDiscoveredList: PropTypes.func,
    actionDisableCreateResourceUI: PropTypes.func,
};
const mapDispatchToProps = (dispatch) => {
    return {
        getResource: (params) => dispatch(ActionCreators.getResource(params)),
        postResource: (params) => dispatch(ActionCreators.postResource(params)),
        requestDeleteResource: (params) => dispatch(ActionCreators.requestDeleteResource(params)),
        requestObserveResource: (params) => dispatch(ActionCreators.observeResource(params)),
        stopObserveResource: () => ActionCreators.stopObserveResource(dispatch),
        discoverResources: () => ActionCreators.discoverResources(dispatch),
        resetDiscoveredList: () => ActionCreators.resetDiscoveredList(dispatch),
        actionEnableClientResourceControlUI: () => dispatch(ActionCreators.actionEnableClientResourceControlUI()),
        actionDisableClientResourceControlUI: () => dispatch(ActionCreators.actionDisableClientResourceControlUI()),
        copyClientCBORFile: (params) => dispatch(ActionCreators.copyClientCBORFile(params)),
    };
};
let mapStateToProps = (state) => {
    return {
        discoveredResources: state.discoveredResources,
        showDiscoveredResources: state.showDiscoveredResources,
        detailResourceInfo: state.detailResourceInfo,
        enableClientResourceControlUI: state.enableClientResourceControlUI,
        isObserving: state.isObservingResource
    };
};
export default connect(mapStateToProps, mapDispatchToProps)(Client);