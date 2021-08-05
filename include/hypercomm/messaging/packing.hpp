#ifndef __HYPERCOMM_MESSAGING_PACKING_HPP__
#define __HYPERCOMM_MESSAGING_PACKING_HPP__

#include "../serialization/pup.hpp"
#include "../utilities.hpp"
#include "messaging.hpp"

namespace hypercomm {
template <typename... Args>
void unpack(std::shared_ptr<CkMessage>&& msg, Args&... args) {
  auto s = serdes::make_unpacker(msg, utilities::get_message_buffer(msg));
  hypercomm::pup(s, std::forward_as_tuple(args...));
}

template <typename... Args>
void unpack(CkMessage* msg, Args&... args) {
  auto src = utilities::wrap_message(msg);
  unpack(std::move(src), args...);
}

template <typename... Args>
std::size_t pack_into(char* buf, const std::tuple<Args&...>& args) {
  auto packer = hypercomm::serdes::make_packer(buf);
  hypercomm::pup(packer, args);
  return packer.size();
}

template <typename... Args>
CkMessage* pack(const Args&... _args) {
  auto args = std::forward_as_tuple(const_cast<Args&>(_args)...);
  auto size = hypercomm::size(args);
  auto msg = CkAllocateMarshallMsg(size);
  auto real_size = pack_into(msg->msgBuf, args);
  CkAssert(size == real_size);
  return msg;
}

template <typename... Args>
hypercomm_msg* pack_to_port(const entry_port_ptr& dst, const Args&... _args) {
  auto args = std::forward_as_tuple(const_cast<Args&>(_args)...);
  auto size = hypercomm::size(args);
  auto msg = hypercomm_msg::make_message(size, dst);
  auto real_size = pack_into(msg->payload, args);
  CkAssert(size == real_size);
  return msg;
}

}  // namespace hypercomm

#endif
