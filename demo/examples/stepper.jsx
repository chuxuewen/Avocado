import { Div, Text, CSS, atom_px } from ':gui';
import { Stepper } from ':gui/stepper';
import Mynavpage from 'library.jsx';

CSS({
  '.strpper_page': {
    width: 'full',
  },
  '.strpper_page .item': {
    width: 'full',
    border_bottom: `${atom_px} #ccc`,
  },
  '.strpper_page .text': {
    width: '140!',
    margin: 13,
  },
})

function change_handle(evt) {
  var stepper = evt.sender;
  stepper.view.prev.value = stepper.value;
}

export const vx = (
  <Mynavpage title="Stepper" source=($(__filename))>
    <Div width="full" class="strpper_page">
      <Div class="item">
        <Text class="text" />
        <Stepper onchange=change_handle style={margin:10} value=10 />
      </Div>
      <Div class="item">
        <Text class="text" />
        <Stepper onchange=change_handle style={margin:10} max=10 min=5 value=6 />
      </Div>
      <Div class="item">
        <Text class="text" value="0" />
        <Stepper onchange=change_handle style={margin:10} step=0.1 />
      </Div>
    </Div>
  </Mynavpage>
)
