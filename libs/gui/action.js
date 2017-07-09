/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the BSD license:
 *
 * Copyright (c) 2015, louis.chu
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of louis.chu nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL louis.chu BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * ***** END LICENSE BLOCK ***** */

export binding('_action');

var { Action, SpawnAction, SequenceAction, KeyframeAction } = exports;

 /**
  * @func create(json[,parent])
  * @arg json {Object|Action}
  * @arg [parent] {GroupAction}
  * @ret {Action}
  */
function create(json, parent) {
	var action = null;
	if ( typeof json == 'object' ) {
		if ( json instanceof Action ) {
			action = json;
		} else {
			// create
			if ( Array.isArray(json) ) { // KeyframeAction
				action = new KeyframeAction();
				for (var i of json)
          action.add(i);
			} else {
				if (json.seq) { // SequenceAction
					action = new SequenceAction();
					for (var i in json) 
						action[i] = json[i];
					var seq = json.seq;
					if (Array.isArray(seq)) {
						for (var i of seq) {
							create(i, action);
						}
					} else {
						create(seq, action);
					}
				} else if (json.spawn) { // SpawnAction
					action = new SpawnAction();
					for (var i in json) 
						action[i] = json[i];
					var spawn = json.spawn;
					if (Array.isArray(spawn)) {
						for (var i of spawn) {
							create(i, action);
						}
					} else {
						create(spawn, action);
					}
				} else { // KeyframeAction
					action = new KeyframeAction();
					for (var i in json) 
						action[i] = json[i];
					var frame = json.frame;
					if ( Array.isArray(frame) ) {
						for (var i of frame) 
							action.add(i);
					} else {
						action.add(frame);
					}
				}
			}
			// end craete
		}
		if ( parent ) { // Cannot be KeyframeAction type
			parent.append(action);
		}
	}
	return action;
}

 /**
 	* @func transition(view,style[,delay[,cb]][,cb])
 	* @arg view 	{View}
 	* @arg style  {Object}
 	* @arg [delay]  {uint} ms
 	* @arg [cb]     {Function}
 	* @ret {KeyframeAction}
 	*/
function transition(view, style, delay, cb) {
	var action = new KeyframeAction();
	if ( typeof delay == 'number' ) {
		action.delay = delay;
	} else if ( typeof delay == 'function' ) {
		cb = delay;
	}
	action.add(); // add frame 0
	action.add(style); // add frame 1
	view.set_action(action);
	action.frame(0).fetch(); // fetch 0 frame style

	if ( typeof cb == 'function' ) {
		view.onaction_keyframe.on(function(evt) {
			//console.log('onaction_keyframe');
			if ( evt.action === action ) {
				if (evt.frame != 1) return;
				cb(evt); // end
			}
			view.onaction_keyframe.off(-1);
		}, -1);
	}

	action.play(); // start play
	return action;
}

export {
	create, transition
};

 /**
	* LINEAR = 0
	* EASE = 1
	* EASE_IN = 2
	* EASE_OUT = 3
	* EASE_IN_OUT = 4
	*/

 /**
  * @class Action abstract class
  *
  * @func play()
  *
  * @func stop()
  *
  * @func seek(ms)
  * @arg ms {uint}
  *
  * @func seek_play(ms)
  * @arg ms {uint}
  *
  * @func seek_stop(ms)
  * @arg ms {uint}
  *
  * @func clear()
  *
  * @get loop {uint}
  *
  * @get delay {uint} ms
  *
  * @get delayed {int} ms
  *
  * @get speed {float} 0.1-10
  *
  * @get speed {bool}
  *
  * @get duration {uint} ms
  *
  * @get parent {Action}
  *
  * @set playing {bool}
  *
  * @set loop {uint}
  *
  * @set delay {uint} ms
  *
  * @set speed {float} 0.1-10
  *
  * @end
  */

 /**
  * @class GroupAction  abstract class
  * @bases Action
	*
  * @get length {uint}
	*
  * @func append(child)
  * @arg child {Action}
 	*
  * @func insert(index, child)
  * @arg index {uint}
  * @arg child {Action}
 	*
  * @func remove_child(index)
  * @arg index {uint}
 	*
  * @func children(index)
  * @arg index {uint}
  * @ret {Action} return child action
  *
  * @end
  */

 /**
  * @class SpawnAction
	*
	* @func spawn(index)
	* @arg index {uint}
	* @ret {Action} return child action
	*
  * @bases GroupAction
  */

 /**
  * @class SequenceAction
	*
	* @func seq(index)
	* @arg index {uint}
	* @ret {Action} return child action
	*
  * @bases GroupAction
  */

 /**
  * @class KeyframeAction
  * @bases Action
	*
  * @func has_property(name)
  * @arg name {emun PropertyName} 
  * @ret {bool}
	*
	* @func match_property(name)
	* @arg name {emun PropertyName} 
	* @ret {bool}
	*
	* @func frame(index)
	* @arg index {uint}
	* @ret {Frame}
	*
	* @func add([time[,curve]][style])
	* arg [time=0] {uint}
	* arg [curve] {Curve}
	* arg [style] {Object}
	* @ret {Frame}
	*
	* @get first {Frame}
	*
	* @get last {Frame}
	*
	* @get length {uint}
	*
	* @get position {uint} get play frame position
	*
	* @get time {uint} ms get play time position
	*
  * @end
  */

 /**
  * @class Frame
  *
	* @func fetch([view]) fetch style attribute by view
	* @arg [view] {View}
	*
	* @func flush() flush frame restore default values
	*
	* @get index {uint} frame index in action
	*
	* @get,set time {uint} ms
	*
	* @get host {KeyframeAction}
	*
	* @get,set translate {Vec2}
  * @get,set scale {Vec2}
  * @get,set skew {Vec2}
  * @get,set origin {Vec2}
  * @get,set margin {Value}
  * @get,set border {Border}
  * @get,set border_width {float}
  * @get,set border_color {Color}
  * @get,set border_radius {float}
  * @get,set min_width {Value}
  * @get,set min_height {Value}
  * @get,set start {Vec2}
  * @get,set ratio {Vec2}
  * @get,set align {Align}
  * @get,set width {Value|float}
  * @get,set height {Value|float}
  * @get,set x {float}
  * @get,set y {float}
  * @get,set scale_x {float}
  * @get,set scale_y {float}
  * @get,set skew_x {float}
  * @get,set skew_y {float}
  * @get,set origin_x {float}
  * @get,set origin_y {float}
  * @get,set rotate_z {float}
  * @get,set opacity {float}
  * @get,set visible {bool}
  * @get,set margin_left {Value}
  * @get,set margin_top {Value}
  * @get,set margin_right {Value}
  * @get,set margin_bottom {Value}
  * @get,set border_left {Border}
  * @get,set border_top, Border, border);
  * @get,set border_right, Border, border);
  * @get,set border_bottom, Border, border);
  * @get,set border_left_width {float}
  * @get,set border_top_width {float}
  * @get,set border_right_width {float}
  * @get,set border_bottom_width {float}
  * @get,set border_left_color {Color}
  * @get,set border_top_color {Color}
  * @get,set border_right_color {Color}
  * @get,set border_bottom_color {Color}
  * @get,set border_radius_left_top {float}
  * @get,set border_radius_right_top {float}
  * @get,set border_radius_right_bottom {float}
  * @get,set border_radius_left_bottom {float}
  * @get,set background_color, Color, color);
  * @get,set newline {bool}
  * @get,set content_align {ContentAlign}
  * @get,set text_align {TextAlign}
  * @get,set max_width {Value}
  * @get,set max_height {Value}
  * @get,set start_x {float}
  * @get,set start_y {float}
  * @get,set ratio_x {float}
  * @get,set ratio_y {float}
  * @get,set repeat {Repeat}
  * @get,set text_background_color {ColorValue}
  * @get,set text_color {ColorValue}
  * @get,set text_size {TextSizeValue}
  * @get,set text_style {TextStyleValue}
  * @get,set text_family {TextFamilyValue}
  * @get,set text_line_height {TextLineHeightValue}
  * @get,set text_shadow {TextShadowValue}
  * @get,set text_decoration {TextDecorationValue}
  * @get,set text_overflow {TextOverflowValue}
  * @get,set text_white_space {TextWhiteSpaceValue}
  * @get,set align_x {Align}
  * @get,set align_y {Align}
  * @get,set shadow {ShadowValue}
  * @get,set src {String}
  * @get,set background_image {String}
	*
  * @end
  */
