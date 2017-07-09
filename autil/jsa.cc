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

#include "jsa.h"
#include "string.h"
#include "string-builder.h"
#include "codec.h"
#include "fs.h"

av_ns(avocado)

//LF
static inline bool is_carriage_return(int c) {
  return c == 0x000D;
}

//CR
static inline bool is_line_feed(int c) {
  return c == 0x000A;
}

static inline bool is_line_terminator(int c) {
  return c == 0x000A || c == 0x000D;
}

// If c is in 'A'-'Z' or 'a'-'z', return its lower-case.
// Else, return something outside of 'A'-'Z' and 'a'-'z'.
// Note: it ignores LOCALE.
static inline int ascii_alpha_to_lower(int c) {
  return c | 0x20;
}

static inline bool is_in_range(int value, int lower_limit, int higher_limit) {
  av_assert(lower_limit <= higher_limit);
  return (uint)(value - lower_limit) <= (uint)(higher_limit - lower_limit);
}

static inline bool is_decimal_digit(int c) {
  // ECMA-262, 3rd, 7.8.3 (p 16)
  return is_in_range(c, '0', '9');
}

static inline bool is_hex_digit(int c) {
  // ECMA-262, 3rd, 7.6 (p 15)
  return is_decimal_digit(c) || is_in_range(ascii_alpha_to_lower(c), 'a', 'f');
}

static inline bool is_int(int64 i) {
  return !(1 << 31 & i);
}

static inline int char_to_number(int c) {
  return c - '0';
}

// Returns the value (0 .. 15) of a hexadecimal character c.
// If c is not a legal hexadecimal character, returns a value < 0.
static inline int hex_char_to_number(int c) {
  if(is_decimal_digit(c)){
    return char_to_number(c);
  }
  return 10 + ascii_alpha_to_lower(c) - 'a';
}

static inline bool is_octal_digit(int c) {
  // ECMA-262, 6th, 7.8.3
  return is_in_range(c, '0', '7');
}

static inline bool is_binary_digit(int c) {
  // ECMA-262, 6th, 7.8.3
  return c == '0' || c == '1';
}

static inline bool is_identifier_start(int c) {
  return is_in_range(ascii_alpha_to_lower(c), 'a', 'z') || c == '_' || c == '$';
}

static inline bool is_xml_element_start(int c) {
  return is_in_range(ascii_alpha_to_lower(c), 'a', 'z');
}

static inline bool is_identifier_part(int c) {
  return is_identifier_start(c) || is_decimal_digit(c);
}

static inline int64 int64_multiplication(int64 i, int multiple, int add) {
  
  double f = 1.0 * i / Int64::max;
  
  if (f * multiple > 1) { // 溢出
    return -1;
  }
  else {
    return i * multiple + add;
  }
}

enum Token {
  UNKNOWN,
  XML_ELEMENT_TAG,        // <xml
  XML_ELEMENT_TAG_END,    // </xml>
  XML_NO_IGNORE_SPACE,    // @@
  XML_COMMENT,            // <!-- comment -->
  //  MULTI_LINE_COMMENT,     // /* comment */
  LPAREN,                 // (
  RPAREN,                 // )
  LBRACK,                 // [
  RBRACK,                 // ]
  LBRACE,                 // {
  RBRACE,                 // }
  LT,                     // <
  GT,                     // >
  ASSIGN,                 // =
  COMMAND,                // `str${
  COMMAND_DATA_BIND,      // `str%%{
  COMMAND_DATA_BIND_ONCE, // `str%{
  COMMAND_END,            // str`
  COLON,                  // :
  SEMICOLON,              // ;
  COMMA,                  // ,
  CONDITIONAL,            // ?
  IDENTIFIER,             // 标识符
  WHITESPACE,             // space
  ADD,                    // +
  SUB,                    // -
  MUL,                    // *
  DIV,                    // /
  MOD,                    // %
  PERIOD,                 // .
  NOT,                    // !
  BIT_NOT,                // ~
  AND,                    // &
  OR,                     // |
  XOR,                    // ^
  AT,                     // @
  NUMBER_LITERAL,         // number
  STRING_LITERAL,         // string
  REGEXP_LITERAL,         // regexp
  AS,                     // as
  EXPORT,                 // export
  FROM,                   // from
  IMPORT,                 // import
  INSTANCEOF,             // instanceof
  IN,                     // in
  IF,                     // if
  OF,                     // of
  RETURN,                 // return
  TYPEOF,                 // typeof
  VAR,                    // var
  CLASS,                  // class
  FUNCTION,               // function
  LET,                    // let
  DEFAULT,                // default
  CONST,                  // const
  EXTENDS,                // extends
  EVENT,                  // event
  ELSE,                   // else
  EOS,                    // eos
  SHELL_HEADER,           //
  ILLEGAL,                // illegal
};

/**
 * @class Scanner # jsx 简易词法分析器
 */
class Scanner : public Object {
public:
  
  Scanner(const uint16* code, uint size)
  : code_(code)
  , size_(size)
  , line_(0)
  , pos_(0)
  , current_(new TokenDesc())
  , next_(new TokenDesc())
  { //
    c0_ = size_ == 0 ? -1: *code_;
    skip_white_space();
    strip_bom();
    current_->token = UNKNOWN;
    
    // skip shell header
    // #!/bin/sh
    if ( c0_ == '#' ) {
      advance();
      if ( c0_ == '!' ) {
        next_->token = SHELL_HEADER;
        next_->string_value.push('#');
        do {
          next_->string_value.push(c0_);
          advance();
        } while(c0_ != '\n' && c0_ != EOS);
        next_->location.beg_pos = 0;
        next_->location.end_pos = pos_;
        return;
      }
    }
    
    scan();
  }
  
  virtual ~Scanner() {
    delete current_;
    delete next_;
  }
  
  struct Location {
    uint beg_pos;
    uint end_pos;
    uint line;
  };
  
  void skip_shell_header() {
    
  }
  
  Token next() {
    
    // Table of one-character tokens, by character (0x00..0x7f only).
    static cbyte one_char_tokens[] = {
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::NOT,           // !
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::AND,           // &
      Token::ILLEGAL,
      Token::LPAREN,       // (
      Token::RPAREN,       // )
      Token::MUL,          // *
      Token::ADD,          // +
      Token::COMMA,        // ,
      Token::SUB,          // -
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::COLON,         // :
      Token::SEMICOLON,     // ;
      Token::ILLEGAL,
      Token::ASSIGN,        // =
      Token::GT,            // >
      Token::CONDITIONAL,   // ?
      Token::AT,            // @
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::LBRACK,      // [
      Token::ILLEGAL,
      Token::RBRACK,      // ]
      Token::XOR,         // ^
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::ILLEGAL,
      Token::LBRACE,        // {
      Token::OR,            // |
      Token::RBRACE,        // }
      Token::BIT_NOT,       // ~
      Token::ILLEGAL
    };
    
    TokenDesc* tok = current_;
    prev_ = tok->token;
    current_ = next_;
    next_ = tok;
    next_->location.beg_pos = pos_;
    next_->location.line = line_;
    next_->string_scape = next_->string_value = Ucs2String::empty;
    next_->before_line_feed = false;
    
    if ((uint)c0_ <= 0x7f) {
      Token token = (Token)one_char_tokens[c0_];
      if (token != ILLEGAL) {
        next_->token = token;
        next_->location.end_pos = pos_ + 1;
        advance();
        return current_->token;
      }
    }
    scan();
    return current_->token;
  }
  
  Token scan_xml_content(bool ignore_space, uint pos) {
    if ( !set_pos(pos) ) {
      next_->location = { pos_, pos_, line_ };
      next_->token = ILLEGAL;
      return ILLEGAL;
    }
    
    next_->location.beg_pos = pos_;
    next_->location.line = line_;
    next_->string_value = next_->string_scape = Ucs2String::empty;
    
    Token token = UNKNOWN;
    
    if (c0_ == '<') {
      // <!-- xml comment --> or <xml or </xml> or <
      token = scan_xml_element();
    }
    else if (c0_ == '$') {  // command block
      //  ${ command }
      advance();
      if (c0_ == '{') {                       // COMMAND
        token = COMMAND;                      //
      } else {
        token = STRING_LITERAL;
        next_->string_value.push('$');
      }
    }
    else if (c0_ == '%') { // Data bind command block
      // %{ command } %%{ command }
      advance();
      if ( c0_ == '{' ) { // COMMAND_DATA_BIND_ONCE
        token = COMMAND_DATA_BIND_ONCE;
      } else if ( c0_ == '%' ) {
        int c = c0_;
        advance();
        if ( c0_ == '{' ) { // COMMAND_DATA_BIND
          token = COMMAND_DATA_BIND;
        } else {
          token = STRING_LITERAL;
          next_->string_value.push('%');
          next_->string_value.push(c);
        }
      }
      else {
        token = STRING_LITERAL;
        next_->string_value.push('%');
      }
    }
    else if (c0_ == '@') { // xml文本转义,不忽略内容文本中的空格以及制表符
      advance();
      if (c0_ == '@') {
        advance();
        token = XML_NO_IGNORE_SPACE;  //
      } else {
        token = STRING_LITERAL;
        next_->string_value.push('@');
      }
    }
    else {
      token = scan_xml_content_string(ignore_space);
    }
    next_->location.end_pos = pos_;
    next_->token = token;
    return token;
  }
  
  Token scan_regexp(uint pos) {
    if ( !set_pos(pos) ) {
      next_->location = { pos_, pos_, line_ };
      next_->token = ILLEGAL;
      return ILLEGAL;
    }
    
    Token token = REGEXP_LITERAL;
    next_->location.beg_pos = pos_;
    next_->location.line = line_;
    next_->string_value = next_->string_scape = Ucs2String::empty;
    
    if (c0_ == '/') {
      advance();
      if (c0_ == '/' || c0_ == '*') { // ILLEGAL
        token = ILLEGAL;
      } else {
        next_->string_value.push('/');
        
        bool is_LBRACK = false; // [
        
        do {
          next_->string_value.push(c0_);
          if ( c0_ == '\\' ) { // 正则转义符
            advance();
            if (c0_ < 0) break;
            next_->string_value.push(c0_);
          } else if ( c0_ == '[' ) {
            is_LBRACK = true;
          } else if ( c0_ == ']' ) {
            is_LBRACK = false;
          }
          advance();
        } while( c0_ >= 0  && (is_LBRACK || c0_ != '/') && !is_line_terminator(c0_) );
        
        if (c0_ == '/') {
          next_->string_value.push('/');
          
          int i = 0, m = 0, g = 0;
          
          while (true) { // regexp flags
            advance();
            if (c0_ == 'i') {
              if (i) break; else { i = 1; next_->string_value.push('i'); }
            } else if (c0_ == 'm') {
              if (m) break; else { m = 1; next_->string_value.push('m'); }
            } else if (c0_ == 'g') {
              if (g) break; else { g = 1; next_->string_value.push('g'); }
            } else {
              break;
            }
          }
        } else {
          token = ILLEGAL;
        }
      }
    }
    next_->location.end_pos = pos_;
    next_->token = token;
    return token;
  }
  
  Token scan_command(uint pos) {
    if ( ! set_pos(pos) ) {
      next_->location = { pos_, pos_, line_ };
      next_->token = ILLEGAL;
      return ILLEGAL;
    }
    
    Token token = COMMAND_END;
    next_->location.beg_pos = pos_;
    next_->location.line = line_;
    
    while ( c0_ != '`' && c0_ >= 0 /*&& !is_line_terminator(c0_)*/ ) {
      int c = c0_;
      advance();
      if (c == '\\') { // 转义符
        if (c0_ < 0 || !scan_string_escape()) {
          token = ILLEGAL; break;
        }
      } else if (c == '$') { // js字符串内部指令开始必须以}结束
        // ${ command }
        if (c0_ == '{') {         // COMMAND
          token = COMMAND; break;
        }
      } else if (c == '%') { // data bind COMMAND
        // %{ command } or %%{ command }
        if ( c0_ == '{' ) {  // COMMAND_DATA_BIND_ONCE
          token = COMMAND_DATA_BIND_ONCE; break;
        } else if ( c0_ == '%' ) {
          advance();
          if ( c0_ == '{' ) { // COMMAND_DATA_BIND
            token = COMMAND_DATA_BIND; break;
          } else {
            back();
          }
        }
      } else if ( is_line_terminator(c) ) { // \n
        next_->string_scape.push(c);
      }
      next_->string_value.push(c);
    }
    if (c0_ == '`') {
      advance();  // consume quote
      next_->string_value.push('`');
    } else if (c0_ != '{') { // err
      token = ILLEGAL;
    }
    
    next_->location.end_pos = pos_;
    next_->token = token;
    return token;
  }
  
  inline Token        token()             { return current_->token; }
  inline Location     location()          { return current_->location; }
  inline Ucs2String&  string_scape()      { return current_->string_scape; }
  inline Ucs2String&  string_value()      { return current_->string_value; }
  inline bool         before_line_feed()  { return current_->before_line_feed; }
  inline Token        prev()              { return prev_; }
  inline Token        peek()              { return next_->token; }
  inline Location     next_location()     { return next_->location; }
  inline Ucs2String&  next_string_scape() { return next_->string_scape; }
  inline Ucs2String&  next_string_value() { return next_->string_value; }
  inline bool         next_before_line_feed() { return next_->before_line_feed; }
  inline bool         has_scape_before()  { return !current_->string_scape.is_empty(); }
  inline bool         has_scape_before_next() { return !next_->string_scape.is_empty(); }
  
private:

  struct TokenDesc {
    Token token;
    Location location;
    Ucs2String string_scape;
    Ucs2String string_value;
    bool       before_line_feed;
  };
  
  void scan() { // scan javascript code
    
    Token token;
    next_->string_value = next_->string_scape = Ucs2String::empty;
    next_->before_line_feed = false;
    
    do {
      // Remember the position of the next token
      next_->location.beg_pos = pos_;
      next_->location.line = line_;
      
      switch (c0_) {
        case ' ':
        case '\t':
        case '\n':
          next_->string_scape.push(c0_);
          if ( c0_ == '\n' ) {
            next_->before_line_feed = true;
          }
          advance();
          token = WHITESPACE;
          break;
        case '"':
        case '\'':
          token = scan_string();
          break;
        case '`': // 检查指令字符串内部是否有 ${
          advance();
          next_->string_value.push('`');
          token = scan_command(pos_);
          break;
        case '<':
          // <!-- xml comment --> or <xml or </xml> or <
          token = scan_xml_element();
          break;
          
        case '$':
          advance();
          if (c0_ == '{') {                       // COMMAND
            // ${ command }
            token = COMMAND;                      //
          } else {
            back(); token = scan_identifier();
          }
          break;
        case '%':
          advance();
          if ( c0_ == '{' ) {         // COMMAND_DATA_BIND_ONCE
            // %{ command }
            token = COMMAND_DATA_BIND_ONCE;
          } else if ( c0_ == '%' ) {
            advance();
            if ( c0_ == '{' ) {       // COMMAND_DATA_BIND
              // %%{ command }
              token = COMMAND_DATA_BIND;
            } else {
              back();
              token = MOD;
            }
          } else {
            token = MOD;
          }
          break;
        case '.': // . Number
          advance();
          if (is_decimal_digit(c0_)) {
            token = scan_number(true);
          } else {
            token = PERIOD;
          }
          break;
        case '/':
          // /  // /*
          advance();
          if (c0_ == '/') { // js 单行注解
            token = skip_single_line_comment();
          } else if (c0_ == '*') {  // js 多行注解
            token = skip_multi_line_comment();
          } else {
            token = DIV;
          }
          break;
        case '(': token = select(LPAREN); break;      //  (
        case ')': token = select(RPAREN); break;      //  )
        case '=': token = select(ASSIGN); break;      // =
        case '>': token = select(GT);     break;      // >
        case '@': token = select(AT);     break;      // @
        case '[': token = select(LBRACK); break;      //  [
        case ']': token = select(RBRACK); break;      //  ]
        case '{': token = select(LBRACE); break;      //  {
        case '}': token = select(RBRACE); break;      //  }
        case '+': token = select(ADD); break;         //  +
        case '-': token = select(SUB); break;         //  -
        case ',': token = select(COMMA); break;       //  ,
        case ':': token = select(COLON); break;       //  :
        case ';': token = select(SEMICOLON); break;   //  ;
        case '?': token = select(CONDITIONAL); break; //  ?
        case '!': token = select(NOT); break;         //  !
        case '|': token = select(OR); break;          //  |
        case '&': token = select(AND); break;         //  &
        case '~': token = select(BIT_NOT); break;     //  ~
        case '^': token = select(XOR); break;         //  ^
        case '*': token = select(MUL); break;         //  !
        default:
          if (is_identifier_start(c0_)) {
            token = scan_identifier();
          } else if (is_decimal_digit(c0_)) {
            token = scan_number(false);
          } else if (skip_white_space()) {
            token = WHITESPACE;
          } else if (c0_ < 0) {
            token = EOS;
          } else {
            next_->string_value.push(c0_);
            token = select(UNKNOWN); // TODO 不做更多严格检查,未知代码
          }
          break;
      }
      // Continue scanning for tokens as long as we're just skipping
      // whitespace.
    } while(token == WHITESPACE);
    
    next_->location.end_pos = pos_;
    next_->token = token;
  }
  
  Token scan_identifier() {
    
    // next_->string_value = Ucs2String::empty;
    
    // Scan the rest of the identifier characters.
    do {
      next_->string_value.push(c0_);
      advance();
    } while(is_identifier_part(c0_));
    
    const int input_length = next_->string_value.length();
    const uint16* input = next_->string_value.c();
    const int kMinLength = 2;
    const int kMaxLength = 10;
    
    if (input_length < kMinLength || input_length > kMaxLength) {
      return IDENTIFIER;
    }
    
    // ----------------------------------------------------------------------------
    // Keyword Matcher
    
#define KEYWORDS(KEYWORD_GROUP, KEYWORD)                          \
KEYWORD_GROUP('a')                                                \
KEYWORD("as", AS)                                                 \
KEYWORD_GROUP('c')                                                \
KEYWORD("class", CLASS)                                           \
KEYWORD("const", CONST)                                           \
KEYWORD_GROUP('d')                                                \
KEYWORD("default", DEFAULT)                                       \
KEYWORD_GROUP('e')                                                \
KEYWORD("export", EXPORT)                                         \
KEYWORD("extends", EXTENDS)                                       \
KEYWORD("event", EVENT)                                           \
KEYWORD("else", ELSE)                                             \
KEYWORD_GROUP('f')                                                \
KEYWORD("from", FROM)                                             \
KEYWORD("function", FUNCTION)                                     \
KEYWORD_GROUP('i')                                                \
KEYWORD("instanceof", INSTANCEOF)                                 \
KEYWORD("import", IMPORT)                                         \
KEYWORD("in", IN)                                                 \
KEYWORD("if", IF)                                                 \
KEYWORD_GROUP('l')                                                \
KEYWORD("let", LET)                                               \
KEYWORD_GROUP('o')                                                \
KEYWORD("of", OF)                                                 \
KEYWORD_GROUP('r')                                                \
KEYWORD("return", RETURN)                                         \
KEYWORD_GROUP('t')                                                \
KEYWORD("typeof", TYPEOF)                                         \
KEYWORD_GROUP('v')                                                \
KEYWORD("var", VAR)                                               \
    
    switch (input[0]) {
      default:
#define KEYWORD_GROUP_CASE(ch)                          \
break;                                                  \
case ch:
#define KEYWORD(keyword, token)                         \
{                                                       \
/* 'keyword' is a char array, so sizeof(keyword) is */  \
/* strlen(keyword) plus 1 for the NUL char. */          \
  const int keyword_length = sizeof(keyword) - 1;       \
  av_assert(keyword_length >= kMinLength);               \
  av_assert(keyword_length <= kMaxLength);               \
  if (input_length == keyword_length &&                 \
    input[1] == keyword[1] &&                           \
    (keyword_length <= 2 || input[2] == keyword[2]) &&  \
    (keyword_length <= 3 || input[3] == keyword[3]) &&  \
    (keyword_length <= 4 || input[4] == keyword[4]) &&  \
    (keyword_length <= 5 || input[5] == keyword[5]) &&  \
    (keyword_length <= 6 || input[6] == keyword[6]) &&  \
    (keyword_length <= 7 || input[7] == keyword[7]) &&  \
    (keyword_length <= 8 || input[8] == keyword[8]) &&  \
    (keyword_length <= 9 || input[9] == keyword[9])) {  \
    return token;                                       \
  }                                                     \
}
      KEYWORDS(KEYWORD_GROUP_CASE, KEYWORD)
    }
    return Token::IDENTIFIER;
  }
  
  // <!-- xml comment --> or <xml or </xml> or <
  Token scan_xml_element() {
    advance();
    if (c0_ == '!') { // <!-- xml comment -->
      
      advance();
      if (c0_ != '-') {
        back(); return LT;
      }
      
      advance();
      if (c0_ != '-') {
        back(); back(); return LT;
      }
      
      advance();
      while (c0_ >= 0) {
        if (c0_ == '-') {
          advance();
          if (c0_ >= 0) {
            if (c0_ == '-') {
              advance();
              if (c0_ >= 0) {
                if (c0_ == '>') {
                  advance();
                  return XML_COMMENT;
                } else {
                  next_->string_value.push('-');
                  next_->string_value.push('-');
                  next_->string_value.push(c0_ == '*' ? 'x' : c0_);
                }
              } else break;
            } else {
              next_->string_value.push('-');
              next_->string_value.push(c0_ == '*' ? 'x' : c0_);
            }
          } else break;
        } else {
          next_->string_value.push(c0_ == '*' ? 'x' : c0_);
        }
        advance();
      }
      
      // Unterminated multi-line comment.
      return ILLEGAL;
    }
    else if ( is_xml_element_start(c0_) ) { // <xml
      
      scan_xml_tag_identifier();
      
      int c = c0_; advance();
      if (c == ':' && is_xml_element_start(c0_)) {
        next_->string_value.push(':');
        scan_xml_tag_identifier();
      } else {
        back();
      }
      return XML_ELEMENT_TAG;
      
    }
    else if (c0_ == '/') { // </xml>
      advance();
      
      if (is_xml_element_start(c0_)) {
        
        scan_xml_tag_identifier();
        
        int c = c0_; advance();
        
        if (c == '>') {
          return XML_ELEMENT_TAG_END;
        }
        if (c == ':' && is_xml_element_start(c0_)) {
          next_->string_value.push(':');
          
          scan_xml_tag_identifier();
          
          if (c0_ == '>') {
            advance();
            return XML_ELEMENT_TAG_END;
          }
        }
      }
    }
    else {
      return LT;
    }
    
    return ILLEGAL;
  }
  
  void scan_xml_tag_identifier() {
    scan_identifier();
    while ( c0_ == '.') {
      advance();
      if ( is_xml_element_start(c0_) ) {
        next_->string_value.push('.');
        scan_identifier();
      } else {
        back(); break;
      }
    }
  }
  
  Token scan_xml_content_string(bool ignore_space) {
    do {
      if ( ignore_space && skip_white_space() ) {
        next_->string_value.push(' ');
      }
      if (c0_ == '\\') {
        advance();
        if (c0_ < 0 || !scan_string_escape()) return ILLEGAL;
      } else if (is_line_terminator(c0_)) {
        next_->string_scape.push( c0_ );
        // Allow CR+LF newlines in multiline string literals.
        if (is_carriage_return(c0_) && is_line_feed(c0_)) advance();
        // Allow LF+CR newlines in multiline string literals.
        if (is_line_feed(c0_) && is_carriage_return(c0_)) advance();
        next_->string_value.push('\\');
        next_->string_value.push('n');
        advance();
      } else if (c0_ == '"') {
        next_->string_value.push('\\'); // 转义
        next_->string_value.push('"');
        advance();
      } else if (c0_ == '<' || c0_ == '$' || c0_ == '%' || c0_ == '@') {
        break;
      } else {
        next_->string_value.push(c0_);
        advance();
      }
    } while(c0_ >= 0);
    
    return STRING_LITERAL;
  }
  
  Token skip_multi_line_comment() {
    advance();
    
    next_->string_scape.push('/');
    next_->string_scape.push('*');
    while (c0_ >= 0){
      int ch = c0_;
      advance();
      // If we have reached the end of the multi-line comment, we
      // consume the '/' and insert a whitespace. This way all
      // multi-line comments are treated as whitespace.
      if (ch == '*' && c0_ == '/') {
        advance();
        next_->string_scape.push('*');
        next_->string_scape.push('/');
        return WHITESPACE;
      } else {
        next_->string_scape.push(ch);
      }
    }
    // Unterminated multi-line comment.
    return ILLEGAL;
  }
  
  Token skip_single_line_comment() {
    advance();
    
    // The line terminator at the end of the line is not considered
    // to be part of the single-line comment; it is recognized
    // separately by the lexical grammar and becomes part of the
    // stream of input elements for the syntactic grammar (see
    next_->string_scape.push('/');
    next_->string_scape.push('/');
    while (c0_ >= 0 && !is_line_terminator(c0_)) {
      next_->string_scape.push(c0_);
      advance();
    }
    return WHITESPACE;
  }
  
  Token scan_string() {
    byte quote = c0_;
    next_->string_value = Ucs2String::empty;
    advance();  // consume quote
    next_->string_value.push(quote);
    
    while (c0_ != quote && c0_ >= 0 && !is_line_terminator(c0_)) {
      int c = c0_;
      advance();
      if (c == '\\') {
        if (c0_ < 0 || !scan_string_escape()) return ILLEGAL;
      } else {
        next_->string_value.push(c);
      }
    }
    if (c0_ != quote) return ILLEGAL;
    
    next_->string_value.push(quote);
    
    advance();  // consume quote
    return STRING_LITERAL;
  }
  
  bool scan_string_escape() {
    int c = c0_;
    advance();
    
    next_->string_value.push('\\');
    // Skip escaped newlines.
    if ( is_line_terminator(c) ) {
      // Allow CR+LF newlines in multiline string literals.
      if (is_carriage_return(c) && is_line_feed(c0_)) advance();
      // Allow LF+CR newlines in multiline string literals.
      if (is_line_feed(c) && is_carriage_return(c0_)) advance();
      next_->string_value.push('\n');
      return true;
    }
    if (c == 'u') {
      if (scan_hex_number(4) == -1) return false;
      next_->string_value.push(&code_[pos_ - 5], 5);
      return true;
    }
    next_->string_value.push(c);
    return true;
  }
  
  int scan_hex_number(int expected_length) {
    av_assert(expected_length <= 4);  // prevent overflow
    int x = 0;
    for (int i = 0; i < expected_length; i++) {
      
      if (!is_hex_digit(c0_)) { // 不是16进制字符
        for (int j = i - 1; j >= 0; j--) {
          back();
        }
        return -1;
      }
      x = x * 16 + hex_char_to_number(c0_);
      advance();
    }
    return x;
  }
  
  Token scan_number(bool seen_period) {
    Token tok = NUMBER_LITERAL;
    next_->string_value = Ucs2String::empty;
    
    if (seen_period) { // 浮点
      tok = scan_decimal_digit(true);
    }
    else if (c0_ == '0') {
      advance();
      next_->string_value = '0';
      
      if (c0_ < 0) { // 结束,10进制 0
        return tok;
      }
      switch (c0_) {
        case 'b': case 'B': // 0b 2进制
          next_->string_value.push(c0_);
          advance();
          if (is_binary_digit(c0_)) {
            do {
              next_->string_value.push(c0_);
              advance();
            } while(is_binary_digit(c0_));
          } else {
            return ILLEGAL;
          }
          break;

        case 'e': case 'E': // 0e+1 / 1e-6
          tok = scan_decimal_digit(false);
          break;
          
        case 'x': case 'X': // 0x 16进制
          next_->string_value.push(c0_);
          advance();
          if (is_hex_digit(c0_)) {
            do {
              next_->string_value.push(c0_);
              advance();
            } while(is_hex_digit(c0_));
          } else {
            return ILLEGAL;
          }
          break;
          
        case '.': // 10进制浮点数
          back();
          next_->string_value = Ucs2String::empty;
          tok = scan_decimal_digit(false);
          break;
          
        default:
          if (is_octal_digit(c0_)) { // 0 8进制
            do {
              next_->string_value.push(c0_);
              advance();
            } while(is_octal_digit(c0_));
          } // else 10进制 0
          break;
      }
    }
    else { // 10进制
      tok = scan_decimal_digit(false);
    }
    
    if (c0_ >= 0) {
      if (is_identifier_part(c0_) || c0_ == '.') {
        return ILLEGAL;
      }
    }
    return tok;
  }
  
  Token scan_decimal_digit(bool seen_period) {
    
    if (seen_period) { // 直接为浮点数
      next_->string_value.push('.');
      do {
        next_->string_value.push(c0_);
        advance();
      } while(is_decimal_digit(c0_));
    }
    else {
      while (is_decimal_digit(c0_)) { // 整数
        next_->string_value.push(c0_);
        advance();
      }
      
      if (c0_ == '.') { // 浮点数
        next_->string_value.push(c0_);
        advance();
        while (is_decimal_digit(c0_)) {
          next_->string_value.push(c0_);
          advance();
        }
      }
    }
    
    // int i = 1.9e-2;  科学记数法
    if (c0_ == 'e' || c0_ == 'E') {
      next_->string_value.push(c0_);
      advance();
      
      if (c0_ == '+' || c0_ == '-') {
        next_->string_value.push(c0_);
        advance();
      }
      
      if (is_decimal_digit(c0_)) {
        do {
          next_->string_value.push(c0_);
          advance();
        }
        while(is_decimal_digit(c0_));
      }
      else {
        return ILLEGAL;
      }
    }
    
    return NUMBER_LITERAL;
  }
  
  bool skip_white_space() {
    int start_position = pos_;
    while(true) {
      switch(c0_) {
        case 0x09: case 0x0A: case 0x0B: case 0x0C: case 0x0D: case 0x20:
          next_->string_scape.push(c0_);
          advance();
          break;
        default:
          return start_position != pos_;
      }
    }
    return false;
  }
  
  /*
   * Remove byte order marker. This catches EF BB BF (the UTF-8 BOM)
   * because the buffer-to-string conversion in `fs.readFileSync()`
   * translates it to FEFF, the UTF-16 BOM.
   */
  void strip_bom() {
    //0xFEFF
    //0xFFFE
    if (c0_ == 0xFEFF || c0_ == 0xFFFE) {
      advance();
    }
  }
  
  bool set_pos(uint pos) {
    av_assert(pos >= 0);
    
    if ( pos < size_ ) {
      if (pos > pos_) {
        do {
          if (c0_ == '\n') line_++;
          pos_++;
          c0_ = code_[pos_];
        } while (pos_ != pos);
      } else {
        while (pos_ != pos) {
          pos_--;
          c0_ = code_[pos_];
          if (c0_ == '\n') line_--;
        }
      }
    } else {
      return false;
    }
    return true;
  }
  
  inline void advance() {
    if (pos_ < size_) {
      pos_++;
      if (c0_ == '\n') line_++;
      c0_ = pos_ == size_ ? -1 : code_[pos_];
    }
    else c0_ = -1;
  }
  
  inline void back() { // undo advance()
    if (pos_ != 0) {
      pos_--;
      c0_ = code_[pos_];
      if (c0_ == '\n') line_--;
    }
  }
  
  inline Token select(Token tok) { advance(); return tok; }
  inline Token select(int next, Token then, Token else_) {
    advance();
    return c0_ == next ? advance(), then: else_;
  }
  
  const uint16*   code_;
  uint            size_;
  uint            pos_;
  uint            line_;
  int             c0_;
  TokenDesc*      current_;
  TokenDesc*      next_;
  Token           prev_;
};

static cUcs2String _SPACE(' ');
static cUcs2String _LT('<');
static cUcs2String _GT('>');
static cUcs2String _ADD('+');
static cUcs2String _SUB('-');
static cUcs2String _DIV('/');
static cUcs2String _ASSIGN('=');
static cUcs2String _PERIOD('.');
static cUcs2String _COMMAND(String("${"));
static cUcs2String _LBRACE('{');
static cUcs2String _RBRACE('}');
static cUcs2String _LBRACK('[');
static cUcs2String _RBRACK(']');
static cUcs2String _LPAREN('(');
static cUcs2String _RPAREN(')');
static cUcs2String _CONDITIONAL('?');
static cUcs2String _NOT('!');
static cUcs2String _OR('|');
static cUcs2String _BIT_NOT('~');
static cUcs2String _XOR('^');
static cUcs2String _MUL('*');
static cUcs2String _AND('&');
static cUcs2String _MOD('%');
static cUcs2String _AT('@');
static cUcs2String _QUOTES('"');
static cUcs2String _NEWLINE('\n');
static cUcs2String _TAG(String("__tag__")); // __tag__
static cUcs2String _CONST(String("const")); // const
static cUcs2String _VAR(String("var"));     // var
static cUcs2String _REQ(String("__req"));   // __req
static cUcs2String _COMMA(',');
static cUcs2String _COLON(':');
static cUcs2String _SEMICOLON(';');
static cUcs2String _PROTO(String("__proto__")); // __proto__
static cUcs2String _VX(String("vx"));       // vx
static cUcs2String _VALUE(String("value"));
static cUcs2String _DATA_BIND(String("new __bind((vd,ctr)=>{return")); // new __bind(func)
static cUcs2String _DATA_BIND_END(String("},0)"));      // },0)
static cUcs2String _DATA_BIND_END_ONCE(String("},1)")); // },1)
static cUcs2String _CHILDREN(String("__child__")); // __child__
static cUcs2String _XML_COMMENT(String("/***"));
static cUcs2String _XML_COMMENT_END(String("**/"));
static cUcs2String _EXPORT_COMMENT(String("/*export*/"));
static cUcs2String _EXPORTS(String("exports"));
static cUcs2String __EXPORT(String("__export"));

Ucs2String format_event_string(cUcs2String& name) {
  //  get onchange() { return this.get_noticer('change') }
  //  set onchange(func) { this.add_default_listener('change', func) }
  //  trigger_change(data) { return this.trigger('change', data) }
  //
  static cUcs2String a1(String("get on"));
  static cUcs2String a2(String("() { return this.get_noticer('"));
  static cUcs2String a3(String("') }"));
  static cUcs2String b1(String("set on"));
  static cUcs2String b2(String("(func) { this.add_default_listener('"));
  static cUcs2String b3(String("', func) }"));
  static cUcs2String c1(String("trigger_"));
  static cUcs2String c2(String("(event, is_event) { return is_event ? this.trigger_with_event('"));
  static cUcs2String c3(String("', event) : this.trigger('"));
  static cUcs2String c4(String("', event) }"));
  
  Ucs2String rv;
  rv.push(a1); rv.push(name); rv.push(a2); rv.push(name); rv.push(a3);
  rv.push(b1); rv.push(name); rv.push(b2); rv.push(name); rv.push(b3);
  rv.push(c1); rv.push(name); rv.push(c2); rv.push(name); rv.push(c3); rv.push(name); rv.push(c4);
  return rv;
}

/**
 * @class Parser
 */
class Parser: public Object {
public:
  
  Parser(cUcs2String& in, cString& path, bool is_jsx)
  : _path(path)
  , _is_jsx(is_jsx)
  , _level(0)
  {
    //String str = in.to_string();
    //LOG(str);
    _scanner = new Scanner(*in, in.length());
  }
  
  virtual ~Parser() {
    Release(_scanner);
  }
  
  Ucs2String transform() {
    parse_document();
    Ucs2String rv = _out.to_basic_string();
    //String str = rv.to_string();
    //LOG(str);
    return rv;
  }
  
private:
  
  void parse_document() {
    
    Token token = _scanner->next();
    
    if ( token == SHELL_HEADER ) {
      push_current_tokent_code();
      token = _scanner->next();
    }
    
    while (token != EOS) {
      if (token == EXPORT) {
        parse_export();
      } else {
        parse_declaration();
      }
      token = _scanner->next();
    }
    
    for (int i = 0; i < _exports.length(); i++) {
      _out.push(_NEWLINE);
      _out.push(_EXPORTS);    // exports.xxx=xxx;
      _out.push(_PERIOD);     // .
      _out.push(_exports[i]); //
      _out.push(_ASSIGN);     // =
      _out.push(_exports[i]); //
      _out.push(_SEMICOLON);  // ;
    }
    
    _out.push(_NEWLINE);
  }

  void parse_declaration() {
    Token tok = _scanner->token();
    
    push_current_tokent_scape();
    
    switch(tok) {
      case WHITESPACE:              // space
      case UNKNOWN:                 // unknown
      case INSTANCEOF:              // instanceof
      case IN:                      // in
      case OF:                      // of
      case RETURN:                  // return
      case TYPEOF:                  // typeof
      case VAR:                     // var
      case FUNCTION:                // function
      case LET:                     // let
      case DEFAULT:                 // let
      case CONST:                   // const
      case COMMAND_END:             // str`
      case EVENT:                   // event
      case ELSE:                    // else
        push_current_tokent_code(); break;
      case FROM:                    // from
      case AS:                      // as
      case IDENTIFIER:              // identifier
        push_current_tokent_code();
        parse_period_expression();  // xx.xx
        break;
      case CLASS:                   // class
        if ( _scanner->prev() == PERIOD ) { // .
          push_current_tokent_code();
          parse_period_expression();  // xx.xx
        } else {
          if ( _level == 0 ) {
            parse_class();
          } else { // class identifier
            push_current_tokent_code();
            parse_period_expression();  // xx.xx
          }
        }
        break;
      case IF:                      // if
        push_current_tokent_code();
        if ( next() == LPAREN ) {   // (
          _out.push(_LPAREN);
          parse_brace(LPAREN, RPAREN);
          _out.push(_RPAREN);
          if ( _scanner->peek() != LBRACE ) { // {
            next();
            parse_expression(); //
          }
        } else {
          error("Syntax error");
        }
        break;
      case LT:                      // <
        _out.push(_LT); break;
      case GT:                      // >
        _out.push(_GT); break;
      case ASSIGN:                  // =
        _out.push(_ASSIGN); break;
      case PERIOD:                  // .
        _out.push(_PERIOD); break;
      case ADD:                     // +
        _out.push(_ADD); break;
      case SUB:                     // -
        _out.push(_SUB); break;
      case COMMA:                   // ,
        _out.push(_COMMA); break;
      case COLON:                   // :
        _out.push(_COLON); break;
      case SEMICOLON:               // ;
        _out.push(_SEMICOLON); break;
      case CONDITIONAL:             // ?
        _out.push(_CONDITIONAL); break;
      case NOT:                     // !
        _out.push(_NOT); break;
      case OR:                     // |
        _out.push(_OR); break;
      case BIT_NOT:                 // ~
        _out.push(_BIT_NOT); break;
      case XOR:                     // ^
        _out.push(_XOR); break;
      case MUL:                     // *
        _out.push(_MUL); break;
      case AND:                     // &
        _out.push(_AND); break;
      case MOD:                     // %
        _out.push(_MOD); break;
      case XML_ELEMENT_TAG:         // <xml
        parse_xml_element(false); break;
      case XML_COMMENT:             // <!-- comment -->
        if (_is_jsx) {              //
          _out.push(_XML_COMMENT);
          _out.push(_scanner->string_value());
          _out.push(_XML_COMMENT_END);
        } else {
          error("Syntax error");
        }
        break;
      case LPAREN:                  // (
        _out.push(_LPAREN);
        parse_brace(LPAREN, RPAREN);
        _out.push(_RPAREN);
        parse_period_expression();
        break;
      case LBRACK:                  // [
        _out.push(_LBRACK);
        parse_brace(LBRACK, RBRACK);
        _out.push(_RBRACK);
        parse_period_expression();
        break;
      case LBRACE:                  // {
        _out.push(_LBRACE);
        parse_brace(LBRACE, RBRACE);
        _out.push(_RBRACE);
        break;
      case COMMAND:                 // `str${
        parse_command_string();
        parse_period_expression();
        break;
      case IMPORT:                  // import
         parse_import(); break;
      case DIV:                     // / regexp
        if (is_legal_const_prefix(false)) {
          parse_regexp();
        } else {
          _out.push(_DIV);
        }
        break;
      case NUMBER_LITERAL:          // number
      case STRING_LITERAL:          // string
        push_current_tokent_code();
        parse_period_expression();
        break;
      case ILLEGAL:                 // illegal
      default: error("Syntax error"); break;
    }
  }
  
  void parse_period_expression() {
    if (_scanner->peek() == PERIOD) { // 点记法调用函数或属性
      _scanner->next();
      push_current_tokent_scape();
      _out.push(_PERIOD); // .
      _scanner->next();
      parse_declaration();
    }
  }
  
  void parse_expression() {
    Token tok = _scanner->token();
    
    push_current_tokent_scape();
    
    switch (tok) {
      case DIV: // regexp
        parse_regexp();
        break;
      default:
        parse_declaration();
        break;
    }
  }
  
  void parse_xml_attribute_value_expression(bool assign) { // 只解析简单表达式,复杂表达式用(expression)包住
    switch(_scanner->token()) {
      case LPAREN:                  // (
        _out.push(_LPAREN); parse_brace(LPAREN, RPAREN); _out.push(_RPAREN);
        break;
      case LBRACK:                  // [
        _out.push(_LBRACK); parse_brace(LBRACK, RBRACK); _out.push(_RBRACK);
        break;
      case LBRACE:                  // {
        _out.push(_LBRACE); parse_brace(LBRACE, RBRACE); _out.push(_RBRACE);
        return ;
        
      case COMMAND:                 // `str${
        parse_xml_command(); return;
        
      case COMMAND_DATA_BIND:       // `str%%{
        parse_xml_command_data_bind(); return;
        
      case COMMAND_DATA_BIND_ONCE:  // `str%{
        parse_xml_command_data_bind_once(); return;
        
      case ADD:                     // +
      case SUB:                     // -
        if ( _scanner->peek() == NUMBER_LITERAL ) { // number
          _out.push(_scanner->token() == ADD ? _ADD: _SUB);
          _scanner->next();
          push_current_tokent_code();
          return;
        } else if ( _scanner->peek() == LPAREN ) {  // (
          _out.push(_scanner->token() == ADD ? _ADD: _SUB);
          next();
          _out.push(_LPAREN); parse_brace(LPAREN, RPAREN); _out.push(_RPAREN);
        }
        break;
      case DIV:                     // / regexp
        if ( assign ) {
          parse_regexp();
        } else {
          error("Syntax error");
        }
        break;
      case IDENTIFIER:              // identifier
      case STRING_LITERAL:          // string
      case COMMAND_END:             // `str${xxx}`
        push_current_tokent_code(); break;
      case NUMBER_LITERAL:          // number
        push_current_tokent_code(); return;
      case ILLEGAL:                 // illegal
      default: error("Syntax error"); break;
    }
    
    // PERIOD
    if (_scanner->peek() == PERIOD) { // 点记法调用函数或属性
      _scanner->next();
      push_current_tokent_scape();
      _out.push(_PERIOD); // .
      _scanner->next();
      parse_xml_attribute_value_expression(false); // 递归
    }
  }
  
  void parse_xml_command_data_bind() {
    _out.push(_DATA_BIND);      // new BindData(func)
    parse_xml_command();
    _out.push(_DATA_BIND_END);
  }
  
  void parse_xml_command_data_bind_once() {
    _out.push(_DATA_BIND); // new BindData(func, true)
    parse_xml_command();
    _out.push(_DATA_BIND_END_ONCE);
  }
  
  void parse_xml_command() {
    av_assert(_scanner->peek() == LBRACE);
    if (_scanner->string_value().is_empty()) { // 非`xxx${xx}xxx`字符串
      String space = _scanner->string_scape();
      _scanner->next();
      _out.push(_LPAREN);  // (
      parse_brace(LBRACE, RBRACE); //
      _out.push(_RPAREN);  // )
      if ( ! space.is_empty()) _out.push(space);
    } else {
      // LOG("%s", _scanner->string_value().to_string().c());
      parse_command_string();
    }
  }
  
  void parse_command_string() {
    if ( _scanner->string_value().is_empty()) {
      error("Syntax error");
    }
    
    String space = _scanner->string_scape();
    
    while(true) {
      av_assert(_scanner->peek() == LBRACE);
      _out.push(_scanner->string_value());
      _out.push(_COMMAND);  // ${
      _scanner->next();
      parse_command_string_block(); // parse { block }
      av_assert(_scanner->peek() == RBRACE);
      _out.push(_RBRACE);   // }
      _scanner->scan_command(_scanner->next_location().end_pos);
      Token tok = _scanner->next();
      if (tok == COMMAND_END) {
        push_current_tokent_code(); break;
      } else if (tok != COMMAND) {
        error("Syntax error");
      }
    }
    if (!space.is_empty()) _out.push(space);
  }
  
  void parse_command_string_block() {
    av_assert( _scanner->token() == LBRACE );
    while(true) {
      if (_scanner->peek() == RBRACE) {
        break;
      } else {
        if (next() == EOS) {
          error("Syntax error");
        } else {
          parse_declaration();
        }
      }
    }
    push_current_tokent_scape();
  }
  
  void parse_brace(Token begin, Token end) {
    av_assert( _scanner->token() == begin );
    uint level = _level;
    _level++;
    while(true) {
      Token token = next();
      if (token == end) {
        break;
      } else if (token == EOS) {
        error("Syntax error");
      } else {
        parse_declaration();
      }
    }
    _level--;
    if ( _level != level ) {
      error("Syntax error");
    }
    push_current_tokent_scape();
  }
  
  void parse_class() {
    av_assert(_scanner->token() == CLASS);
    
    push_current_tokent_code();
    
    Token tok = next();
    if (tok == IDENTIFIER) {      //
      push_current_tokent_code();
      
      tok = next();
      if (tok == EXTENDS) {
        push_current_tokent_code();
        
        while(true) { // find {
          tok = next();
          if (tok == LBRACE) { // {
            break;
          } else if (tok == EOS) {
            error("Syntax error");
          } else {
            parse_declaration();
          }
        }
        
      } else if (tok != LBRACE) { // {
        error("Syntax error");
      }
    } else if (tok == EXTENDS) {  // extends
      push_current_tokent_code();
      
      if (next() != LBRACE) {     // {
        error("Syntax error");
      }
    } else if (tok != LBRACE) {   // {
      error("Syntax error");
    }
    
    av_assert(_scanner->token() == LBRACE); // {
    
    _out.push(_LBRACE); // {
    
    while(true) {
      Token token = next();
      
      if (token == EVENT) {
        // event declaration
        if (next() == IDENTIFIER) { // event onevent
          Ucs2String event = _scanner->string_value();
          if (event.length() > 2 &&
              event[0] == 'o' &&
              event[1] == 'n' && is_xml_element_start(event[2])) {
            _out.push(format_event_string(event.substr(2)));
          } else {
            error("Syntax error, event name incorrect");
          }
        } else { // 这也可能是正确的,不做深入检查
          parse_declaration();
        }
      } else if (token == RBRACE) { // }
        break;
      } else if (token == EOS) {
        error("Syntax error");
      } else {
        parse_declaration();
      }
    }
    
    _out.push(_RBRACE); // {
  }
  
  void parse_export() {
    av_assert(_scanner->token() == EXPORT);
    
    push_current_tokent_scape();
    Token tok = _scanner->next();
    
    switch (tok) {
      case VAR:       // var
      case CLASS:     // class
      case FUNCTION:  // function
      case LET:       // let
      case CONST:     // const
        if (_scanner->peek() == IDENTIFIER) {
          _exports.push(_scanner->next_string_value());
          _out.push(_EXPORT_COMMENT);
          parse_declaration();
        } else
          error("Syntax error");
        break;
      case IDENTIFIER:              // identifier
      case LPAREN:                  // (
      case LBRACK:                  // [
      case LBRACE:                  // {
        _out.push(__EXPORT);        // __export=
        _out.push(_ASSIGN);
        parse_declaration();
        break;
      default: error("Syntax error"); break;
    }
  }
  
  void parse_import() {
    av_assert(_scanner->token() == IMPORT);
    
    Token tok = _scanner->next();

    if (tok == IDENTIFIER) { // identifier
      _out.push(_VAR); // var
      push_current_tokent_scape();
      
      tok = _scanner->peek();
      
      if (tok == AS) {
        // import GUIApplication as GUIApp from ':gui/app';
        Ucs2String id = _scanner->string_value();
        _scanner->next(); // as
        if (_scanner->peek() == IDENTIFIER) { // identifier
          _out.push(_scanner->next_string_value());
          push_current_tokent_scape();
          _out.push(_ASSIGN); // =
          next();
          if (next() == FROM && next() == STRING_LITERAL) { // from
            _out.push(_REQ); // __req(':gui/app').GUIApplication;
            _out.push(_LPAREN); // (
            push_current_tokent_code();
            _out.push(_RPAREN); // )
            _out.push(_PERIOD); // .
            _out.push(id);      // GUIApplication
          } else {
            error("Syntax error");
          }
        } else {
          error("Syntax error");
        }
      } else if (tok == FROM) {
        // import GUIApplication from ':gui/app';
        Ucs2String id = _scanner->string_value();
        _out.push(id);    // GUIApplication
        next();           // from
        _out.push(_ASSIGN); // =
        if (next() == STRING_LITERAL) {
          _out.push(_REQ); // __req(':gui/app').GUIApplication;
          _out.push(_LPAREN); // (
          push_current_tokent_code();
          _out.push(_RPAREN); // )
          _out.push(_PERIOD); // .
          _out.push(id);      // GUIApplication
        } else {
          error("Syntax error");
        }
      } else {
        error("Syntax error");
      }
    }
    else if (tok == LBRACE) { // {
      _out.push(_VAR); // var
      push_current_tokent_scape();
      
      // import { GUIApplication } from ':gui/app';
      parse_declaration();
      if (next() == FROM) {
        _out.push(_ASSIGN); // =
        if (next() == STRING_LITERAL) {
          _out.push(_REQ); // __req(':gui/app');
          _out.push(_LPAREN); // (
          push_current_tokent_code();
          _out.push(_RPAREN); // )
        } else {
          error("Syntax error");
        }
      } else {
        error("Syntax error");
      }
    }
    else if (tok == STRING_LITERAL) {
    
      Ucs2String str = _scanner->string_value();
      if (_scanner->peek() == AS) {
        // import 'test_gui.jsx' as gui;
        
        _out.push(_VAR); // var
        push_current_tokent_scape();
        next(); // as
        if (_scanner->peek() == IDENTIFIER) {
          _out.push(_scanner->next_string_value());
          _out.push(_SPACE); //
          _out.push(_ASSIGN); // =
          next(); // IDENTIFIER
          _out.push(_REQ); // __req(':gui/app');
          _out.push(_LPAREN); // (
          _out.push(str);
          _out.push(_RPAREN); // )
        } else {
          error("Syntax error");
        }
      } else {
        // import 'test_gui.jsx';
        // find identifier
        
        Ucs2String path = str.substr(1, str.length() - 2).trim();
        String basename = Path::basename(path);
        int i = basename.last_index_of('.');
        if (i != -1) {
          basename = basename.substr(0, i);
        }
        
        if ( basename[0] == ':' ) { // :
          basename = basename.substr(1);
        } else {
          i = basename.last_index_of("lib://"); // lib://gui
          if (i == 0) {
            basename = basename.substr(6);
          }
        }
        basename = basename.replace_all('.', '_').replace_all('-', '_');
        
        if (is_identifier_start(basename[0])) {
          i = basename.length();
          while (--i) {
            if (!is_identifier_part(basename[i])) {
              basename = String::empty; break;
            }
          }
          // use the identifier
        } else {
          basename = String::empty;
        }
        
        if (!basename.is_empty()) {
          _out.push(_VAR); // var
          _out.push(_SPACE); //
          _out.push(Coder::decoding_to_uint16(Encoding::utf8, basename)); // identifier
          _out.push(_SPACE); //
          _out.push(_ASSIGN); // =
          _out.push(_SPACE);  //
        }
        _out.push(_REQ); // __req(':gui/app');
        _out.push(_LPAREN); // (
        _out.push(str);
        _out.push(_RPAREN); // )
        push_current_tokent_scape();
      }
    }
    else { // 这可能是个函数调用,不理会
      // import();
      // error("Syntax error");
      push_current_tokent_code();
      parse_period_expression();
    }
  }
  
  bool is_operator(Token token) {
    
    switch (token) {
      case LT:                      // <
      case GT:                      // >
      case ASSIGN:                  // =
      case ADD:                     // +
      case SUB:                     // -
      case MUL:                     // *
      case DIV:                     // /
      case MOD:                     // %
      case NOT:                     // !
      case OR:                      // |
      case BIT_NOT:                 // ~
      case XOR:                     // ^
      case AND:                     // &
      case INSTANCEOF:              // instanceof
      case IN:                      // in
      case OF:                      // of
      case TYPEOF:                  // typeof
        return true;
      default: break;
    }
    
    return false;
  }
  
  bool is_legal_const_prefix(bool xml) {
    Token token = _scanner->prev();
    
    if ( xml ) {
      switch (token) {
        case LT:          // <
        case ADD:         // if (i++<a.length) { }
        case SUB:         // if (i--<a.length) { }
          return false;
          break;
        default: break;
      }
    }
    
    if ( is_operator(token) ) {
      return true;
    }
    
    switch (token) {
      // 上一个词为这些时,下一个词可以为 expression
      case LBRACE:          // {
      case RBRACE:          // }
      case LPAREN:          // (
      case LBRACK:          // [
      case COMMA:           // ,
      case RETURN:          // return
      case ELSE:            // else
      case COLON:           // :
      case SEMICOLON:       // ;
      case CONDITIONAL:     // ?
      case ASSIGN:          // =
        return true;
      case RPAREN:          // )
      case RBRACK:          // ]
        if ( _scanner->before_line_feed() ) {
          return true;
        }
      default: break;
    }
    
    return false;
  }
  
  void parse_regexp() {
    av_assert(_scanner->token() == DIV);
    
    if (_scanner->scan_regexp(_scanner->location().beg_pos) == REGEXP_LITERAL) {
      _scanner->next();
      push_current_tokent_code();
      parse_period_expression();
    } else {
      error("RegExp Syntax error");
    }
  }
  
  bool is_xml_attributes_name(Token token) {
    switch(token) {
      case IDENTIFIER:
      case AS:
      case EXPORT:
      case FROM:
      case IMPORT:
      case INSTANCEOF:
      case IN:
      case OF:
      case RETURN:
      case TYPEOF:
      case VAR:
      case CLASS:
      case FUNCTION:
      case LET:
      case CONST:
        return true;
      default: return false;
    }
  }
  
  void parse_xml_element(bool xml_inl) {
    av_assert(_scanner->token() == XML_ELEMENT_TAG);
    
    if (!_is_jsx || (!xml_inl && !is_legal_const_prefix(true)) ) {
      _out.push(_LT);
      push_current_tokent_code();
      return;
    } else {
      Token token = _scanner->peek();
      if ( token == PERIOD || token == COLON ) { // not xml
        _out.push(_LT);
        push_current_tokent_code();
        return;
      }
    }
    
    push_current_tokent_scape(); // push scape
    
    Token token;
    
    // 转换xml为json对像
    _out.push(_LBRACE); // {
    
    Ucs2String tag_name = _scanner->string_value();
    int index = tag_name.index_of(':');
    bool vx_com = false;
    
    if (index != -1) {
      Ucs2String prefix = tag_name.substr(0, index);
      Ucs2String suffix = tag_name.substr(index + 1);
      
      if (prefix == _VX) {    // <vx:xxxx
        _out.push(_PROTO);    // { __proto__
        _out.push(_COLON);    // :
        _out.push(suffix);    // xxxx
        vx_com = true;
      } else {                // <prefix:suffix
        _out.push(_TAG);        // __tag__
        _out.push(_COLON);      // :
        _out.push(prefix);      // prefix
        _out.push(_COMMA);      // ,
        _out.push(_VALUE);      // value
        _out.push(_COLON);      // :
        _out.push(_QUOTES);     // "
        _out.push(suffix);      // suffix
        _out.push(_QUOTES);     // "
      }
    } else {              // { __tag__: view
      _out.push(_TAG);    // __tag__
      _out.push(_COLON);  // :
      _out.push(tag_name);
    }
    
    // 先解析xml属性部分
     while(true) {
      token = _scanner->next();
      
      if (is_xml_attributes_name(token)) {
      attr:
        if (!_scanner->has_scape_before()) { // xml属性之间必须要有空白符号
          error("Xml Syntax error");
        }
        
        Array<Ucs2String> attribute_name;
        
        // 添加属性
        _out.push(_COMMA);   // ,
        push_current_tokent_scape(); // scape
        attribute_name.push(_scanner->string_value());
        token = next();
        
        while (token == PERIOD) { // .
          if ( is_xml_attributes_name(next()) ) {
            attribute_name.push(_scanner->string_value());
          } else {
            error("Xml Syntax error");
          }
          token = next();
        }
        _out.push(_QUOTES); // "
        _out.push(attribute_name.join('.')); // key
        _out.push(_QUOTES); // "
        _out.push(_COLON);  // :
        
        if (token == ASSIGN) { // =
          // 有=符号,属性必须要有值,否则为异常
          next();
          parse_xml_attribute_value_expression(true); // 解析属性值表达式
          token = _scanner->next();
        } else { // 没有值设置为 ""
          _out.push(_QUOTES);  // "
          _out.push(_QUOTES);  // "
        }
        if (is_xml_attributes_name(token)) {
          goto attr; // 重新开始新属性
        }
      }
      if (token == DIV) {      // /  没有内容结束
        if (_scanner->next() != GT) { // >  语法错误
          error("Xml Syntax error");
        }
        
        if ( !vx_com ) {
          // add chileren
          _out.push(_COMMA);    // ,
          _out.push(_CHILDREN); // children
          _out.push(_COLON);    // :
          _out.push(_LBRACK);   // [
          _out.push(_RBRACK);   // ]
        }
        
        break; // end
      } else if (token == GT) {       //   >  闭合标签,开始解析内容
        if ( vx_com ) { // view xml component cannot have child views.
          error("View xml component cannot have child views.");
        } else {
          // 解析子内容以及子节点
          parse_xml_element_context(tag_name); break;
        }
      } else {
        error("Xml Syntax error");
      }
    }
    
    _out.push(_RBRACE); // }
  }
  
  void complete_xml_content_string(Ucs2StringBuilder& str,
                                   Ucs2StringBuilder& scape,
                                   bool& is_comma, bool ignore_space) {
    if ( scape.string_length() ) {
      _out.push(move(scape));
    }
    if (str.string_length()) {
      Ucs2String s = str.to_basic_string();
      if ( !ignore_space || ! s.is_blank() ) {
        add_xml_children_cut_comma(is_comma);
        _out.push(_QUOTES);   // "
        _out.push(s);
        _out.push(_QUOTES);   // "
      }
      str.clear();
    }
  }
  
  void add_xml_children_cut_comma(bool& is_comma) {
    if (is_comma) {
      _out.push(_COMMA);     // ,
    } else {
      is_comma = true;
    }
  }
  
  void parse_xml_element_context(cUcs2String& tag_name) {
    av_assert(_scanner->token() == GT);  // >
    
    // add chileren
    _out.push(_COMMA);    // ,
    _out.push(_CHILDREN); // children
    _out.push(_COLON);    // :
    _out.push(_LBRACK);   // [
    
    Token token, prev = UNKNOWN;
    Ucs2StringBuilder str, scape;
    bool ignore_space = true;
    uint pos = _scanner->location().end_pos;
    bool is_comma = false;
    
    while(true) {
      token = _scanner->scan_xml_content(ignore_space, pos);
      pos = _scanner->next_location().end_pos;

      switch (token) {
        case XML_COMMENT:    // <!-- comment -->
          /* ignore comment */
          scape.push(_XML_COMMENT);
          scape.push(_scanner->next_string_value());
          scape.push(_XML_COMMENT_END);
          break;
          
        case XML_ELEMENT_TAG: // <xml
          complete_xml_content_string(str, scape, is_comma, ignore_space);
          add_xml_children_cut_comma(is_comma);
          _scanner->next();
          parse_xml_element(true);
          pos = _scanner->location().end_pos;
          break;
          
        case XML_ELEMENT_TAG_END:    // </xml>
          complete_xml_content_string(str, scape, is_comma, ignore_space);
          if (tag_name != _scanner->next_string_value()) {
            error(String::format("Xml Syntax error, The end of the unknown, <%s> ... </%s>",
                                 *to_utf8(tag_name),
                                 *to_utf8(_scanner->next_string_value())) );
          }
          _out.push(_RBRACK);     // ]
          _scanner->next();
          return;
          
        case XML_NO_IGNORE_SPACE: // @@
          complete_xml_content_string(str, scape, is_comma, ignore_space);
          ignore_space = !ignore_space;
          break;
          
        case LT: // <
          str.push(_scanner->next_string_value()); break;
          
        case COMMAND: // ${command}
          complete_xml_content_string(str, scape, is_comma, ignore_space);
          add_xml_children_cut_comma(is_comma);
          _scanner->next();     // command
          _scanner->next();     // {
          _out.push(_LPAREN);  // (
          parse_brace(LBRACE, RBRACE); //
          _out.push(_RPAREN);  // )
          pos = _scanner->location().end_pos;
          break;
          
        case COMMAND_DATA_BIND: // %%{command}
          complete_xml_content_string(str, scape, is_comma, ignore_space);
          add_xml_children_cut_comma(is_comma);
          _scanner->next();     // command
          _scanner->next();     // {
          _out.push(_DATA_BIND);      // new BindData(func)
          _out.push(_LPAREN);  // (
          parse_brace(LBRACE, RBRACE); //
          _out.push(_RPAREN);  // )
          _out.push(_DATA_BIND_END);  // {{]
          pos = _scanner->location().end_pos;
          break;
          
        case COMMAND_DATA_BIND_ONCE: // %{command}
          complete_xml_content_string(str, scape, is_comma, ignore_space);
          add_xml_children_cut_comma(is_comma);
          _scanner->next();     // command
          _scanner->next();     // {
          _out.push(_DATA_BIND);      // new BindData(func,true)
          _out.push(_LPAREN);  // (
          parse_brace(LBRACE, RBRACE); //
          _out.push(_RPAREN);  // )
          _out.push(_DATA_BIND_END_ONCE);  // {{]
          pos = _scanner->location().end_pos;
          break;
          
        case STRING_LITERAL:   // xml context text
          if (!_scanner->next_string_scape().is_empty())
            scape.push(_scanner->next_string_scape());
          str.push(_scanner->next_string_value());
          break;
        default: error("Xml Syntax error", _scanner->next_location()); break;
      }
    }
  }
  
  Buffer to_utf8(cUcs2String s) {
    return Coder::encoding(Encoding::utf8, s);
  }
  
  void error(cString& msg) {
    error(msg, _scanner->location());
  }
  
  void error(cString& msg, Scanner::Location loc) {
    // av_debug(JSX, "%s \nline:%d, %s", *msg, loc.line + 1, *_path);
    av_throw(ERR_SYNTAX_ERROR,
             "%s, Invalid or unexpected token\nline:%d, pos:%d, %s",
             *msg, loc.line + 1, loc.end_pos, *_path);
  }
  
  Token next() {
    Token tok = _scanner->next();
    push_current_tokent_scape();
    return tok;
  }
  
  void push_current_tokent_scape() {
    if (!_scanner->string_scape().is_empty()) {
      _out.push(move(_scanner->string_scape()));
    }
  }
  
  void push_current_tokent_code() {
    push_current_tokent_scape();
    _out.push(_scanner->string_value());
  }
  
  Scanner*          _scanner;
  Ucs2StringBuilder _out;
  cString&          _path;
  bool              _is_jsx;
  Array<Ucs2String> _exports;
  uint              _level;
};

Ucs2String Jsa::transform_jsax(cUcs2String& in, cString& path) av_def_err {
  return Parser(in, path, true).transform();
}

Ucs2String Jsa::transform_jsa(cUcs2String& in, cString& path) av_def_err {
  return Parser(in, path, false).transform();
}

av_end
