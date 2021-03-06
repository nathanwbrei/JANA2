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
#include "ExactlyOnceTests.h"

#include <JANA/JEventSourceGeneratorT.h>
#include <JANA/JApplication.h>

TEST_CASE("ExactlyOnceTests") {

    JApplication app;
    japp = &app;  // For anything relying on the global variable

    auto source = new SimpleSource("SimpleSource", &app);
    auto processor = new SimpleProcessor(&app);

    app.Add(source);
    app.Add(processor);
    app.SetParameterValue("jana:extended_report", 0);

    SECTION("Old engine: JEventProcessor::Init(), Finish() called exactly once") {

        REQUIRE(source->open_count == 0);
        REQUIRE(processor->init_count == 0);
        REQUIRE(processor->finish_count == 0);

        app.SetParameterValue("jana:legacy_mode", 1);
        app.Run(true);

        REQUIRE(source->open_count == 1);
        REQUIRE(processor->init_count == 1);
        REQUIRE(processor->finish_count == 1);
    }


    SECTION("New engine: JEventProcessor::Init(), Finish() called exactly once") {

        REQUIRE(source->open_count == 0);
        REQUIRE(processor->init_count == 0);
        REQUIRE(processor->finish_count == 0);

        app.SetParameterValue("jana:legacy_mode", 0);
        app.Run(true);

        REQUIRE(source->open_count == 1);
        REQUIRE(processor->init_count == 1);
        REQUIRE(processor->finish_count == 1);
    }
}



