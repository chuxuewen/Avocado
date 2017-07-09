import { 
	Div, Hybrid, Text, Button, Image, Indep, Clip ,
	HIGHLIGHTED_DOWN,
} from ':gui';
import Toolbar from ':gui/nav';
import Mynavpage from 'library.jsx';
import 'review.jsx';

function view_code(evt) {
  evt.sender.top_ctr.nav.push(review.vx, 1);
}

function highlighted(evt) {
	var img1 = evt.sender.top_ctr.find('img1');
	var img2 = evt.sender.top_ctr.find('img2');
	var speed = 1;
	if ( evt.status == HIGHLIGHTED_DOWN ) {
		speed = img1 === evt.sender ? 2 : 0.5;
	}
	img1.action.speed = speed;
	img2.action.speed = speed;
}

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
    navbar.background_color="#333"
    navbar.back_text_color="#fff" 
    navbar.title_text_color="#fff"
    toolbar=toolbar_vx
    background_color="#333"
    title="Action" source=($(__filename))>
    <Clip width="full" height="full">

	  	<Indep width=600 align="center" y=-15 opacity=0.5>
	    	<Image onhighlighted=highlighted id="img1" src=($('gear0.png')) 
	    		margin_left="auto" margin_right="auto" 
	    		y=56 width=600 origin="300 300"
	    		action=[
						{ rotate_z: 0, time:0, curve:'linear' }, 
						{ rotate_z: -360, time: 4000, curve:'linear' },
				  ]
	    		action.loop=1e8
	    		action.playing=1
	    	/>
	    	<Image onhighlighted=highlighted id="img2" src=($('gear1.png')) 
	    		margin_left="auto" 
	    		margin_right="auto"
	    		width=361 
	    		origin="180.5 180.5"
	    		action=[
						{ rotate_z: 22.5, time:0, curve:'linear' }, 
						{ rotate_z: 22.5 + 360, time: 2000, curve:'linear' },
					]
	    		action.loop=1e8
	    		action.playing=1
	    	/>
	  	</Indep>

	  </Clip>
  </Mynavpage>
)
