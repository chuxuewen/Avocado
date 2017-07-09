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

#ifndef __avocado_event__
#define __avocado_event__

#include "list.h"
#include "map.h"
#include "util.h"
#include "string.h"
#include "error.h"
#include <functional>

#define av_event(name, ...)                                                            \
public: inline avocado::EventNoticer<__VA_ARGS__>& name () { return _##name; }         \
private:avocado::EventNoticer<__VA_ARGS__>_##name; public:

#define av_init_event(name)  _on##name(#name, this)
#define av_on(name, ...)      on##name().on( __VA_ARGS__ )
#define av_once(name, ...)    on##name().once( __VA_ARGS__ )
#define av_off(name, ...)     on##name().off( __VA_ARGS__ )
#define av_trigger(name, ...)  on##name().trigger( __VA_ARGS__ )

/**
 * @ns avocado
 */

av_ns(avocado)

template<class SendData = Object, class Sender = Object, class ReturnValue = int> class Event;
template<class Event = Event<>> class EventNoticer;

template<class SendData_, class Sender_, class ReturnValue_> class Event: public Object {
  av_hidden_all_copy(Event);
public:
  
  typedef SendData_               SendData;
  typedef Sender_                 Sender;
  typedef ReturnValue_            ReturnValue;
  typedef const SendData          cSendData;
  typedef EventNoticer<Event>     Noticer;

  class _BasicEventNoticer: public Object {
  protected:
    inline static void set_event(Noticer* noticer, Event& evt) {
      evt.noticer_ = noticer;
    }
    inline static void clear_event(Event& evt) {
      evt.noticer_ = nullptr;
      evt.data_ = nullptr;
    }
  };
  friend class _BasicEventNoticer;
  
  ReturnValue   return_value; private:
  Noticer*      noticer_;
  cSendData*    data_;
  
public:
  Event(cSendData& data = SendData())
  : noticer_(nullptr) , data_(&data) , return_value() { }
  inline Noticer* noticer() { return this->noticer_; }
  inline String name() const { return this->noticer_->name_; }
  inline Sender* sender() { return this->noticer_->sender_; }
  inline cSendData* data() const { return data_; }
};

template<class Event> class EventNoticer: public Event::_BasicEventNoticer {
public:
  typedef typename Event::SendData        SendData;
  typedef typename Event::cSendData       cSendData;
  typedef typename Event::Sender          Sender;
  typedef typename Event::ReturnValue     ReturnValue;
  typedef std::function<void(Event&)>     ListenerFunc;
protected:
  class Listener {
  public:
    inline Listener(EventNoticer* noticer) { noticer_ = noticer; }
    virtual ~Listener() { }
    virtual void call(Event& evt) = 0;
    virtual bool is_on_listener() { return false; }
    virtual bool is_on_static_listener() { return false; }
    virtual bool is_on_shell_listener() { return false; }
    virtual bool is_on_func_listener() { return false; }
  protected:
    EventNoticer* noticer_;
  };
  
  // On
  template<class Scope> class OnListener: public Listener {
  public:
    typedef void (Scope::*ListenerFunc)(Event& evt);
    inline OnListener(EventNoticer* noticer, ListenerFunc listener, Scope* scope)
    : Listener(noticer), scope_(scope), listener_( listener ){ }
    virtual bool is_on_listener() { return true; }
    virtual void call(Event& evt) { (scope_->*listener_)(evt); }
    inline bool equals(ListenerFunc listener) { return listener_ == listener; }
    inline bool equals(Scope* scope) { return scope_ == scope; }
  protected:
    Scope* scope_;
    ListenerFunc listener_;
  };

  // ONCE
  template<class Scope> class OnceListener: public OnListener<Scope> {
  public:
    typedef typename OnListener<Scope>::ListenerFunc ListenerFunc;
    inline OnceListener(EventNoticer* noticer, ListenerFunc listener, Scope* scope)
    : OnListener<Scope>(noticer, listener, scope) { }
    virtual void call(Event& evt) {
      (this->scope_->*this->listener_)(evt);
      this->noticer_->off_(this);
    }
  };
  
  // STATIC
  template<class Data> class OnStaticListener: public Listener {
  public:
    typedef void (*ListenerFunc)(Event& evt, Data* data);
    inline OnStaticListener(EventNoticer* noticer, ListenerFunc listener, Data* data)
    : Listener(noticer), listener_(listener), data_(data) { }
    virtual bool is_on_static_listener() { return true; }
    virtual void call(Event& evt) { listener_(evt, data_); }
    inline bool equals(ListenerFunc listener) { return listener_ == listener; }
    inline bool equals(Data* data) { return data_ == data; }
  protected:
    ListenerFunc listener_;
    Data* data_;
  };
  
  // Once STATIC
  template<class Data> class OnceStaticListener: public OnStaticListener<Data> {
  public:
    typedef typename OnStaticListener<Data>::ListenerFunc ListenerFunc;
    inline OnceStaticListener(EventNoticer* noticer, ListenerFunc listener, Data* data)
    : OnStaticListener<Data>(noticer, listener, data) { }
    virtual void call(Event& evt) {
      this->listener_(evt);
      this->noticer_->off_(this);
    }
  };
  
  // Function
  class OnLambdaFunctionListener: public Listener {
  public:
    inline OnLambdaFunctionListener(EventNoticer* noticer, ListenerFunc&& listener, int id)
    : Listener(noticer), listener_(avocado::move(listener)), id_(id) { }
    virtual bool is_on_func_listener() { return true; }
    virtual void call(Event& evt) { listener_(evt); }
    inline bool equals(int id) { return id == id_; }
  protected:
    ListenerFunc listener_;
    int id_;
  };
  
  // Once Function
  class OnceLambdaFunctionListener: public OnLambdaFunctionListener {
  public:
    inline OnceLambdaFunctionListener(EventNoticer* noticer, ListenerFunc&& listener, int id)
    : OnLambdaFunctionListener(noticer, avocado::move(listener), id) { }
    virtual void call(Event& evt) {
      this->listener_(evt);
      this->noticer_->off_(this);
    }
  };
  
  // SHELL
  class OnShellListener: public Listener {
  public:
    inline OnShellListener(EventNoticer* noticer, EventNoticer* shell)
    : Listener(noticer), shell_(shell) { }
    virtual bool is_on_shell_listener() { return true; }
    virtual void call(Event& evt) {
      this->shell_->trigger(evt);
      this->noticer_->set_event_(evt);
    }
    inline bool equals(EventNoticer* shell) { return shell_ == shell; }
  protected:
    EventNoticer* shell_;
  };
  
  // Once Shell
  class OnceShellListener: public OnShellListener {
  public:
    inline OnceShellListener(EventNoticer* noticer, EventNoticer* shell)
    : OnShellListener(noticer, shell) { }
    virtual void action(Event& evt) {
      this->shell_->trigger(evt);
      this->noticer_->set_event_(evt);
      this->noticer_->off_(this);
    }
  };
  
private:
  
  av_hidden_all_copy(EventNoticer);
  typedef typename List<Listener*>::Iterator iterator;
  struct LWrap {
    Listener* listener;
    Listener* operator->() { return listener; }
    Listener* value() { return listener; }
    void del() { delete listener; listener = nullptr; }
  };
  String        name_;
  Sender*       sender_;
  List<LWrap>*  listener_;
  friend class  avocado::Event<SendData, Sender, ReturnValue>;
  friend class  OnShellListener;
  friend class  OnceShellListener;
  
public:
  
  inline EventNoticer(cString& name, Sender* sender = nullptr)
  : name_(name)
  , sender_(sender)
  , listener_(nullptr) { }
  
  virtual ~EventNoticer() {
    if (listener_) {
      off();
      Release(listener_);
    }
  }
  
  /**
   * @fun name
   */
  inline String name() const { return name_; }
  
  /**
   * @func sender
   */
  inline Sender* sender() const { return sender_; }
  
  /**
   * @fun count # 获取侦听器数量
   */
  inline int count() const {
    return listener_ ? listener_->length() : 0;
  }

  template<class Scope>
  void on(void (Scope::*listener)(Event&), Scope* scope) av_def_err {
    get_listener_();
    assert_(listener, scope);
    listener_->push( { new OnListener<Scope>(this, listener, scope) } );
  }

  template <class Data>
  void on( void (*listener)(Event&, Data*), Data* data = nullptr) av_def_err {
    get_listener_();
    assert_static_(listener, data);
    listener_->push( { new OnStaticListener<Data>(this, listener, data) } );
  }

  void on( ListenerFunc listener, int id = 0) {
    get_listener_();
    listener_->push( { new OnLambdaFunctionListener(this, avocado::move(listener), id) } );
  }

  void on(EventNoticer* shell) av_def_err {
    get_listener_();
    assert_shell_(shell);
    listener_->push( { new OnShellListener(this, shell) } );
  }

  template<class Scope>
  void once(void (Scope::*listener)(Event&), Scope* scope) av_def_err {
    get_listener_();
    assert_(listener, scope);
    listener_->push( { new OnceListener<Scope>(this, listener, scope) } );
  }
  
  template <class Data>
  void once( void (*listener)(Event&, Data*), Data* data = nullptr) av_def_err {
    get_listener_();
    assert_static_(listener, data);
    listener_->push( { new OnceStaticListener<Data>(this, listener, data) } );
  }
  
  void once( ListenerFunc listener, int id = 0) {
    get_listener_();
    listener_->push( { new OnceLambdaFunctionListener(this, avocado::move(listener), id) } );
  }
  
  void once(EventNoticer* shell) av_def_err {
    get_listener_();
    assert_shell_(shell);
    listener_->push( { new OnceShellListener(this, shell) } );
  }

  template<class Scope>
  void off( void (Scope::*listener)(Event&) ) {
    if (listener_) {
      typedef OnListener<Scope> _OnListener;
      for ( auto& i : *listener_ ) {
        if ( i.value().value() && i.value()->is_on_listener() &&
            static_cast<_OnListener*>(i.value().value())->equals( listener ) ) {
          i.value().del();
        }
      }
    }
  }

  template<class Scope>
  void off( void (Scope::*listener)(Event&), Scope* scope) {
    if (listener_) {
      typedef OnListener<Scope> _OnListener;
      for ( auto& i : *listener_ ) {
        if( i.value().value() && i.value()->is_on_listener() &&
            static_cast<_OnListener*>(i.value().value())->equals(listener) &&
            static_cast<_OnListener*>(i.value().value())->equals( scope ) ) {
          i.value().del();
          break;
        }
      }
    }
  }
  
  template<class Data>
  void off( void (*listener)(Event&, Data*) ) {
    if (listener_) {
      typedef OnStaticListener<Data> _OnStaticListener;
      for ( auto& i : *listener_ ) {
        if ( i.value().value() && i.value()->is_on_static_listener() &&
            static_cast<_OnStaticListener*>(i.value().value())->equals(listener) ) {
          i.value().del();
          break;
        }
      }
    }
  }
  
  template<class Data>
  void off( void (*listener)(Event&, Data*), Data* data) {
    if (listener_) {
      typedef OnStaticListener<Data> _OnStaticListener;
      for ( auto& i : *listener_ ) {
        if ( i.value().value() && i.value()->is_on_static_listener() &&
            static_cast<_OnStaticListener*>(i.value().value())->equals(listener) &&
            static_cast<_OnStaticListener*>(i.value().value())->equals(data) ) {
          i.value().del();
          break;
        }
      }
    }
  }
  
  void off(int id) {
    if (listener_) {
      for ( auto& i : *listener_ ) {
        if ( i.value().value() && i.value()->is_on_func_listener() &&
              static_cast<OnLambdaFunctionListener*>(i.value().value())->equals(id)
           )
        {//
          i.value().del();
        }
      }
    }
  }
  
  template<class Scope>
  void off(Scope* scope) {
    if (listener_) {
      typedef OnListener<Scope> _OnListener;
      typedef OnStaticListener<Scope> _OnStaticListener;
      for ( auto& i : *listener_ ) {
        if ( i.value().value &&
            (
              (
                i.value()->is_on_listener() &&
                static_cast<_OnListener*>(i.value().value())->equals(scope)
              ) ||
              (
                i.value()->is_on_static_listener() &&
                static_cast<_OnStaticListener*>(i.value().value())->equals(scope)
              )
            )
          )
        {//
          i.value().del();
        }
      }
    }
  }
  
  void off(EventNoticer* shell) {
    if (listener_) {
      for ( auto& i : *listener_ ) {
        if ( i.value().value() && i.value()->is_on_shell_listener() &&
             static_cast<OnShellListener*>(i.value())->equals( shell ) )
        { //
          i.value().del();
          break;
        }
      }
    }
  }

  void off() {
    if (listener_) {
      for ( auto& i : *listener_ ) {
        i.value().del();
      }
    }
  }
  
  ReturnValue trigger() {
    if (listener_) {
      Event evt;
      return move( trigger(evt) );
    }
    return move( ReturnValue() );
  }
  
  ReturnValue trigger(cSendData& data) {
    if (listener_) {
      Event evt(data);
      return move( trigger(evt) );
    }
    return move( ReturnValue() );
  }
  
  ReturnValue& trigger(Event& evt) {
    if (listener_) {
      set_event_(evt);
      for (auto i = listener_->begin(); i != listener_->end(); ) {
        auto j = i++;
        Listener* listener = j.value().listener;
        if ( listener ) {
          // TODO listener->listener_ 如果为空指针或者野指针,会导致程序崩溃。。
          // 应该在对像释放前移除事件侦听器
          listener->call(evt);
        } else {
          listener_->del(j);
        }
      }
      clear_event_(evt);
    }
    return evt.return_value;
  }
  
private:

  inline void set_event_(Event& evt) {
    this->set_event(reinterpret_cast<typename Event::Noticer*>(this), evt);
  }
  inline void clear_event_(Event& evt) {
    this->clear_event(evt);
  }
  
  inline void get_listener_() {
    av_assert(!name_.is_empty());
    if (listener_ == nullptr) {
      listener_ = new List<LWrap>();
    }
  }
  
  void off_(Listener* listener) {
    for ( auto& i : *listener_ ) {
      if ( i.value().value() == listener ) {
        i.value().del();
        break;
      }
    }
  }
  
  template<class Scope>
  void assert_(void (Scope::*listener)(Event&), Scope* scope) av_def_err {
    typedef OnListener<Scope> _OnListener;
    for ( auto& i : *listener_ ) {
      if ( i.value().value() && i.value()->is_on_listener() ) {
        av_assert_err( !(static_cast<_OnListener*>(i.value().value())->equals( listener ) &&
                         static_cast<_OnListener*>(i.value().value())->equals( scope )),
                      ERR_DUPLICATE_LISTENER,
                     "Events have been added over the letter");
      }
    }
  }
  
  template<class Data>
  void assert_static_(void (*listener)(Event&, Data*), Data* data) av_def_err {
    typedef OnStaticListener<Data> _OnStaticListener;
    for ( auto& i : *listener_ ) {
      if ( i.value().value() && i.value()->is_on_static_listener() ) {
        av_assert_err( !(static_cast<_OnStaticListener*>(i.value().value())->equals( listener ) &&
                         static_cast<_OnStaticListener*>(i.value().value())->equals( data )),
                      ERR_DUPLICATE_LISTENER,
                      "Events have been added over the letter");
      }
    }
  }
  
  void assert_shell_(EventNoticer* shell) av_def_err {
    for ( auto& i : *listener_ ) {
      if ( i.value().value() && i.value()->is_on_shell_listener() ) {
        av_assert_err( !static_cast<OnShellListener*>(i.value().value())->equals( shell ),
                      ERR_DUPLICATE_LISTENER,
                      "Events have been added over the letter");
      }
    }
  }
};

/**
 * @class Responder
 */
template<
  class Event = Event<>,
  class Name_ = String,
  class Basic = Object
>
class Responder: public Basic {
  av_hidden_all_copy(Responder);
public:
  typedef Name_ Name;
  typedef EventNoticer<Event>         Noticer;
  typedef typename Event::SendData    SendData;
  typedef typename Event::cSendData   cSendData;
  typedef typename Event::Sender      Sender;
  typedef typename Event::ReturnValue ReturnValue;
  typedef typename Noticer::ListenerFunc ListenerFunc;
  
private:
  
  struct Inl_Noticer {
    inline Inl_Noticer() { av_unreachable(); }
    inline Inl_Noticer(const Name& t, Sender* sender)
    : name(t), value(t.to_string(), sender) { }
    Name    name;
    Noticer value;
  };
  typedef Map<Name, Inl_Noticer*> Events;
  Sender* m_sender;
  Events* m_noticers;
  
public:
  
  inline Responder(Sender* sender)
  : m_sender(sender)
  , m_noticers(nullptr) {
    
  }

  virtual ~Responder() {
    if ( m_noticers ) {
      for (auto& i : *m_noticers) {
        delete i.value();
      }
      Release(m_noticers);
      m_noticers = nullptr;
    }
  }
  
  Noticer* noticer(const Name& name) const {
    if ( m_noticers != nullptr ) {
      auto it = m_noticers->find(name);
      if (it != m_noticers->end()) {
        return &it.value()->value;
      }
    }
    return nullptr;
  }
  
  /**
   * 是否没有任何委托
   * @ret {bool}
   */
  inline bool is_noticer_none() const {
    return m_noticers == nullptr || m_noticers->length();
  }
  
  /**
   * @func sender
   */
  inline Sender* sender() const { return m_sender; }
  
  /**
   * 这是个委托中的事件,委托上的变化会通知到该函数,比如添加删除事件侦听器
   * @arg name {const Type&}
   * @arg count {int}
   */
  virtual void trigger_listener_change(const Name& name, int count, int change) { }
  
  template<class Scope>
  inline void on(const Name& name,
                 void (Scope::*listener)(Event&),
                 Scope* scope) {
    auto del = get_noticer(name);
    del->on(listener, scope);
    trigger_listener_change(name, del->count(), 1);
  }
  
  template<class Scope>
  inline void once(const Name& name,
                   void (Scope::*listener)(Event&),
                   Scope* scope) {
    auto del = get_noticer(name);
    del->once(listener, scope);
    trigger_listener_change(name, del->count(), 1);
  }
  
  template<class Data>
  inline void on(const Name& name,
                 void (*listener)(Event&, Data*),
                 Data* data = nullptr) {
    auto del = get_noticer(name);
    del->once(listener, data);
    trigger_listener_change(name, del->count(), 1);
  }
  
  template<class Data>
  inline void once(const Name& name,
                   void (*listener)(Event&, Data*),
                   Data* data = nullptr) {
    auto del = get_noticer(name);
    del->once(listener, data);
    trigger_listener_change(name, del->count(), 1);
  }
  
  inline void on( const Name& name, ListenerFunc listener, int id = 0) {
    auto del = get_noticer(name);
    del->on(listener, id);
    trigger_listener_change(name, del->count(), 1);
  }
  
  inline void once( const Name& name, ListenerFunc listener, int id = 0) {
    auto del = get_noticer(name);
    del->once(listener, id);
    trigger_listener_change(name, del->count(), 1);
  }
  
  inline void on(const Name& name, Noticer* shell) {
    auto del = get_noticer(name);
    del->on(shell);
    trigger_listener_change(name, del->count(), 1);
  }
  
  /**
   * 添加一个侦听器委托,只侦听一次,后被卸载
   */
  inline void once(const Name& name, Noticer* shell) {
    auto del = get_noticer(name);
    del->once(shell);
    trigger_listener_change(name, del->count(), 1);
  }
  
  template<class Scope>
  inline void off(const Name& name,
                  void (Scope::*listener)(Event&)
                  ) {
    auto del = noticer(name);
    if (del) {
      del->off(listener);
      trigger_listener_change(name, del->count(), -1);
    }
  }
  
  template<class Scope>
  inline void off(const Name& name,
                  void (Scope::*listener)(Event&), Scope* scope) {
    auto del = noticer(name);
    if (del) {
      del->off(listener, scope);
      trigger_listener_change(name, del->count(), -1);
    }
  }
  
  template<class Data>
  inline void off(const Name& name,
                  void (*listener)(Event&, Data*)
                  ) {
    auto del = noticer(name);
    if (del) {
      del->off(listener);
      trigger_listener_change(name, del->count(), -1);
    }
  }
  
  template<class Data>
  inline void off(const Name& name,
                  void (*listener)(Event&, Data*),
                  Data* data) {
    auto del = noticer(name);
    if (del) {
      del->off(listener, data);
      trigger_listener_change(name, del->count(), -1);
    }
  }
  
  /**
   * @func off
   */
  inline void off(const Name& name, int id) {
    auto del = noticer(name);
    if (del) {
      del->off(id);
      trigger_listener_change(name, del->count(), -1);
    }
  }
  
  /**
   * @func off
   */
  inline void off(int id) {
    if (m_noticers) {
      auto end = m_noticers->end();
      for (auto i = m_noticers->begin(); i != end; i++) {
        Inl_Noticer* item = &i.value();
        item->value.off(id);
        trigger_listener_change(item->name, item->value.count(), -1);
      }
    }
  }
  
  /**
   * 卸载这个范围里的所有侦听器
   */
  template<class Scope> inline void off(Scope* scope) {
    if (m_noticers) {
      for ( auto& i : *m_noticers ) {
        Inl_Noticer* inl = i.value();
        inl->value.off(scope);
        trigger_listener_change(inl->name, inl->value.count(), -1);
      }
    }
  }
  
  inline void off(const Name& name, Noticer* shell) {
    auto del = noticer(name);
    if (del) {
      del->off(shell);
      trigger_listener_change(name, del->count(), -1);
    }
  }
  
protected:
  
  /**
   * 卸载指定名称上的全部侦听函数
   */
  inline void off(const Name& name) {
    auto del = noticer(name);
    if (del) {
      del->off();
      trigger_listener_change(name, del->count(), -1);
    }
  }
  
  /**
   * 卸载全部委托上的全部侦听函数
   */
  inline void off() {
    if (m_noticers) {
      for ( auto& i : *m_noticers ) {
        Inl_Noticer* inl = i.value();
        inl->value.off();
        trigger_listener_change(inl->name, inl->value.count(), -1);
      }
    }
  }
  
  /**
   * 发射事件
   * @arg name {const Key&}
   * NOTE: 这个方法能创建默认事件数据
   */
  inline ReturnValue trigger(const Name& name) {
    auto del = noticer(name);
    return move( del ? del->trigger(): ReturnValue() );
  }
  
  /*
   * 发射事件
   * @arg name {const Key&}
   * @arg evt {cSendData&}
   */
  inline ReturnValue trigger(const Name& name, cSendData& data) {
    auto del = noticer(name);
    return move( del ? del->trigger(data): ReturnValue() );
  }
  
  /*
   * 发射事件
   * @arg name {const Key&}
   * @arg evt {Event&}
   */
  inline ReturnValue& trigger(const Name& name, Event& evt) {
    auto del = noticer(name);
    return del ? del->trigger(evt): evt.return_value;
  }
  
private:
  
  Noticer* get_noticer(const Name& name) {
    if (m_noticers == nullptr) {
      m_noticers = new Events();
    }
    auto it = m_noticers->find(name);
    if (it != m_noticers->end()) {
      return &it.value()->value;
    } else {
      return &m_noticers->set(name, new Inl_Noticer(name, m_sender))->value;
    }
  }
  
};

av_end
#endif
