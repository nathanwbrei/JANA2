#pragma once
#include <JANA3/Component.h>
#include <JANA3/Queue.h>

using std::vector, std::map, std::string, std::set;

namespace futurejana {

  struct Topology {

    map<string, Queue> queues;
    vector<Op> components;

  private:
    void link(Op& op, vector<string> input_queue_names, vector<string> output_queue_names) {
      // instantiate queues if they don't already exist
      // add op to corresponding producers, consumers lists
      // Want Topology to own Op eventually
      // Worker has to own a sequential op, somehow
    }

  public:
    void add(SourceOp op) {
      link(op, {}, {op.get_output_queue_name()});
    }

    void add(SinkOp op) {
      add_queues(op, {op.get_input_queue_name()}, {});
    }

    void add(ReduceOp op) {
      add_queues(op, {op.get_input_queue_name()}, {op.get_output_queue_name()});
    }

    void add(MapOp op) {
      add_queues(op, {op.get_input_queue_name()}, {op.get_output_queue_name()});
    }

    void add(GatherOp op) {
      add_queues(op, op.get_input_queue_names(), {op.get_output_queue_name()});
    }

    void add(ScatterOp op) {
      add_queues(op, {op.get_input_queue_name()}, op.get_output_queue_names());
    }
  };
}



