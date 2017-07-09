/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the BSD license:
 *
 * Copyright (c) 2015, Louis.chu
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Louis.chu nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Louis.chu BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ***** END LICENSE BLOCK ***** */

var enum_keys = [
  [ 'AUTO',           'auto' ],
  [ 'FULL',           'full' ],
  [ 'PIXEL',          'pixel' ],
  [ 'PERCENT',        'percent' ],
  [ 'MINUS',          'minus' ],
  [ 'INHERIT',        'inherit' ],
  [ 'VALUE',          'value' ],
  [ 'LIGHT',          'light' ],
  [ 'REGULAR',        'regular' ],
  [ 'BOLD',           'bold' ],
  [ 'OTHER',          'other' ],
  [ 'NONE',           'none' ],
  [ 'OVERLINE',       'overline' ],
  [ 'LINE_THROUGH',   'line_through' ],
  [ 'UNDERLINE',      'underline' ],
  [ 'LEFT',           'left' ],
  [ 'CENTER',         'center' ],
  [ 'RIGHT',          'right' ],
  [ 'LEFT_REVERSE',   'left_reverse' ],
  [ 'CENTER_REVERSE', 'center_reverse' ],
  [ 'RIGHT_REVERSE',  'right_reverse' ],
  [ 'TOP',            'top' ],
  [ 'BOTTOM',         'bottom' ],
  [ 'MIDDLE',         'middle' ],
  [ 'REPEAT',         'repeat' ],
  [ 'REPEAT_X',       'repeat_x' ],
  [ 'REPEAT_Y',       'repeat_y' ],
  [ 'MIRRORED_REPEAT',    'mirrored_repeat' ],
  [ 'MIRRORED_REPEAT_X',  'mirrored_repeat_x' ],
  [ 'MIRRORED_REPEAT_Y',  'mirrored_repeat_x' ],
  [ 'NORMAL',             'normal' ],
  [ 'CLIP',               'clip' ],
  [ 'ELLIPSIS',           'ellipsis' ],
  [ 'CENTER_ELLIPSIS',    'center_ellipsis' ],
  [ 'NO_WRAP',            'no_wrap' ],
  [ 'NO_SPACE',           'no_space' ],
  [ 'PRE',                'pre' ],
  [ 'PRE_LINE',           'pre_line' ],
  // keyboard type
  ['ASCII',               'ascii' ],
  ['NUMBER',              'number' ],
  ['URL',                 'url' ],
  ['NUMBER_PAD',          'number_pad' ],
  ['PHONE_PAD',           'phone_pad' ],
  ['NAME_PHONE_PAD',      'name_phone_pad' ],
  ['EMAIL',               'email' ],
  ['DECIMAL_PAD',         'decimal_pad' ],
  ['TWITTER',             'twitter' ],
  ['WEB_SEARCH',          'web_search' ],
  ['ASCII_NUMBER_PAD',    'ascii_numner_pad' ],
  // keyboard return type
  ['GO',                  'go' ],
  ['JOIN',                'join' ],
  ['NEXT',                'next' ],
  ['ROUTE',               'route' ],
  ['SEARCH',              'search' ],
  ['SEND',                'send' ],
  ['DONE',                'done' ],
  ['EMERGENCY_CALL',      'emergency_call' ],
  ['CONTINUE',            'continue' ],
];

var enum_object = { };
var value_type = [];
var text_arrts_type = [];
var text_style = [];
var text_decoration = [];
var text_align = [];
var text_overflow = [];
var align = [];
var content_align = [];
var repeat = [];
var direction = [];
var text_overflow = [];
var text_white_space = [];
var keyboard_type = [];
var keyboard_return_type = [];

enum_keys.forEach(function(names, index) {
  enum_object[names[1]] = index;
  exports[names[0]] = index;
});
// ValueType
value_type[enum_object.auto] = 1;
value_type[enum_object.full] = 1;
value_type[enum_object.pixel] = 1;
value_type[enum_object.percent] = 1;
value_type[enum_object.minus] = 1;
// TextArrtsType
text_arrts_type[enum_object.inherit] = 1;
text_arrts_type[enum_object.value] = 1;
// TextStyle
text_style[enum_object.light] = 1;
text_style[enum_object.regular] = 1;
text_style[enum_object.bold] = 1;
text_style[enum_object.other] = 1;
// TextDecoration
text_decoration[enum_object.none] = 1;
text_decoration[enum_object.overline] = 1;
text_decoration[enum_object.line_through] = 1;
text_decoration[enum_object.underline] = 1;
// TextAlign
text_align[enum_object.left] = 1;
text_align[enum_object.center] = 1;
text_align[enum_object.right] = 1;
text_align[enum_object.left_reverse] = 1;
text_align[enum_object.center_reverse] = 1;
text_align[enum_object.right_reverse] = 1;
// TextOverflow
text_overflow[enum_object.normal] = 1;
text_overflow[enum_object.clip] = 1;
text_overflow[enum_object.ellipsis] = 1;
text_overflow[enum_object.center_ellipsis] = 1;
// TextWhiteSpace
text_white_space[enum_object.normal] = 1;
text_white_space[enum_object.no_wrap] = 1;
text_white_space[enum_object.no_space] = 1;
text_white_space[enum_object.pre] = 1;
text_white_space[enum_object.pre_line] = 1;
// Align
align[enum_object.left] = 1;
align[enum_object.right] = 1;
align[enum_object.center] = 1;
align[enum_object.top] = 1;
align[enum_object.bottom] = 1;
align[enum_object.none] = 1;
// ContentAlign
content_align[enum_object.left] = 1;
content_align[enum_object.right] = 1;
content_align[enum_object.top] = 1;
content_align[enum_object.bottom] = 1;
// Repeat
repeat[enum_object.none] = 1;
repeat[enum_object.repeat] = 1;
repeat[enum_object.repeat_x] = 1;
repeat[enum_object.repeat_y] = 1;
repeat[enum_object.mirrored_repeat] = 1;
repeat[enum_object.mirrored_repeat_x] = 1;
repeat[enum_object.mirrored_repeat_y] = 1;
// Direction
direction[enum_object.left] = 1;
direction[enum_object.right] = 1;
direction[enum_object.top] = 1;
direction[enum_object.bottom] = 1;
// KeyboardType
keyboard_type[enum_object.ascii] = 1;
keyboard_type[enum_object.number] = 1;
keyboard_type[enum_object.url] = 1;
keyboard_type[enum_object.number_pad] = 1;
keyboard_type[enum_object.phone_pad] = 1;
keyboard_type[enum_object.name_phone_pad] = 1;
keyboard_type[enum_object.email] = 1;
keyboard_type[enum_object.decimal_pad] = 1;
keyboard_type[enum_object.twitter] = 1;
keyboard_type[enum_object.web_search] = 1;
keyboard_type[enum_object.ascii_numner_pad] = 1;
// KeyboardReturnType
keyboard_return_type[enum_object.right] = 1;
keyboard_return_type[enum_object.go] = 1;
//keyboard_return_type[enum_object.google] = 1;
keyboard_return_type[enum_object.join] = 1;
keyboard_return_type[enum_object.next] = 1;
keyboard_return_type[enum_object.route] = 1;
keyboard_return_type[enum_object.search] = 1;
keyboard_return_type[enum_object.send] = 1;
//keyboard_return_type[enum_object.yahoo] = 1;
keyboard_return_type[enum_object.done] = 1;
keyboard_return_type[enum_object.emergency_call] = 1;
keyboard_return_type[enum_object['continue']] = 1;

// ----------------------------

function check_uinteger(value) {
  return Number.isInteger(value) ? value >= 0 : false;
}

function check_integer_ret(value) {
  if (!check_uinteger(value)) {
    throw new Error('Bad argument.');
  }
  return value;
}

function check_unsigned_number(value) {
  return Number.isFinite(value) ? value >= 0 : false;
}

function check_number(value) {
  return Number.isFinite(value);
}

function check_number_ret(value) {
  if (!check_number(value)) {
    throw new Error('Bad argument.');
  }
  return value;
}

function check_unsigned_number_ret(value) {
  if (!check_unsigned_number(value)) {
    throw new Error('Bad argument.');
  }
  return value;
}

function check_is_null_ret(value) {
  if (value === null) {
    throw new Error('Bad argument.');
  }
  return value;
}

function check_enum(enum_obj, value) {
  return Number.isInteger(value) && enum_obj[value];
}

function check_enum_ret(enum_obj, value) {
  if (Number.isInteger(value) && enum_obj[value]) {
    return value;
  } else {
    throw new Error('Bad argument.');
  }
}

function check_string(value) {
  return typeof value == 'string'; 
}

function check_string_ret(value) {
  if (typeof value == 'string') {
    return value;
  } else {
    throw new Error('Bad argument.');
  }
}

class Base {
  toString() {
    return this.to_string();
  }
  to_string() {
    return '[object]';
  }
}

class Enum extends Base {
  _enum: null;
  _value: enum_object.auto;
  get value() {
    return this._value;
  }
  set value(val) {
    this._value = check_enum_ret(this._enum, val);
  }
  constructor(value) {
    super();
    if (check_enum(this._enum, value)) {
      this._value = value;
    }
  }
  to_string() {
    return enum_keys[this._value][1];
  }
}

class TextAlign extends Enum {
  _enum: text_align
  _value: enum_object.left
}

class Align extends Enum {
  _enum: align
  _value: enum_object.left
}

class ContentAlign extends Enum {
  _enum: content_align
  _value: enum_object.left
}

class Repeat extends Enum {
  _enum: repeat
  _value: enum_object.none
}

class Direction extends Enum {
  _enum: direction
  _value: enum_object.left
}

class KeyboardType extends Enum {
  _enum: keyboard_type
  _value: enum_object.normal
}

class KeyboardReturnType extends Enum {
  _enum: keyboard_return_type
  _value: enum_object.normal
}

class Border extends Base {
  _width: 0
  _color: null
  get width() { return this._width; }
  get color() { return this._color; }
  get r() { return this._color._r; }
  get g() { return this._color._g; }
  get b() { return this._color._b; }
  get a() { return this._color._a; }
  set width(value) {
    this._width = check_number_ret(value);
  }
  set color(value) { 
    if (value instanceof Color) {
      this._color = value; 
    } else if (typeof value == 'string') { // 解析字符串
      this._color = check_is_null_ret(parse_color_value(value));
    } else {
      throw new Error('Bad argument.');
    }
  }
  constructor(width, color) {
    super();
    if (arguments.length > 0) {
      if (check_number(width)) this._width = width;
      this._color = color instanceof Color ? color : new Color();
    } else {
      this._color = new Color();
    }
  }
  to_string() {
    return `${this._width} ${this._color}`;
  }
}

class Shadow extends Base {
  _offset_x: 0
  _offset_y: 0
  _size: 0
  _color: null
  get offset_x() { return this._offset_x; }
  get offset_y() { return this._offset_y; }
  get size() { return this._size; }
  get color() { return this._color; }
  get r() { return this._color._r; }
  get g() { return this._color._g; }
  get b() { return this._color._b; }
  get a() { return this._color._a; }
  set offset_x(value) {  this._offset_x = check_number_ret(value); }
  set offset_y(value) { this._offset_y = check_number_ret(value); }
  set size(value) { this._size = check_unsigned_number(value); }
  set color(value) {
    if (value instanceof Color) {
      this._color = value; 
    } else if (typeof value == 'string') { // 解析字符串
      this._color = check_is_null_ret(parse_color_value(value));
    } else {
      throw new Error('Bad argument.');
    }
  }
  constructor(offset_x, offset_y, size, color) {
    super();
    if (arguments.length > 0) {
      if (check_number(offset_x)) this._offset_x = offset_x;
      if (check_number(offset_y)) this._offset_y = offset_y;
      if (check_unsigned_number(size)) this._size = size;
      this._color = color instanceof Color ? color : new Color();
    } else {
      this._color = new Color();
    }
  }
  to_string() {
    return `${this._offset_x} ${this._offset_y} ${this._size} ${this._color}`;
  }
}

function to_hex_string(num) {
  if (num < 16) {
    return '0' + num.toString(16);
  } else {
    return num.toString(16);
  }
}

class Color extends Base {
// @private:
  _r: 0
  _g: 0
  _b: 0
  _a: 255;
// @public:
  get r() { return this._r; }
  get g() { return this._g; }
  get b() { return this._b; }
  get a() { return this._a; }
  
  set r(value) {
    this._r = check_number_ret(value) % 256;
  }
  set g(value) {
    this._g = check_number_ret(value) % 256;
  }
  set b(value) {
    this._b = check_number_ret(value) % 256;
  }
  set a(value) {
    this._a = check_number_ret(value) % 256;
  }
  
  constructor(r, g, b, a) {
    super();
    if (arguments.length > 0) {
      if (check_uinteger(r)) this._r = r % 256;
      if (check_uinteger(g)) this._g = g % 256;
      if (check_uinteger(b)) this._b = b % 256;
      if (check_uinteger(a)) this._a = a % 256;
    }
  }
  
  reverse() {
    return new Color(255 - this._r, 255 - this._g, 255 - this._b, this._a);
  }
  
  to_rgb_string() {
    return `rgb(${this._r}, ${this._g}, ${this._b})`;
  }
  
  to_rgba_string() {
    return `rgba(${this._r}, ${this._g}, ${this._b}, ${this._a})`;
  }
  
  to_string() {
    return `#${to_hex_string(this._r)}${to_hex_string(this._g)}${to_hex_string(this._b)}`;
  }
  
  to_hex32_string() {
    return `#${to_hex_string(this._r)}${to_hex_string(this._g)}${to_hex_string(this._b)}${to_hex_string(this._a)}`;
  }
}

class Vec2 extends Base {
// @private:
  _x: 0
  _y: 0
// @public:  
  get x() { return this._x; }
  get y() { return this._y; }
  set x(value) { this._x = check_number_ret(value); }
  set y(value) { this._y = check_number_ret(value); }
  constructor(x, y) {
    super();
    if (arguments.length > 0) {
      if (check_number(x)) this._x = x;
      if (check_number(y)) this._x = y;
    }
  }
  to_string() {
    return `vec2(${this._x}, ${this._y})`;
  }
}

class Vec3 extends Base {
// @private:
  _x: 0
  _y: 0
  _z: 0
// @public:
  get x() { return this._x; }
  get y() { return this._y; }
  get z() { return this._x; }
  set x(value) { this._x = check_number_ret(value); }
  set y(value) { this._y = check_number_ret(value); }
  set z(value) { this._z = check_number_ret(value); }
  constructor(x, y, z) {
    super();
    if (arguments.length > 0) {
      if (check_number(x)) this._x = x;
      if (check_number(y)) this._x = y;
      if (check_number(z)) this._z = z;
    }
  }
  to_string() {
    return `vec3(${this._x}, ${this._y}, ${this._z})`;
  }
}

class Vec4 extends Base {
 // @private:
  _x: 0
  _y: 0
  _z: 0
  _w: 0
// @public:
  get x() { return this._x; }
  get y() { return this._y; }
  get z() { return this._x; }
  get w() { return this._w; }
  set x(value) { this._x = check_number_ret(value); }
  set y(value) { this._y = check_number_ret(value); }
  set z(value) { this._z = check_number_ret(value); }
  set w(value) { this._w = check_number_ret(value); }
  constructor(x, y, z, w) {
    super();
    if (arguments.length > 0) {
      if (check_number(x)) this._x = x;
      if (check_number(y)) this._x = y;
      if (check_number(z)) this._z = z;
      if (check_number(w)) this._w = w;
    }
  }
  to_string() {
    return `vec4(${this._x}, ${this._y}, ${this._z}, ${this._w})`;
  }
}
 
class Rect extends Base {
// @private:
  _x: 0
  _y: 0
  _width: 0
  _height: 0
// @public:
  get x() { return this._x; }
  get y() { return this._y; }
  get width() { return this._width; }
  get height() { return this._height; }
  set x(value) { this._x = check_number_ret(value); }
  set y(value) { this._y = check_number_ret(value); }
  set width(value) { this._width = check_number_ret(value); }
  set height(value) { this._height = check_number_ret(value); }
  constructor(x, y, width, height) {
    super();
    if (arguments.length > 0) {
      if (check_number(x)) this._x = x;
      if (check_number(y)) this._y = y;
      if (check_number(width)) this._width = width;
      if (check_number(height)) this._height = height;
    }
  }
  to_string() {
    return `rect(${this._x}, ${this._y}, ${this._width}, ${this._height})`;
  }
}
 
class Mat extends Base {
  _value: null;
  get m0() { this._value[0]; }
  get m1() { this._value[1]; }
  get m2() { this._value[2]; }
  get m3() { this._value[3]; }
  get m4() { this._value[4]; }
  get m5() { this._value[5]; }
  set m0(value) { this._value[0] = check_number_ret(value); }
  set m1(value) { this._value[1] = check_number_ret(value); }
  set m2(value) { this._value[2] = check_number_ret(value); }
  set m3(value) { this._value[3] = check_number_ret(value); }
  set m4(value) { this._value[4] = check_number_ret(value); }
  set m5(value) { this._value[5] = check_number_ret(value); }
  constructor(m0, m1, m2, m3, m4, m5) {
    super();
    var value = [1, 0, 0, 0, 1, 0];
    if (arguments.length > 0) {
      if (arguments.length == 1) {
        if (check_number(m0)) {
          value[0] = m0;
          value[4] = m0;
        }
      } else {
        if (check_number(m0)) value[0] = m0;
        if (check_number(m1)) value[1] = m1;
        if (check_number(m2)) value[2] = m2;
        if (check_number(m3)) value[3] = m3;
        if (check_number(m4)) value[4] = m4;
        if (check_number(m5)) value[5] = m5;
      }
    }
    this._value = value;
  }
  to_string() {
    var value = this._value;
    return `mat(${value[0]}, ${value[1]}, ${value[2]}, ${value[3]}, ${value[4]}, ${value[5]})`;
  }
}

class Mat4 extends Base {
  _value: null;
  get m0() { this._value[0]; }
  get m1() { this._value[1]; }
  get m2() { this._value[2]; }
  get m3() { this._value[3]; }
  get m4() { this._value[4]; }
  get m5() { this._value[5]; }
  get m6() { this._value[6]; }
  get m7() { this._value[7]; }
  get m8() { this._value[8]; }
  get m9() { this._value[9]; }
  get m10() { this._value[10]; }
  get m11() { this._value[11]; }
  get m12() { this._value[12]; }
  get m13() { this._value[13]; }
  get m14() { this._value[14]; }
  get m15() { this._value[15]; }
  set m0(value) { this._value[0] = check_number_ret(value); }
  set m1(value) { this._value[1] = check_number_ret(value); }
  set m2(value) { this._value[2] = check_number_ret(value); }
  set m3(value) { this._value[3] = check_number_ret(value); }
  set m4(value) { this._value[4] = check_number_ret(value); }
  set m5(value) { this._value[5] = check_number_ret(value); }
  set m6(value) { this._value[6] = check_number_ret(value); }
  set m7(value) { this._value[7] = check_number_ret(value); }
  set m8(value) { this._value[8] = check_number_ret(value); }
  set m9(value) { this._value[9] = check_number_ret(value); }
  set m10(value) { this._value[10] = check_number_ret(value); }
  set m11(value) { this._value[11] = check_number_ret(value); }
  set m12(value) { this._value[12] = check_number_ret(value); }
  set m13(value) { this._value[13] = check_number_ret(value); }
  set m14(value) { this._value[14] = check_number_ret(value); }
  set m15(value) { this._value[15] = check_number_ret(value); }
  constructor(m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15) {
    super();
    var value = [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1];
    if (arguments.length > 0) {
      if (arguments.length == 1) {
        if (check_number(m0)) {
          value[0] = m0;
          value[5] = m0;
          value[10] = m0;
          value[15] = m0;
        }
      } else {
        if (check_number(m0)) value[0] = m0;
        if (check_number(m1)) value[1] = m1;
        if (check_number(m2)) value[2] = m2;
        if (check_number(m3)) value[3] = m3;
        if (check_number(m4)) value[4] = m4;
        if (check_number(m5)) value[5] = m5;
        if (check_number(m6)) value[6] = m6;
        if (check_number(m7)) value[7] = m7;
        if (check_number(m8)) value[8] = m8;
        if (check_number(m9)) value[9] = m9;
        if (check_number(m10)) value[10] = m10;
        if (check_number(m11)) value[11] = m11;
        if (check_number(m12)) value[12] = m12;
        if (check_number(m13)) value[13] = m13;
        if (check_number(m14)) value[14] = m14;
        if (check_number(m15)) value[15] = m15;
      }
    }
    this._value = value;
  }
  to_string() {
    var value = this._value;
    return `mat4(\
${value[0]}, ${value[1]}, ${value[2]}, ${value[3]}, \
${value[4]}, ${value[5]}, ${value[6]}, ${value[7]}, \
${value[8]}, ${value[9]}, ${value[10]}, ${value[11]}, \
${value[12]}, ${value[13]}, ${value[14]}, ${value[15]})`;
  }
};

class Value extends Base {
  _type: enum_object.auto;
  _value: 0;
  get type() { return this._type; }
  get value() { return this._value; }
  set type(value) { this._type = check_enum_ret(value_type, value); }
  set value(val) { this._value = check_number_ret(val); }
  constructor(type, value) {
    super();
    if (arguments.length > 0) {
      if (check_enum(value_type, type)) {
        this._type = type;
      }
      if (check_number(value)) {
        this._value = value;
      }
    }
  }
  to_string() {
    switch (this._type) {
      case enum_object.auto: return 'auto';
      case enum_object.full: return 'full';
      case enum_object.pixel: return this._value.toString();
      case enum_object.percent: return this._value * 100 + '%';
      default: return this._value + '!';
    }
  }
}

class TextAttrsValue extends Base {
  _type: enum_object.inherit;
  get type() { return this._type; }
  set type(value) {
    this._type = check_enum_ret(text_arrts_type, value);
  }
  constructor(type) {
    super();
    if ( check_enum(text_arrts_type, type) ) {
      this._type = type;
    }
  }
}

class TextAttrsEnumValue extends TextAttrsValue {
  _value: null
  _enum: null
  get value() { return this._value; }
  set value(val) { this._value = check_enum_ret(this._enum, val); }
  constructor(type, value) {
    if (arguments.length > 0) {
      super(type);
      if (check_enum(this._enum, value)) {
        this._value = value;
      }
    } else {
      super();
    }
  }
  to_string() {
    return this._type == enum_object.inherit ? 'inherit' : enum_keys[this._value][1];
  }
}

class ColorValue extends TextAttrsValue {
  _value: null;
  get value() { return this._value; }
  get r() { return this._value._r; }
  get g() { return this._value._g; }
  get b() { return this._value._b; }
  get a() { return this._value._a; }
  set value(val) {
    if (val instanceof Color) {
      this._value = val; 
    } else if (typeof val == 'string') { // 解析字符串
      this._value = check_is_null_ret(parse_color_value(val));
    } else {
      throw new Error('Bad argument.');
    }
  }
  constructor(type, value) {
    if (arguments.length > 0) {
      super(type);
      this._value = value instanceof Color ? value : new Color();
    } else {
      super();
      this._value = new Color();
    }
  }
  to_string() {
    return this._type == enum_object.inherit ? 'inherit' : this._value.to_string();
  }
}

class TextSizeValue extends TextAttrsValue {
  _value: 12;
  get value() { return this._value; }
  set value(val) { this._value = check_unsigned_number_ret(val); }
  constructor(type, value) {
    if (arguments.length > 0) {
      super(type);
      if (check_unsigned_number(value)) {
        this._value = value;
      }
    } else {
      super();
    }
  }
  to_string() {
    return this._type == enum_object.inherit ? 'inherit' : this._value.toString();
  }
}

class TextFamilyValue extends TextAttrsValue {
  _value: '';
  get value() { return this._value; }
  set value(val) { this._value = check_string_ret(val); }
  constructor(type, value) {
    if (arguments.length > 0) {
      super(type);
      if (check_string(value)) {
        this._value = value;
      }
    } else {
      super();
    }
  }
  to_string() {
    return this._type == enum_object.inherit ? 'inherit' : this._value;
  }
}

class TextStyleValue extends TextAttrsEnumValue {
  _value: enum_object.regular;
  _enum: text_style
}

class TextShadowValue extends TextAttrsValue {
  _value: null;
  get value() { return this._value; }
  get offset_x() { return this._value._offset_x; }
  get offset_y() { return this._value._offset_y; }
  get size() { return this._value._size; }
  get color() { return this._value._color; }
  get r() { return this._value._color._r; }
  get g() { return this._value._color._g; }
  get b() { return this._value._color._b; }
  get a() { return this._value._color._a; }
  set value(val) {
    if (val instanceof Shadow) {
      this._value = val;
    } else if (typeof val == 'string') {
      this._value = check_is_null_ret(parse_shadow(val));
    } else {
      throw new Error('Bad argument.');
    }
  }
  constructor(type, value) {
    if (arguments.length > 0) {
      super(type);
      if (value instanceof Shadow) {
        this._value = value;
      } else {
        this._value = new Shadow();
      }
    } else {
      super();
      this._value = new Shadow();
    }
  }
  to_string() {
    return this._type == enum_object.inherit ? 'inherit' : this._value.to_string();
  }
}

class TextLineHeightValue extends TextAttrsValue {
  _is_auto: true
  _height: 0;
  get value() { return { is_auto: this._is_auto, height: this._height }; }
  get is_auto() { return this._is_auto; }
  get height() { return this._height; }
  set value(val) {
    if (typeof val == 'object') {
      this.is_auto = val.is_auto;
      this.height = val.height;
    } else {
      throw new Error('Bad argument.');
    }
  }
  set is_auto(value) {
    this._is_auto = !!value;
  }
  set height(value) {
    this._height = check_unsigned_number_ret(value);
  }
  constructor(type, is_auto, height) {
    if (arguments.length > 0) {
      super(type);
      if (arguments.length > 1) {
        this.is_auto = !!is_auto;
        if (check_unsigned_number(height)) this._height = height;
      }
    } else {
      super();
    }
  }
  to_string() {
    if (this._type == enum_object.inherit) {
      return 'inherit';
    } else if (this._is_auto) {
      return 'auto';
    } else {
      return this._height.to_string();
    }
  }
}

class TextDecorationValue extends TextAttrsEnumValue {
  _value: enum_object.none
  _enum: text_decoration
}

class TextOverflowValue extends TextAttrsEnumValue {
  _value: enum_object.normal
  _enum: text_overflow
}

class TextWhiteSpaceValue extends TextAttrsEnumValue {
  _value: enum_object.normal
  _enum: text_white_space
}

class Curve extends Base {
  _p1_x: 0;
  _p1_y: 0;
  _p2_x: 1;
  _p2_y: 1;
  constructor(p1_x, p1_y, p2_x, p2_y) {
    super();
    if (arguments.length > 0) {
      if (check_number(p1_x)) this._p1_x = p1_x;
      if (check_number(p1_y)) this._p1_y = p1_y;
      if (check_number(p2_x)) this._p2_x = p2_x;
      if (check_number(p2_y)) this._p2_y = p2_y;
    }
  }
  get p1_x() { return this._p1_x; }
  get p1_y() { return this._p1_y; }
  get p2_x() { return this._p2_x; }
  get p2_y() { return this._p2_y; }
  to_string() {
    return `curve(${this.p1_x}, ${this.p1_y}, ${this.p2_x}, ${this.p2_y})`;
  }
}

// ----------------------------

function _text_align(value) { 
  var rev = new TextAlign();
  rev._value = value;
  return rev;
}
function _align(value) { 
  var rev = new Align();
  rev._value = value;
  return rev;
}
function _content_align(value) {
  var rev = new ContentAlign();
  rev._value = value;
  return rev;
}
function _repeat(value) {
  var rev = new Repeat();
  rev._value = value;
  return rev;
}
function _direction(value) {
  var rev = new Direction();
  rev._value = value;
  return rev;
}
function _keyboard_type(value) {
  var rev = new KeyboardType();
  rev._value = value;
  return rev;
}
function _keyboard_return_type(value) {
  var rev = new KeyboardReturnType();
  rev._value = value;
  return rev;
}
function _border(width, color) { 
  var rev = new Border();
  rev._width = width;
  rev._color = color;
  return rev;
}
function _border_rgba(width, r, g, b, a) {
  return _border(width, _color(r, g, b, a));
}
function _shadow(offset_x, offset_y, size, color) { 
  var rev = new Shadow();
  rev._offset_x = offset_x;
  rev._offset_y = offset_y;
  rev._size = size;
  rev._color = color;
  return rev;
}
function _shadow_rgba(offset_x, offset_y, size, r, g, b, a) {
  return _shadow(offset_x, offset_y, size, _color(r, g, b, a));
}
function _color(r, g, b, a) {
  var rev = new Color();
  rev._r = r;
  rev._g = g;
  rev._b = b;
  rev._a = a;
  return rev;
}
function _vec2(x, y) {
  var rev = new Vec2();
  rev._x = x;
  rev._y = y;
  return rev;
}
function _vec3(x, y, z) {
  var rev = new Vec3();
  rev._x = x;
  rev._y = y;
  rev._z = z;
  return rev;
}
function _vec4(x, y, z, w) {
  var rev = new Vec4();
  rev._x = x;
  rev._y = y;
  rev._z = z;
  rev._w = w;
  return rev;
}
function _curve(p1_x, p1_y, p2_x, p2_y) {
  var rev = new Curve();
  rev._p1_x = p1_x;
  rev._p1_y = p1_y;
  rev._p2_x = p2_x;
  rev._p2_y = p2_y;
  return rev;
}
function _rect(x, y, width, height) {
  var rev = new Rect();
  rev._x = x;
  rev._y = y;
  rev._width = width;
  rev._height = height;
  return rev;
}
function _mat(value) {
  var rev = new Mat();
  rev._value = value;
  return rev;
}
function _mat4(value) {
  var rev = new Mat4();
  rev._value = value;
  return rev;
}
function _value(type, value) { 
  var rev = new Value();
  rev._type = type;
  rev._value = value;
  return rev;
}
function _color_value(type, value) { 
  var rev = new ColorValue();
  rev._type = type;
  rev._value = value;
  return rev;
}
function _color_value_rgba(type, r, g, b, a) {
  return _color_value(type, _color(r, g, b, a));
}
function _text_size_value(type, value) {
  var rev = new TextStyleValue();
  rev._type = type;
  rev._value = value;
  return rev;
}
function _text_family_value(type, value) {
  var rev = new TextFamilyValue();
  rev._type = type;
  rev._value = value;
  return rev;
}
function _text_style_value(type, value) {
  var rev = new TextStyleValue();
  rev._type = type;
  rev._value = value;
  return rev;
}
function _text_shadow_value(type, value) { 
  var rev = new TextShadowValue();
  rev._type = type;
  rev._value = value;
  return rev;
}
function _text_shadow_value_rgba(type, offset_x, offset_y, size, r, g, b, a) {
  return _text_shadow_value(type, _shadow_rgba(offset_x, offset_y, size, r, g, b, a));
}
function _text_line_height_value(type, is_auto, height) { 
  var rev = new TextLineHeightValue();
  rev._type = type;
  rev._is_auto = is_auto;
  rev._height = height;
  return rev;
}
function _text_decoration_value(type, value) {
  var rev = new TextDecorationValue();
  rev._type = type;
  rev._value = value;
  return rev;
}
function _text_overflow_value(type, value) {
  var rev = new TextOverflowValue();
  rev._type = type;
  rev._value = value;
  return rev;
}
function _text_white_space_value(type, value) {
  var rev = new TextWhiteSpaceValue();
  rev._type = type;
  rev._value = value;
  return rev;
}

// is

function _is_base(val) {
  return val instanceof Base;
}

// parse

function parse_text_align(str) { 
  if (typeof str == 'string') {
    var value = enum_object[str];
    if (check_enum(text_align, value)) {
      var rev = new TextAlign();
      rev._value = value;
      return rev;
    }
  }
  return null;
}

function parse_align(str) { 
  if (typeof str == 'string') {
    var value = enum_object[str];
    if (check_enum(align, value)) {
      var rev = new Align();
      rev._value = value;
      return rev;
    }
  }
  return null;
}

function parse_content_align(str) {
  if (typeof str == 'string') {
    var value = enum_object[str];
    if (check_enum(content_align, value)) {
      var rev = new ContentAlign();
      rev._value = value;
      return rev;
    }
  }
  return null;
}

function parse_repeat(str) {
  if (typeof str == 'string') {
    var value = enum_object[str];
    if (check_enum(repeat, value)) {
      var rev = new Repeat();
      rev._value = value;
      return rev;
    }
  }
  return null;
}

function parse_direction(str) {
  if (typeof str == 'string') {
    var value = enum_object[str];
    if (check_enum(direction, value)) {
      var rev = new Direction();
      rev._value = value;
      return rev;
    }
  }
  return null;
}

function parse_keyboard_type(str) {
  if (typeof str == 'string') {
    var value = enum_object[str];
    if (check_enum(keyboard_type, value)) {
      var rev = new KeyboardType();
      rev._value = value;
      return rev;
    }
  }
  return null;
}

function parse_keyboard_return_type(str) {
  if (typeof str == 'string') {
    var value = enum_object[str];
    if (check_enum(keyboard_return_type, value)) {
      var rev = new KeyboardReturnType();
      rev._value = value;
      return rev;
    }
  }
  return null;
}

function parse_border(str) { 
  if (typeof str == 'string') {
    // 10 #ff00ff
    var m = str.match(/^ *((?:\d+)?\.?\d+)/);
    if (m) {
      var rev = new Border();
      rev._width = parseFloat(m[1]);
      var color = parse_color(str.substr(m[0].length + 1));
      if (color) {
        rev._color = color;
      }
      return rev;
    }
  }
  return null;
}

function parse_shadow(str) { 
  if (typeof str == 'string') {
    // 10 10 2 #ff00aa
    var m = str.match(/^ *(-?(?:\d+)?\.?\d+) +(-?(?:\d+)?\.?\d+) + ((?:\d+)?\.?\d+)/);
    if (m) {
      var rev = new Shadow();
      rev._offset_x = parseFloat(m[1]);
      rev._offset_y = parseFloat(m[2]);
      rev._size = parseFloat(m[3]);
      var color = parse_color(str.substr(m[0].length + 1));
      if (color) {
        rev._color = color;
      }
    }
  }
  return null;
}

function parse_color(str) {
  if (typeof str == 'string') {
    var m = str.match(/^ *rgb(a)?\( *(\d{1,3}) *, *(\d{1,3}) *, *(\d{1,3})( *, *(\d{1,3}))? *\) *$/);
    if (m) {
      if (m[1] == 'a') { // rgba
        if (m[5]) { // a
          return _color(parseInt(m[2]) % 256, 
                        parseInt(m[3]) % 256,
                        parseInt(m[4]) % 256,
                        parseInt(m[6]) % 256);
        }
      } else { // rgb
        if (!m[5]) {
          return _color(parseInt(m[2]) % 256, 
                        parseInt(m[3]) % 256,
                        parseInt(m[4]) % 256, 255);
        }
      }
    }
    m = str.match(/^#([0-9a-f]{3}([0-9a-f])?([0-9a-f]{2})?([0-9a-f]{2})?)$/i);
    if (m) {
      if (m[4]) { // 8
        return _color(parseInt(m[1].substr(0, 2), 16), 
                      parseInt(m[1].substr(2, 2), 16),
                      parseInt(m[1].substr(4, 2), 16), 
                      parseInt(m[1].substr(6, 2), 16));
      } else if (m[3]) { // 6
        return _color(parseInt(m[1].substr(0, 2), 16), 
                      parseInt(m[1].substr(2, 2), 16),
                      parseInt(m[1].substr(4, 2), 16), 255);
      } else if (m[2]) { // 4
        return _color(parseInt(m[1].substr(0, 1), 16) * 17, 
                      parseInt(m[1].substr(1, 1), 16) * 17,
                      parseInt(m[1].substr(2, 1), 16) * 17, 
                      parseInt(m[1].substr(3, 1), 16) * 17);
      } else { // 3
        return _color(parseInt(m[1].substr(0, 1), 16) * 17, 
                      parseInt(m[1].substr(1, 1), 16) * 17,
                      parseInt(m[1].substr(2, 1), 16) * 17, 255);
      }
    }
  }
  return null;
}

function parse_vec2(str) {
  if (typeof str == 'string') {
    var m = str.match(/^ *(-?(?:\d+)?\.?\d+) +(-?(?:\d+)?\.?\d+) *$/) ||
            str.match(/^ *vec2\( *(-?(?:\d+)?\.?\d+) *, *(-?(?:\d+)?\.?\d+) *\) *$/);
    if (m) {
      return _vec2(parseFloat(m[1]), parseFloat(m[2]));
    }
  }
  return null;
}

function parse_vec3(str) {
  if (typeof str == 'string') {
    var m = str.match(/^ *(-?(?:\d+)?\.?\d+) +(-?(?:\d+)?\.?\d+) +(-?(?:\d+)?\.?\d+) *$/) ||
            str.match(/^ *vec3\( *(-?(?:\d+)?\.?\d+) *, *(-?(?:\d+)?\.?\d+) *, *(-?(?:\d+)?\.?\d+) *\) *$/);
    if (m) {
      return _vec3(parseFloat(m[1]), parseFloat(m[2]), parseFloat(m[3]));
    }
  }
  return null;
}

function parse_vec4(str) {
  if (typeof str == 'string') {
    var m = str.match(/^ *(-?(?:\d+)?\.?\d+) +(-?(?:\d+)?\.?\d+) +(-?(?:\d+)?\.?\d+) +(-?(?:\d+)?\.?\d+) *$/) ||
    str.match(/^ *vec4\( *(-?(?:\d+)?\.?\d+) *, *(-?(?:\d+)?\.?\d+) *, *(-?(?:\d+)?\.?\d+) *, *(-?(?:\d+)?\.?\d+) *\) *$/);
    if (m) {
      return _vec4(parseFloat(m[1]), parseFloat(m[2]), parseFloat(m[3]), parseFloat(m[4]));
    }
  }
  return null;
}

function parse_curve(str) {
  if (typeof str == 'string') {
    var m = str.match(/^ *(-?(?:\d+)?\.?\d+) +(-?(?:\d+)?\.?\d+) +(-?(?:\d+)?\.?\d+) +(-?(?:\d+)?\.?\d+) *$/) ||
    str.match(/^ *curve\( *(-?(?:\d+)?\.?\d+) *, *(-?(?:\d+)?\.?\d+) *, *(-?(?:\d+)?\.?\d+) *, *(-?(?:\d+)?\.?\d+) *\) *$/);
    if (m) {
      return _curve(parseFloat(m[1]), parseFloat(m[2]), parseFloat(m[3]), parseFloat(m[4]));
    }
  }
  return null;
}

function parse_rect(str) {
  if (typeof str == 'string') {
    var m = str.match(/^ *(-?(?:\d+)?\.?\d+) +(-?(?:\d+)?\.?\d+) +(-?(?:\d+)?\.?\d+) +(-?(?:\d+)?\.?\d+) *$/) ||
            str.match(/^ *rect\( *(-?(?:\d+)?\.?\d+) *, *(-?(?:\d+)?\.?\d+) *, *(-?(?:\d+)?\.?\d+) *, *(-?(?:\d+)?\.?\d+) *\) *$/);
    if (m) {
      return _rect(parseFloat(m[1]), parseFloat(m[2]), parseFloat(m[3]), parseFloat(m[4]));
    }
  }
  return null;
}

var parse_mat_reg = new RegExp(`^ *mat\\( *${new Array(6).join('(-?(?:\\d+)?\\.?\\d+) *, *')}(-?(?:\\d+)?\\.?\\d+) *\\) *$`);
var parse_mat4_reg = new RegExp(`^ *mat4\\( *${new Array(16).join('(-?(?:\\d+)?\\.?\\d+) *, *')}(-?(?:\\d+)?\\.?\\d+) *\\) *$`);
function parse_mat(str) {
  if (typeof str == 'string') {
    var m = parse_mat_reg.exec(str);
    if (m) {
      var value = [
        parseFloat(m[1]),
        parseFloat(m[2]),
        parseFloat(m[6]),
        parseFloat(m[4]),
        parseFloat(m[5]),
        parseFloat(m[6]),
      ];
      return _mat(value);
    }
  }
  return null;
}

function parse_mat4(str) {
  if (typeof str == 'string') {
    var m = parse_mat4_reg.exec(str);
    if (m) {
        var value = [
        parseFloat(m[1]),
        parseFloat(m[2]),
        parseFloat(m[6]),
        parseFloat(m[4]),
        parseFloat(m[5]),
        parseFloat(m[6]),
        parseFloat(m[7]),
        parseFloat(m[8]),
        parseFloat(m[9]),
        parseFloat(m[10]),
        parseFloat(m[11]),
        parseFloat(m[12]),
        parseFloat(m[13]),
        parseFloat(m[14]),
        parseFloat(m[15]),
        parseFloat(m[16]),
      ];
      return _mat4(value);
    }
  }
  return null;
}

function parse_value(str) { 
  if (typeof str == 'string') {
    // auto | full | 10.1 | 20% | 60!
    var m = str.match(/^((auto)|(full)|(-?(?:\d+)?\.?\d+)(%|!)?)$/);
    if (m) {
      if (m[2]) { // auto
        return _value(enum_object.auto, 0);
      } else if (m[3]) { // full
        return _value(enum_object.full, 0);
      } else { //
        var type = enum_object.pixel;
        var value = parseFloat(m[4]);
        if (m[5]) {
          if ( m[5] == '%' ) {
            type = enum_object.percent;
            value /= 100;               // %
          } else { // 10!
            type = enum_object.minus;
          }
        }
        return _value(type, value);
      }
    }
  }
  return null;
}

function parse_values(str) {
  if (typeof str == 'str') {
    var ls = str.split(/\s+/);
    var rev = [];
    for (var i = 0; i < ls.length; i++) {
      var val = parse_value(ls[i]);
      if (!val) {
        return null;
      }
      rev.push(val);
    }
    return rev;
  }
  return null;
}

function parse_float_values(str) {
  if (typeof str == 'str') {
    var ls = str.split(/\s+/);
    var rev = [];
    for (var i = 0; i < ls.length; i++) {
      var mat = ls[i].match(/^(-?(?:\d+)?\.?\d+)$/);
      if ( ! mat) {
        throw_error([10]);
        return null;
      }
      rev.push( parseFloat(mat[1]) );
    }
    return rev;
  }
  return null;
}

function parse_color_value(str) { 
  if (typeof str == 'string') {
    if (str == 'inherit') {
      return new ColorValue();
    } else {
      var value = parse_color(str);
      if (value) {
        return _color_value(enum_object.value, value);
      }
    }
  }
  return null;
}

function parse_text_size_value(str) {
  if (typeof str == 'string') {
    if (str == 'inherit') {
      return new TextSizeValue();
    } else {
      if (/^(?:\d+)?\.?\d+$/.test(str)) {
        return _text_size_value(enum_object.value, parseFloat(str));
      }
    }
  }
  return null;
}

function parse_text_family_value(str) {
  if (typeof str == 'string') {
    if (str == 'inherit') {
      return new TextFamilyValue();
    } else {
      return _text_family_value(enum_object.value, str);
    }
  }
  return null;
}

function parse_text_style_value(str) {
  if (typeof str == 'string') {
    if (str == 'inherit') {
      return new TextStyleValue();
    } else {
      var value = enum_object[str];
      if (check_enum(text_style, value)) {
        return _text_style_value(enum_object.value, value);
      }
    }
  }
  return null;
}

function parse_text_shadow_value(str) { 
  if (typeof str == 'string') {
    if (str == 'inherit') {
      return new TextShadowValue();
    } else {
      var value = parse_shadow(str);
      if (value) {
        return _text_shadow_value(enum_object.value, value);
      }
    }
  }
  return null;
}

function parse_text_line_height_value(str) { 
  if (typeof str == 'string') {
    if (str == 'inherit') {
      return new TextLineHeightValue();
    } else if (str == 'auto') {
      return _text_line_height_value(enum_object.value, true, 0);
    } else {
      if (/^(?:\d+)?\.?\d+$/.test(str)) {
        return _text_line_height_value(enum_object.value, fasle, parseFloat(str));
      }
    }
  }
  return null;
}

function parse_text_decoration_value(str) {
  if (typeof str == 'string') {
    if (str == 'inherit') {
      return new TextDecorationValue();
    } else {
      var value = enum_object[str];
      if (check_enum(text_decoration, value)) {
        return _text_decoration_value(enum_object.value, value);
      }
    }
  }
  return null;
}

function parse_text_overflow_value(str) {
  if (typeof str == 'string') {
    if (str == 'inherit') {
      return new TextDecorationValue();
    } else {
      var value = enum_object[str];
      if (check_enum(text_overflow, value)) {
        return _text_overflow_value(enum_object.value, value);
      }
    }
  }
  return null;
}

function parse_text_white_space_value(str) {
  if (typeof str == 'string') {
    if (str == 'inherit') {
      return new TextWhiteSpaceValue();
    } else {
      var value = enum_object[str];
      if (check_enum(text_white_space, value)) {
        return _text_white_space_value(enum_object.value, value);
      }
    }
  }
  return null;
}

// _native

function get_error_msg(reference, enum_value) {
  var message = '';
  if ( reference ) {
    for ( var val of reference ) {
      if ( val ) message += ', ' + JSON.stringify(val);
    }
  }
  if ( enum_value ) {
    for ( var i = 0; i < enum_value.length; i++ ) {
      if ( enum_value[i] ) message += ', ' + JSON.stringify(enum_keys[i][1]);
    }
  }
  return message;
}

var _native = exports._native;
_native._text_align = _text_align;
_native._align = _align;
_native._content_align = _content_align;
_native._repeat = _repeat;
_native._direction = _direction;
_native._keyboard_type = _keyboard_type;
_native._keyboard_return_type = _keyboard_return_type;
_native._border = _border;
_native._border_rgba = _border_rgba;
_native._shadow = _shadow;
_native._shadow_rgba = _shadow_rgba;
_native._color = _color;
_native._vec2 = _vec2;
_native._vec3 = _vec3;
_native._vec4 = _vec4;
_native._curve = _curve;
_native._rect = _rect;
_native._mat = _mat;
_native._mat4 = _mat4;
_native._value = _value;
_native._color_value = _color_value;
_native._color_value_rgba = _color_value_rgba;
_native._text_size_value = _text_size_value;
_native._text_family_value = _text_family_value;
_native._text_style_value = _text_style_value;
_native._text_shadow_value = _text_shadow_value;
_native._text_shadow_value_rgba = _text_shadow_value_rgba;
_native._text_line_height_value = _text_line_height_value;
_native._text_decoration_value = _text_decoration_value;
_native._text_overflow_value = _text_overflow_value;
_native._is_base = _is_base;
_native._text_white_space_value = _text_white_space_value;

_native._parse_text_align_description = function(str) {
  return get_error_msg(0, text_align);
}
_native._parse_align_description = function(str) {
  return get_error_msg(0, align);
}
_native._parse_content_align_description = function(str) {
  return get_error_msg(0, content_align);
}
_native._parse_repeat_description = function(str) {
  return get_error_msg(0, repeat);
}
_native._parse_direction_description = function(str) {
  return get_error_msg(0, direction);
}
_native._parse_keyboard_type_description = function(str) {
  return get_error_msg(0, keyboard_type);
}
_native._parse_keyboard_return_type_description = function(str) {
  return get_error_msg(0, keyboard_return_type);
}
_native._parse_border_description = function(str) {
  return get_error_msg(['10 #ff00aa', '10 rgba(255,255,0,255)']);
}
_native._parse_shadow_description = function(str) {
  return get_error_msg(['10 10 2 #ff00aa', '10 10 2 rgba(255,255,0,255)']);
}
_native._parse_color_description = function(str) {
  return get_error_msg(['rgba(255,255,255,255)', '#ff0', '#ff00', '#ff00ff', '#ff00ffff']);
}
_native._parse_vec2_description = function(str) {
  return get_error_msg(['vec2(1,1)', '1 1']);
}
_native._parse_vec3_description = function(str) {
  return get_error_msg(['vec3(0,0,1)', '0 0 1']);
}
_native._parse_vec4_description = function(str) {
  return get_error_msg(['vec4(0,0,1,1)', '0 0 1 1']);
}
_native._parse_curve_description = function(str) {
  return get_error_msg(['curve(0,0,1,1)', '0 0 1 1']);
}
_native._parse_rect_description = function(str) {
  return get_error_msg(['rect(0,0,-100,200)', '0 0 -100 200']);
}
_native._parse_mat_description = function(str) {
  return get_error_msg(['mat(1,0,0,1,0,1)']);
}
_native._parse_mat4_description = function(str) {
  return get_error_msg(['mat4(1,0,0,1,0,1,0,1,0,0,1,1,0,0,0,1)']);
}
_native._parse_value_description = function(str) {
  return get_error_msg(['auto', 'full', 10, '20%', '60!']);
}
_native._parse_values_description = function(str) {
  return get_error_msg(['auto', 'full', 10, '20%', '60!']);
}
_native._parse_float_values_description = function(str) {
  return get_error_msg([10, '10 20 30 40']);
}
_native._parse_color_value_description = function(str) {
  return get_error_msg(['inherit', 'rgba(255,255,255,255)', '#ff0', '#ff00', '#ff00ff', '#ff00ffff']);
}
_native._parse_text_size_value_description = function(str) {
  return get_error_msg(['inherit', 12]);
}
_native._parse_text_family_value_description = function(str) {
  return get_error_msg(['inherit', 'Ubuntu Mono']);
}
_native._parse_text_style_value_description = function(str) {
  return get_error_msg(['inherit'], text_style);
}
_native._parse_text_shadow_value_description = function(str) {
  return get_error_msg(['inherit', '10 10 2 #ff00aa', '10 10 2 rgba(255,255,0,255)']);
}
_native._parse_text_line_height_value_description = function(str) {
  return get_error_msg(['inherit', 24]);
}
_native._parse_text_decoration_value_description = function(str) {
  return get_error_msg(['inherit'], text_decoration);
}
_native._parse_text_overflow_value_description = function(str) {
  return get_error_msg(['inherit'], text_overflow);
}
_native._parse_text_white_space_value_description = function(str) {
  return get_error_msg(['inherit'], text_white_space);
}

delete exports._native;

// constructor
exports.TextAlign = TextAlign;
exports.Align = Align;
exports.ContentAlign = ContentAlign;
exports.Repeat = Repeat;
exports.Direction = Direction;
exports.KeyboardType = KeyboardType;
exports.KeyboardReturnType = KeyboardReturnType;
exports.Border = Border;
exports.Shadow = Shadow;
exports.Color = Color;
exports.Vec2 = Vec2;
exports.Vec3 = Vec3;
exports.Vec4 = Vec4;
exports.Curve = Curve;
exports.Rect = Rect;
exports.Mat = Mat;
exports.Mat4 = Mat4;
exports.Value = Value;
exports.ColorValue = ColorValue;
exports.TextSizeValue = TextSizeValue;
exports.TextFamilyValue = TextFamilyValue;
exports.TextStyleValue = TextStyleValue;
exports.TextShadowValue = TextShadowValue;
exports.TextLineHeightValue = TextLineHeightValue;
exports.TextDecorationValue = TextDecorationValue;
exports.TextOverflowValue = TextOverflowValue;
exports.TextWhiteSpaceValue = TextWhiteSpaceValue;
 // parse
exports.parse_text_align = parse_text_align;
exports.parse_align = parse_align;
exports.parse_content_align = parse_content_align;
exports.parse_repeat = parse_repeat;
exports.parse_direction = parse_direction;
exports.parse_keyboard_type = parse_keyboard_type;
exports.parse_keyboard_return_type = parse_keyboard_return_type;
exports.parse_border = parse_border;
exports.parse_shadow = parse_shadow;
exports.parse_color = parse_color;
exports.parse_vec2 = parse_vec2;
exports.parse_vec3 = parse_vec3;
exports.parse_vec4 = parse_vec4;
exports.parse_curve = parse_curve;
exports.parse_rect = parse_rect;
exports.parse_mat = parse_mat;
exports.parse_mat4 = parse_mat4;
exports.parse_value = parse_value;
exports.parse_values = parse_values;
exports.parse_float_values = parse_float_values;
exports.parse_color_value = parse_color_value;
exports.parse_text_size_value = parse_text_size_value;
exports.parse_text_family_value = parse_text_family_value;
exports.parse_text_style_value = parse_text_style_value;
exports.parse_text_shadow_value = parse_text_shadow_value;
exports.parse_text_line_height_value = parse_text_line_height_value;
exports.parse_text_decoration_value = parse_text_decoration_value;
exports.parse_text_overflow_value = parse_text_overflow_value;
exports.parse_text_white_space_value = parse_text_white_space_value;
