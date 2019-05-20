//
// Created by nbrei on 3/25/19.
//

#ifndef GREENFIELD_MAPARROW_H
#define GREENFIELD_MAPARROW_H

#include <JANA/JArrow.h>


/// ParallelProcessor transforms S to T and it does so in a way which is thread-safe
/// and ideally stateless. It is conceptually equivalent to the first part
/// of JEventProcessor::Process, i.e. up until the lock is acquired. Alternatively, it could
/// become a JFactorySet, in which case process() would call all Factories present, thereby
/// making sure that everything which can be calculated in parallel has in fact been, before
/// proceeding to the (sequential) Sink.

template <typename S, typename T>
struct ParallelProcessor {
    virtual T process(S s) = 0;
};


/// MapArrow lifts a ParallelProcessor into a streaming async context
template<typename S, typename T>
class MapArrow : public JArrow {

private:
    ParallelProcessor<S,T>& _processor;
    Queue<S> *_input_queue;
    Queue<T> *_output_queue;

public:
    MapArrow(std::string name, ParallelProcessor<S,T>& processor, Queue<S> *input_queue, Queue<T> *output_queue)
           : JArrow(name, true, NodeType::Stage)
           , _processor(processor)
           , _input_queue(input_queue)
           , _output_queue(output_queue) {

        _input_queue->attach_downstream(this);
        _output_queue->attach_upstream(this);
        attach_upstream(_input_queue);
        attach_downstream(_output_queue);
    };

    void execute(JArrowMetrics& result) override {

        auto start_total_time = std::chrono::steady_clock::now();
        std::vector<S> xs;
        std::vector<T> ys;
        xs.reserve(get_chunksize());
        ys.reserve(get_chunksize());
        // TODO: These allocations are unnecessary and should be eliminated

        auto in_status = _input_queue->pop(xs, get_chunksize());

        auto start_latency_time = std::chrono::steady_clock::now();
        for (S &x : xs) {
            ys.push_back(_processor.process(x));
        }
        auto message_count = xs.size();
        auto end_latency_time = std::chrono::steady_clock::now();

        auto out_status = QueueBase::Status::Ready;
        if (!ys.empty()) {
            out_status = _output_queue->push(ys);
        }
        auto end_queue_time = std::chrono::steady_clock::now();


        auto latency = (end_latency_time - start_latency_time);
        auto overhead = (end_queue_time - start_total_time) - latency;

        JArrowMetrics::Status execution_status;
        if (in_status == QueueBase::Status::Ready && out_status == QueueBase::Status::Ready) {
            execution_status = JArrowMetrics::Status::KeepGoing;
        }
        else {
            execution_status = JArrowMetrics::Status::ComeBackLater;
        }
        result.update(execution_status, message_count, 1, latency, overhead);
    }

    size_t get_pending() final { return _input_queue->get_item_count(); }

    size_t get_threshold() final { return _input_queue->get_threshold(); }

    void set_threshold(size_t threshold) final { _input_queue->set_threshold(threshold); }
};


#endif //GREENFIELD_MAPARROW_H
