

#include <queue>
#include <string>
#include <memory>
#include <iostream>


struct Arrow {
    virtual void execute() {}
};

// ArrowsWithInput, in the general case, have a shared pointer to an input queue
template <typename InT>
struct ArrowWithInput {

    // The ArrowWithInput creates its own input queue
    InT input;

    // We want all ArrowsWithOutput to have private access to this
    template <typename OutT> friend class ArrowWithOutput;
};

// If the ArrowWithInput _doesn't_ have an input queue (i.e. it is a Source),
// we handle this with a template specialization which does nothing instead.
template <> struct ArrowWithInput<void> {};


// Similarly, we define the general ArrowWithOutput
template <typename OutT>
struct ArrowWithOutput {

    // ArrowsWithOutput have one or more output queues of the correct type.
    // It doesn't create these, instead it gets them via connect()
    OutT output;

public:
    // We define a mechanism for connecting an ArrowWithOutput to an ArrowWithInput
    template <typename InT>
    void connect(ArrowWithInput<InT>* downstream) {
        output = downstream->input;
    }
};

// In case our arrow doesn't actually have output, don't do any of the above
template <> struct ArrowWithOutput<void> {};


// Finally we create our typed Arrow from S to T by inheriting from ArrowWithInput and ArrowWithOutput.
template <typename S, typename T>
struct ArrowT: public Arrow, public ArrowWithInput<S>, public ArrowWithOutput<T> {
};


struct Event {
    int runNumber;
    int eventNumber;
    std::vector<double> data;
};


struct EvtSrcArrow : public ArrowT<void, Event> {

    void execute() override {
        Event result;
        result.eventNumber = 22;
        output = result;
    }
};

struct EvtProcArrow : public ArrowT<Event, Event> {

    void execute() override {
        auto x = input;
        x.data.push_back(22);
        output = x;
    }
};

struct EvtSinkArrow : public ArrowT<Event, void> {

    void execute() override {
        auto x = input;
        std::cout << "Finished with " << x.eventNumber << std::endl;
    }
};

int main() {
    ArrowT<void, double> x;
    x.output = 22.2;
    ArrowT<double, int> y;
    y.input = 33.3;
    y.output = 4;
    ArrowT<int, void> z;
    z.input = 1;
    x.connect(&y);
    y.connect(&z);
    std::cout << "x.output = " << x.output << ", y.output = " << y.output << std::endl;

}



