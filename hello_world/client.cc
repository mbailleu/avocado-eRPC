#include "rpc.h"

erpc::Rpc<erpc::CTransport> *rpc;
erpc::MsgBuffer req;
erpc::MsgBuffer resp;

void cont_func(void *, void *) {
  assert(resp.get_data_size() == 16);

  // The response has its first 16 bytes set to 'p'. The server sets the 17th
  // byte to 'q', but the client does not receive that byte.
  for (size_t i = 0; i <= 16; i++) {
    printf("%zu: %c\n", i, resp.buf[i]);
    if (i < 16) assert(resp.buf[i] == 'p');
  }
  printf("\n");
}

void sm_handler(int, erpc::SmEventType, erpc::SmErrType, void *) {}

int main() {
  erpc::Nexus nexus("localhost:31850", 0, 0);
  rpc = new erpc::Rpc<erpc::CTransport>(&nexus, nullptr, 0, sm_handler);

  int session_num = rpc->create_session("localhost:31851", 0);
  while (!rpc->is_connected(session_num)) rpc->run_event_loop_once();

  req = rpc->alloc_msg_buffer_or_die(32);
  resp = rpc->alloc_msg_buffer_or_die(16400);
  assert(req.get_data_size() == 32);
  assert(resp.get_data_size() == 16400);

  rpc->enqueue_request(session_num, 9 /* req_type */, &req, &resp, cont_func,
                       nullptr);
  rpc->run_event_loop(100);

  delete rpc;
}
