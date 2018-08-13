#pragma once
class Record
{
public:
	double x, y, z, timeNormelized, timeInt;
	Record();
	Record(double x, double y, double z);
	Record(double x, double y, double z, double time, double timeInt);
	virtual ~Record();
	bool operator == (const Record & record) const;
	bool operator != (const Record & record) const;
};

