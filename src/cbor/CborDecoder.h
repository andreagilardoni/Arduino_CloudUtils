/*
    This file is part of the Arduino_CloudUtils library.

    Copyright (c) 2024 Arduino SA

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#pragma once

#include <map>
#include "../interfaces/Decoder.h"
#include "CBOR.h"
#include "../interfaces/message.h"
#include "./tinycbor/cbor-lib.h"

class CBORMessageDecoderSingleton;

// TODO find a better name
// TODO maybe a template<CBORTag tag, MessageId id> ?
// TODO maybe template<resultStruct> that is also the parameter of encode
// TODO in order to make this more extensible we should not pass Message* as a parameter, templated function may be better (or void*)
class CBORMessageDecoderInterface {
public:
  CBORMessageDecoderInterface(const CBORTag tag, const MessageId id);
  virtual ~CBORMessageDecoderInterface() {}

protected:
  virtual Decoder::Status decode(CborValue* iter, Message *msg) = 0;

private:
  const CBORTag tag;
  const MessageId id;

  friend CBORMessageDecoderSingleton;

  // wrapper for encode function that for the time being only writes the tag in the buffer
  Decoder::Status _decode(CborValue* iter, Message *msg);
};

class CBORMessageDecoderSingleton: public Decoder {
public:
  static CBORMessageDecoderSingleton& getInstance();

  void append(CBORTag id, CBORMessageDecoderInterface* encoder) {
    decoders[id] = encoder;
  }

  Decoder::Status decode(Message* msg, const uint8_t* const buf, size_t &len);
private:
  CBORMessageDecoderSingleton() {}

  static CBORMessageDecoderSingleton singleton;
  std::map<CBORTag, CBORMessageDecoderInterface*> decoders;
};

class CBORMessageDecoder: public Decoder {
public:
  inline Decoder::Status decode(Message* msg, const uint8_t* const buf, size_t &len) {
    return CBORMessageDecoderSingleton::getInstance().decode(msg, buf, len);
  }
};
