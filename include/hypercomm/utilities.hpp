#ifndef __HYPERCOMM_UTIL_HPP__
#define __HYPERCOMM_UTIL_HPP__

#include <charm++.h>
#include <memory>

namespace hypercomm {
namespace utilities {

std::string buf2str(const char* data, const std::size_t& size);
std::string env2str(const envelope* env);

void pack_message(CkMessage*);
void unpack_message(CkMessage*);

CkMessage* unwrap_message(std::shared_ptr<CkMessage>&&);
std::shared_ptr<CkMessage> wrap_message(CkMessage*);
std::shared_ptr<CkMessage> copy_message(const std::shared_ptr<CkMessage>&);

char *get_message_buffer(const CkMessage* msg);

inline char *get_message_buffer(const std::shared_ptr<CkMessage>& msg) {
    return get_message_buffer(msg.get());
}

}
}

#endif