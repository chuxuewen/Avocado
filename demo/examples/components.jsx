import ':gui';
import { Scroll, Clip, Text } from ':gui';
import { Navbutton, Mynavpage } from 'library.jsx';
import 'checkbox.jsx';
import 'overlay.jsx';
import 'stepper.jsx';
import 'nav.jsx';
import 'dialog.jsx';
import 'list.jsx';

export const vx = (
  <Mynavpage title="Components" source=($(__filename))>
    <Scroll width="full" height="full" bounce_lock=0>
    
      <Text class="category_title">Components.</Text>
      <Clip class="category">
        <Navbutton next=nav.vx>Nav</Navbutton>
        <Navbutton next=checkbox.vx>Checkbox</Navbutton>
        <Navbutton next=stepper.vx>Stepper</Navbutton>
        <Navbutton next=overlay.vx>Overlay</Navbutton>
        <Navbutton next=dialog.vx>Dialog</Navbutton>
        <Navbutton next=list.vx view.border_width=0>List</Navbutton>
      </Clip>
      
    </Scroll>
  </Mynavpage>
);
