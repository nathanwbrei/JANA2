#pragma once
#include <JANA3/Component.h>
#include <JANA3/Queue.h>

using std::vector, std::map, std::string, std::set;

namespace futurejana {

  struct Topology {

    map<string, Queue> queues;
    vector<Op*> components;

  private:
    void link(Op& op, vector<string> input_queue_names, vector<string> output_queue_names) {

      for (string& name : input_queue_names) {
        queues[name].consumers.push_back(&op);
      }
      for (string& name : output_queue_names) {
        queues[name].producers.push_back(&op);
      }
      components.push_back(&op);

      // TODO: Who owns op?
    }

  public:

    void configure_queue(string queue_name, size_t empty_threshold, size_t full_threshold) {

      queues[queue_name].empty_threshold = empty_threshold;
      queues[queue_name].full_threshold = full_threshold;
    }

    void add(SourceOp op) {
      link(op, {}, {op.get_output_queue_name()});
    }

    void add(SinkOp op) {
      link(op, {op.get_input_queue_name()}, {});
    }

    void add(ReduceOp op) {
      link(op, {op.get_input_queue_name()}, {op.get_output_queue_name()});
    }

    void add(MapOp op) {
      link(op, {op.get_input_queue_name()}, {op.get_output_queue_name()});
    }

    void add(GatherOp op) {
      link(op, op.get_input_queue_names(), {op.get_output_queue_name()});
    }

    void add(ScatterOp op) {
      link(op, {op.get_input_queue_name()}, op.get_output_queue_names());
    }
  };
}



