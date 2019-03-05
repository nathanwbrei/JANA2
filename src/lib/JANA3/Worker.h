#pragma once

namespace jana {

typedef Event task_t;

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


  friend void EmitOp::execute() {
    Queue output_queue = topologyManager.get_queue(get_output_queue_name());
    int chunksize = output_queue.get_
    for (int i=0; i<)

    vector<Event> 
  }
  void execute(ReduceOp x);
  void execute(MapOp x);
  void execute(FlatMapOp x);



}

}
