/*
 * //******************************************************************
 * //
 * // Copyright 2017 Samsung Electronics All Rights Reserved.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */
import React from 'react';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';
import AppBar from 'material-ui/AppBar';
import IconButton from 'material-ui/IconButton';
import IconMenu from 'material-ui/IconMenu';
import MenuItem from 'material-ui/MenuItem';
import MoreVertIcon from 'material-ui/svg-icons/navigation/more-vert';
import {orange700, white} from 'material-ui/styles/colors';

const Client = require('../Client');

const style = {
    basic: {
        backgroundColor: orange700
    },

    font: {
        fontWeight: 'bold'
    }
}

const handleSignout = () => {
    console.debug("sign out clicked");
    Client.signOut();
};

const handleDisconnect = () => {
    console.debug("disconnect clicked");
    Client.close();
};

const Menu = (props) => (
    <IconMenu
        iconButtonElement={
            <IconButton><MoreVertIcon color={white}/></IconButton>
        }
        targetOrigin={{horizontal: 'right', vertical: 'top'}}
        anchorOrigin={{horizontal: 'right', vertical: 'bottom'}}
    >
        <MenuItem primaryText="Help" />
        {props.signin ? <MenuItem primaryText="Sign out" onTouchTap={handleSignout} />: null}
        <MenuItem primaryText="Server setting" onTouchTap={handleDisconnect}/>
    </IconMenu>
);
Menu.muiName = 'IconMenu';

class MainAppBar extends React.Component {

    constructor(props, context) {
        super(props, context);

        this.state = {
            signin: props.signin
        }
    };

    render() {
        return (
            <MuiThemeProvider>
                <div>
                    <AppBar
                        /* TODO set title */
                        title="Resource Manager"
                        titleStyle={style.font}
                        /* TODO add icon */
                        //iconElementLeft={ }
                        iconElementRight={ <Menu signin={this.state.signin} /> }
                        showMenuIconButton={false}
                        style={style.basic}
                    />
                </div>
            </MuiThemeProvider>
        );
    }
};

export default MainAppBar;
