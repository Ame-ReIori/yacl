// Copyright 2023 Ant Group Co., Ltd.
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

#include <condition_variable>
#include <memory>
#include <mutex>
#include <utility>

#include "brpc/channel.h"
#include "brpc/server.h"
#include "bthread/bthread.h"
#include "bthread/condition_variable.h"

#include "yacl/link/ssl_options.h"
#include "yacl/link/transport/channel.h"
#include "yacl/link/transport/channel_chunked_base.h"

namespace yacl::link::transport::util {

class BlackBoxPushDone;

}

namespace blackbox_interconnect {

class PushInbound;
class TransportOutbound;
class PopInbound;

}  // namespace blackbox_interconnect

namespace yacl::link::transport {

class ChannelBrpcBlackBox;

class ReceiverLoopBlackBox final : public IReceiverLoop {
 public:
  ~ReceiverLoopBlackBox() override;

  void Stop() override;

  // start the receiver loop.
  void Start();
  void AddListener(size_t rank, std::shared_ptr<ChannelBrpcBlackBox> listener) {
    auto ret = listeners_.emplace(rank, std::move(listener));
    if (!ret.second) {
      YACL_THROW_LOGIC_ERROR("duplicated listener for rank={}", rank);
    }
  }

 protected:
  std::map<size_t, std::shared_ptr<ChannelBrpcBlackBox>> listeners_;
  std::vector<std::thread> threads_;
};

class ChannelBrpcBlackBox final : public ChannelChunkedBase {
 public:
  static ChannelChunkedBase::Options GetDefaultOptions() {
    return ChannelChunkedBase::Options{10 * 1000, 512 * 1024, "http", ""};
  }

 public:
  using ChannelChunkedBase::ChannelChunkedBase;

  ~ChannelBrpcBlackBox() override {
    if (is_recv_.load()) {
      StopReceive();
    }
  }

  void PushRequest(org::interconnection::link::PushRequest& request,
                   uint32_t timeout_ms) override;

  void SetPeerHost(const std::string& self_id, const std::string& self_node_id,
                   const std::string& peer_id, const std::string& peer_node_id,
                   const SSLOptions* ssl_opts);

  void SendPopRequest();

  brpc::ChannelOptions GetChannelOption(const SSLOptions* ssl_opts);
  uint32_t GetQueueFullWaitTime() const { return push_wait_ms_; }

  void SetHttpHeader(brpc::Controller* controller, const std::string& topic);
  void OnPopResponse(blackbox_interconnect::TransportOutbound* response);

  // receive related
  void StartReceive();
  bool CanReceive();
  void TryReceive();
  void StopReceive();
  uint32_t GetPopTimeoutS() const { return pop_timeout_s_; }

  void TransResponse(
      const blackbox_interconnect::TransportOutbound* new_response,
      org::interconnection::link::PushResponse* response);

 protected:
  // brpc channel related.
  std::shared_ptr<brpc::Channel> channel_;
  std::string send_topic_;
  std::string recv_topic_;
  std::string host_;
  std::string peer_host_;
  std::atomic_bool is_recv_{false};

  std::map<std::string, std::string> http_headers_;

  uint32_t pop_timeout_s_{
      1};  // Pop 操作超过该时间如果没有消息，传输节点返回空，errorCode 为OK
  uint32_t push_wait_ms_{5000};
  inline static const std::string kUrlPrefix = "/v1/interconn/chan/";
};

}  // namespace yacl::link::transport
