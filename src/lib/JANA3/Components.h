#pragma once
#include <vector>
#include <map>

using std::string, std::vector, std::multimap, std::pair;

struct Op {};

struct EmitOp : Op {
  virtual Event emit() = 0;
  virtual void init() = 0;
  virtual void finish() = 0;
  virtual string get_output_queue_name() = 0;
  virtual bool is_finished() = 0;
};

struct ReduceOp : Op {
  virtual void reduce(Event e) = 0;
  virtual void init() = 0;
  virtual void finish() = 0;
  virtual string get_input_queue_name() = 0;
};

struct MapOp : Op {
  virtual Event map(Event e) = 0;
  virtual string get_input_queue_name() = 0;
  virtual string get_output_queue_name() = 0;
};

struct FlatMapOp : Op {
  virtual vector<Event> flatmap(Event e) = 0;
  virtual string get_input_queue_name() = 0;
  virtual string get_output_queue_name() = 0;
};

struct ScatterOp : Op {
  virtual pair<string, Event> scatter(Event e) = 0;
  virtual string get_input_queue_name() = 0;
  virtual vector<string> get_output_queue_name() = 0;
};

struct FlatScatterOp : Op {
  virtual multimap<string, Event> flatscatter(Event e) = 0;
  virtual string get_input_queue_name() = 0;
  virtual vector<string> get_output_queue_names() = 0;
};

struct GatherOp : Op {
  virtual pair<string, Event> gather(Event e) = 0;
  virtual vector<string> get_input_queue_names() = 0;
  virtual string get_output_queue_name() = 0;
};

struct FlatGatherOp : Op {
  virtual multimap<string, Event> flatgather(Event e) = 0;
  virtual vector<string> get_input_queue_names() = 0;
  virtual string get_output_queue_name() = 0;
}



