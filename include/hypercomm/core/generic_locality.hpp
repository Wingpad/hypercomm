#ifndef __HYPERCOMM_CORE_GENLOC_HPP__
#define __HYPERCOMM_CORE_GENLOC_HPP__

#include "entry_port.hpp"

namespace hypercomm {

using component_port_t = std::pair<component::id_t, component::port_type>;

// TODO elevate this functionality to a more general purpose callback
//      ensure that the reference counting on the callback port is elided when
//      !kCallback
struct destination_ {
  enum type_ : uint8_t { kInvalid, kCallback, kComponentPort };

  type_ type;
  callback_ptr cb;
  component_port_t port;

  destination_(const component_port_t& _1) : type(kComponentPort), port(_1) {}
  destination_(const callback_ptr& _2) : type(kCallback), cb(_2) {}
};

using entry_port_map = comparable_map<entry_port_ptr, destination_>;
using component_map = std::unordered_map<component::id_t, component_ptr>;

struct generic_locality_ {
  entry_port_map entry_ports;
  component_map components;

  generic_locality_(void) { this->update_context(); }

  inline void invalidate_port(entry_port& port) {
    port.alive = port.alive && port.keep_alive();
    if (!port.alive) {
      auto end = std::end(this->entry_ports);
      auto search =
          std::find_if(std::begin(this->entry_ports), end,
                       [&](const typename entry_port_map::value_type& pair) {
                         return &port == pair.first.get();
                       });
      if (search != end) {
        this->entry_ports.erase(search);
      }
    }
  }

  // forces termination of component, regardless of resilience
  inline void invalidate_component(const component::id_t& id) {
    auto search = this->components.find(id);
    if (search != std::end(this->components)) {
      search->second->alive = false;
      search->second->on_invalidation();
      this->components.erase(search);
    }
  }

  inline void update_context(void);

  inline callback_ptr make_connector(const component_ptr& com, const component::port_type& port);

  virtual void try_send(const component_port_t&, component::value_type&&) = 0;
};

// TODO this is a temporary solution
struct connector_ : public callback {
  generic_locality_* self;
  const component_port_t dst;

  connector_(generic_locality_* _1, const component_port_t& _2)
      : self(_1), dst(_2) {}

  virtual return_type send(argument_type&& value) override {
    self->try_send(dst, std::move(value));
  }

  virtual void __pup__(serdes& s) override { CkAbort("don't send me"); }
};

inline callback_ptr generic_locality_::make_connector(const component_ptr& com, const component::port_type& port) {
  return std::make_shared<connector_>(this, std::make_pair(com->id, port));
}

namespace {
CpvDeclare(generic_locality_*, locality_);
}

inline void generic_locality_::update_context(void) {
  if (!CpvInitialized(locality_)) {
    CpvInitialize(generic_locality_*, locality_);
  }

  CpvAccess(locality_) = this;
}

inline generic_locality_* access_context(void) {
  auto& locality = *(&CpvAccess(locality_));
  CkAssert(locality && "locality must be valid");
  return locality;
}

void locally_invalidate_(entry_port& which) {
  access_context()->invalidate_port(which);
}

void locally_invalidate_(const component::id_t& which) {
  access_context()->invalidate_component(which);
}

callback_ptr local_connector_(const component_ptr& com, const component::port_type& port) {
  return access_context()->make_connector(com, port);
}

}

#endif