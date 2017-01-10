#ifndef ERPC_RPC_H
#define ERPC_RPC_H

#include <random>
#include <vector>
#include "common.h"
#include "nexus.h"
#include "session.h"
#include "transport.h"
#include "util/buffer.h"
#include "util/rand.h"

namespace ERpc {

#define rpc_dprintf(fmt, ...)            \
  do {                                   \
    if (RPC_DPRINTF) {                   \
      fprintf(stderr, fmt, __VA_ARGS__); \
      fflush(stderr);                    \
    }                                    \
  } while (0)

// Per-thread RPC object
template <class Transport_>
class Rpc {
  const uint64_t kStartSeqMask = ((1ull << 48) - 1ull);

 public:
  // rpc.cc
  Rpc(Nexus *nexus, void *context, int app_tid,
      session_mgmt_handler_t session_mgmt_handler,
      std::vector<int> fdev_port_vec);

  ~Rpc();

  Session *create_session(int local_fdev_port_index, const char *_rem_hostname,
                          int rem_app_tid, int rem_fdev_port_index);

  void connect_session(Session *session);

  // rpc_datapath.cc
  void send_request(const Session *session, const Buffer *buffer);
  void send_response(const Session *session, const Buffer *buffer);

  // rpc_ev_loop.cc

  /**
   * @brief Run one iteration of the event loop
   */
  void run_event_loop_one();

  /**
   * @brief Run the event loop forever
   */
  inline void run_event_loop() {
    while (true) {
      run_event_loop_one();
    }
  }

  /**
   * @brief Run the event loop for \p timeout_ms milliseconds
   */
  inline void run_event_loop_timeout(size_t timeout_ms) {
    double freq_ghz = nexus->get_freq_ghz();
    uint64_t start_tsc = rdtsc();

    while (true) {
      run_event_loop_one();

      double elapsed_ms = to_sec(rdtsc() - start_tsc, freq_ghz) * 1000;
      if (elapsed_ms > timeout_ms) {
        return;
      }
    }
  }

  // rpc_session_mgmt.cc
  void handle_session_management();

 private:
  // Constructor args
  Nexus *nexus;
  void *context; /* The application context */
  int app_tid;
  session_mgmt_handler_t session_mgmt_handler;
  int num_fdev_ports;
  int fdev_port_arr[kMaxFabDevPorts];

  // Others
  int next_session_num;
  Transport_ *transport; /* The unreliable transport */
  std::vector<Session *> session_vec;
  SessionMgmtHook sm_hook; /* Shared with Nexus for session management */
  SlowRand slow_rand;
  double freq_ghz; /* Copied from Nexus to avoid false sharing */

  // Private methods

  // rpc.cc
  uint64_t generate_start_seq();
  bool is_session_managed(Session *session);
};

/* Instantiate required Rpc classes so they get compiled for the linker */
template class Rpc<InfiniBandTransport>;

}  // End ERpc

#endif  // ERPC_RPC_H