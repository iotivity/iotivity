/*
 *
 * @Copyright Sentence:
 * This program is subject to copyright protection in accordance with the
 * applicable law. It must not, except where allowed by law, by any means or
 * in any form be reproduced, distributed or lent. Moreover, no part of the
 * program may be used, viewed, printed, disassembled or otherwise interfered
 * with in any form, except where allowed by law, without the express written
 * consent of the copyright holder.
 *
 * Copyright (C) 2013 Samsung Electronics Co., Ltd.
 * All Rights Reserved. 
 *
 *
 * @Project/Module Name 	: Control Manager Framework
 * @filename				: ThreadImpl.h
 * @brief					:
 * @author					: Bhupathi Ranjith Kumar (ranjith.bk@samsung.com)
 * @date					: May 15, 2013
 * @version					:
 *
 *                                                                                           
 */

#ifndef THREADIMPL_H_
#define THREADIMPL_H_

#include "Thread/Thread.h"
#include <boost/thread.hpp>

namespace OC
{
namespace Cm
{
namespace Platform
{
namespace Thread
{

class ThreadImpl : public OC::Cm::Platform::Thread::Thread
{

public:

	ThreadImpl(OC::Cm::Platform::Thread::IRunnable* target);

	virtual ~ThreadImpl();

	virtual bool start() ;

	virtual void join() ;

	virtual void interrupt() ;

private:

	OC::Cm::Platform::Thread::IRunnable* p_target;

	boost::thread* p_thread;
};

} /* namespace Thread */
} /* namespace Platform */
} /* namespace Cm */
} /* namespace OC */
#endif /* THREADIMPL_H_ */
