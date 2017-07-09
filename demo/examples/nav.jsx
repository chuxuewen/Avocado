import { 
  Div, Indep, Button, Text, Hybrid
} from ':gui';
import Mynavpage from 'library.jsx';
import { Navbar, Toolbar } from ':gui/nav';
import 'review.jsx';

function hide_show_navbar(evt) {
  var navbar = evt.sender.top_ctr.navbar;
  var hidden = !navbar.hidden
  navbar.set_hidden(hidden, true);
  evt.sender.prev.transition({ height: hidden ? 20 : 0, time: 400 });
}

function hide_show_toolbar(evt) {
  var toolbar = evt.sender.top_ctr.toolbar;
  toolbar.set_hidden(!toolbar.hidden, true);
}

function nav_pop(evt) {
  evt.sender.top_ctr.nav.pop(1);
}

function view_code(evt) {
  evt.sender.top_ctr.nav.push(review.vx, 1);
}

const navbar_vx = (
  <Navbar background_color="#333" back_text_color="#fff" title_text_color="#fff">
    <Indep align_x="right" align_y="center" x=-10>
      <Button text_family="icon" text_color="#fff" text_size=20>\ued63</Button>
    </Indep>
  </Navbar>
)

const toolbar_vx = (
  <Toolbar background_color="#333">
    <Hybrid text_align="center" width="full" height="full">
      <Button onclick=view_code>
        <Text class="toolbar_btn" text_color="#fff">\ue9ab</Text>
      </Button>
    </Hybrid>
  </Toolbar>
)

export const vx = (
  <Mynavpage 
    title="Nav" source=($(__filename)) 
    background_color="#333" navbar=navbar_vx toolbar=toolbar_vx>
    <Div width="full">
      <Div width="full" height=0 />
      <Button class="long_btn2" onclick=hide_show_navbar>Hide/Show Navbar</Button>
      <Button class="long_btn2" onclick=hide_show_toolbar>Hide/Show Toolbar</Button>
      <Button class="long_btn2" onclick=nav_pop>Nav pop</Button>
    </Div>
  </Mynavpage>
)