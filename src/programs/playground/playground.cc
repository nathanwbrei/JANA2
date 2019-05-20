

#include <queue>
#include <string>
#include <memory>

struct ArrowMetrics {};

template <typename T>
struct Mailbox {
    std::queue<T> underlying;
};

struct Arrow {

    enum class Type {Source, Sink, Stage};
    enum class Status {Unopened, Inactive, Running, Draining, Drained, Finished, Closed};

    const std::string _name;
    const Type _type;
    const bool _is_parallel;

    std::vector<Arrow*> upstream;   // non-owning
    std::vector<Arrow*> downstream;

    Status _status = Status::Unopened;
    ArrowMetrics _metrics;
    size_t _chunksize = 1;
    size_t _thread_count = 0;

    Arrow(std::string name, Type type, bool is_parallel)
        : _name(name)
        , _type(type)
        , _is_parallel(is_parallel) {};

    virtual void execute(ArrowMetrics& result) {};

    virtual void initialize() {
        _status = Status::Inactive;
    }

    virtual void finalize() {
        _status = Status::Closed;
    }

    void activate() {
        switch (_status) {
            case Status::Unopened: initialize();
            case Status::Inactive: _status = Status::Running; notify(); break;
            case Status::Running:
            case Status::Drained:
            case Status::Draining: break;
            case Status::Finished:
            case Status::Closed: break; // complain
        }
    }

    void deactivate(bool drain = false) {
        // TODO: Need to lock the arrow mutex, and so does Scheduler
        switch (_status) {
            case Status::Running:
            case Status::Drained:
            case Status::Draining:
                _status = Status::Inactive;
                if (!drain) notify();  // If we don't notify, upstream queues will drain before deactivation
                break;

            case Status::Unopened:
            case Status::Inactive:
            case Status::Finished:
            case Status::Closed: break;
        }
    }

    void notify() {
        // notify all downstream arrows
        for (Arrow* arrow : downstream) {
            arrow->update();
        }
    }

    virtual void update() {
        // check statuses of all upstream arrows
        switch (_status) {
            case Status::Unopened: initialize();
            case Status::Inactive: // If anything downstream is active, I should be active
            case Status::Running: // If everything downstream is finished, I should be draining
            case Status::Draining: // If anything downstream is running, I should be running
            case Status::Drained: // If anything downstream is running, I should be running
            case Status::Finished: // If anything downstream is running, I should be running
            case Status::Closed: break; // I should complain
        }
    }

};

template <typename S, typename T>
class ArrowT : Arrow {
public:
    std::shared_ptr<Mailbox<S>> inbox;
    std::vector<std::shared_ptr<Mailbox<T>>> outboxes;

    ArrowT(std::string name, bool is_parallel) : Arrow(name, Type::Stage, is_parallel) {};

    template <typename U>
    void connect(ArrowT<T,U>* downstream) {
        outboxes.push_back(downstream->inbox);

    }
};

template <typename T>
class ArrowT<T,void> : Arrow {
public:
    std::shared_ptr<Mailbox<T>> inbox;
    ArrowT(std::string name, bool is_parallel) : Arrow(name, Type::Sink, is_parallel) {};
};


template <typename S>
class ArrowT<void,S> : Arrow {
public:
    std::vector<std::shared_ptr<Mailbox<S>>> outboxes;

    ArrowT(std::string name, bool is_parallel) : Arrow(name, Type::Source, is_parallel) {};

    template <typename T>
    void connect(ArrowT<S,T>* downstream) {
        outboxes.push_back(downstream->inbox);
    }
};

int main() {
    ArrowT<void, double> x("input", false);
    ArrowT<double, int> y("process", true);
    ArrowT<int, void> z("output", false);
    x.connect(&y);
    y.connect(&z);

}



