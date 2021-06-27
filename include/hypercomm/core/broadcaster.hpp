#ifndef __HYPERCOMM_CORE_BROADCASTER_HPP__
#define __HYPERCOMM_CORE_BROADCASTER_HPP__

#include "locality_base.hpp"

namespace hypercomm {
template <typename Index>
struct broadcaster : public immediate_action<void(locality_base<Index>*)> {
  using index_type = array_proxy::index_type;
  using section_ptr = typename locality_base<Index>::section_ptr;

  section_ptr section;
  std::shared_ptr<hypercomm::message> msg;

  broadcaster(PUP::reconstruct) {}

  broadcaster(const section_ptr& _1, decltype(msg)&& _2)
      : section(_1), msg(_2) {}

  broadcaster(const section_ptr& _1, hypercomm::message *_2)
      : section(_1), msg(std::static_pointer_cast<hypercomm::message>(utilities::wrap_message(_2))) {}

  virtual void action(locality_base<Index>* _1) override {
    auto& locality = const_cast<locality_base<Index>&>(*_1);
    auto collective =
        std::dynamic_pointer_cast<array_proxy>(locality.__proxy__());
    CkAssert(collective && "must be a valid collective");

    const auto& identity = locality.identity_for(this->section);
    auto ustream = identity->upstream();
    for (const auto& up : ustream) {
      auto copy =
          std::static_pointer_cast<hypercomm_msg>(utilities::copy_message(msg));
      auto next = std::make_shared<broadcaster>(this->section, std::move(copy));
      locality_base<Index>::send_action(collective, conv2idx<CkArrayIndex>(up),
                                        std::move(next));
    }

    locality.receive_message(static_cast<hypercomm::message*>(
        utilities::unwrap_message(std::move(msg))));
  }

  virtual void __pup__(serdes& s) override {
    s | section;
    s | msg;
  }
};
}

#endif
