#ifndef S57SPATIAL_H
#define S57SPATIAL_H

#include "s57dataset_global.h"
#include "s57type.h"

/*
Spatial record structure

Vector record
    |
    |--0001 (1) - ISO/IEC 8211 Record Identifier
        |
        |--VRID (4) - Vector Record Identifier field
            |
            |--<R>--ATTV (*2) - Vector Record Attribute field
            |
            |--VRPC (3) - Vector Record Pointer Control field
            |
            |--<R>--VRPT (*5) - Vector Record Pointer field
            |
            |--SGCC (3) - Coordinate control field
            |
            | alternate coordinate representations
            |
            *--<R>--SG2D (*2) - 2-D Coordinate field
            |
            *--<R>--SG3D (*3) - 3-D coordinate (Sounding Array) field
            |
            *--<R>--ARCC (5) - Arc/Curve definitions field
                    |
                    | alternate arc/curve definitions
                    |
                    *--<R>--AR2D (3*2) - Arc coordinates field
                    |
                    *--<R>--EL2D (5*2) - Ellipse coordinates field
                    |
                    *--<R>--CT2D (*2) - Curve coordinates field
*/

class S57DATASET_EXPORT S57Spatial
{
public:
    S57Spatial();
    ~S57Spatial();


public://ISO8211 Attributes
    VRIDRecord mVRID;

    uint64 LNAM();

    std::vector<ATTVRecord> mATTVs;

    VRPCRecord mVRPC;

    std::vector<VRPTRecord> mVRPTs;

    std::vector<SGCCRecord> mSGCCs;

    //可选
    std::vector<SG2DRecord> mSG2Ds;

    std::vector<SG3DRecord> mSG3Ds;

    std::vector<ARCCRecord> mARCCs;//no load


};

#endif // S57SPATIAL_H
