#pragma once
#include <JANA3/Component.h>
#include <JANA3/Queue.h>

using std::vector, std::map, std::string;

namespace futurejana {

  class Topology {
    map<string, Queue> queues;
    vector<Op> components;

  public:
    Queue& get_queue(string name) {
      return queues[name];
    }

    void add(Op component) {
      components.push_back(component);
    }



  }

}
