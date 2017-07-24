#ifndef PROFILE_RANDOM_H
#define PROFILE_RANDOM_H

#include "large_rpc_tput.h"

size_t get_session_idx_func_random(AppContext *c, size_t) {
  size_t session_vec_size = c->session_num_vec.size();

  // We don't need Lemire's trick bc messages are large
  size_t rand_session_idx = c->fastrand.next_u32() % session_vec_size;
  while (rand_session_idx == c->self_session_idx) {
    rand_session_idx = c->fastrand.next_u32() % session_vec_size;
  }

  return rand_session_idx;
}

void connect_sessions_func_random(AppContext *c) {
  c->self_session_idx = FLAGS_machine_id * FLAGS_num_threads + c->thread_id;

  // Allocate per-session info
  size_t num_sessions = FLAGS_num_machines * FLAGS_num_threads;
  c->session_num_vec.resize(num_sessions);
  std::fill(c->session_num_vec.begin(), c->session_num_vec.end(), -1);

  // Initiate connection for sessions
  fprintf(stderr,
          "large_rpc_tput: Thread %zu: Creating %zu sessions. "
          "Profile = 'random'.",
          c->thread_id, num_sessions);
  for (size_t m_i = 0; m_i < FLAGS_num_machines; m_i++) {
    std::string hostname = get_hostname_for_machine(m_i);

    for (size_t t_i = 0; t_i < FLAGS_num_threads; t_i++) {
      size_t session_idx = (m_i * FLAGS_num_threads) + t_i;
      // Do not create a session to self
      if (session_idx == c->self_session_idx) continue;

      c->session_num_vec[session_idx] = c->rpc->create_session(
          hostname, static_cast<uint8_t>(t_i), kAppPhyPort);

      if (c->session_num_vec[session_idx] < 0) {
        throw std::runtime_error("Failed to create session.");
      }
    }
  }

  while (c->num_sm_resps != FLAGS_num_machines * FLAGS_num_threads - 1) {
    c->rpc->run_event_loop(200);  // 200 milliseconds
    if (ctrl_c_pressed == 1) return;
  }
}

#endif