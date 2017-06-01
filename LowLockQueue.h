#include <atomic>
#include <memory>
#include <utility>
#include "ScopedAtomicAcquireRelease.h"

constexpr size_t CACHE_LINE_SIZE{64};


template <class T>
class LowLockQueue
{
	public:
	using value_type = T;
	using pointer = value_type*;

	public:
	LowLockQueue();
	~LowLockQueue();

	void produce(const T& t);
	bool consume(T& result);

	private:
	struct Node;
	using node_ptr = Node*;
	using node_unique_ptr = std::unique_ptr<Node>;
	using value_unique_ptr = std::unique_ptr<value_type>;
	struct alignas(CACHE_LINE_SIZE) Node
	{
		explicit Node(pointer val)
			:value(val)
			,next(nullptr)
		{}
		value_unique_ptr value;
		std::atomic<node_ptr> next;
	};

	private:
	alignas(CACHE_LINE_SIZE) node_unique_ptr first;
	alignas(CACHE_LINE_SIZE) std::atomic_bool consumerLock;
	alignas(CACHE_LINE_SIZE) node_ptr last;
	alignas(CACHE_LINE_SIZE) std::atomic_bool producerLock;
};


template <class T>
LowLockQueue<T>::LowLockQueue()
{
	first = std::make_unique<Node>(nullptr);
	consumerLock = false;
	last = first.get();
	producerLock = false;
}

template <class T>
LowLockQueue<T>::~LowLockQueue()
{
	while (first)
		first.reset(first->next.load(std::memory_order_relaxed));
}

template <class T>
void LowLockQueue<T>::produce(const T& t)
{
	node_ptr newNode = new Node(new value_type(t));
	ScopedAtomicAcquireRelease lock(producerLock);
	last->next.store(newNode, std::memory_order_release);
	last = newNode;
}

template <class T>
bool LowLockQueue<T>::consume(T& result)
{
	ScopedAtomicAcquireRelease lock(consumerLock);
	node_unique_ptr next(first->next.load(std::memory_order_acquire));
	if (next)
	{
		value_unique_ptr value = std::move(next->value);
		first.swap(next);
		lock.release();
		result = *value;
		return true;
	}
	else
	{
		return false;
	}
}
