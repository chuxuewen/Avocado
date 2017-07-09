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

#include "json-1.h"
#include "gui.h"
#include "agui/view.h"

/**
 * @ns avocado::av8
 */

av8_begin

static cString Undefined("undefined");
static cString Null("null");
static cString True("true");
static cString False("false");
static cString Circular("[Circular]");
static cString Newline('\n');
static cString Comma(',');
static cString Space(' ');
static cString LBrack('[');
static cString RBrack(']');
static cString LBrace('{');
static cString RBrace('}');
static cString COLON(':');
static cString ELLIPSIS(" ... ");
static cString Quotes('"');
static cString BufferPrefix("<Buffer");
static cString GT(">");
static cString FUNCTION("[Function]");
static cString ARRAY("[Array]");
static cString OBJECT("[Object]");

/**
 * @class JSON
 */
class InlJSON {
  uint _indent;
  StringBuilder* _rv;
  Worker* _worker;
  Local<v8::Value> _mark_key;
  
  void push_indent() {
    for (int i = 0; i < _indent; i++) {
      _rv->push(Space);
    }
  }
  
  bool stringify_object(Local<v8::Object> arg) {
    _rv->push(LBrace);
    Local<v8::Array> names = arg->GetPropertyNames();
    if ( names->Length() > 1 ) {
      _indent += 2;
      for (int i = 0, j = 0; i < names->Length(); i++) {
        Local<v8::Value> key = names->Get(i);
        if ( ! key->Equals(_mark_key) ) {
          if (j > 0) _rv->push(Comma); // ,
          _rv->push(Newline); push_indent();
          //  _rv->push(Quotes);
          _rv->push(_worker->to_string_utf8(key));
          //  _rv->push(Quotes);
          _rv->push(COLON); _rv->push(Space);
          bool rv = stringify( arg->Get(key), false ); // value
          if ( ! rv ) return false;
          j++;
        }
      }
      _indent -= 2;
      _rv->push(Newline); push_indent();
    } else {
      _rv->push(Space);
    }
    _rv->push(RBrace);
    return true;
  }
  
  bool stringify_array(Local<v8::Array> arg) {
    _rv->push(LBrack);
    if (arg->Length() > 0) {
      _indent += 2;
      for (int i = 0; i < arg->Length(); i++) {
        if (i > 0)
          _rv->push(Comma);
        _rv->push(Newline);
        push_indent();
        stringify( arg->Get(i), false ); // value
      }
      _indent -= 2;
      _rv->push(Newline);
      push_indent();
    } else {
      _rv->push(Space);
    }
    _rv->push(RBrack);
    return true;
  }
  
  bool stringify_buffer(Local<v8::Object> arg) {
    Buffer* buf = Wrap<Buffer>::unwrap(arg)->self();
    _rv->push(BufferPrefix);
    cchar* hex = "0123456789abcdef";
    byte* s = (byte*)buf->value();
    for (int i = 0; i < buf->length(); i++) {
      byte ch = s[i];
      _rv->push(Space);
      _rv->push( hex[ch >> 4] );
      _rv->push( hex[ch & 15] );
      if (i > 50) {
        _rv->push(ELLIPSIS); break;
      }
    }
    _rv->push(GT);
    return true;
  }
  
  bool stringify_view(Local<v8::Object> arg) {
    _rv->push(LBrace);
    Local<v8::Array> names = arg->GetPropertyNames();
    if ( names->Length() > 0 ) {
      _indent += 2;
      for (int i = 0, j = 0; i < names->Length(); i++) {
        Local<v8::Value> key = names->Get(i);
        if ( ! key->Equals(_mark_key) ) {
          if (j > 0) _rv->push(Comma); // ,
          _rv->push(Newline); push_indent();
          //  _rv->push(Quotes);
          _rv->push(_worker->to_string_utf8(key));
          //  _rv->push(Quotes);
          _rv->push(COLON); _rv->push(Space);
          bool rv = stringify( arg->Get(key), true ); // value
          if ( ! rv ) return false;
          j++;
        }
      }
      _indent -= 2;
      _rv->push(Newline); push_indent();
    } else {
      _rv->push(Space);
    }
    _rv->push(RBrace);
    return true;
  }
  
  bool stringify(Local<v8::Value> arg, bool leaf) {
    if (arg.IsEmpty()) { // error
      return false;
    }
    
    bool rv = true;
    
    if(arg->IsString()) {
      _rv->push(Quotes);
      _rv->push( _worker->to_string_utf8(arg) );
      _rv->push(Quotes);
    } else if (arg->IsFunction()) {
      _rv->push( FUNCTION );
    } else if (arg->IsObject()) {
      
      Local<v8::Object> o = arg.As<v8::Object>();
      if (_worker->has_buffer(arg)) {
        rv = stringify_buffer(o);
      } else if (_worker->gui_value_program() && _worker->gui_value_program()->is_base(arg)) {
        _rv->push(Quotes);
        _rv->push( _worker->to_string_utf8(o) );
        _rv->push(Quotes);
      }
      else if (leaf) {
        if (arg->IsArray()) {
          _rv->push(ARRAY);
        } else {
          _rv->push(OBJECT);
        }
      }
      else if (_worker->has(arg, gui::View::VIEW)) {
        rv = stringify_view(o);
      }
      else {
        if ( o->Has(_mark_key) ) {
          _rv->push( Circular ); return true;
        }
        o->Set( _mark_key, v8::True(_worker->isolate()) );
        //
        if (arg->IsArray()) {
          rv = stringify_array(o.As<v8::Array>());
        } else {
          rv = stringify_object(o);
        }
        //
        o->Delete(_mark_key);
      }
    } else if(arg->IsInt32()) {
      _rv->push( String(arg->ToInt32()->Value()) );
    } else if(arg->IsNumber()) {
      _rv->push( String(arg->ToNumber()->Value()) );
    } else if(arg->IsBoolean()) {
      if (arg->IsTrue()) {
        _rv->push(True);
      } else {
        _rv->push(False);
      }
    } else if(arg->IsDate()) {
      _rv->push(Quotes);
      Local<v8::Function> f =
      arg->ToObject()->Get(_worker->strs()->toJSON()).As<Function>();
      _rv->push( _worker->to_string_ascii(f->Call(arg, 0, NULL)) );
      _rv->push(Quotes);
    } else if(arg->IsNull()) {
      _rv->push(Null);
    } else if(arg->IsUndefined()) {
      _rv->push(Undefined);
    }
    return true;
  }
  
public:
  InlJSON(Worker* worker) : _indent(0), _worker(worker), _rv(NULL) {
    _mark_key = _worker->strs()->___mark_json_stringify__();
  }
  
  bool stringify_console_styled(Local<v8::Value> arg, StringBuilder* out) {
    v8::HandleScope scope(_worker->isolate());
    _rv = out;
    return stringify(arg, false);
  }
};

bool JSON::stringify_console_styled(Worker* worker, Local<v8::Value> arg, StringBuilder* out) {
  return InlJSON(worker).stringify_console_styled(arg, out);
}

av8_end
