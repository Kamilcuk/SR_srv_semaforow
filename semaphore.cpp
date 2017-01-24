#include "semaphore.hpp"
#include "helpers.hpp"

std::vector<std::string> Semaphore::getOwners()
{
	std::lock_guard<std::mutex> guard(varsmutex_);
	return owners;
}

std::string Semaphore::getUuid()
{
	std::lock_guard<std::mutex> guard(varsmutex_);
	return uuid;
}

int Semaphore::getMax()
{
	std::lock_guard<std::mutex> guard(varsmutex_);
	return max;
}

int Semaphore::getMin()
{
	std::lock_guard<std::mutex> guard(varsmutex_);
	return min;
}

int Semaphore::getCur()
{
	std::lock_guard<std::mutex> guard(varsmutex_);
	return cur;
}

SemaphoreData Semaphore::getSemaphoreData()
{
	std::lock_guard<std::mutex> guard(varsmutex_);
	SemaphoreData ret;
	ret.min = min;
	ret.max = max;
	ret.cur = cur;
	ret.owners = owners;
	ret.uuid = uuid;
	return ret;
}

Semaphore::Semaphore(int _cur, int _min, int _max, std::__cxx11::string _uuid)
{
	cur = _cur;
	min = _min;
	max = _max;
	uuid = _uuid;
}

void Semaphore::increment(std::__cxx11::string o) {
	std::unique_lock<decltype(mutex_)> lock(mutex_);

	std::lock_guard<std::mutex> guard(varsmutex_);
	auto ite = std::find(owners.begin(), owners.end(), o);
	if ( ite == owners.end() ) {
		return;
	}
	owners.erase(ite);
	if ( cur < max ) {
		++cur;
	}
	if ( cur > max ) {
		cur = max;
	}
	condition_.notify_one();
}


void Semaphore::decrement(std::__cxx11::string o) {
	std::unique_lock<decltype(mutex_)> lock(mutex_);

	while( ({ std::lock_guard<std::mutex> guard(varsmutex_); cur <= min; }) ) {
		condition_.wait(lock);
	}
	std::lock_guard<std::mutex> guard(varsmutex_);
	owners.push_back(o);
	--cur;
}

bool Semaphore::try_decrement(std::__cxx11::string o) {
	std::unique_lock<decltype(mutex_)> lock(mutex_);

	std::lock_guard<std::mutex> guard(varsmutex_);
	if( cur <= min ) {
		return false;
	}
	owners.push_back(o);
	--cur;
	return true;
}
