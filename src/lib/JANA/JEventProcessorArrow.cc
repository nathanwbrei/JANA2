//
// Created by nbrei on 4/8/19.
//

#include <JANA/JEventProcessorArrow.h>
#include <JANA/JEventProcessor.h>
#include "JEventProcessorArrow.h"


JEventProcessorArrow::JEventProcessorArrow(std::string name)
        : JArrow(std::move(name), true, NodeType::Sink)
        , _input_queue(new EventQueue)
        , _logger(JLogger::nothing()) { };


void JEventProcessorArrow::add_processor(JEventProcessor* processor) {
    _processors.push_back(processor);
}

void JEventProcessorArrow::execute(JArrowMetrics& result) {

    auto start_total_time = std::chrono::steady_clock::now();

    Event x;
    bool success;
    auto in_status = _input_queue->pop(x, success);
    LOG_DEBUG(_logger) << "EventProcessorArrow '" << get_name() << "': "
                       << "pop() returned " << ((success) ? "success" : "failure")
                       << "; queue is now " << to_string(in_status) << LOG_END;

    auto start_latency_time = std::chrono::steady_clock::now();
    if (success) {
        LOG_DEBUG(_logger) << "EventProcessorArrow '" << get_name() << "': Starting event# " << x->GetEventNumber() << LOG_END;
        for (JEventProcessor* processor : _processors) {
            processor->Process(x);
        }
        LOG_DEBUG(_logger) << "EventProcessorArrow '" << get_name() << "': Finished event# " << x->GetEventNumber() << LOG_END;
    }
    auto end_latency_time = std::chrono::steady_clock::now();

    auto out_status = EventQueue::Status::Ready;

    if (success) {
        if (_output_queue != nullptr) {
            out_status = _output_queue->push(x);
        }
        else {
            JEvent& underlying = const_cast<JEvent&>(*x);
            underlying.Release();
        }
    }
    auto end_queue_time = std::chrono::steady_clock::now();

    JArrowMetrics::Status status;
    if (in_status == EventQueue::Status::Ready && out_status == EventQueue::Status::Ready) {
        status = JArrowMetrics::Status::KeepGoing;
    }
    else {
        status = JArrowMetrics::Status::ComeBackLater;
    }
    auto latency = (end_latency_time - start_latency_time);
    auto overhead = (end_queue_time - start_total_time) - latency;
    result.update(status, success, 1, latency, overhead);
}

size_t JEventProcessorArrow::get_pending() {
    return _input_queue->get_item_count();
}

size_t JEventProcessorArrow::get_threshold() {
    return _input_queue->get_threshold();
}

void JEventProcessorArrow::set_threshold(size_t threshold) {
    _input_queue->set_threshold(threshold);
}

