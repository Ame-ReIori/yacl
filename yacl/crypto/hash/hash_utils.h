// Copyright 2019 Ant Group Co., Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <vector>

#include "yacl/base/int128.h"
#include "yacl/crypto/hash/ssl_hash.h"
#include "yacl/crypto/hash/ssl_hash_xof.h"

namespace yacl::crypto {

std::array<uint8_t, 32> Sha256(ByteContainerView data);  // 256-bits

std::array<uint8_t, 32> Sm3(ByteContainerView data);  // 256-bits

std::array<uint8_t, 32> Blake3(ByteContainerView data);  // 256-bits

// XOF (Extendable Output Function) hash functions
std::vector<uint8_t> Shake128(ByteContainerView data,
                              size_t output_length = 16);  // default 128-bits
std::vector<uint8_t> Shake256(ByteContainerView data,
                              size_t output_length = 32);  // default 256-bits

#define DECLARE_HASH_OUT_128(func)                                   \
  inline uint128_t func##_128(ByteContainerView data) {              \
    auto hash_bytes = func(data);                                    \
    uint128_t ret;                                                   \
    YACL_ENFORCE(hash_bytes.size() >= sizeof(ret));                  \
    std::memcpy(reinterpret_cast<uint8_t*>(&ret), hash_bytes.data(), \
                sizeof(ret));                                        \
    return ret;                                                      \
  }

DECLARE_HASH_OUT_128(Sha256);  // uint128_t Sha256_128(ByteContainerView data);
DECLARE_HASH_OUT_128(Sm3);     // uint128_t Sm3_128(ByteContainerView data);
DECLARE_HASH_OUT_128(Blake3);  // uint128_t Blake3_128(ByteContainerView data);

#ifndef YACL_WITH_TONGSUO
std::array<uint8_t, 64> Blake2(ByteContainerView data);  // 512-bits
DECLARE_HASH_OUT_128(Blake2);  // uint128_t Blake2_128(ByteContainerView data);
#endif

#undef DECLARE_HASH_OUT_128

}  // namespace yacl::crypto
