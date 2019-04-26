//
// Created by nbrei on 4/4/19.
//

#ifndef JANA_JWORKER_H
#define JANA_JWORKER_H

#include <thread>
#include <JANA/JLogger.h>
#include <JANA/JScheduler.h>
#include "JWorkerMetrics.h"
#include "JMetrics.h"


using jclock_t = std::chrono::steady_clock;
using duration_t = std::chrono::steady_clock::duration;

class JWorker {
    /// Designed so that the Worker checks in with the Scheduler on his own terms;
    /// i.e. nobody will update the worker's assignment externally. This eliminates
    /// a whole lot of synchronization since we can assume
    /// that the Worker's internal state won't be updated by another thread.

public:
    /// The Worker may be configured to try different backoff strategies
    enum class BackoffStrategy { Constant, Linear, Exponential };
    enum class RunState { Running, Stopping, Stopped };

private:
    /// Machinery that nobody else should modify. These should be protected eventually.
    /// Probably simply make them private and expose via get_status() -> Worker::Status
    unsigned _worker_id;
    unsigned _cpu_id;
    bool _pin_to_cpu;
    RunState _run_state;
    JArrow* _assignment;
    JScheduler* _scheduler;
    std::thread* _thread;    // JWorker encapsulates a thread of some kind. Nothing else should care how.
    JWorkerMetrics _worker_metrics;
    JArrowMetrics _arrow_metrics;
    JLogger _logger;

public:
    /// Configuration options
    unsigned backoff_tries = 4;
    BackoffStrategy backoff_strategy = BackoffStrategy::Exponential;
    duration_t initial_backoff_time = std::chrono::microseconds(1000);
    duration_t checkin_time = std::chrono::milliseconds(500);

    JWorker(unsigned id, JScheduler* scheduler);
    JWorker(unsigned id, unsigned cpuid, JScheduler* scheduler);
    ~JWorker();

    /// If we copy or move the Worker, the underlying std::thread will be left with a
    /// dangling pointer back to `this`. So we forbid copying, assigning, and moving.

    JWorker(const JWorker &other) = delete;
    JWorker(JWorker &&other) = delete;
    JWorker &operator=(const JWorker &other) = delete;

    RunState get_runstate() { return _run_state; };

    void start();
    void request_stop();
    void wait_for_stop();

    /// This is what the encapsulated thread is supposed to be doing
    void loop();

    /// Summarize what/how this Worker is doing. This is meant to be called from
    /// JProcessingController::measure_perf()
    void measure_perf(JMetrics::WorkerSummary& result);

private:
    /// Worker accumulates Arrow-specific metrics. These need to propagate back to
    /// one central place. There are two things which trigger this:
    /// 1. JWorker::measure_perf()
    /// 2. JWorker::loop() after an arrow assignment change
    void publish_arrow_metrics();
};


#endif