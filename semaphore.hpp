#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <vector>
#include <string>

class Semaphore
{
public:
	int cur;
	int min;
	int max;
	std::vector<std::string> owners;

	Semaphore(int _cur, int _min, int _max);

	bool decrement(std::string o);
	bool increment(std::string o);
};

#endif // SEMAPHORE_HPP
