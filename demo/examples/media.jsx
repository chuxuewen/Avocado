import { Div, Button, Video } from ':gui';
import AudioPlayer from ':gui/audio';
import Mynavpage from 'library.jsx';
import ':util/path';

const src_720 = $('piper720p.mp4');
const audio_src = $('all_we_know.mp3');

var audio_player = null;

function PlayVideo(evt) {
  StopAudio(evt);
	var v = evt.sender.top_ctr.find('video');
  v.src = src_720;
  v.start();
}

function PlayAudio(evt) {
  StopVideo(evt);
	if ( !audio_player ) {
		audio_player = new AudioPlayer();
	}
  audio_player.src = audio_src;
  audio_player.start();
}

function StopVideo(evt) {
  evt.sender.top_ctr.find('video').stop();
}

function StopAudio(evt) {
  if ( audio_player ) {
  	audio_player.stop();
    audio_player = null;
  }
}

function Stop(evt) {
  StopVideo(evt);
  StopAudio(evt);
}

function Seek(evt) {
  if ( audio_player ) {
    audio_player.seek(10000); // 10s
  } else {
    evt.sender.top_ctr.find('video').seek(100000); // 100s
  }
}

export const vx = (
  <Mynavpage title="Media" source=($(__filename)) onremove_view=StopAudio>
    <Div width="full">
      <Button class="long_btn" onclick=PlayVideo>Play Video</Button>
      <Button class="long_btn" onclick=PlayAudio>Play Audio</Button>
      <Button class="long_btn" onclick=Stop>Stop</Button>

      <Video margin_top=10 id="video" width="full" background_color="#000" />
    </Div>
  </Mynavpage>
)