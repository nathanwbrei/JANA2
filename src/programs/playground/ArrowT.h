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

#ifndef JANA2_ARROWT_H
#define JANA2_ARROWT_H

#include <queue>
#include <string>
#include <memory>

#include "Arrow.h"

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

int setup() {
    ArrowT<void, double> x("input", false);
    ArrowT<double, int> y("process", true);
    ArrowT<int, void> z("output", false);
    x.connect(&y);
    y.connect(&z);

}

#endif //JANA2_ARROWT_H
