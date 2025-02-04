/*
    This file is part of the Arduino_CloudUtils library.

    Copyright (c) 2024 Arduino SA

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#pragma once

#include <map>
#include "../interfaces/Encoder.h"
#include "CBOR.h"
#include "../interfaces/message.h"
#include "./tinycbor/cbor-lib.h"

class CBORMessageEncoderClass;

// TODO find a better name
// TODO maybe a template<CBORTag tag, MessageId id> ?
// TODO maybe template<resultStruct> that is also the parameter of encode
// TODO in order to make this more extensible we should not pass Message* as a parameter, templated function may be better (or void*)
class CBORMessageEncoderInterface {
public:
  CBORMessageEncoderInterface(const CBORTag tag, const MessageId id);
  virtual ~CBORMessageEncoderInterface() {}

protected:
  virtual Encoder::Status encode(CborEncoder* encoder, Message *msg) = 0;

private:
  const CBORTag tag;
  const MessageId id;

  friend CBORMessageEncoderClass;

  // wrapper for encode function that for the time being only writes the tag in the buffer
  Encoder::Status _encode(CborEncoder* encoder, Message *msg);
};

class CBORMessageEncoderClass: public Encoder {
public:
  static CBORMessageEncoderClass& getInstance();

  void append(CBORTag id, CBORMessageEncoderInterface* encoder) {
    encoders[id] = encoder;
  }

  Encoder::Status encode(Message* message, uint8_t * data, size_t& len);
private:
  // CBORMessageEncoderClass() {}

  std::map<MessageId, CBORMessageEncoderInterface*> encoders;
};

extern CBORMessageEncoderClass& CBORMessageEncoder;
