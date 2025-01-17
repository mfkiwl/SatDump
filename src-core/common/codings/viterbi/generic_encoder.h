/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <memory>
#include <string>

namespace fec
{

       class generic_encoder
       {
       public:
              friend class encoder;
              virtual void generic_work(void *in_buffer, void *out_buffer) = 0;
              static int base_unique_id;
              int my_id;
              int unique_id();
              std::string d_name;

       public:
              typedef std::shared_ptr<generic_encoder> sptr;

              /*!
     * Returns the rate of the code. For every 1 input bit, there
     * are r output bits, so the rate is 1/r. Used for setting
     * things like the encoder block's relative rate.
     *
     * This function MUST be reimplemented by the child class.
     */
              virtual double rate() = 0;

              /*!
     * Returns the input size in items that the encoder object uses
     * to encode a full frame. Often, this number is the number of
     * bits per frame if the input format is unpacked. If the block
     * expects packed bytes, then this value should be the number of
     * bytes (number of bits / 8) per input frame.
     *
     * The child class MUST implement this function.
     */
              virtual int get_input_size() = 0;

              /*!
     * Returns the output size in items that the encoder object
     * produces after encoding a full frame. Often, this number is
     * the number of bits in the outputted frame if the input format
     * is unpacked. If the block produces packed bytes, then this
     * value should be the number of bytes (number of bits / 8) per
     * frame produced. This value is generally something like
     * R*get_input_size() for a 1/R rate code.
     *
     * The child class MUST implement this function.
     */
              virtual int get_output_size() = 0;

              /*!
     * Set up a conversion type required to setup the data properly
     * for this encoder. The encoder itself will not implement the
     * conversion and expects an external wrapper (e.g.,
     * fec.extended_encoder) to read this value and "do the right
     * thing" to format the data.
     *
     * The default behavior is 'none', which means no conversion is
     * required. Whatever the get_input_item_size() value returns,
     * the input is expected to conform directly to this. Generally,
     * this means unpacked bytes.
     *
     * If 'pack', the block expects the inputs to be packed
     * bytes. The wrapper should implement a
     * gr::blocks::pack_k_bits_bb(8) block for this.
     *
     * The child class MAY implement this function. If not
     * reimplemented, it returns "none".
     */
              virtual const char *get_input_conversion();

              /*!
     * Set up a conversion type required to understand the output
     * style of this encoder. Generally an encoder will produce
     * unpacked bytes with a bit set in the LSB.
     *
     * The default behavior is 'none', which means no conversion is
     * required and the encoder produces unpacked bytes.
     *
     * If 'packed_bits', the block produces packed bits and the
     * wrapper should unpack these (using, for instance,
     * gr::block::unpack_k_bits_bb(8)).
     *
     * The child class MAY implement this function. If not
     * reimplemented, it returns "none".
     */
              virtual const char *get_output_conversion();

              /*!
     * Updates the size of the frame to encode.
     *
     * The child class MUST implement this function and interpret
     * how the \p frame_size information affects the block's
     * behavior. It should also provide bounds checks.
     */
              virtual bool set_frame_size(unsigned int frame_size) = 0;

              generic_encoder(void){};
              generic_encoder(std::string name);
              virtual ~generic_encoder();
       };

       /*! see generic_encoder::get_output_size() */
       int get_encoder_output_size(generic_encoder::sptr my_encoder);

       /*! see generic_encoder::get_input_size() */
       int get_encoder_input_size(generic_encoder::sptr my_encoder);

       /*! see generic_encoder::get_input_conversion() */
       const char *get_encoder_input_conversion(generic_encoder::sptr my_encoder);

       /*! see generic_encoder::get_output_conversion() */
       const char *get_encoder_output_conversion(generic_encoder::sptr my_encoder);
} /* namespace fec */
