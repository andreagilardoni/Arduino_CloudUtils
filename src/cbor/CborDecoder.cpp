/*
    This file is part of the Arduino_CloudUtils library.

    Copyright (c) 2024 Arduino SA

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "CborDecoder.h"

Decoder::Status CBORMessageDecoderSingleton::decode(Message* msg, const uint8_t* const buf, size_t &len) { // TODO do we need to propagate the maximum length?
  // prepare cbor structure
  CborValue iter;
  CborTag tag;
  CborParser parser;

  if (cbor_parser_init(buf, len, 0, &parser, &iter) != CborNoError) {
    return Decoder::Status::Error;
  }

  if (iter.type != CborTagType) {
    return Decoder::Status::Error;
  }

  if (cbor_value_get_tag(&iter, &tag) != CborNoError) {
    return Decoder::Status::Error;
  }

  if (cbor_value_advance(&iter) != CborNoError) {
    return Decoder::Status::Error;
  }

  auto decoder_it = decoders.find(tag);

  // check if message.id exists on the decoders list or return error
  if(decoder_it == decoders.end()) {
    return Decoder::Status::Error;
  }

  // encode the message
  if(decoder_it->second->_decode(&iter, msg) == Decoder::Status::Error) {
    return Decoder::Status::Error;
  }

  return Decoder::Status::Complete;
}

CBORMessageDecoderSingleton& CBORMessageDecoderSingleton::getInstance() {
  static CBORMessageDecoderSingleton singleton;

  return singleton;
}

CBORMessageDecoderInterface::CBORMessageDecoderInterface(const CBORTag tag, const MessageId id)
: tag(tag), id(id) {
  // call singleton/global variable and insert this encoder
  CBORMessageDecoderSingleton::getInstance().append(tag, this);
}

Decoder::Status CBORMessageDecoderInterface::_decode(CborValue* iter, Message *msg) {
  CborValue array_iter;
  msg->id = this->id;

  if (cbor_value_get_type(iter) != CborArrayType) {
    return Decoder::Status::Error;
  }

  if (cbor_value_enter_container(iter, &array_iter) != CborNoError) {
    return Decoder::Status::Error;
  }

  return decode(&array_iter, msg);
}
