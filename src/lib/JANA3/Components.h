#pragma once
#include <vector>
#include <map>

using std::string, std::vector, std::multimap, std::pair;


struct Op {
  int chunksize = 1;
  virtual void execute(Topology& t);
};

// Sequential, stateful operations

struct SourceOp : Op {
  virtual vector<Event> inprocess() = 0;
  virtual void init() = 0;
  virtual void finish() = 0;
  virtual string get_output_queue_name() = 0;
  virtual bool is_finished() = 0;
};

struct SinkOp : Op {
  virtual void outprocess(vector<Event>) = 0;
  virtual void init() = 0;
  virtual void finish() = 0;
  virtual string get_input_queue_name() = 0;
  virtual bool is_finished() = 0;
}

struct ReduceOp : Op {
  virtual vector<Event> reduce(vector<Event> e) = 0;
  virtual void init() = 0;
  virtual vector<Event> finish() = 0;
  virtual string get_input_queue_name() = 0;
  virtual string get_output_queue_name() = 0;
  virtual bool is_finished() = 0;
};

// Parallel, stateless operations

struct MapOp : Op {
  virtual vector<Event> map(vector<Event> e) = 0;
  virtual string get_input_queue_name() = 0;
  virtual string get_output_queue_name() = 0;
};

struct ScatterOp : Op {
  virtual multimap<string, Event> scatter(vector<Event> e) = 0;
  virtual string get_input_queue_name() = 0;
  virtual vector<string> get_output_queue_names() = 0;
};

struct GatherOp : Op {
  virtual vector<Event> gather(multimap<string, Event> e) = 0;
  virtual vector<string> get_input_queue_names() = 0;
  virtual string get_output_queue_name() = 0;
};


