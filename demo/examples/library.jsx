import Navpage from ':gui/nav';
import { 
  ViewController, Button, CSS,
  Text, TextNode, atom_px, 
  Indep, is_view_xml,
} from ':gui';

CSS({
  
  '.long_btn': {
    margin: 10,
    margin_bottom: 0,
    width: "full",
    height: 36,
    text_line_height: 36,
    text_color: "#0079ff",
    border_radius: 8,
    border: `${atom_px} #0079ff`,
  },
    
  '.long_btn2': {
    margin: 10,
    margin_bottom: 0,
    width: "full",
    height: 36,
    text_line_height: 36,
    text_color: "#fff",
    border_radius: 8,
    border: `${atom_px} #fff`,
  },
  
  '.next_btn': {
    width: "full",
    text_line_height: 46,
    text_align: "left",
    border_radius: 0,
  },
  
  '.next_btn:normal': {
    background_color: '#fff0', time: 180
  },
  
  '.next_btn:down': {
    background_color: '#E1E4E4', time: 50
  },

  '.input': {
    margin:10,
    margin_bottom:0,
    width:"full",
    height:30,
    background_color:"#eee",
    border_radius:8,
  },

})

export class Navbutton extends ViewController {
  
  load_view(vx) {
    super.load_view(
      <Button
        onclick="click_handle_"
        class="next_btn"
        text_color="#0079ff"
        default_highlighted=0
        border_bottom=`${atom_px} #ccc`>
        <Text margin_left=10 margin_right=40>${vx}</Text>
        <Indep x=-10 align_x="right" align_y="center">
          <Text value="\uedbe" text_family="icon" text_color="#aaa" />
        </Indep>
      </Button>
    );
  }
  
  click_handle_(evt) {
    if ( is_view_xml(this.next) ) {
      var ctr = this.parent;
      while (ctr) {
        if ( ctr instanceof Mynavpage ) {
          ctr.nav.push(this.next, 1); break;
        }
        ctr = ctr.parent;
      }
    }
    // console.log('nav button click');
  }
}

export class Mynavpage extends Navpage {
  source: $(__filename)
}
