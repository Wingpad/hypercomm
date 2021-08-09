#ifndef __HYPERCOMM_CORE_GENLOC_HPP__
#define __HYPERCOMM_CORE_GENLOC_HPP__

#include "../messaging/destination.hpp"
#include "entry_port.hpp"
#include "module.hpp"

/* TODO consider introducing a simplified connection API that
 *     utilizes "port authorities", aka port id counters, to
 *     remove src/dstPort for trivial, unordered connections
 */

namespace hypercomm {
class generic_locality_ : public virtual common_functions_ {
 public:
  template <typename A, typename Enable>
  friend class comproxy;

  entry_port_map entry_ports;
  component_map components;
  message_queue<entry_port_ptr> port_queue;
  std::vector<component_id_t> invalidations;
  component_id_t component_authority = 0;

  using component_type = typename decltype(components)::mapped_type;
  using entry_port_iterator = typename decltype(entry_ports)::iterator;

  generic_locality_(void) { this->update_context(); }
  virtual ~generic_locality_();

  void loopback(message* msg);
  void receive_message(message* msg);
  void receive_value(const entry_port_ptr& port, component::value_type&& value);

  template <typename Destination>
  void open(const entry_port_ptr& ours, const Destination& theirs);
  void try_send(const destination& dest, component::value_type&& value);
  void try_send(const component_port_t& port, component::value_type&& value);

  void resync_port_queue(entry_port_iterator& it);
  void invalidate_port(const entry_port_ptr& port);

  void activate_component(const component_id_t& id);
  void invalidate_component(const component::id_t& id);

  inline void try_collect(const component_id_t& which) {
    this->try_collect(this->components[which]);
  }

  inline void try_collect(const component_type& com) {
    if (com && com->collectible()) {
      this->components.erase(com->id);
    }
  }

  inline void connect(const component_id_t& src,
                      const components::port_id_t& srcPort,
                      const component_id_t& dst,
                      const components::port_id_t& dstPort) {
    this->components[src]->update_destination(
        srcPort, this->make_connector(dst, dstPort));
  }

  inline void connect(const component_id_t& src,
                      const components::port_id_t& srcPort,
                      const callback_ptr& cb) {
    this->components[src]->update_destination(srcPort, cb);
  }

  inline void connect(const entry_port_ptr& srcPort, const component_id_t& dst,
                      const components::port_id_t& dstPort) {
    this->components[dst]->add_listener(srcPort);
    this->open(srcPort, std::make_pair(dst, dstPort));
  }

  callback_ptr make_connector(const component_id_t& com,
                              const component::port_type& port);

  virtual stamp_type __stamp__(void) const { NOT_IMPLEMENTED; }

 protected:
  bool invalidated(const component::id_t& id);
  void update_context(void);

 private:
  template <typename A>
  A* get_component(const component_id_t& id) {
    auto search = this->components.find(id);
    if (search != std::end(this->components)) {
      return dynamic_cast<A*>(search->second.get());
    } else {
      return nullptr;
    }
  }
};

template <typename Destination>
void generic_locality_::open(const entry_port_ptr& ours,
                             const Destination& theirs) {
  ours->alive = true;
  auto pair = this->entry_ports.emplace(ours, theirs);
#if CMK_ERROR_CHECKING
  if (!pair.second) {
    std::stringstream ss;
    ss << "[";
    for (const auto& epp : this->entry_ports) {
      const auto& other_port = epp.first;
      if (comparable_comparator<entry_port_ptr>()(ours, other_port)) {
        ss << "{" << other_port->to_string() << "}, ";
      } else {
        ss << other_port->to_string() << ", ";
      }
    }
    ss << "]";

    CkAbort("fatal> adding non-unique port %s to:\n\t%s\n",
            ours->to_string().c_str(), ss.str().c_str());
  }
#endif
  this->resync_port_queue(pair.first);
}

}  // namespace hypercomm

#endif
