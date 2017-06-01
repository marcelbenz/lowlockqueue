#include <atomic>

class ScopedAtomicAcquireRelease
{
	public:
	explicit ScopedAtomicAcquireRelease(std::atomic<bool>& l);
	~ScopedAtomicAcquireRelease();
	void release();

	private:
	void waitForLock();

	private:
	std::atomic_bool& lock;
	bool lockAcquired;
};
