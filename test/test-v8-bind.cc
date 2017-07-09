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

#include <autil/util.h>
#include <autil/string.h>
#include <av8/v8.h>
#include <autil/fs.h>
#include <autil/http.h>
#include <autil/handle.h>

using namespace avocado;
using namespace avocado::av8;

Local<Value> run_module_script(Worker* worker,
                               Local<v8::String> name,
                               Local<v8::String> source_string, Local<Context> ctx) {
  TryCatch try_catch(worker->isolate());
  ScriptOrigin origin(name);
  if ( ctx.IsEmpty() ) {
    ctx = worker->context();
  }
  
  Local<v8::Script> script;
  v8::ScriptCompiler::Source source(source_string, ScriptOrigin(name));
  
  if ( v8::ScriptCompiler::CompileModule(ctx, &source).ToLocal(&script) ) {
    v8::Local<v8::Value> result;
    
    script = script->GetUnboundScript()->BindToCurrentContext();
    
    if ( ! script->Run(ctx).ToLocal(&result) ) {
      worker->report_exception(&try_catch);
    } else {
      return result;
    }
  } else {
    worker->report_exception(&try_catch);
  }
  return Local<Value>();
}

Local<Value> run_module_script(Worker* worker,
                               cString& name,
                               cString& source, Local<Context> ctx) {
  return run_module_script(worker,
                           worker->New(name)->ToString(),
                           worker->New(source)->ToString(), ctx);
}

void test_v8_bind_module() {
  avocado::Handle<Worker> worker = Worker::create();
  
  avocado::String path2 = FileSearch::shared()->get_absolute_path("res/test-v8-bind-module.js");
  avocado::String source2 = FileSearch::shared()->read("res/test-v8-bind-module.js");
  
  HandleScope scope(worker->isolate());
  
  Local<v8::Context> ctx = v8::Context::New(worker->isolate());
  v8::Context::Scope ctx_scope(ctx);
  
  Local<Value> rev = run_module_script(*worker, path2, source2, ctx);
  
  if (rev.IsEmpty()) {
    LOG("empty");
  } else {
    LOG( worker->to_string_utf8(rev) );
  }
}

void test_v8_bind() {
  start( Path::resources("res/test-v8-bind.js") );
  test_v8_bind_module();
}
