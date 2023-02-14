#include "s57geometry.h"


bool S57DATASET_EXPORT operator==(const XY& a, const XY& b)
{
	bool bRet = a.x == b.x&&a.y&&b.y;
	return bRet;
}
S57Geometry::S57Geometry()
	:mGeometryType(S57GeometryType::Unknown)
{

}

S57Geometry::~S57Geometry()
{

}

S57GeometryType S57Geometry::geometryType()
{
	return mGeometryType;
}



S57Point::S57Point()
{
	mGeometryType = S57GeometryType::Point;
}

S57MultiPoint3D::S57MultiPoint3D()
{
	mGeometryType = S57GeometryType::MultiPoint3D;
}

S57Line::S57Line()
{
	mGeometryType = S57GeometryType::Line;
}

S57Area::S57Area()
{
	mGeometryType = S57GeometryType::Area;
}
