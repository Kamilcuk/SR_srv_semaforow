#ifndef SERVICABLE_HPP
#define SERVICABLE_HPP


class Servicable
{
protected:
	bool _stop = false;
public:
	Servicable();
	virtual ~Servicable();
	virtual void stop();
	virtual void run();
};

#endif // SERVICABLE_HPP
