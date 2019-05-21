//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Jefferson Science Associates LLC Copyright Notice:
//
// Copyright 251 2014 Jefferson Science Associates LLC All Rights Reserved. Redistribution
// and use in source and binary forms, with or without modification, are permitted as a
// licensed user provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice, this
//    list of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products derived
//    from this software without specific prior written permission.
// This material resulted from work developed under a United States Government Contract.
// The Government retains a paid-up, nonexclusive, irrevocable worldwide license in such
// copyrighted data to reproduce, distribute copies to the public, prepare derivative works,
// perform publicly and display publicly and to permit others to do so.
// THIS SOFTWARE IS PROVIDED BY JEFFERSON SCIENCE ASSOCIATES LLC "AS IS" AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
// JEFFERSON SCIENCE ASSOCIATES, LLC OR THE U.S. GOVERNMENT BE LIABLE TO LICENSEE OR ANY
// THIRD PARTES FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
// OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
// Author: Nathan Brei
//

#ifndef JANA2_ARROW_H
#define JANA2_ARROW_H


#include <queue>
#include <string>
#include <memory>

struct ArrowMetrics {};

template <typename T>
struct Mailbox {
    std::queue<T> underlying;
};


struct Arrow;

class GeneralizedArrow {
public:
    enum class Status {Unopened, Inactive, Running, Draining, Drained, Finished, Closed};

private:
    Status _status = Status::Unopened;
    std::vector<GeneralizedArrow*> upstream;
    std::vector<GeneralizedArrow*> downstream;

protected:
    friend class JScheduler;
    void set_status(Status status) { _status = status; } // TODO: Protection
    Status get_status() { return _status; }

    void notify() {
        // notify all downstream arrows
        for (GeneralizedArrow* arrow : downstream) {
            arrow->update();
        }
    };

    virtual void update() {};

public:
    virtual void activate() {};
    virtual void deactivate() {};
};


struct Arrow: public GeneralizedArrow {

    enum class Type {Source, Sink, Stage};

    const std::string _name;
    const Type _type;
    const bool _is_parallel;

    ArrowMetrics _metrics;
    size_t _chunksize = 1;
    size_t _thread_count = 0;

    Arrow(std::string name, Type type, bool is_parallel)
            : _name(std::move(name))
            , _type(type)
            , _is_parallel(is_parallel) {};

    virtual void execute(ArrowMetrics& result) {};

    virtual void initialize() {}

    virtual void finalize() {};

    void activate() override;

    void deactivate() override;

    void update() override;

};

struct Quiver: public GeneralizedArrow {
    // Has a JPerfMetrics, maybe even a scheduler
    // JPerfMetrics _metrics;
    // std::vector<Arrow*> _arrows;

};


#endif //JANA2_ARROW_H
