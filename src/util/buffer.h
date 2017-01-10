#ifndef ERPC_BUFFER_H
#define ERPC_BUFFER_H

#include "common.h"

namespace ERpc {

// Variable-sized buffer
class Buffer {
 public:
  Buffer(size_t size);
  ~Buffer();

  size_t size;
  void *buf;
};

}  // End ERpc

#endif  // ERPC_BUFFER_H