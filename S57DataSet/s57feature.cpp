#include "s57feature.h"
#include <assert.h>
#include "../Utils/utils.h"

S57Feature::S57Feature()
    :mObjectClasses(nullptr)
    ,mGeometry(nullptr)
{

}

S57Feature::~S57Feature()
{
    delptr(mGeometry);
}

std::string S57Feature::name()
{
    std::string strRet;
    if(mObjectClasses)
    {
        strRet=mObjectClasses->acronym(mFRID.OBJL);
    }
    return std::move(strRet);
}

uint32 S57Feature::rcid()
{
    return mFRID.RCID;
}

void S57Feature::setGeometry(S57Geometry* geometry)
{
    mGeometry=geometry;
}

S57Geometry* S57Feature::geometry()
{
    return mGeometry;
}

S57GeometryType S57Feature::geometryType()
{
    S57GeometryType emGeoType=S57GeometryType::Unknown;
    if(mGeometry)
    {
        emGeoType=mGeometry->geometryType();
    }
    return emGeoType;
}

std::string S57Feature::geometryTypeString()
{
    S57GeometryType emGeoType=geometryType();
    std::string strGeoType;
    switch(emGeoType)
    {
    case S57GeometryType::Unknown:
    {
        strGeoType="Unknown";
    }
        break;
    case S57GeometryType::Point:
    {
        strGeoType="Point";
    }
        break;
    case S57GeometryType::MultiPoint3D:
    {
        strGeoType="MultiPoint3D";
    }
        break;
    case S57GeometryType::Line:
    {
        strGeoType="Line";
    }
        break;
    case S57GeometryType::Area:
    {
        strGeoType="Area";
    }
        break;
    }
    return std::move(strGeoType);
}


void S57Feature::setObjectClasses(S57ObjectClasses* objectClasses)
{
    mObjectClasses=objectClasses;
}


uint64 S57Feature::LNAM()
{
    byte bytes[8];
    bytes[0] = (byte)(mFOID.AGEN);
    bytes[1] = (byte)(mFOID.AGEN >> 8);
    bytes[2] = (byte)(mFOID.FIDN);
    bytes[3] = (byte)(mFOID.FIDN >> 8);
    bytes[4] = (byte)(mFOID.FIDN >> 16);
    bytes[5] = (byte)(mFOID.FIDN >> 24);
    bytes[6] = (byte)(mFOID.FIDS);
    bytes[7] = (byte)(mFOID.FIDS >> 8);

    return (ulong)bytes[7] << 56 | (ulong)bytes[6] << 48 |
            (ulong)bytes[5] << 40 | (ulong)bytes[4] << 32 |
            (ulong)bytes[3] << 24 | (ulong)bytes[2] << 16 |
            (ulong)bytes[1] << 8 | bytes[0];
}
