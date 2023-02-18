#include "s57dataset.h"
#include "../Utils/utils.h"
#include "../ISO8211/iso8211.h"
#include <vector>
#include <QFile>
#include <QIODevice>

int DEBUG_ID = 0;

bool loadS57ExtRes(const std::string& path, std::vector<S57ExtRes>& s57ExtRess, int iLen)
{
	bool bRet = false;
	std::string content;
	if (Utils::loadText(path.c_str(), content))
	{
		std::vector<std::string> lines = Utils::split(content, "\r\n");
		for (auto itLine = lines.begin(); itLine != lines.end(); itLine++)
		{
			if (itLine->empty())
				continue;
			if (itLine->at(0) == '#')
				continue;
			std::size_t iCodeEnd = itLine->find_last_of(' ');
			if (iCodeEnd == std::size_t(-1))
				continue;
			std::size_t iCodeStart = itLine->find_last_of(' ', iCodeEnd - 1);
			if (iCodeStart == std::size_t(-1))
				continue;
			std::string strCode = itLine->substr(iCodeStart, iCodeEnd - iCodeStart);
			int iCode = stoi(strCode);
			std::string strAcronym = itLine->substr(iCodeStart - iLen, iLen);
			std::string strDescription = itLine->substr(0, iCodeStart - iLen);
			strDescription = Utils::trim(strDescription);
			S57ExtRes s57ExtRes;
			s57ExtRes.code = iCode;
			s57ExtRes.acronym = strAcronym;
			s57ExtRess.push_back(s57ExtRes);
		}

		bRet = true;
	}
	return bRet;
}

//S57ObjectClasses

bool S57ObjectClasses::load(const std::string& path)
{
	mObjectClasses.clear();
	bool bRet = loadS57ExtRes(path, mObjectClasses, 6);
	for (int i = 0; i < mObjectClasses.size(); i++)
	{
		S57ExtRes& s57ExtRes = mObjectClasses[i];
		int iCode = s57ExtRes.code;
		std::string strAcronym = s57ExtRes.acronym;
		mAcronyms[iCode] = strAcronym;
		mCodes[strAcronym] = iCode;
	}
	return bRet;
}

std::string S57ObjectClasses::acronym(int code)
{
	std::string strAcronym;
	auto itFind = mAcronyms.find(code);
	if (itFind != mAcronyms.end())
	{
		strAcronym = itFind->second;
	}
	return strAcronym;
}

int S57ObjectClasses::code(const std::string& acronym)
{
	int iCode = -1;
	auto itFind = mCodes.find(acronym);
	if (itFind != mCodes.end())
	{
		iCode = itFind->second;
	}
	return iCode;
}

//S57Attributes
bool S57Attributes::load(const std::string& path, const S57AttributesType& attributesType)
{
	mAttributes.clear();
	bool bRet = loadS57ExtRes(path, mAttributes, 6);
	for (int i = 0; i < mAttributes.size(); i++)
	{
		S57ExtRes& s57ExtRes = mAttributes[i];
		int iCode = s57ExtRes.code;
		//查表
		auto itFind = attributesType.mAttributesType.find(iCode);
		if (itFind != attributesType.mAttributesType.end())
		{
			S57AttributeType type = itFind->second;
			s57ExtRes.type = type;
		}
		else
		{
			s57ExtRes.type = S57AttributeType::UNKNOWN;
		}

		std::string strAcronym = s57ExtRes.acronym;
		mAcronyms[iCode] = strAcronym;
		mCodes[strAcronym] = iCode;
		mTypes[iCode] = s57ExtRes.type;
	}
	return bRet;
}

std::string S57Attributes::acronym(int code)
{
	std::string strAcronym;
	auto itFind = mAcronyms.find(code);
	if (itFind != mAcronyms.end())
	{
		strAcronym = itFind->second;
	}
	return strAcronym;
}

S57AttributeType S57Attributes::type(int code)
{
	S57AttributeType emType = S57AttributeType::UNKNOWN;
	auto itFind = mTypes.find(code);
	if (itFind != mTypes.end())
	{
		emType = itFind->second;
	}
	return emType;
}

int S57Attributes::code(const std::string& acronym)
{
	int iCode = mCodes[acronym];
	return iCode;
}

bool S57AttributesType::load(const std::string& path)
{
	std::string content;
	bool bRet = Utils::loadText(path.c_str(), content);
	auto lines = Utils::split(content, "\r\n");
	for (int iLine = 0; iLine < lines.size(); iLine++)
	{
		std::string line = lines[iLine];
		auto vars = Utils::split(line, " ");
		if (vars.size() == 3)
		{
			int iCode = stoi(vars[0]);
			char type = vars[2][0];
			mAttributesType[iCode] = S57AttributeType(type);
		}
	}
	return bRet;
}


void S57DataSet::setRecordSubfield(const DRRecord& drRecord, Subfield& subfield)
{
	if (mNALL == -1)
	{
		if ("NALL" == drRecord.fieldTag)
		{
			mNALL = subfield.uint8Value();
		}
	}

	if (mAALL == -1)
	{
		if ("AALL" == drRecord.fieldTag)
		{
			mAALL = subfield.uint8Value();
		}
	}
}

int S57DataSet::charSize(const DDRField& ddrField)
{
	bool isNATF = "NATF" == ddrField.mFieldTag;
	int iCharSize = charSize(isNATF);
	return iCharSize;
}


S57DataSet::S57DataSet()
	:mS57ObjectClasses(nullptr)
	, mS57Attributes(nullptr)
{

}

void S57DataSet::setS57ObjectClasses(S57ObjectClasses* s57ObjectClasses)
{
	mS57ObjectClasses = s57ObjectClasses;
}

void S57DataSet::setS57Attributes(S57Attributes* s57Attributes)
{
	mS57Attributes = s57Attributes;
}


bool S57DataSet::load(const std::string& path)
{
	clear();
	bool bRet = Utils::loadBinary(path.c_str(), mISO8211Binary);
	if (bRet)
	{
		std::string fieldTag;
		bRet = false;
		bool is000 = Utils::endsWith(path, "000");
		if (is000)//first
		{
			bRet = ISO8211::loadBinary(mISO8211Binary, &fieldTag);
			if (bRet && (fieldTag == "0001"))
			{
				iso8211ConvertToS57Buffer();
				{//更新文件的合并 ...暂未实现
					std::vector<S57DataSet> updateS57DataSets;
					s57BufferMerge(updateS57DataSets);
				}
				createS57Features();
				bRet = true;
			}
			else
			{
				bRet = false;
			}
		}
	}
	return bRet;
}


void S57DataSet::s57BufferMerge(std::vector<S57DataSet>& dataSets)
{

}




void S57DataSet::createS57Features()
{
	for (auto itFeature = mS57FeaturesBuffer.begin(); itFeature != mS57FeaturesBuffer.end(); itFeature++)
	{
		S57Feature* feature = itFeature->second;
		S57Geometry* geometry = nullptr;
		if (feature->mFRID.RCID == DEBUG_ID)
		{
			bool debug = true;
		}

		switch (feature->mFRID.PRIM)
		{
		case PRIM_P:
		{
			if (feature->mFSPTs.size() == 0)
				continue;
			FSPTRecord& fspt = feature->mFSPTs[0];
			unsigned char rcnm = 0;
			ulong rcid = RCID(fspt.NAME, &rcnm);
			std::tuple<unsigned char, ulong> key(rcnm, rcid);
			auto itFindSpatial = mS57SpatialsBuffer.find(key);
			if (itFindSpatial == mS57SpatialsBuffer.end())
				continue;
			S57Spatial* spatial = itFindSpatial->second;
			if ("SOUNDG" == feature->name())
			{
				S57MultiPoint3D* multiPoint3D = nullptr;
				geometry = multiPoint3D = new S57MultiPoint3D();
				for (auto itSG3D = spatial->mSG3Ds.begin(); itSG3D != spatial->mSG3Ds.end(); itSG3D++)
				{
					double x = Utils::div(itSG3D->XCOO, mDSPM.COMF);
					double y = Utils::div(itSG3D->YCOO, mDSPM.COMF);
					double z = Utils::div(itSG3D->VE3D, mDSPM.SOMF);
					multiPoint3D->points.push_back(XYZ(x, y, z));
				}
			}
			else
			{
				S57Point* point = nullptr;
				geometry = point = new S57Point();
				if (spatial->mSG2Ds.size() > 0)
				{
					SG2DRecord& sg2d = spatial->mSG2Ds[0];
					double x = Utils::div(sg2d.XCOO, mDSPM.COMF);
					double y = Utils::div(sg2d.YCOO, mDSPM.COMF);
					point->point.x = x;
					point->point.y = y;
				}
			}
		}
		break;
		case PRIM_L:
		{
			if (feature->mFSPTs.size() == 0)
				continue;
			std::vector<S57Ring2D> edges;
			createEdges(feature, edges);
			S57Line* line = nullptr;
			if (line == nullptr&&edges.size() > 0)
			{
				geometry = line = new S57Line();
			}
			else
			{
				continue;
			}

			S57Ring2D lineBuffer;
			std::vector<S57Ring2D> innerRings;
			createRings(edges, lineBuffer, innerRings);
			if (lineBuffer.points.size() > 0)
			{
				line->points = lineBuffer.points;
			}
			if (line->points.size() == 0)
			{
				delptr(line);
				geometry = nullptr;
			}
		}
		break;
		case PRIM_A:
		{
			if (feature->mFSPTs.size() == 0)
				continue;
			std::vector<S57Ring2D> edges;
			createEdges(feature, edges);
			S57Area* area = nullptr;
			if (area == nullptr&&edges.size() > 0)
			{
				geometry = area = new S57Area();
			}
			else
			{
				continue;
			}

			S57Ring2D exteriorRing;
			std::vector<S57Ring2D> innerRings;
			createRings(edges, exteriorRing, innerRings);
			if (exteriorRing.points.size() > 0)
			{
				area->exteriorRings.push_back(exteriorRing);
				area->interiorRings.insert(area->interiorRings.end(), innerRings.begin(), innerRings.end());
			}
			if (area->exteriorRings.size() == 0)
			{
				delptr(area);
				geometry = nullptr;
			}
		}
		break;
		}


		if (geometry)
		{
			feature->setGeometry(geometry);
		}


		std::stringstream ss;
		ss << "Feature rcid:" << feature->rcid() << "\t"
			<< "name:" << feature->name() << "\t"
			<< "geometry type:" << feature->geometryTypeString() << "\t"
			<< std::endl;

		if (geometry&&mS57Attributes)
		{
			for (int iField = 0; iField < feature->mATTFs.size(); iField++)
			{
				ATTFRecord& attf = feature->mATTFs[iField];
				std::string acronym = mS57Attributes->acronym(attf.ATTL);
				std::string value = textValue(false, attf.ATVL, attf.ATVLSize);
				ss << "\t" << acronym << ":" << value << std::endl;
				//生成字段
				S57Field field;
				field.name = acronym;
				field.value = value;
				field.type = mS57Attributes->type(attf.ATTL);
				feature->mFields[acronym] = field;
			}

			for (int iField = 0; iField < feature->mNATFs.size(); iField++)
			{
				NATFRecord& natf = feature->mNATFs[iField];
				if (natf.ATVLSize > 0)
				{
					std::string acronym = mS57Attributes->acronym(natf.ATTL);
					std::string value = textValue(true, natf.ATVL, natf.ATVLSize);
					ss << "\t" << acronym << ":" << value << std::endl;
					//生成字段
					S57Field field;
					field.name = acronym;
					field.value = value;
					field.type = mS57Attributes->type(natf.ATTL);
					feature->mFields[acronym] = field;
				}
			}
		}

		mFeatures.push_back(feature);

		//Utils::log(ss.str());
	}

	//最后清掉缓存
	for (auto it = mS57SpatialsBuffer.begin(); it != mS57SpatialsBuffer.end(); it++)
	{
		delptr(it->second);
	}
	mS57SpatialsBuffer.clear();
	mS57FeaturesBuffer.clear();//Feature指针已经移入mFeatures
}

void S57DataSet::createEdges(S57Feature * feature, std::vector<S57Ring2D> &edges)
{
	for (int iEdge = 0; iEdge < feature->mFSPTs.size(); iEdge++)//取边的信息
	{
		FSPTRecord& fspt = feature->mFSPTs[iEdge];
		unsigned char rcnm = 0;
		ulong rcid = RCID(fspt.NAME, &rcnm);
		if (765895391 == rcid)
		{
			bool debug = true;
		}

		std::string strDebug = Utils::formatString("%d,%d", rcid, rcnm);
		std::tuple<unsigned char, ulong> key(rcnm, rcid);
		auto itFindSpatial = mS57SpatialsBuffer.find(key);
		if (itFindSpatial == mS57SpatialsBuffer.end())
			continue;


		S57Spatial* spatial = itFindSpatial->second;
		//内外环
		USAG emUSAG = USAG(fspt.USAG);
		ORNT emORNT = ORNT(fspt.ORNT);
		MASK emMASK = MASK(fspt.MASK);

		S57Ring2D ring;

		//[==下面一段规则化处理
		//Connected node连接节点信息
		int iConnectedNodeRCID0 = 0;
		int iConnectedNodeRCID1 = 0;
		if (spatial->mVRPTs.size() > 1)
		{
			VRPTRecord& vrpt0 = spatial->mVRPTs[0];
			VRPTRecord& vrpt1 = spatial->mVRPTs[1];

			iConnectedNodeRCID0 = RCID(vrpt0.NAME);
			iConnectedNodeRCID1 = RCID(vrpt1.NAME);
		}
		else
		{
			assert(false);//只有一个连接点???
		}

		int iStart = 0;//开始
		int iEnd = 0;//结束
		int iInc = 0;//自增量
		int iConnectedNodeRCIDStart = 0;
		int iConnectedNodeRCIDEnd = 0;
		switch (emORNT)
		{
		case ORNT::ORNT_Reverse://逆时针
		{
			iStart = spatial->mSG2Ds.size() - 1;
			iEnd = 0;
			iInc = -1;
			iConnectedNodeRCIDStart = iConnectedNodeRCID1;
			iConnectedNodeRCIDEnd = iConnectedNodeRCID0;
		}
		break;
		case ORNT::ORNT_Forward://顺时针
		{
			iStart = 0;
			iEnd = spatial->mSG2Ds.size() - 1;
			iInc = 1;
			iConnectedNodeRCIDStart = iConnectedNodeRCID0;
			iConnectedNodeRCIDEnd = iConnectedNodeRCID1;
		}
		break;
		}
		//==]

		{
			//起始点
			double x = 0;
			double y = 0;
			if (getSG2DPoint(getSpatial(RCNM_VC, iConnectedNodeRCIDStart), x, y))
			{
				ring.points.push_back(XY(x, y));
			}

			//边
			if (spatial->mSG2Ds.size() == 1)
			{
				if (getSG2DPoint(spatial, x, y))
				{
					ring.points.push_back(XY(x, y));
				}
			}
			else
			{
				for (int iSG2D = iStart; iSG2D != iEnd + iInc; iSG2D += iInc)
				{
					SG2DRecord& sg2d = spatial->mSG2Ds[iSG2D];
					double x = Utils::div(sg2d.XCOO, mDSPM.COMF);
					double y = Utils::div(sg2d.YCOO, mDSPM.COMF);
					ring.points.push_back(XY(x, y));
				}
			}
			//结束点
			if (getSG2DPoint(getSpatial(RCNM_VC, iConnectedNodeRCIDEnd), x, y))
			{
				ring.points.push_back(XY(x, y));
			}
		}
		//
		//if (emUSAG == USAG::USAG_ExteriorBoundaries)
		{
			edges.push_back(ring);
		}
		/*else if (emUSAG == USAG::USAG_InteriorBoundaries)
		{
		area->interiorRings.push_back(ring);
		}*/

		std::stringstream ss;
		ss << "Edge index:" << iEdge;
		for (int i = 0; i < ring.points.size(); i++)
		{
			XY& xy = ring.points[i];
			ss << " X:" << xy.x << ",Y:" << xy.y;
		}
		Utils::log(ss.str());
	}
}

void S57DataSet::iso8211ConvertToS57Buffer()
{
	S57Feature* feature = nullptr;
	S57Spatial* spatial = nullptr;
	//
	Subfield subfield;

	for (int i = 0; i < mDRs.size(); i++)
	{
		DR& dr = mDRs[i];
		std::string tag = dr.drDirectory.fieldTag;
		if ("0001" == tag)
			continue;
		//key value
		if (DSID == tag) {
			for (auto itDRRecord = dr.drFieldArea.drRecords.begin(); itDRRecord != dr.drFieldArea.drRecords.end(); itDRRecord++)
			{
				std::string drRecordFieldTag = itDRRecord->fieldTag;
				DRFieldArea::createSubfield(*itDRRecord, subfield);
				if ("RCNM" == drRecordFieldTag) {
					mDSID.RCNM = subfield.uint8Value();
				}
				else if ("RCID" == drRecordFieldTag) {
					mDSID.RCID = subfield.uint32Value();
				}
				else if ("EXPP" == drRecordFieldTag) {
					mDSID.EXPP = subfield.uint8Value();
				}
				else if ("INTU" == drRecordFieldTag) {
					mDSID.INTU = subfield.uint8Value();
				}
				else if ("DSNM" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					mDSID.DSNM = new char[str_.size() + 1];
					memset(mDSID.DSNM, 0, str_.size() + 1);
					memcpy(mDSID.DSNM, str_.data(), str_.size());
				}
				else if ("EDTN" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					mDSID.EDTN = new char[str_.size() + 1];
					memset(mDSID.EDTN, 0, str_.size() + 1);
					memcpy(mDSID.EDTN, str_.data(), str_.size());
				}
				else if ("UPDN" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					mDSID.UPDN = new char[str_.size() + 1];
					memset(mDSID.UPDN, 0, str_.size() + 1);
					memcpy(mDSID.UPDN, str_.data(), str_.size());
				}
				else if ("UADT" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					memset(mDSID.UADT, 0, sizeof(mDSID.UADT));
					memcpy(mDSID.UADT, str_.data(), str_.size());
				}
				else if ("ISDT" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					memset(mDSID.ISDT, 0, sizeof(mDSID.ISDT));
					memcpy(mDSID.ISDT, str_.data(), str_.size());
				}
				else if ("STED" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					memset(mDSID.STED, 0, sizeof(mDSID.STED));
					memcpy(mDSID.STED, str_.data(), str_.size());
				}
				else if ("PRSP" == drRecordFieldTag) {
					mDSID.PRSP = subfield.uint8Value();
				}
				else if ("PSDN" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					mDSID.PSDN = new char[str_.size() + 1];
					memset(mDSID.PSDN, 0, str_.size() + 1);
					memcpy(mDSID.PSDN, str_.data(), str_.size());
				}
				else if ("PRED" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					mDSID.PRED = new char[str_.size() + 1];
					memset(mDSID.PRED, 0, str_.size() + 1);
					memcpy(mDSID.PRED, str_.data(), str_.size());
				}
				else if ("PROF" == drRecordFieldTag) {
					mDSID.PROF = subfield.uint8Value();
				}
				else if ("AGEN" == drRecordFieldTag) {
					mDSID.AGEN = subfield.uint16Value();
				}
				else if ("COMT" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					mDSID.COMT = new char[str_.size() + 1];
					memset(mDSID.COMT, 0, str_.size() + 1);
					memcpy(mDSID.COMT, str_.data(), str_.size());
				}
				else
					continue;
			}
		}
		else if (DSSI == tag) {
			for (auto itDRRecord = dr.drFieldArea.drRecords.begin(); itDRRecord != dr.drFieldArea.drRecords.end(); itDRRecord++)
			{
				std::string drRecordFieldTag = itDRRecord->fieldTag;
				DRFieldArea::createSubfield(*itDRRecord, subfield);
				if ("DSTR" == drRecordFieldTag) {
					mDSSI.DSTR = subfield.uint8Value();
				}
				else if ("AALL" == drRecordFieldTag) {
					mDSSI.AALL = subfield.uint8Value();
				}
				else if ("NALL" == drRecordFieldTag) {
					mDSSI.NALL = subfield.uint8Value();
				}
				else if ("NOMR" == drRecordFieldTag) {
					mDSSI.NOMR = subfield.uint32Value();
				}
				else if ("NOCR" == drRecordFieldTag) {
					mDSSI.NOCR = subfield.uint32Value();
				}
				else if ("NOGR" == drRecordFieldTag) {
					mDSSI.NOGR = subfield.uint32Value();
				}
				else if ("NOLR" == drRecordFieldTag) {
					mDSSI.NOLR = subfield.uint32Value();
				}
				else if ("NOIN" == drRecordFieldTag) {
					mDSSI.NOIN = subfield.uint32Value();
				}
				else if ("NOCN" == drRecordFieldTag) {
					mDSSI.NOCN = subfield.uint32Value();
				}
				else if ("NOED" == drRecordFieldTag) {
					mDSSI.NOED = subfield.uint32Value();
				}
				else if ("NOFA" == drRecordFieldTag) {
					mDSSI.NOFA = subfield.uint32Value();
				}
				else
					continue;
			}
		}
		else if (DSPM == tag) {
			for (auto itDRRecord = dr.drFieldArea.drRecords.begin(); itDRRecord != dr.drFieldArea.drRecords.end(); itDRRecord++)
			{
				std::string drRecordFieldTag = itDRRecord->fieldTag;
				DRFieldArea::createSubfield(*itDRRecord, subfield);

				if ("RCNM" == drRecordFieldTag) {
					mDSPM.RCNM = subfield.uint8Value();
				}
				else if ("RCID" == drRecordFieldTag) {
					mDSPM.RCID = subfield.uint32Value();
				}
				else if ("HDAT" == drRecordFieldTag) {
					mDSPM.HDAT = subfield.uint8Value();
				}
				else if ("VDAT" == drRecordFieldTag) {
					mDSPM.VDAT = subfield.uint8Value();
				}
				else if ("SDAT" == drRecordFieldTag) {
					mDSPM.SDAT = subfield.uint8Value();
				}
				else if ("CSCL" == drRecordFieldTag) {
					mDSPM.CSCL = subfield.uint32Value();
				}
				else if ("DUNI" == drRecordFieldTag) {
					mDSPM.DUNI = subfield.uint8Value();
				}
				else if ("HUNI" == drRecordFieldTag) {
					mDSPM.HUNI = subfield.uint8Value();
				}
				else if ("PUNI" == drRecordFieldTag) {
					mDSPM.PUNI = subfield.uint8Value();
				}
				else if ("COUN" == drRecordFieldTag) {
					mDSPM.COUN = subfield.uint8Value();
				}
				else if ("COMF" == drRecordFieldTag) {
					mDSPM.COMF = subfield.uint32Value();
				}
				else if ("SOMF" == drRecordFieldTag) {
					mDSPM.SOMF = subfield.uint32Value();
				}
				else if ("COMT" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					mDSPM.COMT = new char[str_.size() + 1];
					memset(mDSPM.COMT, 0, str_.size() + 1);
					memcpy(mDSPM.COMT, str_.data(), str_.size());
				}
				else
					continue;
			}
		}
		else if (DSAC == tag) {
			for (auto itDRRecord = dr.drFieldArea.drRecords.begin(); itDRRecord != dr.drFieldArea.drRecords.end(); itDRRecord++)
			{
				std::string drRecordFieldTag = itDRRecord->fieldTag;
				DRFieldArea::createSubfield(*itDRRecord, subfield);

				if ("RCNM" == drRecordFieldTag) {
					mDSAC.RCNM = subfield.uint8Value();
				}
				else if ("RCID" == drRecordFieldTag) {
					mDSAC.RCID = subfield.uint32Value();
				}
				else if ("PACC" == drRecordFieldTag) {
					mDSAC.PACC = subfield.uint32Value();
				}
				else if ("HACC" == drRecordFieldTag) {
					mDSAC.HACC = subfield.uint32Value();
				}
				else if ("SACC" == drRecordFieldTag) {
					mDSAC.SACC = subfield.uint32Value();
				}
				else if ("FPMF" == drRecordFieldTag) {
					mDSAC.FPMF = subfield.uint32Value();
				}
				else if ("COMT" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					mDSAC.COMT = new char[str_.size() + 1];
					memset(mDSAC.COMT, 0, str_.size() + 1);
					memcpy(mDSAC.COMT, str_.data(), str_.size());
				}
				else
					continue;
			}
		}
		else if (CATD == tag) {
			for (auto itDRRecord = dr.drFieldArea.drRecords.begin(); itDRRecord != dr.drFieldArea.drRecords.end(); itDRRecord++)
			{
				std::string drRecordFieldTag = itDRRecord->fieldTag;
				DRFieldArea::createSubfield(*itDRRecord, subfield);

				if ("RCNM" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					memset(mCATD.RCNM, 0, sizeof(mCATD.RCNM));
					memcpy(mCATD.RCNM, str_.data(), str_.size());
				}
				else if ("RCID" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					memset(mCATD.RCID, 0, sizeof(mCATD.RCID));
					memcpy(mCATD.RCID, str_.data(), str_.size());
				}
				else if ("FILE" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					mCATD.FILE = new char[str_.size() + 1];
					memset(mCATD.FILE, 0, str_.size() + 1);
					memcpy(mCATD.FILE, str_.data(), str_.size());
				}
				else if ("LFIL" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					mCATD.LFIL = new char[str_.size() + 1];
					memset(mCATD.LFIL, 0, str_.size() + 1);
					memcpy(mCATD.LFIL, str_.data(), str_.size());
				}
				else if ("VOLM" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					mCATD.VOLM = new char[str_.size() + 1];
					memset(mCATD.VOLM, 0, str_.size() + 1);
					memcpy(mCATD.VOLM, str_.data(), str_.size());
				}
				else if ("IMPL" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					memset(mCATD.IMPL, 0, sizeof(mCATD.IMPL));
					memcpy(mCATD.IMPL, str_.data(), str_.size());
				}
				else if ("SLAT" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					mCATD.SLAT = new char[str_.size() + 1];
					memset(mCATD.SLAT, 0, str_.size() + 1);
					memcpy(mCATD.SLAT, str_.data(), str_.size());
				}
				else if ("WLON" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					mCATD.WLON = new char[str_.size() + 1];
					memset(mCATD.WLON, 0, str_.size() + 1);
					memcpy(mCATD.WLON, str_.data(), str_.size());
				}
				else if ("NLAT" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					mCATD.NLAT = new char[str_.size() + 1];
					memset(mCATD.NLAT, 0, str_.size() + 1);
					memcpy(mCATD.NLAT, str_.data(), str_.size());
				}
				else if ("ELON" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					mCATD.ELON = new char[str_.size() + 1];
					memset(mCATD.ELON, 0, str_.size() + 1);
					memcpy(mCATD.ELON, str_.data(), str_.size());
				}
				else if ("CRCS" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					mCATD.CRCS = new char[str_.size() + 1];
					memset(mCATD.CRCS, 0, str_.size() + 1);
					memcpy(mCATD.CRCS, str_.data(), str_.size());
				}
				else if ("COMT" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					mCATD.COMT = new char[str_.size() + 1];
					memset(mCATD.COMT, 0, str_.size() + 1);
					memcpy(mCATD.COMT, str_.data(), str_.size());
				}
				else
					continue;
			}
		}
		//Feature
		else if (FRID == tag)
		{
			feature = new S57Feature();
			//
			for (auto itDRRecord = dr.drFieldArea.drRecords.begin(); itDRRecord != dr.drFieldArea.drRecords.end(); itDRRecord++)
			{
				std::string drRecordFieldTag = itDRRecord->fieldTag;
				DRFieldArea::createSubfield(*itDRRecord, subfield);

				if ("RCNM" == drRecordFieldTag) {
					feature->mFRID.RCNM = subfield.uint8Value();
				}
				else if ("RCID" == drRecordFieldTag) {
					feature->mFRID.RCID = subfield.uint32Value();
				}
				else if ("PRIM" == drRecordFieldTag) {
					feature->mFRID.PRIM = subfield.uint8Value();
				}
				else if ("GRUP" == drRecordFieldTag) {
					feature->mFRID.GRUP = subfield.uint8Value();
				}
				else if ("OBJL" == drRecordFieldTag) {
					feature->mFRID.OBJL = subfield.uint16Value();
					feature->setObjectClasses(mS57ObjectClasses);
				}
				else if ("RVER" == drRecordFieldTag) {
					feature->mFRID.RVER = subfield.uint16Value();
				}
				else if ("RUIN" == drRecordFieldTag) {
					feature->mFRID.RUIN = subfield.uint8Value();
				}
				else
					continue;
			}
			if (mS57FeaturesBuffer[feature->mFRID.RCID])
			{
				assert(false);//has exist
			}
			mS57FeaturesBuffer[feature->mFRID.RCID] = feature;
		}
		else if (FOID == tag) {
			for (auto itDRRecord = dr.drFieldArea.drRecords.begin(); itDRRecord != dr.drFieldArea.drRecords.end(); itDRRecord++)
			{
				std::string drRecordFieldTag = itDRRecord->fieldTag;
				DRFieldArea::createSubfield(*itDRRecord, subfield);

				if ("AGEN" == drRecordFieldTag) {
					feature->mFOID.AGEN = subfield.uint16Value();
				}
				else if ("FIDN" == drRecordFieldTag) {
					feature->mFOID.FIDN = subfield.uint32Value();
				}
				else if ("FIDS" == drRecordFieldTag) {
					feature->mFOID.FIDS = subfield.uint16Value();
				}
				else
					continue;
			}
		}
		else if (NATF == tag) {
			NATFRecord natf;
			for (auto itDRRecord = dr.drFieldArea.drRecords.begin(); itDRRecord != dr.drFieldArea.drRecords.end(); itDRRecord++)
			{
				std::string drRecordFieldTag = itDRRecord->fieldTag;
				DRFieldArea::createSubfield(*itDRRecord, subfield);
				int binaryOffset = itDRRecord->binaryOffset;

				if ("*ATTL" == drRecordFieldTag) {
					natf.ATTL = subfield.uint16Value();

					if (natf.ATTL == 30)
					{
						break;
					}

					if (natf.ATTL > 402)
					{
						bool debug = true;
					}
				}
				else if ("ATVL" == drRecordFieldTag) {
					const char* data = &mISO8211Binary[itDRRecord->binaryOffset];
					int charSize_ = charSize(true);
					int textLength_ = textBytesLength(data, charSize_);
					if (textLength_ > 0)
					{
						natf.ATVL = new char[textLength_ + charSize_];
						natf.ATVLSize = textLength_;
						memset(natf.ATVL, 0, textLength_ + charSize_);
						memcpy(natf.ATVL, data, textLength_);

						feature->mNATFs.push_back(natf);
					}
				}
				else
					continue;
			}
		}
		else if (ATTF == tag) {
			ATTFRecord attf;
			for (auto itDRRecord = dr.drFieldArea.drRecords.begin(); itDRRecord != dr.drFieldArea.drRecords.end(); itDRRecord++)
			{
				std::string drRecordFieldTag = itDRRecord->fieldTag;
				DRFieldArea::createSubfield(*itDRRecord, subfield);
				int binaryOffset = itDRRecord->binaryOffset;

				if ("*ATTL" == drRecordFieldTag) {
					attf.ATTL = subfield.uint16Value();

					if (attf.ATTL == 30)
					{
						break;
					}
				}
				else if ("ATVL" == drRecordFieldTag) {
					const char* data = &mISO8211Binary[itDRRecord->binaryOffset];
					int charSize_ = charSize(false);
					int textLength_ = textBytesLength(data, charSize_);
					if (textLength_ > 0)
					{
						attf.ATVL = new char[textLength_ + charSize_];
						attf.ATVLSize = textLength_;
						memset(attf.ATVL, 0, textLength_ + charSize_);
						memcpy(attf.ATVL, data, textLength_);

						feature->mATTFs.push_back(attf);
					}
				}
				else
					continue;
			}

		}
		else  if (FFPC == tag) {
			for (auto itDRRecord = dr.drFieldArea.drRecords.begin(); itDRRecord != dr.drFieldArea.drRecords.end(); itDRRecord++)
			{
				std::string drRecordFieldTag = itDRRecord->fieldTag;
				DRFieldArea::createSubfield(*itDRRecord, subfield);

				if ("FFUI" == drRecordFieldTag) {
					feature->mFFPC.FFUI = subfield.uint8Value();
				}
				else if ("FFIX" == drRecordFieldTag) {
					feature->mFFPC.FFIX = subfield.uint16Value();
				}
				else if ("NFPT" == drRecordFieldTag) {
					feature->mFFPC.NFPT = subfield.uint16Value();
				}
				else
					continue;
			}
		}
		else if (FFPT == tag) {
			FFPTRecord ffpt;
			for (auto itDRRecord = dr.drFieldArea.drRecords.begin(); itDRRecord != dr.drFieldArea.drRecords.end(); itDRRecord++)
			{
				std::string drRecordFieldTag = itDRRecord->fieldTag;
				DRFieldArea::createSubfield(*itDRRecord, subfield);

				if ("*LNAM" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					memset(ffpt.LNAM, 0, sizeof(ffpt.LNAM));
					memcpy(ffpt.LNAM, str_.data(), str_.size());
				}
				else if ("RIND" == drRecordFieldTag) {
					ffpt.RIND = subfield.uint8Value();
				}
				else if ("COMT" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					ffpt.COMT = new char[str_.size() + 1];
					memset(ffpt.COMT, 0, str_.size() + 1);
					memcpy(ffpt.COMT, str_.data(), str_.size());
					feature->mFFPTs.push_back(ffpt);
				}
				else
					continue;
				//feature->fields.push_back(subfield);
			}
		}
		else if (FSPC == tag) {
			for (auto itDRRecord = dr.drFieldArea.drRecords.begin(); itDRRecord != dr.drFieldArea.drRecords.end(); itDRRecord++)
			{
				std::string drRecordFieldTag = itDRRecord->fieldTag;
				DRFieldArea::createSubfield(*itDRRecord, subfield);

				if ("*FSUI" == drRecordFieldTag) {
					feature->mFSPC.FSUI = subfield.uint8Value();
				}
				else if ("FSIX" == drRecordFieldTag) {
					feature->mFSPC.FSIX = subfield.uint16Value();
				}
				else if ("NSPT" == drRecordFieldTag) {
					feature->mFSPC.NSPT = subfield.uint16Value();
				}
				else
					continue;
				//feature->fields.push_back(subfield);
			}
		}
		else if (FSPT == tag) {
			FSPTRecord fspt;
			for (auto itDRRecord = dr.drFieldArea.drRecords.begin(); itDRRecord != dr.drFieldArea.drRecords.end(); itDRRecord++)
			{
				std::string drRecordFieldTag = itDRRecord->fieldTag;
				DRFieldArea::createSubfield(*itDRRecord, subfield);
				int binaryOffset = itDRRecord->binaryOffset;

				if ("*NAME" == drRecordFieldTag) {
					auto bin_ = subfield.binary();
					memset(fspt.NAME, 0, sizeof(fspt.NAME));
					memcpy(fspt.NAME, bin_.data(), bin_.size());
					//if (fspt.rcid() == 749052531
					//	|| fspt.rcid() == 765895283)
					//{
					//	bool debug = true;
					//}
				}
				else if ("ORNT" == drRecordFieldTag) {
					fspt.ORNT = subfield.uint8Value();
				}
				else if ("USAG" == drRecordFieldTag) {
					fspt.USAG = subfield.uint8Value();
				}
				else if ("MASK" == drRecordFieldTag) {
					fspt.MASK = subfield.uint8Value();
					feature->mFSPTs.push_back(fspt);
				}
				else
					continue;
				//feature->fields.push_back(subfield);
			}

		}
		//Spatial
		else if (VRID == tag) {
			spatial = new S57Spatial();
			//
			for (auto itDRRecord = dr.drFieldArea.drRecords.begin(); itDRRecord != dr.drFieldArea.drRecords.end(); itDRRecord++)
			{
				std::string drRecordFieldTag = itDRRecord->fieldTag;
				DRFieldArea::createSubfield(*itDRRecord, subfield);

				if ("RCNM" == drRecordFieldTag) {
					spatial->mVRID.RCNM = subfield.uint8Value();
				}
				else if ("RCID" == drRecordFieldTag) {
					spatial->mVRID.RCID = subfield.uint32Value();
				}
				else if ("RVER" == drRecordFieldTag) {
					spatial->mVRID.RVER = subfield.uint16Value();
				}
				else if ("RUIN" == drRecordFieldTag) {
					spatial->mVRID.RUIN = subfield.uint8Value();
				}
				else
					continue;
			}

			if (spatial
				&&spatial->mVRID.RCNM == 130
				&& spatial->mVRID.RCID == 765899426)
			{
				bool debug = true;
			}

			std::tuple<unsigned char, ulong> key(spatial->mVRID.RCNM, spatial->mVRID.RCID);
			auto itFind = mS57SpatialsBuffer.find(key);
			if (itFind != mS57SpatialsBuffer.end())
			{
				assert(false);//has exist
			}
			mS57SpatialsBuffer[key] = spatial;
		}
		else if (ATTV == tag) {
			ATTVRecord attv;
			for (auto itDRRecord = dr.drFieldArea.drRecords.begin(); itDRRecord != dr.drFieldArea.drRecords.end(); itDRRecord++)
			{
				std::string drRecordFieldTag = itDRRecord->fieldTag;
				DRFieldArea::createSubfield(*itDRRecord, subfield);

				if ("*ATTL" == drRecordFieldTag) {
					attv.ATTL = subfield.uint16Value();
				}
				else if ("ATVL" == drRecordFieldTag) {
					auto str_ = subfield.toString();
					attv.ATVL = new char[str_.size() + 1];
					memset(attv.ATVL, 0, str_.size() + 1);
					memcpy(attv.ATVL, str_.data(), str_.size());
					spatial->mATTVs.push_back(attv);
				}
				else
					continue;
			}
		}
		else if (VRPC == tag) {
			for (auto itDRRecord = dr.drFieldArea.drRecords.begin(); itDRRecord != dr.drFieldArea.drRecords.end(); itDRRecord++)
			{
				std::string drRecordFieldTag = itDRRecord->fieldTag;
				DRFieldArea::createSubfield(*itDRRecord, subfield);
				if ("VPUI" == drRecordFieldTag) {
					spatial->mVRPC.VPUI = subfield.uint8Value();
				}
				else if ("VPIX" == drRecordFieldTag) {
					spatial->mVRPC.VPIX = subfield.uint16Value();
				}
				else if ("NVPT" == drRecordFieldTag) {
					spatial->mVRPC.NVPT = subfield.uint16Value();
				}
				else
					continue;
			}
		}
		else if (VRPT == tag) {
			VRPTRecord vrpt;
			for (auto itDRRecord = dr.drFieldArea.drRecords.begin(); itDRRecord != dr.drFieldArea.drRecords.end(); itDRRecord++)
			{
				std::string drRecordFieldTag = itDRRecord->fieldTag;
				DRFieldArea::createSubfield(*itDRRecord, subfield);
				if ("*NAME" == drRecordFieldTag) {
					auto bin_ = subfield.binary();
					memset(vrpt.NAME, 0, sizeof(vrpt.NAME));
					memcpy(vrpt.NAME, bin_.data(), bin_.size());
				}
				else if ("ORNT" == drRecordFieldTag) {
					vrpt.ORNT = subfield.uint8Value();
				}
				else if ("USAG" == drRecordFieldTag) {
					vrpt.USAG = subfield.uint8Value();
				}
				else if ("TOPI" == drRecordFieldTag) {
					vrpt.TOPI = subfield.uint8Value();
				}
				else if ("MASK" == drRecordFieldTag) {
					vrpt.MASK = subfield.uint8Value();
					spatial->mVRPTs.push_back(vrpt);
				}
				else
					continue;
			}
		}
		else if (SGCC == tag) {
			SGCCRecord sgcc;
			for (auto itDRRecord = dr.drFieldArea.drRecords.begin(); itDRRecord != dr.drFieldArea.drRecords.end(); itDRRecord++)
			{
				std::string drRecordFieldTag = itDRRecord->fieldTag;
				DRFieldArea::createSubfield(*itDRRecord, subfield);
				if ("CCUI" == drRecordFieldTag) {
					sgcc.CCUI = subfield.uint8Value();
				}
				else if ("CCIX" == drRecordFieldTag) {
					sgcc.CCIX = subfield.uint16Value();
				}
				else if ("CCNC" == drRecordFieldTag) {
					sgcc.CCNC = subfield.uint16Value();
				}
				else
					continue;
			}
			spatial->mSGCCs.push_back(sgcc);
		}
		else if (SG2D == tag) {

			SG2DRecord sg2d;
			for (auto itDRRecord = dr.drFieldArea.drRecords.begin(); itDRRecord != dr.drFieldArea.drRecords.end(); itDRRecord++)
			{
				std::string drRecordFieldTag = itDRRecord->fieldTag;
				DRFieldArea::createSubfield(*itDRRecord, subfield);
				if ("*YCOO" == drRecordFieldTag) {
					sg2d.YCOO = subfield.int32Value();
				}
				else if ("XCOO" == drRecordFieldTag) {
					sg2d.XCOO = subfield.int32Value();
					spatial->mSG2Ds.push_back(sg2d);
				}
				else
					continue;
			}

		}
		else if (SG3D == tag) {
			SG3DRecord sg3d;
			for (auto itDRRecord = dr.drFieldArea.drRecords.begin(); itDRRecord != dr.drFieldArea.drRecords.end(); itDRRecord++)
			{
				std::string drRecordFieldTag = itDRRecord->fieldTag;
				DRFieldArea::createSubfield(*itDRRecord, subfield);
				if ("*YCOO" == drRecordFieldTag) {
					sg3d.YCOO = subfield.int32Value();
				}
				else if ("XCOO" == drRecordFieldTag) {
					sg3d.XCOO = subfield.int32Value();
				}
				else if ("VE3D" == drRecordFieldTag) {
					sg3d.VE3D = subfield.int32Value();
					spatial->mSG3Ds.push_back(sg3d);
				}
				else
					continue;
			}

		}

	}
}



bool S57DataSet::save(const std::string& path)
{
	bool bRet = ISO8211::save(path);
	return bRet;
}

void S57DataSet::clear()
{
	mISO8211Binary.clear();
	for (auto it = mS57FeaturesBuffer.begin(); it != mS57FeaturesBuffer.end(); it++)
	{
		delete it->second;
	}
	mS57FeaturesBuffer.clear();

	for (auto it = mFeatures.begin(); it != mFeatures.end(); it++)
	{
		delete *it;
	}
	mS57FeaturesBuffer.clear();

	for (auto it = mS57SpatialsBuffer.begin(); it != mS57SpatialsBuffer.end(); it++)
	{
		delete it->second;
	}
	mS57SpatialsBuffer.clear();
}

void S57DataSet::initMeta()
{
	{//0000
		//Tree

		DDRField field;
		field.mFieldTag = ISO8211_0000;
		field.mFieldLabel = "";
		field.mSubfieldInfos.push_back(SubfieldInfo("", "", "", "b12", false));
		{//Field control field
			field.mFieldControlField.dataStructureCode = DATA_STRUCTURE_CODE_0;
			field.mFieldControlField.dataTypeCode = DATA_TYPE_CODE_0;
			//field.mFieldControlField.auxiliaryControls init is "00"
			//field.mFieldControlField.printableGraphics init is ";&"
			//field.mFieldControlField.truncatedEscapeSequence init is "   "
		}
		mMetaDDRFields.push_back(field);
	}

	{//0001
		DDRField field;
		field.mFieldTag = ISO8211_0001;
		//field.mFieldLabel = "ISO/IEC 8211 Record Identifier";
		field.mFieldLabel = "ISO 8211 Record Identifier";
		field.mSubfieldInfos.push_back(SubfieldInfo("", "", "", "b12", false));
		{//Field control field
			field.mFieldControlField.dataStructureCode = DATA_STRUCTURE_CODE_0;
			field.mFieldControlField.dataTypeCode = DATA_TYPE_CODE_5;
			//field.mFieldControlField.auxiliaryControls init is "00"
			//field.mFieldControlField.printableGraphics init is ";&"
			//field.mFieldControlField.truncatedEscapeSequence init is "   "
		}
		mMetaDDRFields.push_back(field);
	}

	{//DSID
		DDRField field;
		field.mFieldTag = DSID;
		//field.mFieldLabel = "Data Set Identification";
		field.mFieldLabel = "Data set identification field";
		field.mSubfieldInfos.push_back(SubfieldInfo("Record name", "RCNM", "A(2)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Record identification number", "RCID", "I(10)", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Exchange purpose", "EXPP", "A(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Intended usage", "INTU", "I(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Data set name", "DSNM", "A()", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Edition number", "EDTN", "A()", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Update number", "UPDN", "A()", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Update application date", "UADT", "A(8)", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Issue date", "ISDT", "A(8)", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Edition number of S - 57", "STED", "R(4)", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Product Specification", "PRSP", "A(3)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Product specification description", "PSDN", "A()", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Product specification edition number", "PRED", "A()", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Application profile identification", "PROF", "A(2)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Producing agency", "AGEN", "A(2)", "b12", false));
		//field.mSubfieldInfos.push_back(std::tuple<string, string, string, string>("Application profile identification", "PROF", "A(2)", ""));
		//field.mSubfieldInfos.push_back(std::tuple<string, string, string, string>("Producing agency", "AGEN", "A(2)", ""));
		field.mSubfieldInfos.push_back(SubfieldInfo("Comment", "COMT", "A()", "", false));
		{//Field control field
			field.mFieldControlField.dataStructureCode = DATA_STRUCTURE_CODE_1;
			field.mFieldControlField.dataTypeCode = DATA_TYPE_CODE_6;
			//field.mFieldControlField.auxiliaryControls init is "00"
			//field.mFieldControlField.printableGraphics init is ";&"
			//field.mFieldControlField.truncatedEscapeSequence init is "   "
		}
		mMetaDDRFields.push_back(field);
	}

	{//DSSI
		DDRField field;
		field.mFieldTag = DSSI;
		//field.mFieldLabel = "Data Set Structure information";
		field.mFieldLabel = "Data set structure information field";
		field.mSubfieldInfos.push_back(SubfieldInfo("Data structure", "DSTR", "A(2)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("ATTF lexical level", "AALL", "I(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("NATF lexical level", "NALL", "I(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Number of meta records", "NOMR", "I()", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Number of cartographic records", "NOCR", "I()", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Number of geo records", "NOGR", "I()", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Number of collection records", "NOLR", "I()", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Number of isolated node records", "NOIN", "I()", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Number of connected node records", "NOCN", "I()", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Number of edge records", "NOED", "I()", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Number of face records", "NOFA", "I()", "b14", false));
		{//Field control field
			field.mFieldControlField.dataStructureCode = DATA_STRUCTURE_CODE_1;
			field.mFieldControlField.dataTypeCode = DATA_TYPE_CODE_6;
			//field.mFieldControlField.auxiliaryControls init is "00"
			//field.mFieldControlField.printableGraphics init is ";&"
			//field.mFieldControlField.truncatedEscapeSequence init is "   "
		}
		mMetaDDRFields.push_back(field);
	}

	{//DSPM
		DDRField field;
		field.mFieldTag = DSPM;
		//field.mFieldLabel = "Data Set Parameter";
		field.mFieldLabel = "Data set parameter field";
		field.mSubfieldInfos.push_back(SubfieldInfo("Record name", "RCNM", "A(2)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Record identification number", "RCID", "I(10)", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Horizontal geodetic datum", "HDAT", "I(3)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Vertical datum", "VDAT", "I(2)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Sounding datum", "SDAT", "I(2)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Compilation scale of data", "CSCL", "I()", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Units of depth measurement", "DUNI", "I(2)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Units of height measurement", "HUNI", "I(2)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Units of positional accuracy", "PUNI", "I(2)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Coordinate units", "COUN", "A(2)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Coordinate multiplication factor", "COMF", "I()", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("3 - D(sounding) multiplication factor", "SOMF", "I()", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Comment", "COMT", "A()", "", false));
		{//Field control field
			field.mFieldControlField.dataStructureCode = DATA_STRUCTURE_CODE_1;
			field.mFieldControlField.dataTypeCode = DATA_TYPE_CODE_6;
			//field.mFieldControlField.auxiliaryControls init is "00"
			//field.mFieldControlField.printableGraphics init is ";&"
			//field.mFieldControlField.truncatedEscapeSequence init is "   "
		}
		mMetaDDRFields.push_back(field);
	}

	{//DSPR
		DDRField field;
		field.mFieldTag = DSPR;
		field.mFieldLabel = "Data Set Projection";
		field.mSubfieldInfos.push_back(SubfieldInfo("Projection", "PROJ", "A(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Projection parameter 1", "PRP1", "R()", "b24*)", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Projection parameter 2", "PRP2", "R()", "b24*)", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Projection parameter 3", "PRP3", "R()", "b24*)", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Projection parameter 4", "PRP4", "R()", "b24*)", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("False Easting", "FEAS", "R()", "b24*)", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("False Northing", "FNOR", "R()", "b24*)", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Floating point multiplication factor", "FPMF", "I()", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Comment", "COMT", "A()", "", false));
		mMetaDDRFields.push_back(field);
	}

	{//DSRC
		DDRField field;
		field.mFieldTag = DSRC;
		field.mFieldLabel = "Data Set Registration Control Field Tag";
		field.mSubfieldInfos.push_back(SubfieldInfo("Registration point ID", "*RPID", "A(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Registration point Latitude or Northing", "RYCO", "R()", "b24*)", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Registration point Longitude or Easting", "RXCO", "R()", "b24*)", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Coordinate units for registration	point", "CURP", "A(2)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Floating point multiplication factor", "FPMF", "I()", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Registration point X-value", "RXVL", "R()", "b24", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Registration point Y-value", "RYVL", "R()", "b24", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Comment", "COMT", "A()", "", false));
		mMetaDDRFields.push_back(field);
	}


	{//DSHT
		DDRField field;
		field.mFieldTag = DSHT;
		field.mFieldLabel = "Data Set History";
		field.mSubfieldInfos.push_back(SubfieldInfo("Record name", "RCNM", "A(2)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Record identification number", "RCID", "I(10)", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Producing agency code", "PRCO", "A(2)", "b12", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Earliest source date", "ESDT", "A(8)", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Latest source date", "LSDT", "A(8)", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Data collection criteria", "DCRT", "A()", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Compilation date", "CODT", "A(8)", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Comment", "COMT", "A()", "", false));
	}

	{//DSAC
		DDRField field;
		field.mFieldTag = DSAC;
		field.mFieldLabel = "Data Set Accuracy";
		field.mSubfieldInfos.push_back(SubfieldInfo("Record name", "RCNM", "A(2)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Record identification number", "RCID", "I(10)", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Absolute positional accuracy", "PACC", "R()", "b14*)", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Absolute horizontal/vertical measurement accuracy", "HACC", "R()", "b14*)", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Absolute sounding accuracy", "SACC", "R()", "b14*)", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Floating point multiplication factor", "FPMF", "I()", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Comment", "COMT", "A()", "", false));
		mMetaDDRFields.push_back(field);
	}

	{//CATD
		DDRField field;
		field.mFieldTag = CATD;
		field.mFieldLabel = "Catalogue Directory";
		field.mSubfieldInfos.push_back(SubfieldInfo("Record name", "RCNM", "A(2)", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Record identification number", "RCID", "I(10)", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("File name", "FILE", "A()", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("File long name", "LFIL", "A()", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Volume", "VOLM", "A()", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Implementation", "IMPL", "A(3)", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Southernmost latitude", "SLAT", "R()", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Westernmost longitude", "WLON", "R()", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Northernmost latitude", "NLAT", "R()", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Easternmost Longitude", "ELON", "R()", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("CRC", "CRCS", "A()", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Comment", "COMT", "A()", "", false));
		mMetaDDRFields.push_back(field);
	}

	{//CATX
		DDRField field;
		field.mFieldTag = CATX;
		field.mFieldLabel = "Catalogue Cross Reference";
		field.mSubfieldInfos.push_back(SubfieldInfo("Record name", "*RCNM", "A(2)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Record identification number", "RCID", "I(10)", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Name 1", "NAM1", "A(12)", "B(40)", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Name 2", "NAM2", "A(12)", "B(40)", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Comment", "COMT", "A()", "", false));
		mMetaDDRFields.push_back(field);
	}

	{//DDDF
		DDRField field;
		field.mFieldTag = DDDF;
		field.mFieldLabel = "Data Dictionary Definition";
		field.mSubfieldInfos.push_back(SubfieldInfo("Record name", "RCNM", "A(2)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Record identification number", "RCID", "I(10)", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Object or attribute", "OORA", "A(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Object or attribute acronym", "OAAC", "A(6)", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Object or attribute label/code", "OACO", "I(5)", "b12", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Object or attribute long label", "OALL", "A()", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Type of object or attribute", "OATY", "A(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Definition", "DEFN", "A()", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Authorizing agency", "AUTH", "A(2)", "b12", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Comment", "COMT", "A()", "", false));
		mMetaDDRFields.push_back(field);
	}

	{//DDDR
		DDRField field;
		field.mFieldTag = DDDR;
		field.mFieldLabel = "Data Dictionary Definition Reference";
		field.mSubfieldInfos.push_back(SubfieldInfo("Reference type", "*RFTP", "A(2)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Reference value", "RFVL", "A()", "", false));
		mMetaDDRFields.push_back(field);
	}

	{//DDDI
		DDRField field;
		field.mFieldTag = DDDI;
		field.mFieldLabel = "Data Dictionary Domain Identifier";
		field.mSubfieldInfos.push_back(SubfieldInfo("Record name", "RCNM", "A(2)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Record identification number", "RCID", "I(10)", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Attribute label/code", "ATLB", "I(5)", "b12", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Attribute domain type", "ATDO", "A(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Attribute domain value measurement unit", "ADMU", "A()", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Attribute domain format", "ADFT", "A()", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Authorizing agency", "AUTH", "A(2)", "b12", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Comment", "COMT", "A()", "", false));
		mMetaDDRFields.push_back(field);
	}

	{//DDOM
		DDRField field;
		field.mFieldTag = DDOM;
		field.mFieldLabel = "Data Dictionary Domain";
		field.mSubfieldInfos.push_back(SubfieldInfo("Range or value", "RAVA", "A(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Domain value", "DVAL", "A()", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Domain value short description", "DVSD", "A()", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Domain value definition", "DEFN", "A()", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Authorizing agency", "AUTH", "A(2)", "b12", false));
		mMetaDDRFields.push_back(field);
	}

	{//DDRF
		DDRField field;
		field.mFieldTag = DDRF;
		field.mFieldLabel = "Data Dictionary Domain Reference";
		field.mSubfieldInfos.push_back(SubfieldInfo("Reference type", "*RFTP", "A(2)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Reference value", "RFVL", "A()", "", false));
		mMetaDDRFields.push_back(field);
	}

	{//DDSI
		DDRField field;
		field.mFieldTag = DDSI;
		field.mFieldLabel = "Data Dictionary Schema Identifier";
		field.mSubfieldInfos.push_back(SubfieldInfo("Record name", "RCNM", "A(2)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Record identification number", "RCID", "I(10)", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Object label/code", "OBLB", "I(5)", "b12", false));
		mMetaDDRFields.push_back(field);
	}

	{//DDSC
		DDRField field;
		field.mFieldTag = DDSC;
		field.mFieldLabel = "Data Dictionary Schema";
		field.mSubfieldInfos.push_back(SubfieldInfo("Attribute label/code ", "*ATLB", "I(5)", "b12", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Attribute set", "ASET", "A(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Authorizing agency", "AUTH", "A(2)", "b12", false));
		mMetaDDRFields.push_back(field);
	}

	{//FRID
		DDRField field;
		field.mFieldTag = FRID;
		//field.mFieldLabel = "Feature Record Identifier";
		field.mFieldLabel = "Feature record identifier field";
		field.mSubfieldInfos.push_back(SubfieldInfo("Record name", "RCNM", "A(2)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Record identification number", "RCID", "I(10)", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Object geometric primitive", "PRIM", "A(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Group", "GRUP", "I(3)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Object label/code", "OBJL", "I(5)", "b12", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Record version", "RVER", "I(3)", "b12", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Record update instruction", "RUIN", "A(1)", "b11", false));
		{//Field control field
			field.mFieldControlField.dataStructureCode = DATA_STRUCTURE_CODE_1;
			field.mFieldControlField.dataTypeCode = DATA_TYPE_CODE_6;
			//field.mFieldControlField.auxiliaryControls init is "00"
			//field.mFieldControlField.printableGraphics init is ";&"
			memcpy(field.mFieldControlField.truncatedEscapeSequence, TRUNCATED_ESCAPE_SEQUENCE_Lexical_level_0, strlen(TRUNCATED_ESCAPE_SEQUENCE_Lexical_level_0));
		}
		mMetaDDRFields.push_back(field);
	}

	{//FOID
		DDRField field;
		field.mFieldTag = FOID;
		//field.mFieldLabel = "Feature Object Identifier";
		field.mFieldLabel = "Feature object identifier field";
		field.mSubfieldInfos.push_back(SubfieldInfo("Producing agency", "AGEN", "A(2)", "b12", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Feature identification number", "FIDN", "I(10)", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Feature identification subdivision", "FIDS", "I(5)", "b12", false));
		{//Field control field
			field.mFieldControlField.dataStructureCode = DATA_STRUCTURE_CODE_1;
			field.mFieldControlField.dataTypeCode = DATA_TYPE_CODE_6;
			//field.mFieldControlField.auxiliaryControls init is "00"
			//field.mFieldControlField.printableGraphics init is ";&"
			memcpy(field.mFieldControlField.truncatedEscapeSequence, TRUNCATED_ESCAPE_SEQUENCE_Lexical_level_0, strlen(TRUNCATED_ESCAPE_SEQUENCE_Lexical_level_0));
		}
		mMetaDDRFields.push_back(field);
	}

	{//ATTF
		DDRField field;
		field.mFieldTag = ATTF;
		//field.mFieldLabel = "Feature record attribute";
		field.mFieldLabel = "Feature record attribute field";
		field.mSubfieldInfos.push_back(SubfieldInfo("Attribute label/code", "*ATTL", "I(5)", "b12", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Attribute value", "ATVL", "A()", "", false));
		{//Field control field
			field.mFieldControlField.dataStructureCode = DATA_STRUCTURE_CODE_2;
			field.mFieldControlField.dataTypeCode = DATA_TYPE_CODE_6;
			//field.mFieldControlField.auxiliaryControls init is "00"
			//field.mFieldControlField.printableGraphics init is ";&"
			memcpy(field.mFieldControlField.truncatedEscapeSequence, TRUNCATED_ESCAPE_SEQUENCE_Lexical_level_1, strlen(TRUNCATED_ESCAPE_SEQUENCE_Lexical_level_1));
		}
		mMetaDDRFields.push_back(field);
	}


	{//NATF
		//ISO/IEC 10646
		DDRField field;
		field.mFieldTag = NATF;
		//field.mFieldLabel = "Feature record national attribute";
		field.mFieldLabel = "Feature record national attribute field";
		field.mSubfieldInfos.push_back(SubfieldInfo("Attribute label/code", "*ATTL", "I(5)", "b12", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Attribute value", "ATVL", "A()", "", false));
		{//Field control field
			field.mFieldControlField.dataStructureCode = DATA_STRUCTURE_CODE_2;
			field.mFieldControlField.dataTypeCode = DATA_TYPE_CODE_6;
			//field.mFieldControlField.auxiliaryControls init is "00"
			//field.mFieldControlField.printableGraphics init is ";&"
			memcpy(field.mFieldControlField.truncatedEscapeSequence, TRUNCATED_ESCAPE_SEQUENCE_Lexical_level_2, strlen(TRUNCATED_ESCAPE_SEQUENCE_Lexical_level_2));
		}
		mMetaDDRFields.push_back(field);
	}

	{//FFPC
		DDRField field;
		field.mFieldTag = FFPC;
		field.mFieldLabel = "Feature Record to Feature Object Pointer Control";
		field.mSubfieldInfos.push_back(SubfieldInfo("Feature object pointer update instruction", "FFUI", "A(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Feature object pointer index", "FFIX", "I()", "b12", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Number of feature object pointers", "NFPT", "I()", "b12", false));
		mMetaDDRFields.push_back(field);
	}

	{//FFPT
		DDRField field;
		field.mFieldTag = FFPT;
		//field.mFieldLabel = "Feature Record to Feature Object Pointer";
		field.mFieldLabel = "Feature record to feature object pointer field";
		field.mSubfieldInfos.push_back(SubfieldInfo("Long Name", "*LNAM", "A(17)", "B(64)", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Relationship indicator", "RIND", "A()", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Comment", "COMT", "A()", "", false));
		mMetaDDRFields.push_back(field);
	}

	{//FSPC
		DDRField field;
		field.mFieldTag = FSPC;
		//field.mFieldLabel = "Feature Record to Spatial Record Pointer Control";
		field.mFieldLabel = "Feature record to spatial record pointer field";
		field.mSubfieldInfos.push_back(SubfieldInfo("Feature to spatial record pointer update instruction", "FSUI", "A(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Feature to spatial record pointer index", "FSIX", "I()", "b12", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Number of feature to spatial record pointers", "NSPT", "I()", "b12", false));
		mMetaDDRFields.push_back(field);
	}

	{//FSPT
		DDRField field;
		field.mFieldTag = FSPT;
		//field.mFieldLabel = "Feature Record to Spatial Record Pointer";
		field.mFieldLabel = "Feature record to spatial record pointer field";
		field.mSubfieldInfos.push_back(SubfieldInfo("Name", "*NAME", "A(12)", "B(40)", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Orientation", "ORNT", "A(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Usage indicator", "USAG", "A(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Masking indicator", "MASK", "A(1)", "b11", false));
		mMetaDDRFields.push_back(field);
	}

	{//VRID
		DDRField field;
		field.mFieldTag = VRID;
		//field.mFieldLabel = "std::vector Record Identifier";
		field.mFieldLabel = "std::vector record identifier field";
		field.mSubfieldInfos.push_back(SubfieldInfo("Record name", "RCNM", "A(2)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Record identification number", "RCID", "I(10)", "b14", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Record version", "RVER", "I(3)", "b12", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Record update instruction", "RUIN", "A(1)", "b11", false));
		{//Field control field
			field.mFieldControlField.dataStructureCode = DATA_STRUCTURE_CODE_1;
			field.mFieldControlField.dataTypeCode = DATA_TYPE_CODE_6;
			//field.mFieldControlField.auxiliaryControls init is "00"
			//field.mFieldControlField.printableGraphics init is ";&"
			memcpy(field.mFieldControlField.truncatedEscapeSequence, TRUNCATED_ESCAPE_SEQUENCE_Lexical_level_0, strlen(TRUNCATED_ESCAPE_SEQUENCE_Lexical_level_0));
		}
		mMetaDDRFields.push_back(field);
	}

	{//ATTV
		//Latin1
		DDRField field;
		field.mFieldTag = ATTV;
		//field.mFieldLabel = "std::vector Record Attribute";
		field.mFieldLabel = "std::vector record attribute field";
		field.mSubfieldInfos.push_back(SubfieldInfo("Attribute label/code", "*ATTL", "I(5)", "b12", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Attribute value", "ATVL", "A()", "", false));
		{//Field control field
			field.mFieldControlField.dataStructureCode = DATA_STRUCTURE_CODE_2;
			field.mFieldControlField.dataTypeCode = DATA_TYPE_CODE_6;
			//field.mFieldControlField.auxiliaryControls init is "00"
			//field.mFieldControlField.printableGraphics init is ";&"
			memcpy(field.mFieldControlField.truncatedEscapeSequence, TRUNCATED_ESCAPE_SEQUENCE_Lexical_level_0, strlen(TRUNCATED_ESCAPE_SEQUENCE_Lexical_level_0));
		}
		mMetaDDRFields.push_back(field);
	}


	{//VRPC
		DDRField field;
		field.mFieldTag = VRPC;
		field.mFieldLabel = "std::vector Record Pointer Control";
		field.mSubfieldInfos.push_back(SubfieldInfo("std::vector record pointer update instruction", "VPUI", "A(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("std::vector record pointer index", "VPIX", "I()", "b12", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Number of std::vector record pointers", "NVPT", "I()", "b12", false));
		mMetaDDRFields.push_back(field);
	}

	{//VRPT
		DDRField field;
		field.mFieldTag = VRPT;
		//field.mFieldLabel = "std::vector Record Pointer";
		field.mFieldLabel = "std::vector record pointer field";
		field.mSubfieldInfos.push_back(SubfieldInfo("Name", "*NAME", "A(12)", "B(40)", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Orientation", "ORNT", "A(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Usage indicator", "USAG", "A(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Topology indicator", "TOPI", "A(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Masking indicator", "MASK", "A(1)", "b11", false));
		mMetaDDRFields.push_back(field);
	}

	{//SGCC
		DDRField field;
		field.mFieldTag = SGCC;
		field.mFieldLabel = "Coordinate control";
		field.mSubfieldInfos.push_back(SubfieldInfo("Coordinate update instruction", "CCUI", "A(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Coordinate index", "CCIX", "I()", "b12", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Number of coordinates", "CCNC", "I()", "b12", false));
		mMetaDDRFields.push_back(field);
	}

	{//SG2D
		DDRField field;
		field.mFieldTag = SG2D;
		//field.mFieldLabel = "2-D Coordinate";
		field.mFieldLabel = "2-D Coordinate field";
		field.mSubfieldInfos.push_back(SubfieldInfo("Coordinate in Y axis", "*YCOO", "R()", "b24", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Coordinate in X axis", "XCOO", "R()", "b24", false));
		{//Field control field
			field.mFieldControlField.dataStructureCode = DATA_STRUCTURE_CODE_2;
			field.mFieldControlField.dataTypeCode = DATA_TYPE_CODE_5;
			//field.mFieldControlField.auxiliaryControls init is "00"
			//field.mFieldControlField.printableGraphics init is ";&"
			//field.mFieldControlField.truncatedEscapeSequence init is "   "
		}
		mMetaDDRFields.push_back(field);
	}

	{//SG3D
		DDRField field;
		field.mFieldTag = SG3D;
		//field.mFieldLabel = "3-D Coordinate (Sounding Array)";
		field.mFieldLabel = "3-D Coordinate field";
		field.mSubfieldInfos.push_back(SubfieldInfo("Coordinate in Y axis", "*YCOO", "R()", "b24", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Coordinate in X axis", "XCOO", "R()", "b24", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("3-D (sounding) value", "VE3D", "R()", "b24", false));
		{//Field control field
			field.mFieldControlField.dataStructureCode = DATA_STRUCTURE_CODE_2;
			field.mFieldControlField.dataTypeCode = DATA_TYPE_CODE_5;
			//field.mFieldControlField.auxiliaryControls init is "00"
			//field.mFieldControlField.printableGraphics init is ";&"
			//field.mFieldControlField.truncatedEscapeSequence init is "   "
		}
		mMetaDDRFields.push_back(field);
	}

	{//ARCC
		DDRField field;
		field.mFieldTag = ARCC;
		field.mFieldLabel = "Arc/Curve definition";
		field.mSubfieldInfos.push_back(SubfieldInfo("Arc/Curve type", "ATYP", "A(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Construction surface", "SURF", "A(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Curve order", "ORDR", "I(1)", "b11", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Interpolated point resolution", "RESO", "R()", "b14*)", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Floating point multiplication factor", "FPMF", "I()", "b14", false));
		mMetaDDRFields.push_back(field);
	}

	{//AR2D
		DDRField field;
		field.mFieldTag = AR2D;
		field.mFieldLabel = "Arc coordinate";
		field.mSubfieldInfos.push_back(SubfieldInfo("Start point", "STPT", "@", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Centre point", "CTPT", "@", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("End point", "ENPT", "@", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Coordinate in Y axis", "*YCOO", "R()", "b24", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Coordinate in X axis", "XCOO", "R()", "b24", false));
		mMetaDDRFields.push_back(field);
	}

	{//EL2D
		DDRField field;
		field.mFieldTag = EL2D;
		field.mFieldLabel = "Ellipse coordinates";
		field.mSubfieldInfos.push_back(SubfieldInfo("Start point", "STPT", "@", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Centre point", "CTPT", "@", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("End point", "ENPT", "@", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Conjugate diameter point major axis", "CDPM", "@", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Conjugate diameter point minor axis", "CDPR", "@", "", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Coordinate in Y axis", "*YCOO", "R()", "b24", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Coordinate in X axis", "XCOO", "R()", "b24", false));
		mMetaDDRFields.push_back(field);
	}

	{//CT2D
		DDRField field;
		field.mFieldTag = CT2D;
		field.mFieldLabel = "Curve Coordinates";
		field.mSubfieldInfos.push_back(SubfieldInfo("Coordinate in Y axis", "*YCOO", "R()", "b24", false));
		field.mSubfieldInfos.push_back(SubfieldInfo("Coordinate in X axis", "XCOO", "R()", "b24", false));
		mMetaDDRFields.push_back(field);
	}

	std::set<std::string> types;
	for (auto it = mMetaDDRFields.begin(); it != mMetaDDRFields.end(); it++)
	{
		mMetaDDRFieldsTable[it->mFieldTag] = *it;
		//
		for (auto itSubfieldInfo = it->mSubfieldInfos.begin(); itSubfieldInfo != it->mSubfieldInfos.end(); itSubfieldInfo++)
		{
			std::string bin = itSubfieldInfo->binFormat;
			std::string asc = itSubfieldInfo->ascFormat;
			types.insert(bin);
			types.insert(asc);
		}
	}
	//for (auto it = types.begin(); it != types.end(); it++)
	//{
	//	ecdisLog(it->c_str());
	//}


}

void S57DataSet::initMetaUpdate()
{

}

void S57DataSet::initMeta000()
{
	//下面是常用的字段结构,以便使用最简单的结构生成000
	//Field Tag,Parent Field Tag
	std::vector<std::tuple<std::string, std::string>> dictBuffer;
	/*
			Data Set General Information record
				|
				|--0001 (1) ISO/IEC 8211 Record Identifier
					|
					|--DSID (16) - Data Set Identification field
						|
						|--DSSI (11) - Data Set Structure information field
			*/
	dictBuffer.push_back(std::tuple<std::string, std::string>(ISO8211_0001, ""));
	dictBuffer.push_back(std::tuple<std::string, std::string>(DSID, ISO8211_0001));
	dictBuffer.push_back(std::tuple<std::string, std::string>(DSSI, DSID));
	/*
			Data Set Geographic Reference record
				|
				|--0001 (1) ISO/IEC 8211 Record Identifier
					|
					|--DSPM (12) Data Set Parameter field
			*/
	dictBuffer.push_back(std::tuple<std::string, std::string>(DSPM, ISO8211_0001));
	/*
			Feature record
				|
				|--0001 (1) - ISO/IEC 8211 Record Identifier
					|
					|--FRID(7) - Feature record identifier field
						|
						|--FOID(3) - Feature object identifier field
						|
						|--<R>--ATTF(*2) - Feature record attribute field
						|
						|--<R>--NATF(*2) - Feature record national attribute field
						|
						|--FFPC(3) - Feature record to feature object pointer control field
						|
						|--<R>--FFPT(*3) - Feature record to feature object pointer field
						|
						|--FSPC(3) - Feature record to spatial record pointer control field
						|
						|--<R>--FSPT(*4) - Feature record to spatial record pointer field
			*/
	dictBuffer.push_back(std::tuple<std::string, std::string>(FRID, ISO8211_0001));
	dictBuffer.push_back(std::tuple<std::string, std::string>(FOID, FRID));
	dictBuffer.push_back(std::tuple<std::string, std::string>(ATTF, FRID));
	dictBuffer.push_back(std::tuple<std::string, std::string>(NATF, FRID));
	dictBuffer.push_back(std::tuple<std::string, std::string>(FFPT, FRID));
	dictBuffer.push_back(std::tuple<std::string, std::string>(FSPT, FRID));
	/*
			std::vector record
					|
					|--0001 (1) - ISO/IEC 8211 Record Identifier
						|
						|--VRID (4) - std::vector Record Identifier field
							|
							|--<R>--ATTV (*2) - std::vector Record Attribute field
							|
							|--VRPC (3) - std::vector Record Pointer Control field
							|
							|--<R>--VRPT (*5) - std::vector Record Pointer field
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
	dictBuffer.push_back(std::tuple<std::string, std::string>(VRID, ISO8211_0001));
	dictBuffer.push_back(std::tuple<std::string, std::string>(ATTV, VRID));
	dictBuffer.push_back(std::tuple<std::string, std::string>(VRPT, VRID));
	dictBuffer.push_back(std::tuple<std::string, std::string>(SG2D, VRID));
	dictBuffer.push_back(std::tuple<std::string, std::string>(SG3D, VRID));
	//
	for (auto it = dictBuffer.begin(); it != dictBuffer.end(); it++)
	{
		std::string fieldTag = std::get<0>(*it);
		std::string fieldTagParent = std::get<1>(*it);
		auto itFind = mMetaDDRDirectory.find(fieldTagParent);
		if (itFind == mMetaDDRDirectory.end())
		{
			std::vector<std::string> list;
			list.push_back(fieldTag);
			mMetaDDRDirectory[fieldTagParent] = list;
		}
		else
		{
			itFind->second.push_back(fieldTag);
		}
		mMetaDDRDirectoryParent[fieldTag] = fieldTagParent;
	}
	auto voidFunc = [&](const std::string& fieldTag)->void {};
	ddrDirectoryFunction("", "", voidFunc, true, 0);//调试信息
}

void S57DataSet::CreateCodeOutput()
{
	S57DataSet ds;
	ds.initMeta();
	ds.initMeta000();
	std::string metaStructs;
	std::string metaCreateInterface;
	std::string metaCreateIntrefaceImplements;
	auto strCode = ds.createCode(&metaStructs, &metaCreateInterface, &metaCreateIntrefaceImplements);//生成元数据的代码
	Utils::log(strCode.c_str());
}

std::string S57DataSet::createCode(std::string* metaStructs, std::string* metaCreateInterfaces, std::string* metaCreateInterfaceImplements)
{
	std::string strRet;

	std::stringstream strMetaStructs;//结构体代码
	std::stringstream strCreateInterfaces;//DS创建Record的接口代码
	std::stringstream strCreateInterfaceImplements;//DS创建Record的接口实现代码
	std::stringstream strISO8211ConvertToS57Dataset;
	strISO8211ConvertToS57Dataset << "=========================================================================================" << std::endl;
	int itIndex = 0;
	for (auto it = mMetaDDRFields.begin(); it != mMetaDDRFields.end(); it++, itIndex++)
	{
		//strMetaStruct
		{
			bool isValid = true;
			if (it->mFieldTag == "FRID")
			{
				bool debug = true;
			}

			std::stringstream strMetaStruct;
			strMetaStruct << "//" << it->mFieldLabel << std::endl;
			strMetaStruct << "struct " << it->mFieldTag << "Record" << std::endl;
			strMetaStruct << "{" << std::endl;
			for (auto itSubfieldInfo = it->mSubfieldInfos.begin(); itSubfieldInfo != it->mSubfieldInfos.end(); itSubfieldInfo++)
			{
				std::string fieldTag = itSubfieldInfo->fieldTag;
				std::string ascType = itSubfieldInfo->ascFormat;
				std::string binType = itSubfieldInfo->binFormat;
				std::string member;
				std::string type = GetType(ascType, binType);
				if (fieldTag.empty() || type == "@")
				{
					isValid = false;
					break;
				}
				DataFormat dataFormat = DataFormat::Parse(type);
				auto sFieldTag = Utils::leftTrim(fieldTag, "*");

				if (dataFormat.emDataFormatType == DataFormatType::ASC)
				{
					std::stringstream sMember;
					if (dataFormat.emFlag != ISO8211_FLAG::UT)
					{
						sMember << "A";
						sMember << "_ ";//fix
						sMember << sFieldTag;
						sMember << "[";
						sMember << int(dataFormat.Width);
						sMember << "]";
					}
					else
					{
						sMember << "A";
						sMember << " ";
						sMember << sFieldTag;
					}
					sMember << " ";
					member = sMember.str();
				}
				else if (dataFormat.emDataFormatType == DataFormatType::UByte || dataFormat.emDataFormatType == DataFormatType::SByte)
				{
					std::string strType = Utils::rightTrim(type, "*)");
					member += strType;
					member += " ";
					member += sFieldTag;
				}
				else if (dataFormat.emDataFormatType == DataFormatType::Bin)
				{
					std::stringstream sMember;
					if (dataFormat.emFlag != ISO8211_FLAG::UT)
					{
						sMember << "B";
						sMember << "_ ";//fix
						sMember << sFieldTag;
						sMember << "[";
						sMember << int(dataFormat.Width / 8);
						sMember << "]";
					}
					else
					{
						sMember << "B";
						sMember << " ";
						sMember << sFieldTag;
					}
					sMember << " ";
					member = sMember.str();
				}
				else if (dataFormat.emDataFormatType == DataFormatType::Real)
				{
					std::stringstream sMember;
					if (dataFormat.emFlag != ISO8211_FLAG::UT)
					{
						sMember << "R";
						sMember << "_ ";//fix
						sMember << sFieldTag;
						sMember << "[";
						sMember << int(dataFormat.Width);
						sMember << "]";
					}
					else
					{
						sMember << "R";
						sMember << " ";
						sMember << sFieldTag;
					}
					sMember << " ";
					member = sMember.str();

				}
				else if (dataFormat.emDataFormatType == DataFormatType::INT)
				{
					std::stringstream sMember;
					if (dataFormat.emFlag != ISO8211_FLAG::UT)
					{
						sMember << "I";
						sMember << "_ ";//fix
						sMember << sFieldTag;
						sMember << "[";
						sMember << int(dataFormat.Width);
						sMember << "]";
					}
					else
					{
						sMember << "I";
						sMember << " ";
						sMember << sFieldTag;
					}
					sMember << " ";
					member = sMember.str();

				}
				else
				{
					assert(false);
				}


				strMetaStruct << "\t" << member << ";" << std::endl;
			}

			strMetaStruct << "};" << std::endl;
			if (isValid)
			{
				strMetaStructs << strMetaStruct.str();
			}
		}
		//strCreateInterfaces
		{
			if (it->mFieldTag != "0000" && it->mFieldTag != "0001")
			{
				//
				std::string fieldTagRep;
				bool isRep = false;
				{
					if (it->mSubfieldInfos.size() > 0)
					{
						auto item = it->mSubfieldInfos.at(0);
						std::string fieldTag = item.fieldTag;
						if (fieldTag.size() > 0 && fieldTag[0] == '*')
						{
							fieldTagRep = fieldTag;
							isRep = true;
						}
					}
				}

				if (isRep)
				{


					std::stringstream strCreateInterface;
					strCreateInterface << "\tDR create" << it->mFieldTag << "(const std::vector<" << it->mFieldTag << "Record>&records);" << std::endl;
					strCreateInterfaces << strCreateInterface.str();
				}
				else
				{
					std::stringstream strCreateInterface;
					strCreateInterface << "\tDR create" << it->mFieldTag << "(const " << it->mFieldTag << "Record& record);" << std::endl;
					strCreateInterfaces << strCreateInterface.str();
				}
			}
		}
		//strCreateInterfaceImplements
		{

			//
			std::string fieldTagRep;
			bool isRep = false;
			{
				if (it->mSubfieldInfos.size() > 0)
				{
					auto item = it->mSubfieldInfos.at(0);
					std::string fieldTag = item.fieldTag;
					if (fieldTag.size() > 0 && fieldTag[0] == '*')
					{
						fieldTagRep = fieldTag;
						isRep = true;
					}
				}
			}

			if (isRep)
			{
				std::stringstream strMetaCreateInterfaceImplement;

				bool isValid = true;

				strMetaCreateInterfaceImplement << "DR S57DataSet::create" << it->mFieldTag << "(const std::vector<" << it->mFieldTag << "Record>& records)" << std::endl;
				strMetaCreateInterfaceImplement << "{" << std::endl;
				strMetaCreateInterfaceImplement << "\tDR dr;" << std::endl;
				//strMetaCreateInterfaceImplement << "\tdr.drDirectory.fieldTag = \"" << it->mFieldTag << "\";" << endl;
				strMetaCreateInterfaceImplement << "\tdr.setFieldTag(" << it->mFieldTag << ");" << std::endl;
				strMetaCreateInterfaceImplement << "\tDRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);" << std::endl;
				strMetaCreateInterfaceImplement << "\tstd::vector<TypeValue> typeValues;" << std::endl;
				strMetaCreateInterfaceImplement << "\tfor (auto itRecord = records.begin(); itRecord!=records.end(); itRecord++)" << std::endl;
				strMetaCreateInterfaceImplement << "\t{" << std::endl;
				for (auto itSubfieldInfo = it->mSubfieldInfos.begin(); itSubfieldInfo != it->mSubfieldInfos.end(); itSubfieldInfo++)
				{
					auto item = *itSubfieldInfo;
					std::string fieldTag = item.fieldTag;
					std::string ascType = item.ascFormat;
					std::string binType = item.binFormat;
					std::string type = GetType(ascType, binType);
					if (fieldTag.empty() || type == "@")
					{
						isValid = false;
						break;
					}
					auto sFieldTag = Utils::leftTrim(fieldTag, "*");


					DataFormat dataFormat = DataFormat::Parse(type);
					if (dataFormat.emDataFormatType == DataFormatType::Bin
						|| dataFormat.emDataFormatType == DataFormatType::INT
						|| dataFormat.emDataFormatType == DataFormatType::ASC
						|| dataFormat.emDataFormatType == DataFormatType::Real
						)
					{
						int width = -1;
						if (dataFormat.Width > 0)
						{
							width = dataFormat.Width / 8;
						}
						strMetaCreateInterfaceImplement << "\t\t{TypeValue typeValue(\"" << type << "\", itRecord->" << sFieldTag << "," << width << "); typeValues.push_back(typeValue); }" << std::endl;
					}
					else
					{
						strMetaCreateInterfaceImplement << "\t\t{TypeValue typeValue(\"" << type << "\", itRecord->" << sFieldTag << "); typeValues.push_back(typeValue); }" << std::endl;
					}


				}
				strMetaCreateInterfaceImplement << "\t}" << std::endl;
				strMetaCreateInterfaceImplement << "\twriteToDRRecords(drFieldArea,\"" << fieldTagRep << "\", typeValues);" << std::endl;
				strMetaCreateInterfaceImplement << "\tdr.drFieldArea = drFieldArea;" << std::endl;
				strMetaCreateInterfaceImplement << "\treturn dr;" << std::endl;
				strMetaCreateInterfaceImplement << "}" << std::endl << std::endl;

				if (isValid)
				{
					strCreateInterfaceImplements << strMetaCreateInterfaceImplement.str();
				}
			}
			else
			{
				bool isValid = true;

				std::stringstream strMetaCreateInterfaceImplement;
				strMetaCreateInterfaceImplement << "DR S57DataSet::create" << it->mFieldTag << "(const " << it->mFieldTag << "Record& record)" << std::endl;
				strMetaCreateInterfaceImplement << "{" << std::endl;
				strMetaCreateInterfaceImplement << "\tDR dr;" << std::endl;
				//strMetaCreateInterfaceImplement << "\tdr.drDirectory.fieldTag = \"" << it->mFieldTag << "\";" << endl;
				strMetaCreateInterfaceImplement << "\tdr.setFieldTag(" << it->mFieldTag << ");" << std::endl;
				strMetaCreateInterfaceImplement << "\tDRFieldArea drFieldArea = createDRFieldArea(dr.drDirectory.fieldTag);" << std::endl;



				for (auto itSubfieldInfo = it->mSubfieldInfos.begin(); itSubfieldInfo != it->mSubfieldInfos.end(); itSubfieldInfo++)
				{
					std::string fieldTag = itSubfieldInfo->fieldTag;
					std::string ascType = itSubfieldInfo->ascFormat;
					std::string binType = itSubfieldInfo->binFormat;
					std::string member;
					std::string type = GetType(ascType, binType);
					if (fieldTag.empty() || type == "@")
					{
						isValid = false;
						break;
					}
					DataFormat dataFormat = DataFormat::Parse(type);
					auto sFieldTag = Utils::leftTrim(fieldTag, "*");
					//writeToDRRecords(drFieldArea, "RCNM", record.RCNM);
					if ((
						dataFormat.emDataFormatType == DataFormatType::INT
						|| dataFormat.emDataFormatType == DataFormatType::ASC
						|| dataFormat.emDataFormatType == DataFormatType::Real
						|| dataFormat.emDataFormatType == DataFormatType::Bin
						)
						&& dataFormat.emFlag != ISO8211_FLAG::UT && dataFormat.Width > 0)
					{
						int width = dataFormat.Width;
						if (dataFormat.emDataFormatType == DataFormatType::Bin)
						{
							width /= 8;
						}
						strMetaCreateInterfaceImplement << "\twriteToDRRecords(drFieldArea, \"" << fieldTag << "\", record." << sFieldTag << "," << width << ");" << std::endl;
					}
					else
					{
						strMetaCreateInterfaceImplement << "\twriteToDRRecords(drFieldArea, \"" << fieldTag << "\", record." << sFieldTag << ");" << std::endl;
					}
				}

				strMetaCreateInterfaceImplement << "\tdr.drFieldArea = drFieldArea;" << std::endl;
				strMetaCreateInterfaceImplement << "\treturn dr;" << std::endl;
				strMetaCreateInterfaceImplement << "}" << std::endl << std::endl;
				if (isValid)
				{
					strCreateInterfaceImplements << strMetaCreateInterfaceImplement.str();
				}
			}
		}
		//strISO8211ConvertToS57Dataset
		if (it->mFieldTag.empty() == false)
		{
			bool isValid = true;


			if (itIndex == 0)
			{
				strISO8211ConvertToS57Dataset << "if(" << it->mFieldTag << "==tag){" << std::endl;
			}
			else
			{
				strISO8211ConvertToS57Dataset << "} else if(" << it->mFieldTag << "==tag){" << std::endl;
			}

			strISO8211ConvertToS57Dataset << "\t for(auto itDRRecord=dr.drFieldArea.drRecords.begin();itDRRecord!=dr.drFieldArea.drRecords.end();itDRRecord++)\r\n"
				"\t{\r\n"
				"\t\t std::string drRecordFieldTag=itDRRecord->fieldTag;\r\n"
				"\t\t DRFieldArea::createSubfield(*itDRRecord,subfield);\r\n";
			int itSubfieldInfoIndex = 0;
			for (auto itSubfieldInfo = it->mSubfieldInfos.begin(); itSubfieldInfo != it->mSubfieldInfos.end(); itSubfieldInfo++, itSubfieldInfoIndex++)
			{
				auto item = *itSubfieldInfo;
				std::string fieldTag = item.fieldTag;
				std::string ascType = item.ascFormat;
				std::string binType = item.binFormat;
				std::string type = GetType(ascType, binType);
				if (fieldTag.empty() || type == "@")
				{
					isValid = false;
					break;
				}
				auto sFieldTag = Utils::leftTrim(fieldTag, "*");

				strISO8211ConvertToS57Dataset << "\t";

				std::string parentTag = it->mFieldTag;
				std::transform(parentTag.begin(), parentTag.end(), parentTag.begin(), ::toupper);


				std::string strFormat1;
				std::string strFormat2;
				if (itSubfieldInfoIndex == 0)
				{
					strFormat1 = "if(\"%s\"==drRecordFieldTag){\n"
						"\t\t m%s.%s=%s \n"
						"\t}";
					strFormat2 = "if(\"%s\"==drRecordFieldTag){\n"
						"\t\t %s\n"
						"\t}";
				}
				else
				{
					strFormat1 = "else if(\"%s\"==drRecordFieldTag){\n"
						"\t\t m%s.%s=%s \n"
						"\t}";
					strFormat2 = "else if(\"%s\"==drRecordFieldTag){\n"
						"\t\t %s\n"
						"\t}";
				}
				//typedef bool b1;
				//typedef unsigned char b11;
				//typedef unsigned short b12;
				//typedef unsigned int b14;

				//typedef short b2;
				//typedef int b24;
				//typedef float b3, b34;

				////ASC
				////变长
				//typedef char* A;
				//typedef char* B;
				//typedef char* R;
				//typedef char* I;
				////固定
				//typedef char A_;
				//typedef char B_;
				//typedef char R_;
				//typedef char I_;
				std::string strConvert;
				bool isAsc = false;
				if ("b1" == type) { strConvert = "subfield.uint8Value();"; }
				else if (Utils::startWith(type, "b11")) { strConvert = "subfield.uint8Value();"; }
				else if (Utils::startWith(type, "b12")) { strConvert = "subfield.uint16Value();"; }
				else if (Utils::startWith(type, "b14")) { strConvert = "subfield.uint32Value();"; }
				else if (Utils::startWith(type, "b2")) { strConvert = "subfield.int16Value();"; }
				else if (Utils::startWith(type, "b24")) { strConvert = "subfield.int32Value();"; }
				else if (Utils::startWith(type, "b3")) { strConvert = "subfield.floatValue();"; }
				else if (Utils::startWith(type, "b34")) { strConvert = "subfield.floatValue();"; }
				else if ("A()" == type || "B()" == type || "R()" == type || "I()" == type)
				{
					std::string script = "auto str_=subfield.toString();\r\n"
						"\t\t m%s.%s=new char[str_.size()+1];\r\n"
						"\t\t memset(m%s.%s,0,str_.size()+1);\r\n"
						"\t\t memcpy(m%s.%s,str_.data(),str_.size());";
					strConvert = Utils::formatString(script.c_str()
						, parentTag.c_str(), fieldTag.c_str()
						, parentTag.c_str(), fieldTag.c_str()
						, parentTag.c_str(), fieldTag.c_str());
					isAsc = true;
				}
				else if (Utils::startWith(type, "A(")
					|| Utils::startWith(type, "B(")
					|| Utils::startWith(type, "R(")
					|| Utils::startWith(type, "I("))
				{

					std::string script = "auto str_=subfield.toString();\r\n"
						"\t\t memset(m%s.%s,0,sizeof(m%s.%s));\r\n"
						"\t\t memcpy(m%s.%s,str_.data(),str_.size());";
					strConvert = Utils::formatString(script.c_str()
						, parentTag.c_str(), fieldTag.c_str(), parentTag.c_str(), fieldTag.c_str()
						, parentTag.c_str(), fieldTag.c_str());
					isAsc = true;
				}
				else
				{
					assert(false);//未适配类型
				}


				std::string strDRRecord;
				if (isAsc == false)
				{
					strDRRecord = Utils::formatString(strFormat1.c_str()
						, fieldTag.c_str(), parentTag.c_str(), fieldTag.c_str(), strConvert.c_str());
				}
				else
				{
					strDRRecord = Utils::formatString(strFormat2.c_str()
						, fieldTag.c_str(), strConvert.c_str());
				}


				strISO8211ConvertToS57Dataset << strDRRecord << std::endl;





			}
			strISO8211ConvertToS57Dataset << "\t}";
			if (mMetaDDRFields.size() == (itIndex + 1))
			{
				strISO8211ConvertToS57Dataset << std::endl << "}" << std::endl;
			}
		}
	}

	strRet += "save to s57meta.h\n";
	strRet += strMetaStructs.str();
	strRet += "\n\n\n\n";

	strRet += "save to s57.h\n";
	strRet += strCreateInterfaces.str();
	strRet += "\n\n\n\n";

	strRet += "save to s57meta.cpp\n";
	strRet += strCreateInterfaceImplements.str();
	strRet += "\n\n\n\n";


	if (metaStructs)
	{
		*metaStructs = strMetaStructs.str();
	}
	if (metaCreateInterfaces)
	{
		*metaCreateInterfaces = strCreateInterfaces.str();
	}
	if (metaCreateInterfaceImplements)
	{
		*metaCreateInterfaceImplements = strCreateInterfaceImplements.str();
	}

	auto s = strISO8211ConvertToS57Dataset.str();
	Utils::log(s);
	return strRet;
}

#ifdef WIN32
#define OS_CODEC "UTF-8"
#else
#define OS_CODEC "UTF-8"
#endif

int S57DataSet::textBytesLength(const char* str, int charSize)
{
	int iTextLength = 0;
	switch (charSize)
	{
	case 1:
		do
		{
			char ch = str[iTextLength];
			if (ch == char(ISO8211_FLAG::UT)
				|| ch == char(ISO8211_FLAG::FT))
			{
				break;
			}
			iTextLength += 1;
		} while (true);
		break;
	case 2:
		do
		{
			uint16 ch = *(uint16*)&str[iTextLength];
			if (ch == char(ISO8211_FLAG::UT)
				|| ch == char(ISO8211_FLAG::FT))
			{
				break;
			}
			iTextLength += 2;
		} while (true);
		break;
	default:
		assert(false);//传参错误
	}
	return iTextLength;
}

int S57DataSet::charSize(bool isNATF)
{
	int iCharSize = 1;
	if (isNATF)
	{
		if (mNALL == 2)//ENC is using UCS-2 / UTF-16 encoding
		{
			iCharSize = 2;
		}
		else if (mNALL == 1)// ENC is using Lex level 1 (ISO 8859_1) encoding
		{

		}
		else
		{
			if (mAALL == 1)// ENC is using Lex level 1 (ISO 8859_1) encoding for "General Text"
			{

			}
		}
	}
	return iCharSize;
}

std::string S57DataSet::textValue(bool isNATF, const char* text, int textBytesLength)
{

	std::vector<char> bytes;
	if (isNATF)
	{
		switch (mNALL)
		{
		case 2:
		{
			Utils::encodeConvert("UTF-16", text, textBytesLength, OS_CODEC, bytes);
		}
		break;
		case 1:
		{
			Utils::encodeConvert("ISO-8859-1", text, textBytesLength, OS_CODEC, bytes);
		}
		break;
		}

	}
	else
	{
		if (mAALL == 1)// ENC is using Lex level 1 (ISO 8859_1) encoding for "General Text"
		{
			Utils::encodeConvert("ISO-8859-1", text, textBytesLength, OS_CODEC, bytes);
		}
		else
		{
			bytes.clear();
			if (textBytesLength > 0)
			{
				bytes.resize(textBytesLength);
				memcpy(&bytes[0], text, textBytesLength);
			}
		}
	}
	std::string strRet(bytes.begin(), bytes.end());
	return std::move(strRet);
}

void S57DataSet::Sample()
{

	{
		S57DataSet ds;

		///*
		ds.initMeta();
		ds.initMeta000();

		//DSID
		DSIDRecord dsid;
		{
			memset(&dsid, 0, sizeof(dsid));
			dsid.RCNM = 10;
			dsid.RCID = 1;
			dsid.EXPP = 1;
			dsid.INTU = 3;
			dsid.DSNM = "mid         ";
			dsid.EDTN = "8";
			dsid.UPDN = "0";
			memcpy(dsid.UADT, "20160215", 8);
			memcpy(dsid.ISDT, "20160215", 8);
			memcpy(dsid.STED, "03.1", 4);
			dsid.PRSP = 1;
			dsid.PSDN = "";
			dsid.PRED = "2.0";
			dsid.PROF = 1;
			dsid.AGEN = 70;
			dsid.COMT = "China MSA(t#WzNDWX>bm;>>lo9c8bk<bc>nlnoj>8cbk<>;o;)";
			auto drDSID = ds.createDSID(dsid);

			{//DSSI
				DSSIRecord dssi;
				dssi.DSTR = 2;
				dssi.AALL = 1;
				dssi.NALL = 2;
				dssi.NOMR = 5;
				dssi.NOCR = 0;
				dssi.NOGR = 808;
				dssi.NOLR = 8;
				dssi.NOIN = 255;
				dssi.NOCN = 568;
				dssi.NOED = 686;
				dssi.NOFA = 0;
				auto drDSSI = ds.createDSSI(dssi);
				drDSID.drs.push_back(drDSSI);
			}

			ds.mDRs.push_back(drDSID);
		}

		DSPMRecord dspm;
		{
			memset(&dspm, 0, sizeof(dspm));
			dspm.RCNM = 20;
			dspm.RCID = 2;
			dspm.HDAT = 2;
			dspm.VDAT = 3;
			dspm.SDAT = 23;
			dspm.CSCL = 8000000;
			dspm.DUNI = 1;
			dspm.HUNI = 1;
			dspm.PUNI = 1;
			dspm.COUN = 1;
			dspm.COMF = 10000000;
			dspm.SOMF = 10;
			dspm.COMT = "LUANHEKOU TO CAOFEIDIAN";
			auto drDSPM = ds.createDSPM(dspm);
			ds.mDRs.push_back(drDSPM);
		}


		{//std::vector
			VRIDRecord vrid;
			memset(&vrid, 0, sizeof(vrid));
			vrid.RCNM = RCNM_VC;
			vrid.RCID = 1;
			vrid.RVER = 1;
			vrid.RUIN = 1;
			auto drVRID = ds.createVRID(vrid);
			//
			{
				std::vector<SG2DRecord> records;
				int xys[] =
				{
					411000000,
					1046660000
				};//y,x....
				int xysCount = sizeof(xys) / sizeof(int);
				for (int i = 0; i < xysCount; i += 2)
				{
					int x = xys[i + 1];
					int y = xys[i];
					SG2DRecord record;
					record.XCOO = x;
					record.YCOO = y;
					records.push_back(record);
				}

				auto drSG2D = ds.createSG2D(records);
				drVRID.drs.push_back(drSG2D);
			}
			ds.mDRs.push_back(drVRID);

		}

		{//std::vector
			VRIDRecord vrid;
			memset(&vrid, 0, sizeof(vrid));
			vrid.RCNM = RCNM_VE;//边
			vrid.RCID = 2;
			vrid.RVER = 1;
			vrid.RUIN = 1;
			auto drVRID = ds.createVRID(vrid);
			//VRPT
			{
				int values[] =
				{
					0,255,255,1,255,
					0,255,255,2,255
				};
				std::vector<VRPTRecord> records;
				int valuesCount = sizeof(values) / sizeof(int);
				for (int i = 0; i < valuesCount; i += 5)
				{
					VRPTRecord record;
					memset(record.NAME, 0, 5);
					SetName(RCNM_VC, 1, record.NAME);//关键
					record.ORNT = values[i + 1];
					record.USAG = values[i + 2];
					record.TOPI = values[i + 3];
					record.MASK = values[i + 4];
					records.push_back(record);
				}

				auto drVRPT = ds.createVRPT(records);
				drVRID.drs.push_back(drVRPT);
			}
			//SG2D
			{
				std::vector<SG2DRecord> records;
				int xys[] = {
					411000000
					, 1046660000
					, 411000000
					, 1153000000
					, 247000000
					, 1153000000
					, 247000000
					, 1046600000
					, 411000000
					, 1046660000 };//y,x....

				int xysCount = sizeof(xys) / sizeof(int);
				for (int i = 0; i < xysCount; i += 2)
				{
					int x = xys[i + 1];
					int y = xys[i];
					SG2DRecord record;
					record.XCOO = x;
					record.YCOO = y;
					records.push_back(record);
				}

				auto drSG2D = ds.createSG2D(records);
				drVRID.drs.push_back(drSG2D);
			}
			ds.mDRs.push_back(drVRID);
			//
		}

		{//Feature
			FRIDRecord frid;
			frid.RCNM = 100;
			frid.RCID = 2;
			frid.PRIM = 3;
			frid.GRUP = 2;
			frid.OBJL = 71;
			frid.RVER = 0;
			frid.RUIN = 1;
			auto drFRID = ds.createFRID(frid);
			{//FSPT
				FSPTRecord fspt;
				memset(&fspt, 0, sizeof(fspt));
				S57DataSet::SetName(RCNM_VE, 2, fspt.NAME);//关键,RCID为2的边
				fspt.ORNT = 1;
				fspt.USAG = 1;
				fspt.MASK = 2;
				std::vector<FSPTRecord> records;
				records.push_back(fspt);
				auto drFSPT = ds.createFSPT(records);
				drFRID.drs.push_back(drFSPT);
			}

			//{//ATTF
			//	ECDIS::S57::ATTFRecord attf;
			//	attf.ATTL = 116;//OBJNAM
			//	attf.ATVL = "test";
			//	std::vector<ATTFRecord> records;
			//	records.push_back(attf);
			//	auto drATTF = ds.createATTF(records);
			//	drFRID.drs.push_back(drATTF);
			//}
			ds.mDRs.push_back(drFRID);
		}








		ds.save(R"(d:\test\Test0001.000)");
		ds.load(R"(d:\test\Test0001.000)");
		//*/

	}
}

S57Spatial* S57DataSet::getSpatial(unsigned char rcnm, ulong rcid)
{
	S57Spatial* spatial = nullptr;
	std::tuple<unsigned char, ulong> key(rcnm, rcid);
	auto itFind = mS57SpatialsBuffer.find(key);
	if (itFind != mS57SpatialsBuffer.end())
	{
		spatial = itFind->second;
	}
	return spatial;
}

bool S57DataSet::getSG2DPoint(S57Spatial* spatial, double& x, double& y)
{
	bool bRet = false;
	if (spatial)
	{
		if (spatial->mSG2Ds.size() > 0)
		{
			SG2DRecord& sg2d = spatial->mSG2Ds[0];
			x = Utils::div(sg2d.XCOO, mDSPM.COMF);
			y = Utils::div(sg2d.YCOO, mDSPM.COMF);
			bRet = true;
		}
	}
	return bRet;
}

bool isConnectedEdge(const S57Ring2D& mainRing, const S57Ring2D& secondRing, bool& isTailInsert, bool& isForword)//相连的边
{
	const XY& xyExteriorHead = mainRing.points[0];
	const XY& xyExteriorTail = mainRing.points[mainRing.points.size() - 1];

	const XY& xyOtherHead = secondRing.points[0];
	const XY& xyOtherTail = secondRing.points[secondRing.points.size() - 1];

	//顺时针

	if (xyExteriorHead.x == xyOtherTail.x&&xyExteriorHead.y == xyOtherTail.y)//外环的头与另一个环的尾相连
	{
		isTailInsert = true;
		isForword = true;
		return true;
	}

	if (xyExteriorTail.x == xyOtherHead.x&&xyExteriorTail.y == xyOtherHead.y)//外环的尾与另一个环的头相联
	{
		isTailInsert = true;
		isForword = true;
		return true;
	}

	//逆时针

	if (xyExteriorHead.x == xyOtherHead.x&&xyExteriorHead.y == xyOtherHead.y)//外环的头与另一个环的头相连
	{
		isTailInsert = true;
		isForword = false;
		return true;
	}

	if (xyExteriorTail.x == xyOtherTail.x&&xyExteriorTail.y == xyOtherTail.y)//外环的尾与另一个环的尾相连
	{
		isTailInsert = true;
		isForword = false;
		return true;
	}
	return false;
}

bool S57DataSet::createRings(const std::vector<S57Ring2D>& edges, S57Ring2D& exteriorRing, std::vector<S57Ring2D>& innerRings)//创建外环
{
	bool bRet = false;
	if (edges.size() == 1)
	{
		exteriorRing = edges[0];
		bRet = true;
	}
	else if (edges.size() > 1)
	{
		//思路:不停查找首尾相连的边,形成外环
		std::vector<int> usedEdges;
		std::vector<S57Ring2D> edgesClone = edges;
		int iEdgeCount = edges.size();
		for (int iUsedEdge = usedEdges.size(); iUsedEdge < iEdgeCount; iUsedEdge++)
		{
			if (usedEdges.size() == 0)
			{
				const S57Ring2D& ring = edgesClone[0];
				exteriorRing.points.insert(exteriorRing.points.end(), ring.points.begin(), ring.points.end());
				usedEdges.push_back(0);
				vectorRemoveAt<S57Ring2D>(edgesClone, 0);
			}
			else
			{
				bool isTailInsert = true;//头插,尾插
				bool isForword = true;//顺时针,逆时针
				auto itFindEdge = std::find_if(edgesClone.begin(), edgesClone.end(), [&](const S57Ring2D& ringItem)->bool {
					if (ringItem.points.size() > 0 && exteriorRing.points.size() > 0)
					{
						bool bRet = isConnectedEdge(exteriorRing, ringItem, isTailInsert, isForword);
						return bRet;
					}
					return false;
				});

				if (itFindEdge != edgesClone.end())
				{
					int index = std::distance(edgesClone.begin(), itFindEdge);

					int iInc = 0;
					int iStart = 0;
					int iEnd = 0;
					if (isForword)
					{
						iInc = 1;
						iStart = 0;
						iEnd = itFindEdge->points.size() - 1;
					}
					else
					{
						iInc = -1;
						iStart = itFindEdge->points.size() - 1;
						iEnd = 0;
					}




					if (isTailInsert)
					{
						for (int i = iStart; i != (iEnd + iInc); i += iInc)
						{
							exteriorRing.points.push_back(itFindEdge->points[i]);
						}
					}
					else
					{
						for (int i = iStart; i != (iEnd + iInc); i += iInc)
						{
							exteriorRing.points.insert(exteriorRing.points.begin(), itFindEdge->points[i]);
						}
					}

					vectorRemoveAt<S57Ring2D>(edgesClone, index);

					usedEdges.push_back(index);
					bRet = true;
				}
				else
				{
					//判断内环
					for (int i = 0; i < edgesClone.size(); i++)
					{
						S57Ring2D& ring = edgesClone[i];
						if (ring.points.size() > 0)
						{
							bool isClosedRing = ring.points[0] == ring.points[ring.points.size() - 1];//闭环
							if (isClosedRing)
							{
								innerRings.push_back(ring);
								bRet = true;
							}
						}
					}

					//bRet = false;
				}
			}
		}
	}
	return bRet;
}