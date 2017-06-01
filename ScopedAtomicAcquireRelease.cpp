#include <cassert>
#include "ScopedAtomicAcquireRelease.h"

ScopedAtomicAcquireRelease::ScopedAtomicAcquireRelease(std::atomic<bool>& l)
	: lock(l)
	, lockAcquired(false)
{
	waitForLock();
}

ScopedAtomicAcquireRelease::~ScopedAtomicAcquireRelease()
{
	if (lockAcquired)
		release();
}

void ScopedAtomicAcquireRelease::release()
{
	assert(lockAcquired);
	lock.store(false, std::memory_order_release);
	lockAcquired = false;
}

void ScopedAtomicAcquireRelease::waitForLock()
{
	assert(!lockAcquired);
	while (lock.exchange(true, std::memory_order_acquire)) {}
	lockAcquired = true;
}

