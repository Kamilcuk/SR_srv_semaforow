#include "uuid.hpp"

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

Uuid::Uuid()
{

}

std::string Uuid::getRandom()
{
	return to_string( boost::uuids::random_generator()() );
}
