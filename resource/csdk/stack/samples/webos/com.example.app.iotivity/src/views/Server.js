import React from 'react';
import {connect} from 'react-redux';
import PropTypes from 'prop-types';

import {MarqueeText} from '@enact/moonstone/Marquee';
import Button from '@enact/moonstone/Button';
import Divider from '@enact/moonstone/Divider';
import ExpandableList from '@enact/moonstone/ExpandableList';
import ExpandableInput from '@enact/moonstone/ExpandableInput';
import SwitchItem from '@enact/moonstone/SwitchItem';

import * as ActionCreators from '../actions/ActionCreators';

import {Layout, Cell} from '@enact/ui/Layout';

class Server extends React.Component {
    constructor (props) {
        super(props);
        this.state = {
            resourceUri: null,
            resourceType: null,
            resourceQuestion: null,
            resourceAnswer:null,
            resourceUpdateCompleted:true,
            resourceObservable:false,
        };
    }
    resourceTypeChanged(ev){
        this.setState({
            resourceType: ev.data,
            resourceUpdateCompleted:false,
        });
        console.log(this.state.resourceType);
    }
    resourceUriChanged(ev){
        this.setState({
            resourceUri: ev.value,
            resourceUpdateCompleted:false,
        });
        console.log(this.state.resourceUri);
    }
    resourceQuestionChanged(ev){
        this.setState({
            resourceQuestion: ev.value,
            resourceUpdateCompleted:false,
        });
        console.log(this.state.resourceQuestion);
    }
    resourceAnswerChanged(ev){
        this.setState({
            resourceAnswer: ev.value,
            resourceUpdateCompleted:false,
        });
        console.log(this.state.resourceAnswer);
    }
    resourceObservableChanged(ev){
        //console.log(ev);
        this.setState({
            resourceObservable: ev.selected,
            resourceUpdateCompleted:false,
        });
        console.log(this.state.resourceObservable);
    }
    createResource(){
        let param={
            uri:this.state.resourceUri,
            question:this.state.resourceQuestion,
            answer:this.state.resourceAnswer,
            types:this.state.resourceType,
            observable:this.state.resourceObservable
        }
        this.props.actionDisableServerResourceControlUI();
        this.props.createResource(param);
        this.setState({
            resourceUri: null,
            resourceType: null,
            resourceQuestion: null,
            resourceAnswer:null,
            resourceObservable:false,
            resourceUpdateCompleted:true,
        });
    }
    deleteResource(){
        let param={
            uri:this.state.resourceUri,
        }
        this.props.actionDisableServerResourceControlUI();
        this.props.deleteResource(param);
        this.setState({
            resourceUri: null,
            resourceType: null,
            resourceQuestion: null,
            resourceAnswer:null,
            resourceUpdateCompleted:true,
        });
    }
    render() {
        const {serverEnabled, enableServerResourceControlUI} = this.props;
        const onResourceUriChanged = this.resourceUriChanged.bind(this);
        const onResourceQuestionChanged = this.resourceQuestionChanged.bind(this);
        const onResourceAnswerChanged = this.resourceAnswerChanged.bind(this);
        const onResourceTypeChanged = this.resourceTypeChanged.bind(this);
        const onObservableResourceChanged = this.resourceObservableChanged.bind(this);
        const onCreateResourceTapped = this.createResource.bind(this);
        const onDeleteResourceTapped = this.deleteResource.bind(this);
        return(
            <div>
                <Layout align="start">
                    <Cell shrink>
                        {serverEnabled?
                            <Button small onClick={this.props.stopServer}>stop server</Button>
                            :<Button small onClick={this.props.startServer}>start server</Button>
                        }
                    </Cell>
                    <Cell>
                        {serverEnabled?
                            <div style={{"lineHeight":60+"px"}}>
                                <MarqueeText marqueeOn="render">Server is running</MarqueeText>
                            </div>
                            :null
                        }
                    </Cell>
                </Layout>
                <Layout align="start">
                    <Cell>
                    <Divider />
                    {(serverEnabled&&enableServerResourceControlUI)?
                        <div>
                            <ExpandableInput title="uri" onChange={onResourceUriChanged} />
                            <ExpandableList
                                closeOnSelect
                                onSelect={onResourceTypeChanged}
                                select={'radio'}
                                title={'type'}
                            >
                            {[
                                {disabled:false, children: 'core.fan'},
                                {disabled:false, children: 'core.light'},
                            ]}
                            </ExpandableList>
                            <ExpandableInput title="question" noneText="ex) How many angels can dance on the head of a pin?" onChange={onResourceQuestionChanged} />
                            <ExpandableInput title="answer" noneText="ex) As many as wanting." onChange={onResourceAnswerChanged} />
                            <SwitchItem disabled={false} onToggle={onObservableResourceChanged}>observable</SwitchItem>
                            <div style={{"paddingTop": 9+"px"}}>
                                <Button
                                    disabled={this.state.resourceQuestion&&this.state.resourceAnswer&&this.state.resourceUri&&this.state.resourceType?false:true}
                                    small
                                    onClick={onCreateResourceTapped}>
                                    create resource
                                </Button>
                                <Button disabled={this.state.resourceUri?false:true} small onClick={onDeleteResourceTapped}>delete resource</Button>
                            </div>
                        </div>
                        :null
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
    deleteResource: PropTypes.func,
    actionDisableServerResourceControlUI: PropTypes.func,
};
const mapDispatchToProps = (dispatch) => {
    return {
        startServer: () => ActionCreators.startServer(dispatch),
        stopServer: () => ActionCreators.stopServer(dispatch),
        createResource: (params) => dispatch(ActionCreators.createResource(params)),
        deleteResource: (params) => dispatch(ActionCreators.deleteResource(params)),
        actionDisableServerResourceControlUI: () => dispatch(ActionCreators.actionDisableServerResourceControlUI()),
    };
};
let mapStateToProps = (state) => {
    return {
        serverEnabled:state.serverEnabled,
        discoveredResources:state.discoveredResources,
        showDiscoveredResources:state.showDiscoveredResources,
        enableServerResourceControlUI:state.enableServerResourceControlUI,
    };
};
export default connect(mapStateToProps, mapDispatchToProps)(Server);
