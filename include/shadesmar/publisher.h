//
// Created by squadrick on 7/30/19.
//

#ifndef shadesmar_PUBLISHER_H
#define shadesmar_PUBLISHER_H

#include <cstdint>

#include <cstring>
#include <iostream>
#include <memory>
#include <string>

#include <msgpack.hpp>

#include <shadesmar/memory.h>

namespace shm {
template <typename msgT, uint32_t queue_size> class Publisher {

public:
  explicit Publisher(std::string topic_name);

  bool publish(std::shared_ptr<msgT> msg);
  bool publish(msgT &msg);
  bool publish(msgT *msg);

private:
  std::string topic_name_;
  std::shared_ptr<Memory<queue_size>> mem_;
};

template <typename msgT, uint32_t queue_size>
Publisher<msgT, queue_size>::Publisher(std::string topic_name)
    : topic_name_(topic_name) {
  static_assert(std::is_base_of<BaseMsg, msgT>::value,
                "msgT must derive from BaseMsg");
  mem_ = std::make_shared<Memory<queue_size>>(topic_name);
}

template <typename msgT, uint32_t queue_size>
bool Publisher<msgT, queue_size>::publish(std::shared_ptr<msgT> msg) {
  return publish(msg.get());
}

template <typename msgT, uint32_t queue_size>
bool Publisher<msgT, queue_size>::publish(msgT &msg) {
  return publish(&msg);
}

template <typename msgT, uint32_t queue_size>
bool Publisher<msgT, queue_size>::publish(msgT *msg) {
  uint32_t seq = mem_->fetch_inc_counter();
  msg->seq = seq;
  msgpack::sbuffer buf;
  try {
    msgpack::pack(buf, *msg);
  } catch (...) {
    return false;
  }
  return mem_->write(buf.data(), seq, buf.size());
}

} // namespace shm
#endif // shadesmar_PUBLISHER_H
