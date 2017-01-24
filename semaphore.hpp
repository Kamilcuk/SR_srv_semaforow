#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>

class Semaphore
{
private:
	//std::mutex mutex_;
	//std::condition_variable condition_;

public:
	int cur;
	int min;
	int max;
	std::string uuid;
	std::vector<std::string> owners;

	Semaphore(int _cur, int _min, int _max, std::string _uuid);

	void increment(std::string o);

	void decrement(std::string o);

	bool try_decrement(std::string o);
};


#endif // SEMAPHORE_HPP
