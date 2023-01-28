#include "s57type.h"


double S57Field::toDouble()
{
	double dValue = stod(value);
	return dValue;
}

int S57Field::toInt()
{
	int iValue = stoi(value);
	return iValue;
}
std::string S57Field::toString()
{
	return value;
}