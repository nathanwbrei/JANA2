

#include <greenfield/ThreadManager.h>
#include <greenfield/Worker.h>
#include <greenfield/Logger.h>

using std::string;

namespace greenfield {


    ThreadManager::ThreadManager(Scheduler& scheduler) : _scheduler(scheduler) {

        _status = Status::Idle;
    };


    ThreadManager::~ThreadManager() {
        LOG_DEBUG(logger) << "ThreadManager destruction started." << LOG_END;
        stop();
        join();
        LOG_DEBUG(logger) << "ThreadManager destruction finished." << LOG_END;
    }


    ThreadManager::Status ThreadManager::get_status() {
        return _status;
    }


    std::vector<ThreadManager::WorkerStatus> ThreadManager::get_worker_statuses() {
        std::vector<WorkerStatus> statuses;
        for (Worker* worker : _workers) {
            if (worker != nullptr) {  // TODO: Get rid of this when idle state no longer = shutdown
                WorkerStatus status;
                status.worker_id = worker->worker_id;
                status.arrow_name = ((worker->assignment == nullptr) ? "idle" : worker->assignment->get_name());
                status.is_running = !worker->shutdown_achieved;
                statuses.push_back(status);
            }
        }
        return statuses;
    }


    ThreadManager::Response ThreadManager::run(int nthreads) {

        LOG_DEBUG(logger) << "ThreadManager::run() called." << LOG_END;
        if (_status != Status::Idle) {
            return Response::AlreadyRunning;
        }
        _status = Status::Running;
        _workers.reserve(nthreads);

        for (int i=0; i<nthreads; ++i) {
            Worker * worker = new Worker(i, _scheduler);
            worker->_logger = logger;
            _workers.push_back(worker);
        }
        LOG_INFO(logger) << "ThreadManager status changed to RUNNING." << LOG_END;
        return Response::Success;
    }


    ThreadManager::Response ThreadManager::stop() {
        if (_status == Status::Idle) {
            return Response::NotRunning;
        }
        _status = Status::Stopping;
        bool all_stopped = true;
        for (Worker * worker : _workers) {
            worker->shutdown_requested = true;
            all_stopped &= worker->shutdown_achieved;
        }
        if (all_stopped) {
            return Response::Success;
        }
        else {
            return Response::InProgress;
        }
    }


    ThreadManager::Response ThreadManager::join() {

        LOG_DEBUG(logger) << "ThreadManager::join() called." << LOG_END;
        if (_status == Status::Idle) {
            return Response::NotRunning;
        }

        // Destroying workers will join their respective threads.
        while (_workers.size() > 0) {
            Worker* worker = _workers.back();
            _workers.pop_back();
            delete worker;
        }

        _status = Status::Idle;
        LOG_INFO(logger) << "ThreadManager status changed to IDLE" << LOG_END;
        return Response::Success;
    }

}


