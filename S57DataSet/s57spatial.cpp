#include "s57spatial.h"

S57Spatial::S57Spatial()
{

}

uint64 S57Spatial::LNAM()
{
    byte bytes[5];
    bytes[0] = mVRID.RCNM;
    bytes[1] = (byte)(mVRID.RCID);
    bytes[2] = (byte)(mVRID.RCID >> 8);
    bytes[3] = (byte)(mVRID.RCID >> 16);
    bytes[4] = (byte)(mVRID.RCID >> 24);
    return (ulong)bytes[4] << 32 | (ulong)bytes[3] << 24 |
           (ulong)bytes[2] << 16 | (ulong)bytes[1] << 8 | bytes[0];
}

S57Spatial::~S57Spatial()
{

}
