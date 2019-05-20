//
// Created by nbrei on 4/8/19.
//

#include <JANA/JEventSourceArrow.h>
#include <JANA/JApplication.h>
#include "JEventSourceArrow.h"
#include "JEventProcessorArrow.h"


using SourceStatus = JEventSource::RETURN_STATUS;

JEventSourceArrow::JEventSourceArrow(std::string name,
                                     JEventSource* source,
                                     JResourcePoolSimple<JFactorySet>* pool
                                     )
    : JArrow(name, false, NodeType::Source)
    , _source(source)
    , _pool(pool)
    , _logger(JLogger::nothing()) {};


void JEventSourceArrow::execute(JArrowMetrics& result) {

    if (!is_active()) {
        result.update_finished();
        return;
    }

    SourceStatus in_status = SourceStatus::kSUCCESS;
    auto start_time = std::chrono::steady_clock::now();
    try {
        size_t item_count = get_chunksize();
        while (item_count-- != 0) {
            auto event = std::make_shared<JEvent>(japp);
            event->SetFactorySet(japp->GetFactorySet());
            event->SetJEventSource(_source);
            _source->GetEvent(event);
            _chunk_buffer.push_back(event);
        }
    }
    catch (SourceStatus rs) {
        in_status = rs;
    }
    catch (...) {
        in_status = SourceStatus::kERROR;
    }
    auto latency_time = std::chrono::steady_clock::now();
    auto message_count = _chunk_buffer.size();
    auto out_status = _output_queue->push(_chunk_buffer);
    _chunk_buffer.clear();
    auto finished_time = std::chrono::steady_clock::now();

    LOG_DEBUG(_logger) << "JEventSourceArrow '" << get_name() << "': "
                       << "Emitted " << message_count << " events; last GetEvent "
                       << ((in_status==SourceStatus::kSUCCESS) ? "succeeded" : "failed")
                       << LOG_END;

    auto latency = latency_time - start_time;
    auto overhead = finished_time - latency_time;
    JArrowMetrics::Status status;

    if (in_status == SourceStatus::kNO_MORE_EVENTS) {
        // There should be a _source.Close() of some kind
        status = JArrowMetrics::Status::Finished;
    }
    else if (in_status == SourceStatus::kSUCCESS && out_status == EventQueue::Status::Ready) {
        status = JArrowMetrics::Status::KeepGoing;
    }
    else {
        status = JArrowMetrics::Status::ComeBackLater;
    }
    result.update(status, message_count, 1, latency, overhead);
}

void JEventSourceArrow::initialize() {
    assert(_status == Status::Unopened);
    LOG_INFO(_logger) << "JEventSourceArrow '" << get_name() << "': "
                      << "Initializing" << LOG_END;
    _source->Open();
    _status = Status::Inactive;
}

void JEventSourceArrow::connect(JEventProcessorArrow& processor) {
    _output_queue = processor._input_queue;

}

