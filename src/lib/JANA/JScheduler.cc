
#include <JANA/JScheduler.h>


JScheduler::JScheduler(const std::vector<JArrow*>& arrows)
    : _arrows(arrows)
    , _next_idx(0)
    {
        _logger = JLogger::everything(); //JLoggingService::logger("JScheduler");
    }


JArrow* JScheduler::next_assignment(uint32_t worker_id, JArrow* assignment, JArrowMetrics::Status last_result) {


    // Short-circuit our scheduler visit if everything is in order.
    if (assignment != nullptr && (last_result == JArrowMetrics::Status::KeepGoing ||
                                  last_result == JArrowMetrics::Status::NotRunYet)) {
        return assignment;
    }

    _mutex.lock();

    // Check latest arrow back in
    if (assignment != nullptr) {
        assignment->update_thread_count(-1);

        if (last_result == JArrowMetrics::Status::Finished) {
            LOG_INFO(_logger) << "JScheduler: Drained arrow " << assignment->get_name() << LOG_END;
            assignment->set_status(JArrow::Status::Drained);
        }

        if (assignment->get_status() == JArrow::Status::Drained && assignment->get_thread_count() == 0) {

            // This was the last worker running this arrow, so it can now deactivate
            // We know it is the last worker because we stopped assigning them
            // once is_upstream_finished started returning true
            assignment->set_status(JArrow::Status::Finished);

            LOG_INFO(_logger) << "JScheduler: Finished arrow " << assignment->get_name() << LOG_END;
            assignment->notify_downstream(false);
        }
    }

    // Choose a new arrow. Loop over all arrows, starting at where we last left off, and pick the first
    // arrow that works
    size_t current_idx = _next_idx;
    do {
        JArrow* candidate = _arrows[current_idx];
        current_idx += 1;
        current_idx %= _arrows.size();

        auto status = candidate->get_status();
        if ((status == JArrow::Status::Running || status == JArrow::Status::Draining) &&
            (candidate->is_parallel() || candidate->get_thread_count() == 0)) {

            // Found a plausible candidate; done
            _next_idx = current_idx; // Next time, continue right where we left off
            candidate->update_thread_count(1);

            LOG_DEBUG(_logger) << "JScheduler: (" << worker_id << ", "
                              << ((assignment == nullptr) ? "idle" : assignment->get_name())
                              << ", " << to_string(last_result) << ") => "
                              << candidate->get_name() << "  [" << candidate->get_thread_count() << "]" << LOG_END;
            _mutex.unlock();
            return candidate;
        }

    } while (current_idx != _next_idx);

    _mutex.unlock();
    return nullptr;  // We've looped through everything with no luck
}


void JScheduler::last_assignment(uint32_t worker_id, JArrow* assignment, JArrowMetrics::Status result) {

    LOG_DEBUG(_logger) << "JScheduler: (" << worker_id << ", "
                       << ((assignment == nullptr) ? "idle" : assignment->get_name())
                       << ", " << to_string(result) << ") => Shutting down!" << LOG_END;
    _mutex.lock();
    if (assignment != nullptr) {
        assignment->update_thread_count(-1);
    }
    _mutex.unlock();
}



