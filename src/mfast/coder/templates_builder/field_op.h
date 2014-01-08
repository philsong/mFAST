#ifndef FIELD_OP_H_CWKVZBY7
#define FIELD_OP_H_CWKVZBY7
#include <cstring>
#include <map>
#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/lexical_cast.hpp>

#include "mfast/arena_allocator.h"
#include "mfast/field_instruction.h"
#include "xml_util.h"
#include "../common/exceptions.h"

namespace mfast {
namespace coder {


using namespace tinyxml2;
using namespace boost::assign; // bring 'map_list_of()' into scope

struct tag_reason;
typedef boost::error_info<tag_referenced_by,std::string> reason_info;


class field_op
{
public:
  operator_enum_t op_;
  const op_context_t*   context_;
  value_storage initial_value_;
  arena_allocator*   alloc_;

  field_op()
    : op_(operator_none)
    , context_(0)
    , alloc_(0)
  {
  }

  template <typename Instruction>
  field_op(const Instruction* inst,
           const XMLElement*  element,
           arena_allocator&   alloc)
    : op_(inst->field_operator())
    , context_(inst->op_context())
    , initial_value_(inst->initial_value())
    , alloc_(&alloc)
  {
    if (element) {
      const XMLElement* field_op_element = find_field_op_element(*element);
      if (field_op_element) {
        parse_field_op(*field_op_element, alloc);
        const char* init_value_str = get_optional_attr(*field_op_element, "value", 0);
        if (init_value_str)
          set_init_value(init_value_str, inst);
      }
    }
  }

  field_op(const decimal_field_instruction* inst,
           const XMLElement*                element,
           arena_allocator&                 alloc);

  arena_allocator&   alloc()
  {
    return *alloc_;
  }

private:

  void set_init_value(const char* init_value_str,
                      const int32_field_instruction* )
  {
    if (init_value_str) {
      initial_value_.present(true);
      initial_value_.set(boost::lexical_cast<int32_t>(init_value_str));
    }
  }

  void set_init_value(const char* init_value_str,
                      const uint32_field_instruction* )
  {
    if (init_value_str){
      initial_value_.present(true);
      initial_value_.set(boost::lexical_cast<uint32_t>(init_value_str));
    }
  }

  void set_init_value(const char* init_value_str,
                      const int64_field_instruction* )
  {
    if (init_value_str){
      initial_value_.present(true);
      initial_value_.set(boost::lexical_cast<int64_t>(init_value_str));
    }
  }

  void set_init_value(const char* init_value_str,
                      const uint64_field_instruction* )
  {
    if (init_value_str){
      initial_value_.present(true);
      initial_value_.set(boost::lexical_cast<uint64_t>(init_value_str));
    }
  }

  void set_init_value(const char* init_value_str,
                      const ascii_field_instruction*)
  {
    if (init_value_str) {
      initial_value_ = string_value_storage( string_dup(init_value_str, alloc()) ).storage_;
    }
  }

  void set_init_value(const char* init_value_str,
                      const byte_vector_field_instruction*);

  const XMLElement* find_field_op_element(const XMLElement& element) const;

  void parse_field_op(const XMLElement& field_op_element,
                      arena_allocator&  alloc);

};

}
}


#endif /* end of include guard: FIELD_OP_H_CWKVZBY7 */