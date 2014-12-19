#include "fast_decoder_core.h"

namespace mfast
{
  namespace coder
  {
    void
    fast_decoder_core::visit(const nested_message_mref& mref)
    {
      decoder_pmap_saver<true_type> saver(this);
      info_entry* saved_active_info = this->active_message_info_;


      if (this->current_->is_next_bit_set()) {
        uint32_t template_id;

        strm_.decode(template_id, false_type());
        // find the message with corresponding template id
        active_message_info_ = repo_.find(template_id);

        if (active_message_info_ == 0) {
          BOOST_THROW_EXCEPTION(fast_dynamic_error("D9") << template_id_info(template_id)
                                                         << referenced_by_info(this->active_message().name()));
        }
      }

      mref.set_target_instruction(this->active_message().instruction(), false_type() );
      message_decode_function_t decode = active_message_info_->decode_fun_;
      (this->*decode)(mref.target());

      this->active_message_info_ = saved_active_info;

    }

    message_type&
    fast_decoder_core::decode_segment(fast_istreambuf& sb)
    {

      strm_.reset(&sb);

      decoder_presence_map pmap;
      this->current_ = &pmap;
      strm_.decode(pmap);

      if (pmap.is_next_bit_set()) {
        uint32_t template_id;

        strm_.decode(template_id, false_type());

        // find the message with corresponding template id
        active_message_info_ = repo_.find(template_id);

        if (active_message_info_ == 0) {
          BOOST_THROW_EXCEPTION(fast_dynamic_error("D9") << coder::template_id_info(template_id));
        }
      }

      // we have to keep the active_message_ in a new variable
      // because after the accept_mutator(), the active_message_
      // may change because of the decoding of dynamic template reference
      message_type& message = this->active_message();

      if (force_reset_ || message.instruction()->has_reset_attribute()) {
        repo_.reset_dictionary();
      }

      message_decode_function_t decode = active_message_info_->decode_fun_;
      (this->*decode)(message.mref());

      return message;
    }

    message_cref
    fast_decoder_core::decode_stream(const char*& first, const char* last, bool force_reset)
    {
      assert(first < last);
      fast_istreambuf sb(first, last-first);
      this->force_reset_ = force_reset;
      message_cref result = this->decode_segment(sb).cref();
      first = sb.gptr();
      return result;
    }


  } /* coder */

} /* mfast */

