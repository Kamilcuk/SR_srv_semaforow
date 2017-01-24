#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>

struct SemaphoreData
{
	int cur;
	int min;
	int max;
	std::string uuid;
	std::vector<std::string> owners;
};

class Semaphore : private SemaphoreData
{
private:
	std::mutex varsmutex_;

	std::mutex mutex_;
	std::condition_variable condition_;

public:
	Semaphore(int _cur, int _min, int _max, std::string _uuid);

	Semaphore(const Semaphore&) = delete;

	void increment(std::string o);

	void decrement(std::string o);

	bool try_decrement(std::string o);

	std::vector<std::string> getOwners();
	std::string getUuid();
	int getMax();
	int getMin();
	int getCur();

	SemaphoreData getSemaphoreData();
};


#endif // SEMAPHORE_HPP
