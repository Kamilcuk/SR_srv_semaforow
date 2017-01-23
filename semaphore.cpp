#include "semaphore.hpp"
#include "helpers.hpp"

Semaphore::Semaphore(int _cur, int _min, int _max) :
	cur(_cur),
	min(_min),
	max(_max)
{

}

bool Semaphore::decrement(std::__cxx11::string o)
{
	if ( cur > min ) {
		if ( std::find(owners.begin(), owners.end(), o) == owners.end() ) {
			owners.push_back(o);
		}
		--cur;
		return true;
	}
	return false;
}

bool Semaphore::increment(std::__cxx11::string o)
{
	if ( std::find(owners.begin(), owners.end(), o) == owners.end() ) {
		return false;
	}
	if ( cur < max ) {
		++cur;
	}
	return true;
}
