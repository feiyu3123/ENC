

#include "../ISO8211/iso8211.h"
#include "s57meta.h"
#include "s57dataset.h"


b24 FSPTRecord::rcid(char* RCNM)
{
	if (RCNM)
	{
		*RCNM = NAME[0];
	}

	UnionInt unInt;
	unInt.u.byte0 = NAME[1];
	unInt.u.byte1 = NAME[2];
	unInt.u.byte2 = NAME[3];
	unInt.u.byte3 = NAME[4];

	b24 rcid_ = unInt.intValue;
	return rcid_;
}

DR S57DataSet::createDSID(const DSIDRecord& record)
{
	DR dr;
	dr.setFieldTag(DSID);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	writeToDRRecords(drFieldArea, "RCNM", record.RCNM);
	writeToDRRecords(drFieldArea, "RCID", record.RCID);
	writeToDRRecords(drFieldArea, "EXPP", record.EXPP);
	writeToDRRecords(drFieldArea, "INTU", record.INTU);
	writeToDRRecords(drFieldArea, "DSNM", record.DSNM);
	writeToDRRecords(drFieldArea, "EDTN", record.EDTN);
	writeToDRRecords(drFieldArea, "UPDN", record.UPDN);
	writeToDRRecords(drFieldArea, "UADT", record.UADT, 8);
	writeToDRRecords(drFieldArea, "ISDT", record.ISDT, 8);
	writeToDRRecords(drFieldArea, "STED", record.STED, 4);
	writeToDRRecords(drFieldArea, "PRSP", record.PRSP);
	writeToDRRecords(drFieldArea, "PSDN", record.PSDN);
	writeToDRRecords(drFieldArea, "PRED", record.PRED);
	writeToDRRecords(drFieldArea, "PROF", record.PROF);
	writeToDRRecords(drFieldArea, "AGEN", record.AGEN);
	writeToDRRecords(drFieldArea, "COMT", record.COMT);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createDSSI(const DSSIRecord& record)
{
	DR dr;
	dr.setFieldTag(DSSI);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	writeToDRRecords(drFieldArea, "DSTR", record.DSTR);
	writeToDRRecords(drFieldArea, "AALL", record.AALL);
	writeToDRRecords(drFieldArea, "NALL", record.NALL);
	writeToDRRecords(drFieldArea, "NOMR", record.NOMR);
	writeToDRRecords(drFieldArea, "NOCR", record.NOCR);
	writeToDRRecords(drFieldArea, "NOGR", record.NOGR);
	writeToDRRecords(drFieldArea, "NOLR", record.NOLR);
	writeToDRRecords(drFieldArea, "NOIN", record.NOIN);
	writeToDRRecords(drFieldArea, "NOCN", record.NOCN);
	writeToDRRecords(drFieldArea, "NOED", record.NOED);
	writeToDRRecords(drFieldArea, "NOFA", record.NOFA);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createDSPM(const DSPMRecord& record)
{
	DR dr;
	dr.setFieldTag(DSPM);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	writeToDRRecords(drFieldArea, "RCNM", record.RCNM);
	writeToDRRecords(drFieldArea, "RCID", record.RCID);
	writeToDRRecords(drFieldArea, "HDAT", record.HDAT);
	writeToDRRecords(drFieldArea, "VDAT", record.VDAT);
	writeToDRRecords(drFieldArea, "SDAT", record.SDAT);
	writeToDRRecords(drFieldArea, "CSCL", record.CSCL);
	writeToDRRecords(drFieldArea, "DUNI", record.DUNI);
	writeToDRRecords(drFieldArea, "HUNI", record.HUNI);
	writeToDRRecords(drFieldArea, "PUNI", record.PUNI);
	writeToDRRecords(drFieldArea, "COUN", record.COUN);
	writeToDRRecords(drFieldArea, "COMF", record.COMF);
	writeToDRRecords(drFieldArea, "SOMF", record.SOMF);
	writeToDRRecords(drFieldArea, "COMT", record.COMT);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createDSPR(const DSPRRecord& record)
{
	DR dr;
	dr.setFieldTag(DSPR);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	writeToDRRecords(drFieldArea, "PROJ", record.PROJ);
	writeToDRRecords(drFieldArea, "PRP1", record.PRP1);
	writeToDRRecords(drFieldArea, "PRP2", record.PRP2);
	writeToDRRecords(drFieldArea, "PRP3", record.PRP3);
	writeToDRRecords(drFieldArea, "PRP4", record.PRP4);
	writeToDRRecords(drFieldArea, "FEAS", record.FEAS);
	writeToDRRecords(drFieldArea, "FNOR", record.FNOR);
	writeToDRRecords(drFieldArea, "FPMF", record.FPMF);
	writeToDRRecords(drFieldArea, "COMT", record.COMT);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createDSRC(const std::vector<DSRCRecord>& records)
{
	DR dr;
	dr.setFieldTag(DSRC);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	std::vector<TypeValue> typeValues;
	for (auto itRecord = records.begin(); itRecord != records.end(); itRecord++)
	{
		{TypeValue typeValue("b11", itRecord->RPID); typeValues.push_back(typeValue); }
		{TypeValue typeValue("b24*)", itRecord->RYCO); typeValues.push_back(typeValue); }
		{TypeValue typeValue("b24*)", itRecord->RXCO); typeValues.push_back(typeValue); }
		{TypeValue typeValue("b11", itRecord->CURP); typeValues.push_back(typeValue); }
		{TypeValue typeValue("b14", itRecord->FPMF); typeValues.push_back(typeValue); }
		{TypeValue typeValue("b24", itRecord->RXVL); typeValues.push_back(typeValue); }
		{TypeValue typeValue("b24", itRecord->RYVL); typeValues.push_back(typeValue); }
		{TypeValue typeValue("A()", itRecord->COMT, -1); typeValues.push_back(typeValue); }
	}
	writeToDRRecords(drFieldArea, "*RPID", typeValues);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createDSAC(const DSACRecord& record)
{
	DR dr;
	dr.setFieldTag(DSAC);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	writeToDRRecords(drFieldArea, "RCNM", record.RCNM);
	writeToDRRecords(drFieldArea, "RCID", record.RCID);
	writeToDRRecords(drFieldArea, "PACC", record.PACC);
	writeToDRRecords(drFieldArea, "HACC", record.HACC);
	writeToDRRecords(drFieldArea, "SACC", record.SACC);
	writeToDRRecords(drFieldArea, "FPMF", record.FPMF);
	writeToDRRecords(drFieldArea, "COMT", record.COMT);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createCATD(const CATDRecord& record)
{
	DR dr;
	dr.setFieldTag(CATD);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	writeToDRRecords(drFieldArea, "RCNM", record.RCNM, 2);
	writeToDRRecords(drFieldArea, "RCID", record.RCID, 10);
	writeToDRRecords(drFieldArea, "FILE", record.FILE);
	writeToDRRecords(drFieldArea, "LFIL", record.LFIL);
	writeToDRRecords(drFieldArea, "VOLM", record.VOLM);
	writeToDRRecords(drFieldArea, "IMPL", record.IMPL, 3);
	writeToDRRecords(drFieldArea, "SLAT", record.SLAT);
	writeToDRRecords(drFieldArea, "WLON", record.WLON);
	writeToDRRecords(drFieldArea, "NLAT", record.NLAT);
	writeToDRRecords(drFieldArea, "ELON", record.ELON);
	writeToDRRecords(drFieldArea, "CRCS", record.CRCS);
	writeToDRRecords(drFieldArea, "COMT", record.COMT);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createCATX(const std::vector<CATXRecord>& records)
{
	DR dr;
	dr.setFieldTag(CATX);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	std::vector<TypeValue> typeValues;
	for (auto itRecord = records.begin(); itRecord != records.end(); itRecord++)
	{
		{TypeValue typeValue("b11", itRecord->RCNM); typeValues.push_back(typeValue); }
		{TypeValue typeValue("b14", itRecord->RCID); typeValues.push_back(typeValue); }
		{TypeValue typeValue("B(40)", itRecord->NAM1, 5); typeValues.push_back(typeValue); }
		{TypeValue typeValue("B(40)", itRecord->NAM2, 5); typeValues.push_back(typeValue); }
		{TypeValue typeValue("A()", itRecord->COMT, -1); typeValues.push_back(typeValue); }
	}
	writeToDRRecords(drFieldArea, "*RCNM", typeValues);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createDDDF(const DDDFRecord& record)
{
	DR dr;
	dr.setFieldTag(DDDF);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	writeToDRRecords(drFieldArea, "RCNM", record.RCNM);
	writeToDRRecords(drFieldArea, "RCID", record.RCID);
	writeToDRRecords(drFieldArea, "OORA", record.OORA);
	writeToDRRecords(drFieldArea, "OAAC", record.OAAC, 6);
	writeToDRRecords(drFieldArea, "OACO", record.OACO);
	writeToDRRecords(drFieldArea, "OALL", record.OALL);
	writeToDRRecords(drFieldArea, "OATY", record.OATY);
	writeToDRRecords(drFieldArea, "DEFN", record.DEFN);
	writeToDRRecords(drFieldArea, "AUTH", record.AUTH);
	writeToDRRecords(drFieldArea, "COMT", record.COMT);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createDDDR(const std::vector<DDDRRecord>& records)
{
	DR dr;
	dr.setFieldTag(DDDR);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	std::vector<TypeValue> typeValues;
	for (auto itRecord = records.begin(); itRecord != records.end(); itRecord++)
	{
		{TypeValue typeValue("b11", itRecord->RFTP); typeValues.push_back(typeValue); }
		{TypeValue typeValue("A()", itRecord->RFVL, -1); typeValues.push_back(typeValue); }
	}
	writeToDRRecords(drFieldArea, "*RFTP", typeValues);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createDDDI(const DDDIRecord& record)
{
	DR dr;
	dr.setFieldTag(DDDI);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	writeToDRRecords(drFieldArea, "RCNM", record.RCNM);
	writeToDRRecords(drFieldArea, "RCID", record.RCID);
	writeToDRRecords(drFieldArea, "ATLB", record.ATLB);
	writeToDRRecords(drFieldArea, "ATDO", record.ATDO);
	writeToDRRecords(drFieldArea, "ADMU", record.ADMU);
	writeToDRRecords(drFieldArea, "ADFT", record.ADFT);
	writeToDRRecords(drFieldArea, "AUTH", record.AUTH);
	writeToDRRecords(drFieldArea, "COMT", record.COMT);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createDDOM(const DDOMRecord& record)
{
	DR dr;
	dr.setFieldTag(DDOM);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	writeToDRRecords(drFieldArea, "RAVA", record.RAVA);
	writeToDRRecords(drFieldArea, "DVAL", record.DVAL);
	writeToDRRecords(drFieldArea, "DVSD", record.DVSD);
	writeToDRRecords(drFieldArea, "DEFN", record.DEFN);
	writeToDRRecords(drFieldArea, "AUTH", record.AUTH);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createDDRF(const std::vector<DDRFRecord>& records)
{
	DR dr;
	dr.setFieldTag(DDRF);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	std::vector<TypeValue> typeValues;
	for (auto itRecord = records.begin(); itRecord != records.end(); itRecord++)
	{
		{TypeValue typeValue("b11", itRecord->RFTP); typeValues.push_back(typeValue); }
		{TypeValue typeValue("A()", itRecord->RFVL, -1); typeValues.push_back(typeValue); }
	}
	writeToDRRecords(drFieldArea, "*RFTP", typeValues);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createDDSI(const DDSIRecord& record)
{
	DR dr;
	dr.setFieldTag(DDSI);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	writeToDRRecords(drFieldArea, "RCNM", record.RCNM);
	writeToDRRecords(drFieldArea, "RCID", record.RCID);
	writeToDRRecords(drFieldArea, "OBLB", record.OBLB);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createDDSC(const std::vector<DDSCRecord>& records)
{
	DR dr;
	dr.setFieldTag(DDSC);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	std::vector<TypeValue> typeValues;
	for (auto itRecord = records.begin(); itRecord != records.end(); itRecord++)
	{
		{TypeValue typeValue("b12", itRecord->ATLB); typeValues.push_back(typeValue); }
		{TypeValue typeValue("b11", itRecord->ASET); typeValues.push_back(typeValue); }
		{TypeValue typeValue("b12", itRecord->AUTH); typeValues.push_back(typeValue); }
	}
	writeToDRRecords(drFieldArea, "*ATLB", typeValues);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createFRID(const FRIDRecord& record)
{
	DR dr;
	dr.setFieldTag(FRID);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	writeToDRRecords(drFieldArea, "RCNM", record.RCNM);
	writeToDRRecords(drFieldArea, "RCID", record.RCID);
	writeToDRRecords(drFieldArea, "PRIM", record.PRIM);
	writeToDRRecords(drFieldArea, "GRUP", record.GRUP);
	writeToDRRecords(drFieldArea, "OBJL", record.OBJL);
	writeToDRRecords(drFieldArea, "RVER", record.RVER);
	writeToDRRecords(drFieldArea, "RUIN", record.RUIN);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createFOID(const FOIDRecord& record)
{
	DR dr;
	dr.setFieldTag(FOID);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	writeToDRRecords(drFieldArea, "AGEN", record.AGEN);
	writeToDRRecords(drFieldArea, "FIDN", record.FIDN);
	writeToDRRecords(drFieldArea, "FIDS", record.FIDS);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createATTF(const std::vector<ATTFRecord>& records)
{
	DR dr;
	dr.setFieldTag(ATTF);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	std::vector<TypeValue> typeValues;
	for (auto itRecord = records.begin(); itRecord != records.end(); itRecord++)
	{
		{TypeValue typeValue("b12", itRecord->ATTL); typeValues.push_back(typeValue); }
		{TypeValue typeValue("A()", itRecord->ATVL, -1); typeValues.push_back(typeValue); }
	}
	writeToDRRecords(drFieldArea, "*ATTL", typeValues);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createNATF(const std::vector<NATFRecord>& records)
{
	DR dr;
	dr.setFieldTag(NATF);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	std::vector<TypeValue> typeValues;
	for (auto itRecord = records.begin(); itRecord != records.end(); itRecord++)
	{
		{TypeValue typeValue("b12", itRecord->ATTL); typeValues.push_back(typeValue); }
		{TypeValue typeValue("A()", itRecord->ATVL, -1); typeValues.push_back(typeValue); }
	}
	writeToDRRecords(drFieldArea, "*ATTL", typeValues);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createFFPC(const FFPCRecord& record)
{
	DR dr;
	dr.setFieldTag(FFPC);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	writeToDRRecords(drFieldArea, "FFUI", record.FFUI);
	writeToDRRecords(drFieldArea, "FFIX", record.FFIX);
	writeToDRRecords(drFieldArea, "NFPT", record.NFPT);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createFFPT(const std::vector<FFPTRecord>& records)
{
	DR dr;
	dr.setFieldTag(FFPT);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	std::vector<TypeValue> typeValues;
	for (auto itRecord = records.begin(); itRecord != records.end(); itRecord++)
	{
		{TypeValue typeValue("B(64)", itRecord->LNAM, 8); typeValues.push_back(typeValue); }
		{TypeValue typeValue("b11", itRecord->RIND); typeValues.push_back(typeValue); }
		{TypeValue typeValue("A()", itRecord->COMT, -1); typeValues.push_back(typeValue); }
	}
	writeToDRRecords(drFieldArea, "*LNAM", typeValues);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createFSPC(const FSPCRecord& record)
{
	DR dr;
	dr.setFieldTag(FSPC);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	writeToDRRecords(drFieldArea, "FSUI", record.FSUI);
	writeToDRRecords(drFieldArea, "FSIX", record.FSIX);
	writeToDRRecords(drFieldArea, "NSPT", record.NSPT);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createFSPT(const std::vector<FSPTRecord>& records)
{
	DR dr;
	dr.setFieldTag(FSPT);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	std::vector<TypeValue> typeValues;
	for (auto itRecord = records.begin(); itRecord != records.end(); itRecord++)
	{
		{TypeValue typeValue("B(40)", itRecord->NAME, 5); typeValues.push_back(typeValue); }
		{TypeValue typeValue("b11", itRecord->ORNT); typeValues.push_back(typeValue); }
		{TypeValue typeValue("b11", itRecord->USAG); typeValues.push_back(typeValue); }
		{TypeValue typeValue("b11", itRecord->MASK); typeValues.push_back(typeValue); }
	}
	writeToDRRecords(drFieldArea, "*NAME", typeValues);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createVRID(const VRIDRecord& record)
{
	DR dr;
	dr.setFieldTag(VRID);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	writeToDRRecords(drFieldArea, "RCNM", record.RCNM);
	writeToDRRecords(drFieldArea, "RCID", record.RCID);
	writeToDRRecords(drFieldArea, "RVER", record.RVER);
	writeToDRRecords(drFieldArea, "RUIN", record.RUIN);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createATTV(const std::vector<ATTVRecord>& records)
{
	DR dr;
	dr.setFieldTag(ATTV);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	std::vector<TypeValue> typeValues;
	for (auto itRecord = records.begin(); itRecord != records.end(); itRecord++)
	{
		{TypeValue typeValue("b12", itRecord->ATTL); typeValues.push_back(typeValue); }
		{TypeValue typeValue("A()", itRecord->ATVL, -1); typeValues.push_back(typeValue); }
	}
	writeToDRRecords(drFieldArea, "*ATTL", typeValues);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createVRPC(const VRPCRecord& record)
{
	DR dr;
	dr.setFieldTag(VRPC);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	writeToDRRecords(drFieldArea, "VPUI", record.VPUI);
	writeToDRRecords(drFieldArea, "VPIX", record.VPIX);
	writeToDRRecords(drFieldArea, "NVPT", record.NVPT);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createVRPT(const std::vector<VRPTRecord>& records)
{
	DR dr;
	dr.setFieldTag(VRPT);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	std::vector<TypeValue> typeValues;
	for (auto itRecord = records.begin(); itRecord != records.end(); itRecord++)
	{
		{TypeValue typeValue("B(40)", itRecord->NAME, 5); typeValues.push_back(typeValue); }
		{TypeValue typeValue("b11", itRecord->ORNT); typeValues.push_back(typeValue); }
		{TypeValue typeValue("b11", itRecord->USAG); typeValues.push_back(typeValue); }
		{TypeValue typeValue("b11", itRecord->TOPI); typeValues.push_back(typeValue); }
		{TypeValue typeValue("b11", itRecord->MASK); typeValues.push_back(typeValue); }
	}
	writeToDRRecords(drFieldArea, "*NAME", typeValues);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createSGCC(const SGCCRecord& record)
{
	DR dr;
	dr.setFieldTag(SGCC);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	writeToDRRecords(drFieldArea, "CCUI", record.CCUI);
	writeToDRRecords(drFieldArea, "CCIX", record.CCIX);
	writeToDRRecords(drFieldArea, "CCNC", record.CCNC);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createSG2D(const std::vector<SG2DRecord>& records)
{
	DR dr;
	dr.setFieldTag(SG2D);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	std::vector<TypeValue> typeValues;
	for (auto itRecord = records.begin(); itRecord != records.end(); itRecord++)
	{
		{TypeValue typeValue("b24", itRecord->YCOO); typeValues.push_back(typeValue); }
		{TypeValue typeValue("b24", itRecord->XCOO); typeValues.push_back(typeValue); }
	}
	writeToDRRecords(drFieldArea, "*YCOO", typeValues);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createSG3D(const std::vector<SG3DRecord>& records)
{
	DR dr;
	dr.setFieldTag(SG3D);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	std::vector<TypeValue> typeValues;
	for (auto itRecord = records.begin(); itRecord != records.end(); itRecord++)
	{
		{TypeValue typeValue("b24", itRecord->YCOO); typeValues.push_back(typeValue); }
		{TypeValue typeValue("b24", itRecord->XCOO); typeValues.push_back(typeValue); }
		{TypeValue typeValue("b24", itRecord->VE3D); typeValues.push_back(typeValue); }
	}
	writeToDRRecords(drFieldArea, "*YCOO", typeValues);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createARCC(const ARCCRecord& record)
{
	DR dr;
	dr.setFieldTag(ARCC);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	writeToDRRecords(drFieldArea, "ATYP", record.ATYP);
	writeToDRRecords(drFieldArea, "SURF", record.SURF);
	writeToDRRecords(drFieldArea, "ORDR", record.ORDR);
	writeToDRRecords(drFieldArea, "RESO", record.RESO);
	writeToDRRecords(drFieldArea, "FPMF", record.FPMF);
	dr.drFieldArea = drFieldArea;
	return dr;
}

DR S57DataSet::createCT2D(const std::vector<CT2DRecord>& records)
{
	DR dr;
	dr.setFieldTag(CT2D);
	DRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);
	std::vector<TypeValue> typeValues;
	for (auto itRecord = records.begin(); itRecord != records.end(); itRecord++)
	{
		{TypeValue typeValue("b24", itRecord->YCOO); typeValues.push_back(typeValue); }
		{TypeValue typeValue("b24", itRecord->XCOO); typeValues.push_back(typeValue); }
	}
	writeToDRRecords(drFieldArea, "*YCOO", typeValues);
	dr.drFieldArea = drFieldArea;
	return dr;
}

