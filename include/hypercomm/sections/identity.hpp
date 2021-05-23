#ifndef __HYPERCOMM_SECTIONS_IDENTITY_HPP__
#define __HYPERCOMM_SECTIONS_IDENTITY_HPP__

#include <cstdint>

namespace hypercomm {

using reduction_id_t = component_id_t;

template <typename Index>
class identity {
  reduction_id_t current_ = 0;

 public:
  reduction_id_t next_reduction(void) { return current_++; }
  virtual std::vector<Index> upstream(void) const = 0;
  virtual std::vector<Index> downstream(void) const = 0;
};
}

#endif