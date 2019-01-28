#include "rpc.h"

erpc::Rpc<erpc::CTransport> *rpc;

void req_handler(erpc::ReqHandle *req_handle, void *) {
  // The two options below show how to use the preallocated response msgbuf,
  // or the dynamic response msgbuf.

  // Option 1:
  // auto &resp = req_handle->pre_resp_msgbuf;
  // rpc->resize_msg_buffer(&resp, 16);
  // End option 1

  // Option 2
  auto &resp = req_handle->dyn_resp_msgbuf;
  resp = rpc->alloc_msg_buffer(16);
  // End option 2

  assert(resp.get_data_size() == 16);

  memset(resp.buf, 'p', 16);  // Client will see first 16 B of response = 'p'
  resp.buf[16] = 'q';         // Client won't see this byte
  rpc->enqueue_response(req_handle, &resp);
}

int main() {
  erpc::Nexus nexus("localhost:31851", 0, 0);
  nexus.register_req_func(9 /* req type */, req_handler);

  rpc = new erpc::Rpc<erpc::CTransport>(&nexus, nullptr, 0, nullptr);
  rpc->run_event_loop(100000);
}
