import { Div, Button, Input } from ':gui';
import ':util/fs';
import ':util/path';
import alert from ':gui/dialog';
import Mynavpage from 'library.jsx';

const filename = path.documents('test.txt');

function WriteFile(evt) {
	fs.write_file(filename, evt.sender.top_ctr.find('input').value, function() {
		alert('Write OK.');
	}.catch(function(err) {
		alert(err.message);
	}));
}

function WriteFileSync(evt) {
	try {
		fs.write_file_sync(filename, evt.sender.top_ctr.find('input').value);
		alert('Write OK.');
	} catch (err) {
		alert(err.message);
	}
}

function ReadFile(evt) {
	fs.read_file(filename, function(buf) {
		alert(buf.to_string('utf-8'));
	}.catch(function(err) {
		alert(err.message);
	}));
}

function Remove(evt) {
	try {
		fs.rm_r_sync(filename);
		alert('Remove OK.');
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
    		placeholder="Please enter write content.."
    		value="Hello."
    		return_type="done" onkeyenter=keyenter />
      <Button class="long_btn" onclick=WriteFile>WriteFile</Button>
      <Button class="long_btn" onclick=WriteFileSync>WriteFileSync</Button>
      <Button class="long_btn" onclick=ReadFile>ReadFile</Button>
      <Button class="long_btn" onclick=ReadFile>ReadFileSync</Button>
      <Button class="long_btn" onclick=Remove>Remove</Button>
    </Div>
  </Mynavpage>
)