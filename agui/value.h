/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the BSD license:
 *
 * Copyright (c) 2015, xuewen.chu
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of xuewen.chu nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL xuewen.chu BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef __avocado__gui__value__
#define __avocado__gui__value__

#include "autil/util.h"
#include "autil/array.h"
#include "mathe.h"

/**
 * @ns avocado::gui
 */

av_gui_begin

class FontFamilysID;

namespace value {
  
  struct Enum {
    enum _Enum: byte {
      AUTO = 0,           // auto,
      FULL,               // full,
      PIXEL,              // pixel,
      PERCENT,            // percent,
      MINUS,              // minus,
      INHERIT,            // inherit,
      VALUE,              // value,
      LIGHT,              // light,
      REGULAR,            // regular,
      BOLD,               // bold,
      OTHER,              // other,
      NONE,               // none,
      OVERLINE,           // overline,
      LINE_THROUGH,       // line_through,
      UNDERLINE,          // underline,
      LEFT,               // left,
      CENTER,             // center,
      RIGHT,              // right,
      LEFT_REVERSE,       // left_reverse,
      CENTER_REVERSE,     // center_reverse,
      RIGHT_REVERSE,      // right_reverse,
      TOP,                // top,
      BOTTOM,             // bottom,
      MIDDLE,             // middle,
      REPEAT,             // repeat
      REPEAT_X,           // repeat_x
      REPEAT_Y,           // repeat_y
      MIRRORED_REPEAT,    // mirrored_repeat
      MIRRORED_REPEAT_X,  // mirrored_repeat_x
      MIRRORED_REPEAT_Y,  // mirrored_repeat_y
      NORMAL,             // normal
      CLIP,               // clip
      ELLIPSIS,           // ellipsis
      CENTER_ELLIPSIS,    // center-ellipsis
      NO_WRAP,            // no_wrap
      NO_SPACE,           // no_space
      PRE,                // pre
      PRE_LINE,           // pre_line
      // keyboard type
      ASCII,              // ascii
      NUMBER,             // number
      URL,                // url
      NUMBER_PAD,         // number_pad
      PHONE_PAD,          // phone_pad
      NAME_PHONE_PAD,     // name_phone_pad
      EMAIL,              // email
      DECIMAL_PAD,        // decimal_pad
      TWITTER,            // twitter
      WEB_SEARCH,         // web_search
      ASCII_NUMBER_PAD,   // ascii_numner_pad
      // keyboard return type
      GO,                 // go
      JOIN,               // join
      NEXT,               // next
      ROUTE,              // route
      SEARCH,             // search
      SEND,               // send
      DONE,               // done
      EMERGENCY_CALL,     // emergency_call
      CONTINUE,           // continue
    };
  };
  
  /**
   * @enum KeyboardType
   */
  enum class KeyboardType: byte {
    NORMAL            = Enum::NORMAL,
    ASCII             = Enum::ASCII,
    NUMBER            = Enum::NUMBER,
    URL               = Enum::URL,
    NUMBER_PAD        = Enum::NUMBER_PAD,
    PHONE_PAD         = Enum::PHONE_PAD,
    NAME_PHONE_PAD    = Enum::NAME_PHONE_PAD,
    EMAIL             = Enum::EMAIL,
    DECIMAL_PAD       = Enum::DECIMAL_PAD,
    TWITTER           = Enum::TWITTER,
    WEB_SEARCH        = Enum::WEB_SEARCH,
    ASCII_NUMBER_PAD  = Enum::ASCII_NUMBER_PAD,
  };
  
  /**
   * @enum KeyboardReturnType
   */
  enum KeyboardReturnType: byte {
    NORMAL            = Enum::NORMAL,
    GO                = Enum::GO,
    JOIN              = Enum::JOIN,
    NEXT              = Enum::NEXT,
    ROUTE             = Enum::ROUTE,
    SEARCH            = Enum::SEARCH,
    SEND              = Enum::SEND,
    DONE              = Enum::DONE,
    EMERGENCY_CALL    = Enum::EMERGENCY_CALL,
    CONTINUE          = Enum::CONTINUE,
  };
  
  struct CGRect {
    Vec2 origin;
    Vec2  size;
  };
  
  struct CGRegion {
    float x, y;
    float x2, y2;
    float w, h;
  };
  
  /**
   * @enum Direction
   */
  enum class Direction: byte {
    LEFT   = Enum::LEFT,
    RIGHT  = Enum::RIGHT,
    TOP    = Enum::TOP,
    BOTTOM = Enum::BOTTOM,
  };
  
  /**
   * @enum ValueType
   */
  enum class ValueType: byte {
    AUTO = Enum::AUTO,        // 自动值  auto
    FULL = Enum::FULL,        // 吸附到父视图(client边距与父视图重叠)
    PIXEL = Enum::PIXEL,      // 像素值  px
    PERCENT = Enum::PERCENT,  // 百分比  %
    MINUS = Enum::MINUS,      // 减法(parent-value) parent-value
  };
  
  /**
   * @enum TextArrtsType
   */
  enum class TextArrtsType: byte {
    INHERIT = Enum::INHERIT,
    VALUE = Enum::VALUE,
  };
  
  /**
   * @enum TextStyle
   */
  enum class TextStyle: byte {
    LIGHT = Enum::LIGHT,
    REGULAR = Enum::REGULAR,
    BOLD = Enum::BOLD,
    OTHER = Enum::OTHER,
  };
  
  /**
   * @enum TextDecoration
   */
  enum class TextDecoration: byte {
    NONE = Enum::NONE,                  // 没有
    OVERLINE = Enum::OVERLINE,          // 上划线
    LINE_THROUGH = Enum::LINE_THROUGH,  // 中划线
    UNDERLINE = Enum::UNDERLINE,        // 下划线
  };
  
  enum class TextOverflow: byte {
    NORMAL = Enum::NONE,                      // 不做任何处理
    CLIP = Enum::CLIP,                        // 剪切
    ELLIPSIS = Enum::ELLIPSIS,                // 剪切并显示省略号
    CENTER_ELLIPSIS = Enum::CENTER_ELLIPSIS,  // 剪切并居中显示省略号
  };
  
  enum class TextWhiteSpace: byte {
    NORMAL = Enum::NORMAL,                    // 保留所有空白,使用自动wrap
    NO_WRAP = Enum::NO_WRAP,                  // 合并空白序列,不使用自动wrap
    NO_SPACE = Enum::NO_SPACE,                // 合并空白序列,使用自动wrap
    PRE = Enum::PRE,                          // 保留所有空白,不使用自动wrap
    PRE_LINE = Enum::PRE_LINE,                // 合并空白符序列,但保留换行符,使用自动wrap
  };
  
  /**
   * @enum TextAlign 文本对齐方式
   */
  enum class TextAlign: byte {
    LEFT = Enum::LEFT,           // 左对齐
    CENTER = Enum::CENTER,       // 居中
    RIGHT = Enum::RIGHT,         // 右对齐
    LEFT_REVERSE = Enum::LEFT_REVERSE,   // 左对齐并反向
    CENTER_REVERSE = Enum::CENTER_REVERSE, // 居中对齐并反向
    RIGHT_REVERSE = Enum::RIGHT_REVERSE,  // 右对齐并反向
  };
  
  /**
   * @enum Align 对齐方式
   */
  enum class Align: byte {
    NONE = Enum::NONE,
    LEFT  = Enum::LEFT,
    RIGHT = Enum::RIGHT,
    CENTER = Enum::CENTER,
    TOP = Enum::TOP,
    BOTTOM = Enum::BOTTOM,
  };
  
  /**
   * @enum ContentAlign div 内容对齐方式
   */
  enum class ContentAlign: byte {
    LEFT   = Enum::LEFT,    // 水平左对齐
    RIGHT  = Enum::RIGHT,   // 水平右对齐
    TOP    = Enum::TOP,     // 垂直上对齐
    BOTTOM = Enum::BOTTOM,  // 垂直下对齐
  };
  
  /**
   * @enum Repeat 纹理重复方式
   */
  enum class Repeat: byte {
    NONE = Enum::NONE,
    REPEAT = Enum::REPEAT,
    REPEAT_X = Enum::REPEAT_X,
    REPEAT_Y = Enum::REPEAT_Y,
    MIRRORED_REPEAT = Enum::MIRRORED_REPEAT,
    MIRRORED_REPEAT_X = Enum::MIRRORED_REPEAT_X,
    MIRRORED_REPEAT_Y = Enum::MIRRORED_REPEAT_Y,
  };
  
  /**
   * @struct Border
   */
  struct Border {
    float width;
    Color color;
    inline Border(float w = 0, Color c = Color())
      : width(w), color(c)
    { }
  };
  
  /**
   * @struct Shadow
   */
  struct Shadow {
    float   offset_x;
    float   offset_y;
    float   size;
    Color   color;
    bool operator==(const Shadow&) const;
    inline bool operator!=(const Shadow& shadow) const {
      return ! operator==(shadow);
    }
  };
  
  // Compound value
  
  /**
   * @struct Value
   */
  struct Value {
    ValueType type;
    float value;
    inline Value(ValueType t = ValueType::AUTO, float v = 0) : type(t), value(v) { }
    inline Value(float v) : type(ValueType::PIXEL), value(v) { }
  };
  
  /**
   * @struct ColorValue
   */
  struct ColorValue {
    TextArrtsType type;
    Color         value;
  };
  
  /**
   * @struct TextSizeValue
   */
  struct TextSizeValue {
    TextArrtsType type;
    float         value;
  };
  
  /**
   * @struct TextFamilyValue
   */
  struct TextFamilyValue {
    TextArrtsType type;
    const FontFamilysID* value;
    cString& name() const;
    const Array<String>& names() const;
  };
  
  /**
   * @struct TextStyleValue
   */
  struct TextStyleValue {
    TextArrtsType type;
    TextStyle     value;
  };
  
  /**
   * @struct TextShadowValue
   */
  struct TextShadowValue {
    TextArrtsType type;
    Shadow        value;
  };
  
  /**
   * @struct TextLineHeight
   */
  struct TextLineHeight {
    bool    is_auto;
    float   height;
    bool operator!=(const TextLineHeight& value) const;
    bool operator==(const TextLineHeight& value) const;
  };
  
  /**
   * @struct TextLineHeightValue
   */
  struct TextLineHeightValue {
    TextArrtsType type;
    TextLineHeight value;
  };
  
  /**
   * @struct TextDecorationValue
   */
  struct TextDecorationValue {
    TextArrtsType   type;
    TextDecoration  value;
  };
  
  /**
   * @struct TextOverflowValue
   */
  struct TextOverflowValue {
    TextArrtsType   type;
    TextOverflow    value;
  };
  
  /**
   * @struct TextWhiteSpaceValue
   */
  struct TextWhiteSpaceValue {
    TextArrtsType     type;
    TextWhiteSpace    value;
  };
  
}

using value::Direction;
using value::ValueType;
using value::TextArrtsType;
using value::TextDecoration;
using value::TextOverflow;
using value::TextWhiteSpace;
using value::TextAlign;
using value::Align;
using value::ContentAlign;
using value::Repeat;
using value::Value;
using value::Border;
using value::TextLineHeight;
using value::TextStyle;
typedef value::Shadow ShadowValue;
using value::ColorValue;
using value::TextSizeValue;
using value::TextStyleValue;
using value::TextFamilyValue;
using value::TextLineHeightValue;
using value::TextShadowValue;
using value::TextDecorationValue;
using value::TextOverflowValue;
using value::TextWhiteSpaceValue;
using value::CGRect;
using value::CGRegion;
using value::KeyboardType;
using value::KeyboardReturnType;

av_gui_end
#endif
