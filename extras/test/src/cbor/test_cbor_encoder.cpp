/*
    This file is part of the Arduino_CloudUtils library.

    Copyright (c) 2024 Arduino SA

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <cbor/CborEncoder.h>

enum : MessageId {
    CBORTestMessageId = 0x0123,
};

enum : CBORTag {
    CBORTestMessageTag = 0x0321,
};


struct CBORTestMessage {
    Message m;
    char parameter[20];
};

//TODO should this be a singleton?
class CustomMessageEncoder: public CBORMessageEncoderInterface { // FIXME better names
public:
    CustomMessageEncoder()
    : CBORMessageEncoderInterface(CBORTestMessageTag, CBORTestMessageId) {}

protected:
    Encoder::Status encode(CborEncoder* encoder, Message *msg) override {
        CBORTestMessage * testMessage = (CBORTestMessage *) msg;
        CborEncoder array_encoder;

        if(cbor_encoder_create_array(encoder, &array_encoder, 1) != CborNoError) {
            return Encoder::Status::Error;
        }

        if(cbor_encode_text_stringz(&array_encoder, testMessage->parameter) != CborNoError) {
            return Encoder::Status::Error;
        }

        if(cbor_encoder_close_container(encoder, &array_encoder) != CborNoError) {
            return Encoder::Status::Error;
        }
        return Encoder::Status::Complete;
    }
} customMessageEncoder;

SCENARIO( "A custom encoder is defined", "[cbor][encode]" ) {
    uint8_t buffer[100]; // shared buffer for encoding
    const size_t buf_len = sizeof(buffer);

    GIVEN( "A Message with an id that the global encoder is able to encode" ) {
        CBORTestMessage cmd {
            CBORTestMessageId,
            "abcdef",
        };
        size_t res_len=buf_len;
        Encoder::Status res = CBORMessageEncoder.encode((Message*)&cmd, buffer, res_len);

        THEN( "Message encode result is Complete" ) {
            REQUIRE(res == Encoder::Status::Complete);
        }

        THEN( "the encode result matches the expectations" ) {
            std::vector<uint8_t> res(buffer, buffer+res_len);

            REQUIRE_THAT(res, Catch::Matchers::Equals(std::vector<uint8_t>{
                0xD9, 0x03, 0x21, 0x81, 0x66, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
            }));
        }
    }
}
