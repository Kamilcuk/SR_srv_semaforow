#include "servicable.hpp"

#include <stdexcept>

Servicable::Servicable()
{

}

Servicable::~Servicable()
{

}

void Servicable::stop()
{
	_stop = true;
}

void Servicable::run()
{
	std::runtime_error("Sservicable::run");
}
