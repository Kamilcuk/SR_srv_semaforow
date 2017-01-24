#include "semaphore.hpp"
#include "helpers.hpp"

Semaphore::Semaphore(int _cur, int _min, int _max, std::__cxx11::string _uuid) :
	cur(_cur),
	min(_min),
	max(_max),
	uuid(_uuid)
{

}

void Semaphore::increment(std::__cxx11::string o) {
	//std::unique_lock<decltype(mutex_)> lock(mutex_);
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
	//condition_.notify_one();
}

void Semaphore::decrement(std::__cxx11::string o) {
	//std::unique_lock<decltype(mutex_)> lock(mutex_);
	while( cur <= min ) {
		//condition_.wait(lock);
	}
	owners.push_back(o);
	--cur;
}

bool Semaphore::try_decrement(std::__cxx11::string o) {
	//std::unique_lock<decltype(mutex_)> lock(mutex_);
	if( cur <= min ) {
		return false;
	}
	owners.push_back(o);
	--cur;
	return true;
}

