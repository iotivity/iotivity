import React from 'react';
import {connect} from 'react-redux';
import PropTypes from 'prop-types';

import Button from '@enact/moonstone/Button';
import Divider from '@enact/moonstone/Divider';
import CheckboxItem from '@enact/moonstone/CheckboxItem';
import BodyText from '@enact/moonstone/BodyText';
import SpotlightContainerDecorator from '@enact/moonstone/Scroller';
import ExpandableInput from '@enact/moonstone/ExpandableInput';

import * as ActionCreators from '../actions/ActionCreators';

import Changeable from '@enact/ui/Changeable';
import {Group} from '@enact/ui/Group';
import {Layout} from '@enact/ui/Layout';

const SelectableGroup = Changeable({mutable: true}, Group);
let discoveredItems=[];

class Client extends React.Component {
    constructor (props) {
        super(props);
        this.state = {
            isSelectDiscoveredItem:false,
            selectedDiscoveredItemIndex:null,
            resourceUri:null,
            resourceQuestion:null,
            requestResourceCompleted:true,
        };
    }
    componentWillReceiveProps (props) {
        console.log(props);
        if(props.discoveredResources&&props.discoveredResources.length>0){
            discoveredItems=[];
            for(let i=0; i<props.discoveredResources.length; i++){
                discoveredItems.push({
                    value:props.discoveredResources[i],
                    children:props.discoveredResources[i].addr.addr+" (port:"+props.discoveredResources[i].addr.port+")",
                });
            }
        }
        else {
            discoveredItems=[];
            this.setState({
                isSelectDiscoveredItem:false,
            });
        }
    }
    resourceUriChanged(ev){
        this.setState({
            resourceUri: ev.value,
            requestResourceCompleted:false,
        });
        console.log(this.state.resourceUri);
    }
    resourceQuestionChanged(ev){
        this.setState({
            resourceQuestion: ev.value,
            requestResourceCompleted:false,
        });
        console.log(this.state.resourceQuestion);
    }
    getResource(){
        let param={
            uri:this.state.resourceUri,
            question:this.state.resourceQuestion,
            destination:this.state.selectedDiscoveredItemIndex.addr
        }
        this.props.actionDisableClientResourceControlUI();
        this.props.getResource(param);
        this.setState({
            resourceUri: null,
            resourceQuestion: null,
            requestResourceCompleted:true,
            isSelectDiscoveredItem:false,
            selectedDiscoveredItemIndex: null,
        });
    }
    requestDeleteResource(){
        let param={
            uri:this.state.resourceUri,
            destination:this.state.selectedDiscoveredItemIndex.addr
        }
        this.props.actionDisableClientResourceControlUI();
        this.props.requestDeleteResource(param);
        this.setState({
            resourceUri: null,
            resourceQuestion: null,
            resourceCompleted:true,
            isSelectDiscoveredItem:false,
            selectedDiscoveredItemIndex: null,
        });
    }
    requestObserveResource(){
        let param={
            uri:this.state.resourceUri,
            destination:this.state.selectedDiscoveredItemIndex.addr,
        }
        this.props.actionDisableClientResourceControlUI();
        this.props.requestObserveResource(param);
        this.setState({
            resourceUri: null,
            resourceQuestion: null,
            resourceCompleted:true,
            isSelectDiscoveredItem:false,
            selectedDiscoveredItemIndex: null,
        });
    }
    onSelectedResource (ev){
        console.log(ev);
        for(let i=0 ; i<this.props.discoveredResources.length; i++){
            if(JSON.stringify(this.props.discoveredResources[i]).indexOf(ev.data.slice(0,30))>0){
                console.log(this.props.discoveredResources[i]);
                this.setState({
                    isSelectDiscoveredItem:true,
                    selectedDiscoveredItemIndex:this.props.discoveredResources[i]
                });
                console.log(this.state.selectedDiscoveredItemIndex);
            }
        }
        this.props.actionEnableClientResourceControlUI();
    }
    render() {
        const {discoveredResources,showDiscoveredResources,enableClientResourceControlUI,detailResourceInfo,...rest} = this.props;
        const onToggleHandler=this.onSelectedResource.bind(this);
        const onResourceUriChanged = this.resourceUriChanged.bind(this);
        const onResourceQuestionChanged = this.resourceQuestionChanged.bind(this);
        const onGetResourceTapped = this.getResource.bind(this);
        const onDeleteResourceTapped = this.requestDeleteResource.bind(this);
        const onObserveResourceTapped = this.requestObserveResource.bind(this);
        return(
            <div>
                <Layout align="start">
                    {showDiscoveredResources?
                    <SpotlightContainerDecorator focusableScrollbar style={{"height":210+"px"}} direction="both" horizontalScrollbar="auto" verticalScrollbar="auto">
                        <SelectableGroup
                            childComponent={CheckboxItem}
                            onSelect={onToggleHandler}
                            selectedProp="selected"
                            select="radio"
                        >
                        {discoveredItems}
                        </SelectableGroup>
                        <div style={{"paddingTop": 9+"px"}} >
                            <Button small onClick={this.props.resetDiscoveredList}>OK</Button>
                        </div>

                    </SpotlightContainerDecorator>
                    :<Button small onClick={this.props.discoverResources}>Discover Resources</Button>
                    }
                </Layout>
                <Divider />
                <Layout align="start">
                    {this.state.isSelectDiscoveredItem&&enableClientResourceControlUI?
                    <SpotlightContainerDecorator focusableScrollbar style={{"height":600+"px"}} direction="both" horizontalScrollbar="auto" verticalScrollbar="auto">
                        <div>{JSON.stringify(this.state.selectedDiscoveredItemIndex.payload,null,4)}</div>
                        <ExpandableInput title="uri" onChange={onResourceUriChanged} />
                        <ExpandableInput title="question" onChange={onResourceQuestionChanged} />
                        <div style={{"paddingTop": 9+"px"}}>
                            <Button disabled={this.state.resourceUri&&this.state.resourceQuestion?false:true} small onClick={onGetResourceTapped}>get resource</Button>
                            <Button disabled={this.state.resourceUri?false:true} small onClick={onDeleteResourceTapped}>delete resource</Button>
                            <Button disabled={this.state.resourceUri?false:true} small onClick={onObserveResourceTapped}>observe resource</Button>
                        </div>
                    </SpotlightContainerDecorator>
                    :null}
                    {detailResourceInfo?
                    <SpotlightContainerDecorator focusableScrollbar style={{"height":600+"px"}} direction="both" horizontalScrollbar="auto" verticalScrollbar="auto">
                        <BodyText>{JSON.stringify(detailResourceInfo,null,4)}</BodyText>
                    </SpotlightContainerDecorator>
                    :null}
                </Layout>
            </div>
        )
    }
}
Client.propTypes = {
    getResource: PropTypes.func,
    requestDeleteResource: PropTypes.func,
    requestObserveResource: PropTypes.func,
    discoverResources: PropTypes.func,
    resetDiscoveredList: PropTypes.func,
    actionDisableCreateResourceUI: PropTypes.func,
};
const mapDispatchToProps = (dispatch) => {
    return {
        getResource: (params) => dispatch(ActionCreators.getResource(params)),
        requestDeleteResource: (params) => dispatch(ActionCreators.requestDeleteResource(params)),
        requestObserveResource: (params) => dispatch(ActionCreators.observeResource(params)),
        discoverResources: () => ActionCreators.discoverResources(dispatch),
        resetDiscoveredList: () => ActionCreators.resetDiscoveredList(dispatch),
        actionDisableClientResourceControlUI: () => dispatch(ActionCreators.actionDisableClientResourceControlUI()),
        actionEnableClientResourceControlUI: () => dispatch(ActionCreators.actionEnableClientResourceControlUI()),
    };
};
let mapStateToProps = (state) => {
    return {
        discoveredResources:state.discoveredResources,
        showDiscoveredResources:state.showDiscoveredResources,
        detailResourceInfo:state.detailResourceInfo,
        enableClientResourceControlUI:state.enableClientResourceControlUI,
    };
};
export default connect(mapStateToProps, mapDispatchToProps)(Client);
