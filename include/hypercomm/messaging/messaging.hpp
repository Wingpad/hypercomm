#ifndef __HYPERCOMM_MESSAGING_MESSAGING_HPP__
#define __HYPERCOMM_MESSAGING_MESSAGING_HPP__

#include <hypercomm/messaging/messaging.decl.h>

#include "../core/entry_port.hpp"

namespace hypercomm {
namespace messaging {

static constexpr auto __null_msg__ =
    std::numeric_limits<CMK_REFNUM_TYPE>::max();

struct __msg__ : public CMessage___msg__ {
  entry_port_ptr dst;
  char *payload;

  static void *pack(__msg__ *msg);
  static __msg__ *unpack(void *buf);
  static __msg__ *make_message(const std::size_t &user_size,
                               const entry_port_ptr &dst);
  static __msg__ *make_null_message(const entry_port_ptr &dst);

  inline bool is_null(void) const {
    return (UsrToEnv(this)->getRef() == __null_msg__);
  }
};
}  // namespace messaging

using message = messaging::__msg__;
using message_ptr = std::shared_ptr<message>;

}  // namespace hypercomm

using hypercomm_msg = hypercomm::message;
using CMessage_hypercomm_msg = hypercomm::messaging::CMessage___msg__;

#endif
