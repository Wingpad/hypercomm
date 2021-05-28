#ifndef __TESTER_HH__
#define __TESTER_HH__

#include <hypercomm/core/locality.hpp>
#include <hypercomm/core/persistent_port.hpp>
#include "tester.decl.h"

using namespace hypercomm;

struct entry_method_like : public hypercomm::component {
  entry_method_like(const id_t& _1) : component(_1) {}

  virtual bool keep_alive(void) const override { return true; }

  virtual int num_expected(void) const override { return 1; }
};

struct say_hello : virtual public entry_method_like {
  say_hello(const id_t& _1) : entry_method_like(_1) {}
  virtual value_t action(void) override;
};

struct my_redn_com : virtual public entry_method_like {
  locality* self;
  my_redn_com(const id_t& _1, locality* _2) : entry_method_like(_1), self(_2) {}
  virtual value_t action(void) override;
};

struct nop_combiner : public combiner {
  virtual combiner::return_type send(combiner::argument_type&& args) override {
    return args.empty() ? combiner::return_type{} : args[0];
  }

  virtual void __pup__(hypercomm::serdes&) override {}
};

#endif
