//
// Created by nbrei on 4/8/19.
//

#ifndef JANA2_JEVENTPROCESSORARROW_H
#define JANA2_JEVENTPROCESSORARROW_H


#include <JANA/JEvent.h>
#include <JANA/JArrow.h>
#include <JANA/Queue.h>

class JEventProcessorArrow : public JArrow {

public:
    using Event = std::shared_ptr<const JEvent>;
    using EventQueue = Queue<Event>;
    friend class JEventSourceArrow;  // So that we can connect the two. TODO: Do this at the JArrow level instead

private:
    std::vector<JEventProcessor*> _processors;
    EventQueue* _input_queue;
    EventQueue* _output_queue;
    JLogger _logger;

public:

    explicit JEventProcessorArrow(std::string name);
    void add_processor(JEventProcessor* processor);

    void execute(JArrowMetrics& result) final;
    size_t get_pending() final;
    size_t get_threshold() final;
    void set_threshold(size_t) final;
};


#endif //JANA2_JEVENTPROCESSORARROW_H
