#ifndef S57META_H
#define S57META_H
#include "s57type.h"


//Data Set Identification
struct DSIDRecord
{
    b11 RCNM;
    b14 RCID;
    b11 EXPP;
    b11 INTU;
    A DSNM;
    A EDTN;
    A UPDN;
    A_ UADT[8];
    A_ ISDT[8];
    R_ STED[4];
    b11 PRSP;
    A PSDN;
    A PRED;
    b11 PROF;
    b12 AGEN;
    A COMT;
};
//Data Set Structure information
struct DSSIRecord
{
    b11 DSTR;
    b11 AALL;
    b11 NALL;
    b14 NOMR;
    b14 NOCR;
    b14 NOGR;
    b14 NOLR;
    b14 NOIN;
    b14 NOCN;
    b14 NOED;
    b14 NOFA;
};
//Data Set Parameter
struct DSPMRecord
{
    b11 RCNM;
    b14 RCID;
    b11 HDAT;
    b11 VDAT;
    b11 SDAT;
    b14 CSCL;
    b11 DUNI;
    b11 HUNI;
    b11 PUNI;
    b11 COUN;
    b14 COMF;
    b14 SOMF;
    A COMT;
};
//Data Set Projection
struct DSPRRecord
{
    b11 PROJ;
    b24 PRP1;
    b24 PRP2;
    b24 PRP3;
    b24 PRP4;
    b24 FEAS;
    b24 FNOR;
    b14 FPMF;
    A COMT;
};
//Data Set Registration Control Field Tag
struct DSRCRecord
{
    b11 RPID;
    b24 RYCO;
    b24 RXCO;
    b11 CURP;
    b14 FPMF;
    b24 RXVL;
    b24 RYVL;
    A COMT;
};
//Data Set Accuracy
struct DSACRecord
{
    b11 RCNM;
    b14 RCID;
    b14 PACC;
    b14 HACC;
    b14 SACC;
    b14 FPMF;
    A COMT;
};
//Catalogue Directory
struct CATDRecord
{
    A_ RCNM[2];
    I_ RCID[10];
    A FILE;
    A LFIL;
    A VOLM;
    A_ IMPL[3];
    R SLAT;
    R WLON;
    R NLAT;
    R ELON;
    A CRCS;
    A COMT;
};
//Catalogue Cross Reference
struct CATXRecord
{
    b11 RCNM;
    b14 RCID;
    B_ NAM1[5];
    B_ NAM2[5];
    A COMT;
};
//Data Dictionary Definition
struct DDDFRecord
{
    b11 RCNM;
    b14 RCID;
    b11 OORA;
    A_ OAAC[6];
    b12 OACO;
    A OALL;
    b11 OATY;
    A DEFN;
    b12 AUTH;
    A COMT;
};
//Data Dictionary Definition Reference
struct DDDRRecord
{
    b11 RFTP;
    A RFVL;
};
//Data Dictionary Domain Identifier
struct DDDIRecord
{
    b11 RCNM;
    b14 RCID;
    b12 ATLB;
    b11 ATDO;
    A ADMU;
    A ADFT;
    b12 AUTH;
    A COMT;
};
//Data Dictionary Domain
struct DDOMRecord
{
    b11 RAVA;
    A DVAL;
    A DVSD;
    A DEFN;
    b12 AUTH;
};
//Data Dictionary Domain Reference
struct DDRFRecord
{
    b11 RFTP;
    A RFVL;
};
//Data Dictionary Schema Identifier
struct DDSIRecord
{
    b11 RCNM;
    b14 RCID;
    b12 OBLB;
};
//Data Dictionary Schema
struct DDSCRecord
{
    b12 ATLB;
    b11 ASET;
    b12 AUTH;
};
//Feature Record Identifier
struct FRIDRecord
{
    b11 RCNM;
    b14 RCID;
    b11 PRIM;
    b11 GRUP;
    b12 OBJL;
    b12 RVER;
    b11 RUIN;
};
//Feature Object Identifier
struct FOIDRecord
{
    b12 AGEN;
    b14 FIDN;
    b12 FIDS;
};
//Feature record attribute
struct ATTFRecord
{
    b12 ATTL;
    A ATVL;
    //
    b14 ATVLSize;//ext
};
//Feature record national attribute
struct NATFRecord
{
    b12 ATTL;
    A ATVL;
    //
    b14 ATVLSize;//ext
};
//Feature Record to Feature Object Pointer Control
struct FFPCRecord
{
    b11 FFUI;
    b12 FFIX;
    b12 NFPT;
};
//Feature Record to Feature Object Pointer
struct FFPTRecord
{
    B_ LNAM[8];
    b11 RIND;
    A COMT;
};
//Feature Record to Spatial Record Pointer Control
struct FSPCRecord
{
    b11 FSUI;
    b12 FSIX;
    b12 NSPT;
};
//Feature Record to Spatial Record Pointer
struct FSPTRecord
{
    B_ NAME[5];
    b11 ORNT;
    b11 USAG;
    b11 MASK;
	b24 rcid(char* RCNM=nullptr);
};
//Vector Record Identifier
struct VRIDRecord
{
    b11 RCNM;
    b14 RCID;
    b12 RVER;
    b11 RUIN;
};
//Vector Record Attribute
struct ATTVRecord
{
    b12 ATTL;
    A ATVL;
};
//Vector Record Pointer Control
struct VRPCRecord
{
    b11 VPUI;
    b12 VPIX;
    b12 NVPT;
};
//Vector Record Pointer
struct VRPTRecord
{
    B_ NAME[5];
    b11 ORNT;
    b11 USAG;
    b11 TOPI;
    b11 MASK;
};
//Coordinate control
struct SGCCRecord
{
    b11 CCUI;
    b12 CCIX;
    b12 CCNC;
};
//2-D Coordinate
struct SG2DRecord
{
    b24 YCOO;
    b24 XCOO;
};
//3-D Coordinate (Sounding Array)
struct SG3DRecord
{
    b24 YCOO;
    b24 XCOO;
    b24 VE3D;
};
//Arc/Curve definition
struct ARCCRecord
{
    b11 ATYP;
    b11 SURF;
    b11 ORDR;
    b14 RESO;
    b14 FPMF;
};
//Curve Coordinates
struct CT2DRecord
{
    b24 YCOO;
    b24 XCOO;
};





#endif
