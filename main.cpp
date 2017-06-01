#include <chrono>
#include <ctime>
#include <iostream>
#include <thread>
#include <sstream>
#include <unistd.h>
#include "LowLockQueue.h"

// 
std::string operator+(const std::string& str, const std::thread::id& id)
{
	std::stringstream ss;
	ss << id;
	return str + ss.str();
}

int main()
{
	const int numOfMessageToProduce = 100;
	const int numOfMessageToConsume = numOfMessageToProduce;

	LowLockQueue<std::string> queue;

	auto doProduce = [&]()
	{
		for (int i = 0; i < numOfMessageToProduce; ++i)
		{
			queue.produce("message #" + std::to_string(i) + " from " + std::this_thread::get_id());
			usleep(10);
		}
	};
	auto doConsume = [&]()
	{
		std::string val;
		for (int i = 0; i < numOfMessageToConsume; )
		{
			if (queue.consume(val))
			{
//				std::string output = "" + std::this_thread::get_id() + " received #" + std::to_string(i) + " : " + val + "\n";
//				std::cout << output;
				++i;
				usleep(10);
			}
		}
	};

	auto start = std::chrono::system_clock::now();

	std::thread p1{doProduce};
	std::thread p2{doProduce};
	std::thread p3{doProduce};
	std::thread c1{doConsume};
	std::thread c2{doConsume};
	std::thread c3{doConsume};

	p1.join();
	p2.join();
	p3.join();
	c1.join();
	c2.join();
	c3.join();

	auto stop = std::chrono::system_clock::now();
	std::cout << "elapsed: " << std::chrono::duration<double>(stop-start).count() << "s" << std::endl;

	return 0;
}
