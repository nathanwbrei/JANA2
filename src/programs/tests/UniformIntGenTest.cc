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


#include "catch.hpp"

#include <iostream>
#include <iomanip>
#include <thread>
#include <mutex>

std::mutex output_mutex;

void exec() {

    std::hash<std::thread::id> hasher;
    long seed = clock() + hasher(std::this_thread::get_id());
    std::mt19937 generator(seed);
    std::uniform_int_distribution<long> distribution(1, 100);

    long sum = 0;
    auto start_timepoint = std::chrono::steady_clock::now();
    for (int i=0; i<10000; ++i) {
        sum += distribution(generator);
    }
    auto finish_timepoint = std::chrono::steady_clock::now();
    auto int_micros = std::chrono::duration_cast<std::chrono::microseconds>(finish_timepoint-start_timepoint);

    double sum2 = 0;
    start_timepoint = std::chrono::steady_clock::now();
    for (int i=0; i<10000; ++i) {
        sum2 += generator();
    }
    finish_timepoint = std::chrono::steady_clock::now();
    auto double_micros = std::chrono::duration_cast<std::chrono::microseconds>(finish_timepoint-start_timepoint);

    std::lock_guard<std::mutex> lock(output_mutex);

    std::cout << std::setw(16) << std::this_thread::get_id()
              << std::setw(20) << double_micros.count()
              << std::setw(20) << int_micros.count()
              << std::endl;

}

TEST_CASE("std::uniform_int_distribution thread scaling", "[.][fun]") {
    for (int nthreads = 1; nthreads <= 16; ++nthreads) {
        output_mutex.lock();
        std::cout << "---------------------------------------------------------------" << std::endl;
        std::cout << "Scaling to nthreads = " << nthreads << std::endl;
        std::cout << "---------------------------------------------------------------" << std::endl;
        std::cout << "Thread ID              rand double us          rand int us" << std::endl;
        output_mutex.unlock();

        std::vector<std::thread> threads;
        for (int i=0; i<nthreads; ++i) {
            threads.emplace_back(exec);
        }
        for (int i=0; i<nthreads; ++i) {
            threads[i].join();
        }
    }
}
