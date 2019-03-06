#pragma once
#include <JANA3/Component.h>
#include <JANA3/Queue.h>

using std::vector, std::map, std::string, std::set;

namespace futurejana {

  class Topology {

    map<string, Queue> _queues;
    vector<Op> _components;

   public:

    bool validate() {
      // verify that all queues have at least one producer and one consumer
      // verify that at least one EmitOp, one ReduceOp exist
    }

  }

}

