#pragma once

namespace futurejana {

  // ThreadManager controls the assignment of ops to threads, implementing
  // some kind of rebalancing strategy


  class Worker {

    Op _assignment;
    bool _finished;

  public:
    void loop() {
      while (!_finished) {
        _assignment.execute(topologyManager);

        // Worry about who updates the worker's assignment later
      }
    }

  // Right now this only reports thread as
  struct ThreadStatus {
    string op_name;
    int nthreads;
    int events_completed;
    double latency;
  }

  class IThreadManager {
  public:
    void run(Topology& topology, int nthreads = 0);
    void soft_stop();
    void hard_stop();
    vector<ThreadStatus> status();

  }



} 


