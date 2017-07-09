import { Div, Button, Input } from ':gui';
import ':util/storage';
import alert from ':gui/dialog';
import Mynavpage from 'library.jsx';

const key = 'test';

function keyenter(evt) {
	evt.sender.blur();
}

function Get(evt) {
	alert(storage.get(key));
}

function Set(evt) {
	storage.set(key, evt.sender.top_ctr.find('input').value);
}

function Del(evt) {
	storage.del(key);
}

function Clear(evt) {
	storage.clear(key);
}

export const vx = (
  <Mynavpage title="File System" source=($(__filename))>
    <Div width="full">
    	<Input class="input" id="input" 
    		placeholder="Please enter value .." 
    		value="Hello."
    		return_type="done" onkeyenter=keyenter />
      <Button class="long_btn" onclick=Get>Get</Button>
      <Button class="long_btn" onclick=Set>Set</Button>
      <Button class="long_btn" onclick=Del>Del</Button>
      <Button class="long_btn" onclick=Clear>Clear</Button>
    </Div>
  </Mynavpage>
)