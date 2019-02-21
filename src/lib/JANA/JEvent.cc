//
//    File: JEvent.cc
// Created: Sun Oct 15 21:15:05 CDT 2017
// Creator: davidl (on Darwin harriet.local 15.6.0 i386)
//
// ------ Last repository commit info -----
// [ Date ]
// [ Author ]
// [ Source ]
// [ Revision ]
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Jefferson Science Associates LLC Copyright Notice:  
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

#include "JEvent.h"
#include "JEventSource.h"

//---------------------------------
// JEvent    (Constructor)
//---------------------------------
JEvent::JEvent(JApplication* aApplication) : mApplication(aApplication), mThreadManager(nullptr)
{
	if(mApplication != nullptr) mThreadManager = mApplication->GetJThreadManager();
}

//---------------------------------
// ~JEvent    (Destructor)
//---------------------------------
JEvent::~JEvent()
{
	Release();
}

//---------------------------------
// GetEventSource
//---------------------------------
JEventSource* JEvent::GetEventSource(void) const
{
	return mEventSource;
}

//---------------------------------
// SetJApplication
//---------------------------------
void JEvent::SetJApplication(JApplication* app)
{
	mApplication = app;
	if(mApplication != nullptr) mThreadManager = mApplication->GetJThreadManager();
}

//---------------------------------
// SetJEventSource
//---------------------------------
void JEvent::SetJEventSource(JEventSource* aSource)
{
	mEventSource = aSource;
	mEventSource->IncrementEventCount();
	if( mIsBarrierEvent ) mEventSource->IncrementBarrierCount();
}

//---------------------------------
// SetFactorySet
//---------------------------------
void JEvent::SetFactorySet(JFactorySet* aFactorySet)
{
	mFactorySet = aFactorySet;
}

//---------------------------------
// Release
//---------------------------------
void JEvent::Release(void)
{
	// TODO: Delete me after JFactorySets attached to JThreads instead of JApplication
	//Release all (pointers to) resources, called when recycled to pool
	//if(mFactorySet != nullptr) {
	//	mApplication->Recycle(const_cast<JFactorySet*>(mFactorySet));
	//	mFactorySet = nullptr;
	//}

	if(mEventSource != nullptr ){
		mEventSource->DecrementEventCount();
		if(mIsBarrierEvent) mEventSource->DecrementBarrierCount();
		mEventSource = nullptr;
	}

	mLatestBarrierEvent = nullptr;
	mIsBarrierEvent = false; //In case user forgets to clear it
}
