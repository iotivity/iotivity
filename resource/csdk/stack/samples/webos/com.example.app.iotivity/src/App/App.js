import kind from '@enact/core/kind';
import MoonstoneDecorator from '@enact/moonstone/MoonstoneDecorator';
import Panels from '@enact/moonstone/Panels';
import React from 'react';

import MainPanel from '../views/MainPanel';

class App extends React.Component {
    constructor (props) {
        super(props);
   }
   closeHandler() {
       window.close();
   }
    render(props) {
       const onClose = this.closeHandler.bind(this);
       return (
           <div {...props}>
               <Panels onApplicationClose={onClose} >
                   <MainPanel />
               </Panels>
           </div>
       );
   }
}

export default MoonstoneDecorator(App);
