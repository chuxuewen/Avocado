import { Div, Button, Input } from ':gui';
import ':util';
import ':util/http';
import alert from ':gui/dialog';
import Mynavpage from 'library.jsx';

function url(evt) {
	return evt.sender.top_ctr.find('input').value;
}

function Get(evt) {
	http.get(url(evt), function(buf) {
		alert(buf.to_string('utf-8').substr(0, 200).trim() + '...');
	}.catch(function(err) {
		alert(err.message);
	}));
}

function Post(evt) {
	http.post(url(evt), 'post data', function(buf) {
		alert(buf.to_string('utf-8').substr(0, 200).trim() + '...');
	}.catch(function(err) {
		alert(err.message);
	}));
}

function GetSync(evt) {
	try {
		alert(http.get_sync(url(evt)).to_string('utf-8').substr(0, 200).trim() + '...');
	} catch (err) {
		alert(err.message);
	}
}

function PostSync(evt) {
	try {
		alert(http.post_sync(url(evt), 'post data').to_string('utf-8').substr(0, 200).trim() + '...');
	} catch (err) {
		alert(err.message);
	}
}

function keyenter(evt) {
	evt.sender.blur();
}

export const vx = (
  <Mynavpage title="File System" source=($(__filename))>
    <Div width="full">
    	<Input class="input" id="input" 
    		placeholder="Please enter http url .." 
    		value="https://github.com/"
    		return_type="done" onkeyenter=keyenter />
      <Button class="long_btn" onclick=Get>Get</Button>
      <Button class="long_btn" onclick=Post>Post</Button>
      <Button class="long_btn" onclick=GetSync>GetSync</Button>
      <Button class="long_btn" onclick=PostSync>PostSync</Button>
      <Button class="long_btn" onclick=util.garbage_collection>GC</Button>
    </Div>
  </Mynavpage>
)