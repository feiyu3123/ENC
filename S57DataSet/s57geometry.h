#ifndef S57GEOMETRY_H
#define S57GEOMETRY_H
#include "s57dataset_global.h"
#include <vector>
#include "s57type.h"

struct S57DATASET_EXPORT XY
{
	double x;
	double y;
	XY()
	{

	}
	XY(double x_, double y_)
	{
		x = x_;
		y = y_;
	}

};

struct S57DATASET_EXPORT XYZ
{
	double x;
	double y;
	double z;
	XYZ()
	{

	}
	XYZ(double x_, double y_, double z_)
	{
		x = x_;
		y = y_;
		z = z_;
	}

};


enum class S57DATASET_EXPORT S57GeometryType
{
	Unknown,
	Point,//点
	MultiPoint3D,//多点,测深
	Line,//线
	Area,//面
};

class S57DATASET_EXPORT S57Geometry
{
public:
	S57Geometry();
	virtual ~S57Geometry();
	S57GeometryType geometryType();
protected:
	S57GeometryType mGeometryType;
};

class S57DATASET_EXPORT S57Point :public S57Geometry
{
public:
	S57Point();
	XY point;
};

class S57DATASET_EXPORT S57MultiPoint3D :public S57Geometry
{
public:
	S57MultiPoint3D();
	std::vector<XYZ> points;
};

template <class T>
class S57DATASET_EXPORT S57Ring
{
public:
	std::vector<T> points;
};

typedef public S57Ring<XY> S57Ring2D;

class S57DATASET_EXPORT S57Line :public S57Geometry
{
public:
	S57Line();
	std::vector<XY> points;
};

class S57DATASET_EXPORT S57Area :public S57Geometry
{
public:
	S57Area();
	std::vector<S57Ring2D> exteriorRings;
	std::vector<S57Ring2D> interiorRings;
};





#endif // S57GEOMETRY_H
