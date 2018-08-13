#include "Record.h"


Record::Record() :
	x(0.0), y(0.0), z(0.0), timeInt(0.0), timeNormelized(0.0)
{
}


Record::Record(double x, double y, double z):
	x(x), y(y), z(z), timeInt(0.0), timeNormelized(0.0)
{
}


Record::Record(double x, double y, double z, double timeNormelized, double timeInt):
	x(x), y(y), z(z), timeInt(timeInt), timeNormelized(timeNormelized)
{
}


Record::~Record()
{
}


bool Record::operator == (const Record & record) const
{
	return this->x == record.x &&
		this->y == record.y &&
		this->z == record.z &&
		this->timeInt == record.timeInt &&
		this->timeNormelized == record.timeNormelized;
}


bool Record::operator != (const Record & record) const
{
	return !(*this == record);
}
