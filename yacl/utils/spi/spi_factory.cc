// Copyright 2023 Ant Group Co., Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "yacl/utils/spi/spi_factory.h"

namespace yacl {

inline namespace literals {
SpiArgKey operator""_arg(const char* name, size_t s) {
  return SpiArgKey(std::string(name, s));
}
}  // namespace literals

const std::string& SpiArg::Key() const { return key_; }

bool SpiArg::HasValue() { return value_.has_value(); }

SpiArgs::SpiArgs(std::initializer_list<SpiArg> args) {
  for (const auto& item : args) {
    insert({item.Key(), item});
  }
}

SpiArg SpiArgs::Get(const SpiArgKey& arg) {
  auto it = find(arg.Key());
  if (it == end()) {
    return SpiArg{arg.Key()};
  } else {
    return it->second;
  }
}

}  // namespace yacl
