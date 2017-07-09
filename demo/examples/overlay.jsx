import { Div, Text, CSS, atom_px, Button, Indep, New } from ':gui';
import { Navbutton, Mynavpage } from 'library.jsx';
import Overlay from ':gui/overlay';

function show_overlay(evt) {
  New(
    <Overlay>
      <Div>
        <Navbutton>Menu A</Navbutton>
        <Navbutton>Menu B------C</Navbutton>
        <Navbutton>Menu C</Navbutton>
        <Navbutton view.border_width=0>Menu D</Navbutton>
      </Div>
    </Overlay>
  ).show_by_view(evt.sender);
}

function show_overlay2(evt) {
  var com = New(
    <Overlay>
      <Div>
        <Navbutton>Hello.</Navbutton>
        <Navbutton>Who are you going to?</Navbutton>
        <Navbutton view.border_width=0>Do I know you?</Navbutton>
      </Div>
    </Overlay>
  );
  com.priority = 'left';
  com.show_by_view(evt.sender);
}

function show_overlay3(evt) {
  var com = New(
    <Overlay>
      <Div>
        <Navbutton view.text_color="#fff">Hello.</Navbutton>
        <Navbutton view.text_color="#fff">Who are you going to?</Navbutton>
        <Navbutton view.text_color="#fff">Do I know you?</Navbutton>
        <Navbutton view.text_color="#fff" view.border_width=0>What country are you from?</Navbutton>
      </Div>
    </Overlay>
  );
  com.priority = 'left';
  com.background_color = '#000';
  com.show_by_view(evt.sender);
}

export const vx = (
  <Mynavpage title="Overlay" source=($(__filename))>
    <Div width="full" height="full">
      <Indep align_y="top" width="full">
        <Button class="long_btn" onclick=show_overlay> Show Overlay </Button>
      </Indep>
      <Indep align_y="bottom" y=-10 width="full">
        <Button class="long_btn" onclick=show_overlay> Show Overlay </Button>
      </Indep>
      <Indep align_y="center">
        <Button class="long_btn" onclick=show_overlay2> Show Overlay </Button>
      </Indep>
      <Indep align_y="center" align_x="right">
        <Button class="long_btn" onclick=show_overlay3> Show Overlay </Button>
      </Indep>
    </Div>
  </Mynavpage>
)