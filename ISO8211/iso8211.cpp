#include "iso8211.h"

#include <iostream>
#include "../Utils/utils.h"

bool DebugISO8211Serial = false;

DRRecord::DRRecord()
{

}

DRRecord::DRRecord(const std::string& fieldTag_, const std::string& type_, const std::vector<char>& binary_, ISO8211_FLAG emFlag_)
	:fieldTag(fieldTag_)
	, type(type_)
	, binary(binary_)
	, emFlag(emFlag_)
{

}

void DRRecord::setBinaryOffset(int iBinaryOffset)
{
	binaryOffset = iBinaryOffset;

	if (274127 == iBinaryOffset)
	{
		bool debug = true;
	}
}



TypeValue::TypeValue(const std::string& strType, int data)
{
	type = strType;
	intValue = data;
}

TypeValue::TypeValue(const std::string& strType, const char* data, int dataLen)
	:
	binValue(data)
{
	type = strType;

	binLength = dataLen;
}

TypeValue::~TypeValue()
{
	//delptr(binValue);
}

void DR::setFieldTag(const std::string& fieldTag)
{
	drDirectory.fieldTag = fieldTag;
	if (fieldTag == "FRID" || fieldTag == "VRID")
	{
		drLeader.entryMap.sizeOfFieldLengthField = '2';
		drLeader.entryMap.sizeOfFieldPositionField = '2';
	}

}



DDRLeader::DDRLeader()
{
	//Default
	interChangeLevel = '3';
	leaderIdentifier = 'L';
	inLineCodeExtensionIndicator = 'E';
	versionNumber = '1';
	applicationIndicator = ' ';
	memcpy(fieldControlLength, "09", 2);
	memcpy(extendCharaterSetIndicator, " ! ", 3);
	entryMap.reserved = '0';
	entryMap.sizeOfFieldTagField = '4';
}

DRLeader::DRLeader()
{
	//Default
	interChangeLevel = ' ';
	leaderIdentifier = 'D';
	inLineCodeExtensionIndicator = ' ';
	versionNumber = ' ';
	applicationIndicator = ' ';
	memcpy(fieldControlLength, "  ", 2);
	memcpy(extendCharaterSetIndicator, "   ", 3);
	entryMap.reserved = '0';
	entryMap.sizeOfFieldLengthField = '5';
	entryMap.sizeOfFieldPositionField = '5';
	entryMap.sizeOfFieldTagField = '4';
}



Subfield::Subfield()
	:arrayIndex(-1)
{

}

Subfield::~Subfield()
{

}

unsigned char Subfield::uint8Value()
{
	unsigned char value = *(unsigned char*)(&bytes[0]);
	return value;
}

void Subfield::setUInt8Value(unsigned char value)
{
	emDataType = DataType::UInt8;
	bytes.resize(1);
	bytes[0] = value;
}

unsigned short Subfield::uint16Value()
{
	//unsigned short value = *(unsigned short*)(&bytes[0]);
	//return value;
	auto b = getIntBinary(&bytes[0], 2);
	unsigned short value = *(unsigned short*)(&b[0]);
	return value;
}

void Subfield::setUInt16Value(unsigned short value)
{
	emDataType = DataType::UInt16;
	setIntBinary(value, 2, bytes);
}

unsigned int Subfield::uint32Value()
{
	//unsigned int value = *(unsigned int*)(&bytes[0]);
	//return value;
	auto b = getIntBinary(&bytes[0], 4);
	unsigned int value = *(unsigned int*)(&b[0]);
	return value;
}

void Subfield::setUInt32Value(unsigned int value)
{
	emDataType = DataType::UInt32;
	setIntBinary(value, 4, bytes);
}

char Subfield::int8Value()
{
	char value = *(char*)(&bytes[0]);
	return value;
}

void Subfield::setInt8Value(char value)
{
	emDataType = DataType::Int8;
	bytes.resize(1);
	bytes[0] = value;
}

short Subfield::int16Value()
{
	short value = *(short*)(&bytes[0]);
	return value;
}

void Subfield::setInt16Value(short value)
{
	emDataType = DataType::Int16;
	setIntBinary(value, 2, bytes);
}

int Subfield::int32Value()
{
	auto b = getIntBinary(&bytes[0], 4);
	int value = *(int*)(&b[0]);
	return value;
}

void Subfield::setInt32Value(int value)
{
	emDataType = DataType::Int32;
	setIntBinary(value, 4, bytes);
}

void Subfield::setDataTypeFromDataFormat(const DataFormat& dataFormat)
{
	switch (dataFormat.emDataFormatType)
	{
	case DataFormatType::SByte:
	{
		switch (dataFormat.Width)
		{
		case 1:
		{
			emDataType = DataType::Int8;
		}
		break;
		case 2:
		{
			emDataType = DataType::Int16;
		}
		break;
		case 4:
		{
			emDataType = DataType::Int32;
		}
		break;
		}
	}
	break;
	case DataFormatType::UByte:
	{
		switch (dataFormat.Width)
		{
		case 1:
		{
			emDataType = DataType::UInt8;
		}
		break;
		case 2:
		{
			emDataType = DataType::UInt16;
		}
		break;
		case 4:
		{
			emDataType = DataType::UInt32;
		}
		break;
		}
	}
	break;
	case DataFormatType::Double:
	{
		emDataType = DataType::Double;
		auto v = doubleValue();

		bool debug = true;
	}
	break;
	//
	case DataFormatType::ASC:
	{
		emDataType = DataType::ASC;
	}
	break;
	case DataFormatType::Real:
	{
		emDataType = DataType::Real;
	}
	break;
	case DataFormatType::Bin:
	{
		emDataType = DataType::Binary;
	}
	break;

	default:
	{
		assert(false);
	}
	break;

	}
}

int Subfield::convert(const std::string& type, const char* binary, int binaryCount, Subfield& subfield, DataFormat& dataFormat, int iCharSize)
{
	dataFormat = DataFormat::Parse(type);
	int iLength = 0;
	if (dataFormat.Width > 0)
	{
		iLength = dataFormat.Width;
		if (dataFormat.emDataFormatType == DataFormatType::Bin && iLength > 0)
		{
			iLength /= 8;
		}
		std::vector<char> data;
		data.resize(iLength);
		memcpy(&data[0], binary, iLength);
		subfield.setBinary(data);
		subfield.setDataTypeFromDataFormat(dataFormat);
	}
	else
	{
		/*
		char szUT[] = { char(ISO8211_FLAG::UT),0 };
		char szFT[] = { char(ISO8211_FLAG::FT),0 };
		int iUTIndex = Utils::indexOf(binary, szUT);
		int iFTIndex = Utils::indexOf(binary, szFT);
		iLength = 0;
		if (iUTIndex > -1)
		{
			iLength = iUTIndex;
		}
		if (iFTIndex > -1)
		{
			if (iLength > -1 && iFTIndex < iLength)
			{
				iLength = iFTIndex;
			}
			if (iUTIndex < 0)
			{
				iLength = iFTIndex;
			}
		}

		if (iLength > -1)
		{
			std::vector<char> data;
			data.resize(iLength);
			if (iLength > 0)
			{
				memcpy(&data[0], binary, iLength);
				subfield.setBinary(data);
			}
			subfield.setDataTypeFromDataFormat(dataFormat);
		}*/
		switch (iCharSize)
		{
		case 1:
		{
			do
			{
				char ch = binary[iLength];
				if (ch == char(ISO8211_FLAG::UT)
					|| ch == char(ISO8211_FLAG::FT))
				{
					iLength += 1;
					break;
				}
				iLength += 1;
			} while (true);
		}
		break;
		case 2:
		{
			do
			{
				unsigned short ch = *(unsigned short*)(&binary[iLength]);
				if (ch == char(ISO8211_FLAG::UT)
					|| ch == char(ISO8211_FLAG::FT))
				{
					if (DebugISO8211Serial)
					{//编码
						if (iLength > 0)
						{
							std::vector<char> hex;
							hex.resize(iLength);
							memcpy(&hex[0], binary, iLength);
							auto hexString = Utils::hexToString(hex);
							std::vector<char> stringBytes;
							Utils::encodeConvert("UTF-16", binary, iLength, "GBK", stringBytes);
							std::string strString(stringBytes.begin(), stringBytes.end());
							Utils::log(hexString);
						}
					}

					iLength += 2;
					break;
				}
				iLength += 2;
			} while (true);
		}
		break;
		}
		if (iLength > 0)
		{
			std::vector<char> data;
			data.resize(iLength);
			memcpy(&data[0], binary, iLength);
			subfield.setBinary(data);
			subfield.setDataTypeFromDataFormat(dataFormat);
		}
	}

	char chFlag = binary[iLength];
	if ((subfield.emDataType == Subfield::DataType::ASC)
		&& (chFlag == char(ISO8211_FLAG::UT) || chFlag == char(ISO8211_FLAG::FT))
		)
	{
		dataFormat.emFlag = (ISO8211_FLAG)chFlag;
	}

	return iLength;
}

unsigned int Subfield::uintValue()
{
	unsigned int value = 0;
	switch (bytes.size())
	{
	case 1:value = uint8Value(); break;
	case 2:value = uint16Value(); break;
	case 4:value = uint32Value(); break;
	}
	return value;
}
int Subfield::intValue()
{
	int value = 0;
	switch (bytes.size())
	{
	case 1:value = int8Value(); break;
	case 2:value = int16Value(); break;
	case 4:value = int32Value(); break;
	}
	return value;
}

double Subfield::doubleValue()
{
	double value = *(double*)(&bytes[0]);
	return value;
}
std::string Subfield::stringValue()
{
	std::string value = std::string(bytes.begin(), bytes.end());
	return value;
}

std::string Subfield::typeString()
{

	std::stringstream ss;
	switch (emDataType)
	{
	case Subfield::DataType::UInt8:
	{
		ss << "UInt8";
		break;
	}
	case Subfield::DataType::UInt16:
	{
		ss << "UInt16";
		break;
	}
	case Subfield::DataType::UInt32:
	{
		ss << "UInt32";
		break;
	}
	case Subfield::DataType::Int8:
	{
		ss << "Int8";
		break;
	}
	case Subfield::DataType::Int16:
	{
		ss << "Int16";
		break;
	}
	case Subfield::DataType::Int32:
	{
		ss << "Int32";
		break;
	}
	case Subfield::DataType::Double:
	{
		ss << "Double";
		break;
	}
	case Subfield::DataType::ASC:
	{
		ss << "ASC";
		break;
	}
	default:
		break;
	}

	std::string s = ss.str();
	return s;
}

std::string Subfield::toString()
{
	std::stringstream ss;
	switch (emDataType)
	{
	case Subfield::DataType::UInt8:
	{
		ss << (unsigned int)uint8Value();
		break;
	}
	case Subfield::DataType::UInt16:
	{
		ss << (unsigned int)uint16Value();
		break;
	}
	case Subfield::DataType::UInt32:
	{
		ss << (unsigned int)uintValue();
		break;
	}
	case Subfield::DataType::Int8:
	{
		ss << (int)int8Value();
		break;
	}
	case Subfield::DataType::Int16:
	{
		ss << (int)int16Value();
		break;
	}
	case Subfield::DataType::Int32:
	{
		ss << (int)intValue();
		break;
	}
	case Subfield::DataType::Double:
	{
		ss << doubleValue();
		break;
	}
	case Subfield::DataType::ASC:
	{
		ss << stringValue();
		break;
	}
	default:
		break;
	}
	std::string s = ss.str();
	return s;
}

bool Subfield::operator==(const Subfield& ddv)
{
	bool bRet = false;

	bRet = (tagName == ddv.tagName && emDataType == ddv.emDataType && bytes == ddv.bytes);

	return bRet;
}

std::vector<char> Subfield::binary()
{
	return bytes;
}

void Subfield::setBinary(const std::vector<char>& binary)
{
	bytes = binary;
}


void Subfield::setIntBinary(int value, int width, std::vector<char>& binary)
{
	binary.resize(width);
	unsigned int nMask = 0xff;
	for (int i = 0; i < width; i++)
	{
		int iOut = i;
		binary[iOut] = (char)((value & nMask) >> (i * 8));//小尾
		nMask <<= 8;
	}
}

std::vector<char> Subfield::getIntBinary(const char* bytes, int width)
{
	std::vector<char> binary;
	binary.resize(width);
	//for (int i = 0; i < width; i++)
	//	binary[width - i - 1] = bytes[i];
	for (int i = 0; i < width; i++)//小尾
		binary[i] = bytes[i];
	return binary;
}


class FindDRRecordByTag
{
protected:
	std::string mTag;
public:
	FindDRRecordByTag(const std::string& tag)
	{
		mTag = tag;
	}
	bool operator()(const DRRecord& drRecord)
	{
		bool bRet = Utils::endsWith(drRecord.fieldTag, mTag.c_str());
		return bRet;
	}

};

void DRFieldArea::createSubfield(const DRRecord& drRecord, Subfield& subfield)
{
	bool bRet = false;

	DataFormat dataFormat = DataFormat::Parse(drRecord.type);
	subfield.tagName = drRecord.fieldTag;
	if (drRecord.binary.size() > 0)
	{
		if (dataFormat.emDataFormatType == DataFormatType::ASC)
		{
			std::string str_(drRecord.binary.begin(), drRecord.binary.end());
			str_ += char(ISO8211_FLAG::UT);
			Subfield::convert(drRecord.type, str_.c_str(), str_.size(), subfield, dataFormat);
		}
		else
		{
			Subfield::convert(drRecord.type, &drRecord.binary[0], drRecord.binary.size(), subfield, dataFormat);
		}
	}
	else
	{
		subfield.setBinary(std::vector<char>());
		subfield.setDataTypeFromDataFormat(dataFormat);
	}
}

int DataFormat::CalcWidth(const char* str, int& width, bool hasBrackets)
{
	int iBegin = 0;
	int iEnd = 0;
	if (hasBrackets)
	{
		iBegin += 1;
		iEnd += 1;
	}
	int iLen = strlen(str);
	while (iEnd < iLen)
	{
		bool isNumber = Utils::isNumberChar(str[iEnd]);
		if (!isNumber)
			break;
		iEnd++;
	}

	if ((hasBrackets && iEnd > 1)//带括号
		|| (!hasBrackets && iEnd > 0))
	{
		std::string strNum(str + iBegin, iEnd - iBegin);
		width = stoi(strNum);
	}
	else
	{
		width = 0;
	}


	return iEnd;
}

DataFormat DataFormat::Parse(const std::string& strType)
{
	//A()
	//A(1)
	//A(12)
	//A(17)
	//A(2)
	//A(3)
	//A(6)
	//A(8)
	//B(40)
	//B(64)		bit
	//I()
	//I(1)
	//I(10)
	//I(2)
	//I(3)
	//I(5)		int (转成5个asc码)
	//R()
	//R(4)		double asc码
	//b11
	//b12
	//b14
	//b14*)
	//b24
	//b24*)
	//bin
	DataFormat df;
	memset(&df, 0, sizeof(df));
	//asc
	if (Utils::startWith(strType, "A"))
	{
		df.emDataFormatType = DataFormatType::ASC;
		bool isBrackets = *(strType.c_str() + 1) == '(';
		CalcWidth(strType.c_str() + 1, df.Width, isBrackets);
		if (df.Width == 0)
		{
			df.emFlag = ISO8211_FLAG::UT;
		}
	}
	else if (Utils::startWith(strType, "B"))
	{
		df.emDataFormatType = DataFormatType::Bin;
		bool isBrackets = *(strType.c_str() + 1) == '(';
		CalcWidth(strType.c_str() + 1, df.Width, isBrackets);
		if (df.Width == 0)
		{
			assert(false);
		}
	}
	else if (Utils::startWith(strType, "I"))
	{
		df.emDataFormatType = DataFormatType::INT;
		bool isBrackets = *(strType.c_str() + 1) == '(';
		CalcWidth(strType.c_str() + 1, df.Width, isBrackets);
		if (df.Width == 0)
		{
			df.emFlag = ISO8211_FLAG::UT;
		}
	}
	else if (Utils::startWith(strType, "R"))
	{
		df.emDataFormatType = DataFormatType::Real;
		bool isBrackets = *(strType.c_str() + 1) == '(';
		CalcWidth(strType.c_str() + 1, df.Width, isBrackets);
		if (df.Width == 0)
		{
			df.emFlag = ISO8211_FLAG::UT;
		}
	}
	//bin
	else if (Utils::startWith(strType, "b11"))
	{
		df.emDataFormatType = DataFormatType::UByte;
		df.Width = 1;
	}
	else if (Utils::startWith(strType, "b1"))
	{
		df.emDataFormatType = DataFormatType::UByte;
		CalcWidth(strType.c_str() + 2, df.Width);
	}
	else if (Utils::startWith(strType, "b2"))
	{
		df.emDataFormatType = DataFormatType::SByte;
		CalcWidth(strType.c_str() + 2, df.Width);
	}
	else if (strType == "b48")
	{
		df.emDataFormatType = DataFormatType::Double;
		df.Width = 8;
	}
	else if (strType == "@")//ISO/IEC 8211 Cartesian label
	{

	}
	else
	{
		assert(false);
	}

	if (Utils::endsWith(strType, "*)"))
	{
		df.emFlag = ISO8211_FLAG::UT;
	}

	return df;
}

FieldControlField::FieldControlField()
{
	dataStructureCode = DATA_STRUCTURE_CODE_2;
	dataTypeCode = DATA_TYPE_CODE_6;
	memcpy(auxiliaryControls, AUXILIARY_CONTROLS, strlen(AUXILIARY_CONTROLS));
	memcpy(printableGraphics, PRINTABLE_GRAPHICS, strlen(PRINTABLE_GRAPHICS));
	memcpy(truncatedEscapeSequence, TRUNCATED_ESCAPE_SEQUENCE_Lexical_level_0, strlen(TRUNCATED_ESCAPE_SEQUENCE_Lexical_level_0));
}

SubfieldInfo::SubfieldInfo()
{
}

SubfieldInfo::SubfieldInfo(const char* subfieldName_, const char* fieldTag_, const char* ascFormat_, const char* binFormat_, bool isRepeated_)
	: subfieldName(subfieldName_)
	, fieldTag(fieldTag_)
	, ascFormat(ascFormat_)
	, binFormat(binFormat_)
	, isRepeated(isRepeated_)
{
}

SubfieldInfo::SubfieldInfo(const SubfieldInfo& subfieldInfo)
{
	*this = subfieldInfo;
}

SubfieldInfo& SubfieldInfo::operator= (const SubfieldInfo& other)
{
	subfieldName = other.subfieldName;
	fieldTag = other.fieldTag;
	ascFormat = other.ascFormat;
	binFormat = other.binFormat;
	isRepeated = other.isRepeated;
	return *this;
}


int DDRField::subfieldInfoRepeatedIndex()//S57、S100(*YCOO,XCOO,ZC00) repeated index
{
	int iFind = -1;
	int iSubfieldInfo = 0;
	for (auto it = mSubfieldInfos.begin(); it != mSubfieldInfos.end(); it++, iSubfieldInfo++)
	{
		std::string strLabel = it->fieldTag;
		bool isRep = it->isRepeated;
		if (isRep)//&& Utils::startWith(strLabel, "\\*") == false)
		{
			iFind = iSubfieldInfo;
			break;
		}
	}
	return iFind;
}

int DDRField::subfieldInfoS100RepeatedIndex()//S100(\\*NATC) repeated index
{
	int iFind = -1;
	int iSubfieldInfo = 0;
	for (auto it = mSubfieldInfos.begin(); it != mSubfieldInfos.end(); it++, iSubfieldInfo++)
	{
		std::string strLabel = it->fieldTag;
		if (Utils::startWith(strLabel, "\\*"))
		{
			iFind = iSubfieldInfo;
			break;
		}
	}
	return iFind;
}

std::string DDRField::content()
{
	std::stringstream ss;
	char* bytes = (char*)&mFieldControlField;
	int bytesCount = sizeof(mFieldControlField);
	for (int i = 0; i < bytesCount; i++)
	{
		ss << bytes[i];
	}
	ss << mFieldLabel;
	ss << char(ISO8211_FLAG::UT);
	//Data set identification fieldRCNM!RCID!EXPP!INTU!DSNM!EDTN!UPDN!UADT!ISDT!STED!PRSP!PSDN!PRED!PROF!AGEN!COMT(b11,b14,2b11,3A,2A(8),R(4),b11,2A,b11,b12,A)
	std::string fieldTags;
	std::vector<std::tuple<std::string, int>> fieldTypeTable;
	for (auto it = mSubfieldInfos.begin(); it != mSubfieldInfos.end(); it++)
	{
		std::string fieldTag = it->fieldTag;
		std::string ascType = it->ascFormat;
		std::string binType = it->binFormat;
		std::string strType = ISO8211::GetType(ascType, binType);
		if (Utils::endsWith(strType, "()"))
		{
			strType = Utils::rightTrim(strType, "()");
		}

		if (fieldTags.size() > 0)
		{
			fieldTags += "!";
		}

		fieldTags += fieldTag;

		if (fieldTypeTable.size() == 0)
		{
			auto item = std::tuple<std::string, int>(strType, 1);
			fieldTypeTable.push_back(item);
		}
		else
		{
			int iItemLast = fieldTypeTable.size() - 1;
			auto itemLast = fieldTypeTable.at(iItemLast);
			std::string strItemLastFieldType = std::get<0>(itemLast);
			int iItemLastCount = std::get<1>(itemLast);
			if (strItemLastFieldType == strType)
			{
				iItemLastCount += 1;
				fieldTypeTable[iItemLast] = std::tuple<std::string, int>(strType, iItemLastCount);
			}
			else
			{
				auto item = std::tuple<std::string, int>(strType, 1);
				fieldTypeTable.push_back(item);
			}
		}
	}
	ss << fieldTags;
	ss << char(ISO8211_FLAG::UT);
	ss << "(";
	int iFieldType = 0;
	for (auto it = fieldTypeTable.begin(); it != fieldTypeTable.end(); it++, iFieldType++)
	{
		std::string strFieldType = std::get<0>(*it);
		int iFieldTypeCount = std::get<1>(*it);

		if (iFieldType > 0)
		{
			ss << ",";
		}
		if (iFieldTypeCount > 1)
			ss << iFieldTypeCount;
		ss << strFieldType;
	}
	ss << ")";
	ss << char(ISO8211_FLAG::FT);

	std::string s = ss.str();
	return s;
}

ISO8211::ISO8211()
{

}



void ISO8211::createDDRDirectory(const std::string& fieldTag, std::string& ddrDirectory)
{//
	auto itFind = mMetaDDRDirectory.find(fieldTag);
	if (itFind != mMetaDDRDirectory.end())
	{
		auto fieldTagChildList = itFind->second;
		for (auto it = fieldTagChildList.begin(); it != fieldTagChildList.end(); it++)
		{
			ddrDirectory += fieldTag;
			ddrDirectory += *it;

			auto itFindChild = mMetaDDRDirectory.find(*it);
			if (itFindChild != mMetaDDRDirectory.end())
			{
				createDDRDirectory(*it, ddrDirectory);
			}
		}
	}
	else
	{
		if (ddrDirectory.size() == 0)
			ddrDirectory = fieldTag;
	}

}



int ISO8211::CalcSize(int value)
{
	int mi = 1;
	while (true)
	{
		int iMax = int(pow(10, mi));
		if (value < iMax)
			break;
		mi++;
	}
	return mi;
}

void ISO8211::SetName(unsigned char RCNM, unsigned RCID, char* bytes)
{
	char szName[5] = {
		(char)RCNM,
		static_cast<char>(RCID & 0xff),
		static_cast<char>((RCID & 0xff00) >> 8),
		static_cast<char>((RCID & 0xff0000) >> 16),
		static_cast<char>((RCID & 0xff000000) >> 24)
	};
	memcpy(bytes, szName, 5);
}

void ISO8211::GetNAME(const char* bytes, unsigned char& RCNM, unsigned int& RCID)
{
	RCNM = bytes[0];
	RCID = bytes[1]
		+ bytes[2] * 256
		+ bytes[3] * 256 * 256
		+ bytes[4] * 256 * 256 * 256;
}

int ISO8211::charSize(const DDRField&)
{
	return 1;
}

void ISO8211::setRecordSubfield(const DRRecord& drRecord, Subfield& subfield)//空实现
{

}


bool ISO8211::load(const std::string& path)
{
	mISO8211Binary.clear();
	bool bRet = Utils::loadBinary(path.c_str(), mISO8211Binary);
	if (bRet)
	{
		bRet = loadBinary(mISO8211Binary);
	}
	return bRet;
}



bool ISO8211::loadBinary(const std::vector<char>& binary, std::string* firstFieldAreaFieldTag)
{
	if (DebugISO8211Serial)
	{
		std::stringstream ss;
		ss << "ISO8211 Binary Size:|" << binary.size() << "|";
		Utils::log(ss.str().c_str());
	}
	mMetaDDRFields.clear();
	mMetaDDRFieldsTable.clear();
	mMetaDDRDirectory.clear();
	mMetaDDRDirectoryParent.clear();
	mDRs.clear();
	if (binary.size() > 24)
	{
		char szUTs[] = { char(ISO8211_FLAG::UT), 0 };
		char szFTs[] = { char(ISO8211_FLAG::FT), 0 };
		char szSpliter[] = { ',',0 };

		int iBinary = 0;
		if (DebugISO8211Serial)
		{
			int iSize = sizeof(DDRLeader);
			std::stringstream ss;
			ss << "READ(DDR Leader):|" << iBinary << "-" << iSize << "|";
			ss << std::string(&binary[0] + iBinary, iSize);
			Utils::log(ss.str().c_str());
		}
		DDRLeader* ddrLeader = (DDRLeader*)((char*)&binary[iBinary]); iBinary += 24;



		int iRecordLength = Utils::stingToInt(ddrLeader->recordLength, 5);
		int iBaseAddressOfFieldArea = Utils::stingToInt(ddrLeader->baseAddressOfFieldArea, 5);


		int iSizeOfFieldTagField = ddrLeader->entryMap.sizeOfFieldTagField - '0';
		int iSizeOfFieldLengthField = ddrLeader->entryMap.sizeOfFieldLengthField - '0';
		int iSizeOfFieldPositionField = ddrLeader->entryMap.sizeOfFieldPositionField - '0';

		int iDirectorySize = iSizeOfFieldTagField + iSizeOfFieldLengthField + iSizeOfFieldPositionField;
		int iDirectoryCount = (iBaseAddressOfFieldArea - 24) / iDirectorySize;

		//[== Field tags
		std::vector<std::tuple<std::string, int, int>> fieldTags;//Field tag,Field length,Field position

		for (int iDirectory = 0; iDirectory < iDirectoryCount; iDirectory++)
		{
			char* szFieldTag = new char[iSizeOfFieldTagField + 1]; memset(szFieldTag, 0, iSizeOfFieldTagField + 1);
			char* szFieldLengh = new char[iSizeOfFieldLengthField + 1]; memset(szFieldLengh, 0, iSizeOfFieldLengthField + 1);
			char* szFieldPosition = new char[iSizeOfFieldPositionField + 1]; memset(szFieldPosition, 0, iSizeOfFieldPositionField + 1);

			int iBinaryBegin = iBinary;
			memcpy(szFieldTag, &binary[iBinary], iSizeOfFieldTagField); iBinary += iSizeOfFieldTagField;
			memcpy(szFieldLengh, &binary[iBinary], iSizeOfFieldLengthField); iBinary += iSizeOfFieldLengthField;
			memcpy(szFieldPosition, &binary[iBinary], iSizeOfFieldPositionField); iBinary += iSizeOfFieldPositionField;

			int iFieldLength = Utils::stingToInt(szFieldLengh, iSizeOfFieldLengthField);
			int iFieldPosition = Utils::stingToInt(szFieldPosition, iSizeOfFieldPositionField);

			std::tuple<std::string, int, int> tup(szFieldTag, iFieldLength, iFieldPosition);
			fieldTags.push_back(tup);

			if (DebugISO8211Serial)
			{
				std::stringstream ss;
				ss << "READ(DDR Directory)Item:|" << iBinaryBegin << "-" << (iBinaryBegin + iDirectorySize) << "|";
				ss << "Field Tag:" << szFieldTag << "\t";
				ss << std::string(&binary[0] + iBinaryBegin, iDirectorySize);
				Utils::log(ss.str().c_str());
			}

			delptr(szFieldTag);
			delptr(szFieldLengh);
			delptr(szFieldPosition);
		}

		if (binary[iBinary++] != char(ISO8211_FLAG::FT))
		{
			return false;
		}

		//==]

		//[==DDR field area
		if (DebugISO8211Serial)
		{
			std::stringstream ss;
			ss << "READ(DDR Field Area):|" << iBinary << "|";
			Utils::log(ss.str().c_str());
		}

		std::vector<DDRField> ddrFields;
		int iField = 0;
		for (auto itFieldTag = fieldTags.begin(); itFieldTag != fieldTags.end(); itFieldTag++, iField++)
		{
			std::string strFieldTag = std::get<0>(*itFieldTag);
			int iFieldLength = std::get<1>(*itFieldTag);
			int iFieldPosition = std::get<2>(*itFieldTag);

			int iFieldAreaBegin = iBaseAddressOfFieldArea + iFieldPosition;

			std::string strFieldArea = std::string(&binary[iFieldAreaBegin], iFieldLength);



			std::vector<std::string> strFieldAreas = Utils::split(strFieldArea, szUTs);
			if (strFieldAreas.size() < 2)
				return false;


			FieldControlField* fieldControlField = (FieldControlField*)&binary[iFieldAreaBegin];

			if (iField == 1 && firstFieldAreaFieldTag)
			{
				*firstFieldAreaFieldTag = strFieldTag;
			}



			DDRField ddrField;
			ddrField.mFieldControlField = *fieldControlField;
			ddrField.mFieldTag = strFieldTag;
			if (strFieldAreas.size() > 0)
			{
				ddrField.mFieldLabel = strFieldAreas[0].substr(sizeof(FieldControlField));
			}


			if (strFieldAreas.size() == 2)//directory tree
			{

			}
			else if (strFieldAreas.size() > 2)//types
			{
				std::string strType = strFieldAreas[2];
				strType = Utils::rightTrim(strType, szFTs);
				if (strType.size() > 2)
				{
					strType = strType.substr(1, strType.size() - 2);
				}


				std::string strSubfieldName = strFieldAreas[1];
				//Exist rep field name
				std::vector<std::tuple<std::string, bool>> subfieldLabels;//field name/is rep

				std::string findStr1 = "!";
				std::string findStr2 = "\\\\*";//S100 repeated
				std::string findStr3 = "*";//S57 repeated
				int iSubStrIndex = 0;
				do
				{
					std::string buffer = strSubfieldName.substr(iSubStrIndex);

					bool isRep = false;
					std::string fieldName;
					if (Utils::startWith(buffer, findStr1.c_str()))
					{
						fieldName = buffer.substr(findStr1.size(), 4);
						iSubStrIndex += (findStr1.size() + 4);
					}
					else if (Utils::startWith(buffer, findStr2.c_str()))
					{
						fieldName = "\\*" + buffer.substr(findStr2.size(), 4);
						iSubStrIndex += (findStr2.size() + 4);
						isRep = true;
					}
					else if (Utils::startWith(buffer, findStr3.c_str()))
					{
						fieldName = buffer.substr(0, findStr3.size() + 4);
						iSubStrIndex += (findStr3.size() + 4);
						isRep = true;
					}
					else
					{
						fieldName = buffer.substr(0, 4);
						iSubStrIndex += 4;
					}

					//Utils::log(fieldName.c_str());

					std::tuple<std::string, bool> tup(fieldName, isRep);
					subfieldLabels.push_back(tup);
				} while (iSubStrIndex < strSubfieldName.size());

				std::vector<std::string> types = Utils::split(strType, szSpliter);
				int iSubfieldIndex = 0;
				for (int iType = 0; iType < types.size(); iType++)
				{
					std::string type = types.at(iType);
					int iNumberLength = 0;
					for (int iChar = 0; iChar < type.size(); iChar++, iNumberLength++)
					{
						char ch = type.at(iChar);
						if (!Utils::isNumberChar(ch))
						{
							break;
						}
					}

					int iSubfieldCount = 1;
					if (iNumberLength > 0)
					{
						iSubfieldCount = Utils::stingToInt(type.c_str(), iNumberLength);
						type = type.substr(iNumberLength);
					}
					std::vector<SubfieldInfo> subfieldInfos;//Subfield name/Label/ASC Format/Bin Format/is Rep
					for (int iSubfield = 0; iSubfield < iSubfieldCount; iSubfield++, iSubfieldIndex++)
					{
						std::string subfieldType = type;
						std::string subfieldLabel;
						bool isRep = false;
						if (iSubfield < subfieldLabels.size())
						{
							std::tuple<std::string, bool> fieldTuple = subfieldLabels[iSubfieldIndex];
							subfieldLabel = std::get<0>(fieldTuple);
							isRep = std::get<1>(fieldTuple);
						}

						if (isRep)
						{
							bool debug1 = true;
						}

						SubfieldInfo subfieldInfo("", subfieldLabel.c_str(), "", subfieldType.c_str(), isRep);

						subfieldInfos.push_back(subfieldInfo);

					}

					ddrField.mSubfieldInfos.insert(ddrField.mSubfieldInfos.end(), subfieldInfos.begin(), subfieldInfos.end());
				}
			}
			mMetaDDRFieldsTable[ddrField.mFieldTag] = ddrField;
			ddrFields.push_back(ddrField);
			iBinary += (iFieldLength);
		}

		if (binary[iBinary - 1] != char(ISO8211_FLAG::FT))
		{
			return false;
		}

		//==]

		//[==DRs
		while (iBinary < binary.size())
		{
			DRLeader* drLeader = (DRLeader*)&binary[iBinary];
			if (drLeader->leaderIdentifier != 'D')
			{
				assert(false);
				return false;
			}
			int iDRLeaderSize = sizeof(DRLeader);
			int iBaseAddressOfFieldArea = Utils::stingToInt(drLeader->baseAddressOfFieldArea, 5);

			int iSizeOfFieldLengthField = drLeader->entryMap.sizeOfFieldLengthField - '0';
			int iSizeOfFieldPositionField = drLeader->entryMap.sizeOfFieldPositionField - '0';
			int iSizeOfFieldTagField = drLeader->entryMap.sizeOfFieldTagField - '0';
			int iDRDirectorySize = iSizeOfFieldLengthField + iSizeOfFieldPositionField + iSizeOfFieldTagField;

			int iDRDirectoryCount = (iBaseAddressOfFieldArea - iDRLeaderSize) / iDRDirectorySize;

			int iDRDirectoryField = iBinary + iDRLeaderSize;//index

			std::vector<std::tuple<std::string, int, int>> drs;
			for (int iDRDirectory = 0; iDRDirectory < iDRDirectoryCount; iDRDirectory++)
			{
				std::string drFieldTag(&binary[iDRDirectoryField], iSizeOfFieldTagField); iDRDirectoryField += iSizeOfFieldTagField;
				int iFieldLength = Utils::stingToInt(&binary[iDRDirectoryField], iSizeOfFieldLengthField); iDRDirectoryField += iSizeOfFieldLengthField;
				int iFieldPosition = Utils::stingToInt(&binary[iDRDirectoryField], iSizeOfFieldPositionField); iDRDirectoryField += iSizeOfFieldPositionField;
				//iDRDirectoryFieldLeader += 1;//FT
				auto tup = std::tuple<std::string, int, int>(drFieldTag, iFieldLength, iFieldPosition);
				drs.push_back(tup);
			}

			if (binary[iDRDirectoryField] != char(ISO8211_FLAG::FT))
				return false;

			int iDRSubfield = iDRDirectoryField + 1;//FT

			for (auto itDR = drs.begin(); itDR != drs.end(); itDR++)
			{
				std::string fieldTag = std::get<0>(*itDR);
				int iFieldLength = std::get<1>(*itDR);
				int iFieldPostion = std::get<2>(*itDR);
				auto itFindFieldTag = mMetaDDRFieldsTable.find(fieldTag);
				DR dr;
				dr.drLeader = *drLeader;
				dr.drDirectory.fieldTag = fieldTag;
				if (itFindFieldTag != mMetaDDRFieldsTable.end())
				{
					DDRField ddrField = itFindFieldTag->second;

					if (DebugISO8211Serial)
					{
						if ("FACS" == fieldTag)
						{
							bool debug = true;
						}

						if ("NATF" == ddrField.mFieldTag)
						{
							bool debug = true;
						}

						std::stringstream ss;
						ss << "Field tag:" << fieldTag;
						Utils::log(ss.str().c_str());
					}


					int iFindRepeated = ddrField.subfieldInfoRepeatedIndex();
					//int iFindS100Repeated = ddrField.subfieldInfoS100RepeatedIndex();


					if (277583 == iDRSubfield)
					{
						bool debug = true;
					}

					bool isLoop = true;//
					bool isRepeated = false;
					int iRepeatedIndex = 0;
					int iDRSubfieldSize = 0;
					for (int iSubfieldInfo = 0; isLoop && iSubfieldInfo < ddrField.mSubfieldInfos.size(); iSubfieldInfo++)
					{
						auto item = ddrField.mSubfieldInfos[iSubfieldInfo];
						std::string fieldTag = item.fieldTag;
						std::string type = item.binFormat;
						std::string sSubfieldName = fieldTag;
						int index = iDRSubfield + iDRSubfieldSize;



						//
						DRRecord drRecord;
						Subfield subfield;
						DataFormat dataFormat;

						//[== debug hex std::string
						//std::vector<char> buffer;
						//buffer.resize(20);
						//memcpy(&buffer[0], &binary[index], 20);
						//std::string strHex = Utils::hexTostd::string(buffer);
						//==]
						if (203577 == index)
						{
							bool debug = true;
						}

						int iCharSize = charSize(ddrField);

						if ((iFindRepeated > -1 && iSubfieldInfo == iFindRepeated))//Repeated
						{
							for (iSubfieldInfo = iFindRepeated; iSubfieldInfo < ddrField.mSubfieldInfos.size(); iSubfieldInfo++)
							{
								item = ddrField.mSubfieldInfos[iSubfieldInfo];
								fieldTag = item.fieldTag;
								type = item.binFormat;
								sSubfieldName = fieldTag;
								index = iDRSubfield + iDRSubfieldSize;

								if (277583 == index)
								{
									bool debug = true;
								}

								if ("ATVL" == fieldTag)
								{
									bool debug = true;
								}

								//
								int iLength = createRecord(binary, index, type, sSubfieldName, drRecord, subfield, dataFormat, iCharSize);
								//if (dataFormat.emFlag != ISO8211_FLAG::Unknown)//结束符号
								//	iLength += 1;
								drRecord.setBinaryOffset(index);
								if (277596 == index)
								{
									bool debug = true;
								}

								//if ("*ATTL" == fieldTag)//越出
								//{
								//	if (subfield.uint16Value() == unsigned short(ISO8211_FLAG::FT))
								//	{
								//		isLoop = false;
								//		break;
								//	}
								//}
								//if (dataFormat.emFlag != ISO8211_FLAG::FT)//下一个为FT结束符号时,不添加DRRecord,ATTL的处理
								{
									dr.drFieldArea.drRecords.push_back(drRecord);
									outputRecord(index, iDRSubfield, iDRSubfieldSize, iLength, sSubfieldName, subfield);
								}

								iDRSubfieldSize += iLength;

								setRecordSubfield(drRecord, subfield);

								if ((iDRSubfieldSize + 1) >= iFieldLength)
								{
									iDRSubfieldSize = iFieldLength;
									isLoop = false;
									break;
								}
								else
								{
									if ((iSubfieldInfo + 1) >= ddrField.mSubfieldInfos.size())
									{
										iSubfieldInfo = iFindRepeated - 1;
									}
								}
							}
						}
						else//Normal
						{

							if ("ATVL" == fieldTag)
							{
								bool debug = true;
							}

							int iLength = createRecord(binary, index, type, sSubfieldName, drRecord, subfield, dataFormat, iCharSize);
							//if (dataFormat.emFlag != ISO8211_FLAG::Unknown)//结束符号
							//	iLength += 1;
							drRecord.setBinaryOffset(index);
							if (277583 == index)
							{
								bool debug = true;
							}

							//if (dataFormat.emFlag != ISO8211_FLAG::FT)//下一个为FT结束符号时,不添加DRRecord,ATTL的处理
							{
								dr.drFieldArea.drRecords.push_back(drRecord);
								outputRecord(index, iDRSubfield, iDRSubfieldSize, iLength, sSubfieldName, subfield);
							}

							iDRSubfieldSize += iLength;

							setRecordSubfield(drRecord, subfield);

							if ((iDRSubfieldSize + 1) >= iFieldLength)
							{
								iDRSubfieldSize = iFieldLength;
								isLoop = false;
								break;
							}
						}

					}
					iDRSubfield += iDRSubfieldSize;
					iBinary = iDRSubfield;
					mDRs.push_back(dr);
				}
			}


		}

		if (DebugISO8211Serial)
		{
			std::stringstream ss;
			ss << "Binary size:" << binary.size();
			Utils::log(ss.str().c_str());
		}
		//==]
		return true;
	}
	return false;
}

void ISO8211::outputRecord(int index, int iDRSubfield, int iDRSubfieldSize, int iLength, std::string& sSubfieldName, Subfield& subfield)
{
	if (DebugISO8211Serial)
	{
		int __iStart = index;
		int __iEnd = iDRSubfield + iDRSubfieldSize + iLength;
		if (__iEnd == 197857)
		{
			bool debug = true;
		}

		if (201015 == __iStart)
		{
			bool debug = true;
		}


		std::stringstream ss;
		ss << "\t[" << __iStart << "-" << __iEnd << "]" << sSubfieldName << ":" << subfield.toString();
		Utils::log(ss.str().c_str());
	}
}


int ISO8211::createRecord(const std::vector<char>& binary, int index, const std::string& type, const std::string& sSubfieldName, DRRecord& drRecord, Subfield& subfield, DataFormat& dataFormat, int iCharSize)
{
	subfield = Subfield();
	dataFormat = DataFormat();
	int iLength = Subfield::convert(type, &binary[index], binary.size() - index, subfield, dataFormat, iCharSize);
	drRecord = DRRecord(sSubfieldName, type, subfield.binary(), dataFormat.emFlag);
	return iLength;
}



std::vector<char> ISO8211::createBinary()
{
	std::vector<char> binary;
	{
		auto ddrField0000 = mMetaDDRFieldsTable[ISO8211_0000];
		char* bytes = (char*)&ddrField0000.mFieldControlField;
		int bytesCount = sizeof(ddrField0000.mFieldControlField);
		//[==Calc DDR Directory
		std::vector<std::string> strFieldTags;//
		std::string ddr0000FieldArea;
		createDDRDirectory("0001", ddr0000FieldArea);//tree

		for (int i = 0; i < ddr0000FieldArea.size(); i += 4)
		{
			std::string fieldTag_ = ddr0000FieldArea.substr(i, 4);
			auto itFindFieldTag_ = std::find(strFieldTags.begin(), strFieldTags.end(), fieldTag_);
			if (itFindFieldTag_ == strFieldTags.end())
			{
				strFieldTags.push_back(fieldTag_);
			}
		}
		ddr0000FieldArea.insert(ddr0000FieldArea.begin(), char(ISO8211_FLAG::UT));
		for (int i = 0; i < bytesCount; i++)//Field control field
		{
			ddr0000FieldArea.insert(ddr0000FieldArea.begin(), bytes[bytesCount - i - 1]);
		}
		ddr0000FieldArea.push_back(char(ISO8211_FLAG::FT));
		//==]
		std::vector<std::tuple<std::string, std::string>> ddrFieldArea;//Field tag,Field tag content;
		ddrFieldArea.push_back(std::tuple<std::string, std::string>("0000", ddr0000FieldArea));
		binary.insert(binary.end(), ddr0000FieldArea.begin(), ddr0000FieldArea.end());

		//

		int fieldAreaSize = 0;
		for (auto it = strFieldTags.begin(); it != strFieldTags.end(); it++)
		{
			std::string strFieldTag = *it;
			auto ddrField = mMetaDDRFieldsTable[strFieldTag];
			std::string ddrFieldTagContent = ddrField.content();
			fieldAreaSize += ddrFieldTagContent.size();
			ddrFieldArea.push_back(std::tuple<std::string, std::string>(strFieldTag, ddrFieldTagContent));
			binary.insert(binary.end(), ddrFieldTagContent.begin(), ddrFieldTagContent.end());
		}


		//[==Field tag leader
		std::string strFieldTagLeaders;
		int iSizeOfFieldLengthField = 6;
		int iSizeOfFieldPositionField = 6;
		{
			int iFieldContentPos = 0;
			for (auto it = ddrFieldArea.begin(); it != ddrFieldArea.end(); it++)
			{
				std::string strFieldTag = std::get<0>(*it);
				int iFieldContentLen = std::get<1>(*it).size();


				strFieldTagLeaders += strFieldTag;
				strFieldTagLeaders += Utils::intToString(iFieldContentLen, iSizeOfFieldLengthField);
				strFieldTagLeaders += Utils::intToString(iFieldContentPos, iSizeOfFieldPositionField);

				iFieldContentPos += iFieldContentLen;
			}
			strFieldTagLeaders += char(ISO8211_FLAG::FT);
			binary.insert(binary.begin(), strFieldTagLeaders.begin(), strFieldTagLeaders.end());//write from head
		}
		//==]

		//[==Create DRs
		std::vector<char> drsBinary;
		{
			for (auto itDR = mDRs.begin(); itDR != mDRs.end(); itDR++)
			{
				std::vector<char> drBinary;
				{//0001,//b12:0,0,char(ISO8211_FLAG::FT)
					drBinary.push_back(0);
					drBinary.push_back(0);
					drBinary.push_back(char(ISO8211_FLAG::FT));
				}

				//DR field area
				std::vector<int> drBinaryLens;
				drBinaryLens.push_back(3);//0001,//b12:0,0,char(ISO8211_FLAG::FT)
				{
					createDRFieldAreaBinary(itDR, drBinary, drBinaryLens);
				}


				//auto lambdaFindDRLength = [=](const std::string& fieldTag)->int
				//{
				//	for (auto it = drLengths.begin(); it != drLengths.end(); it++)
				//	{
				//		std::string fieldTagItem = std::get<0>(*it);
				//		if (fieldTagItem == fieldTag)
				//		{
				//			int len = std::get<1>(*it);
				//			return len;
				//		}
				//	}
				//	return -1;
				//};

				//[==DR directory
				std::string drDirectoryContent;
				int iSizeOfFieldLengthField = itDR->drLeader.entryMap.sizeOfFieldLengthField - '0';
				int iSizeOfFieldPositionField = itDR->drLeader.entryMap.sizeOfFieldPositionField - '0';

				int iFieldPosition = 0;
				int iFieldLength = 0;
				std::vector<std::string> drDirectory;
				{
					//createDRDirectory(itDR->drDirectory.fieldTag, drDirectory);//
					createDRDirectory(*itDR, drDirectory);
					for (int i = 0; i < drDirectory.size(); i++)
					{
						std::string fieldTag = drDirectory[i];
						iFieldLength = drBinaryLens[i];
						std::string sFieldLength = Utils::intToString(iFieldLength, iSizeOfFieldLengthField);
						std::string sFieldPosition = Utils::intToString(iFieldPosition, iSizeOfFieldPositionField);
						drDirectoryContent += fieldTag;
						drDirectoryContent += sFieldLength;
						drDirectoryContent += sFieldPosition;

						iFieldPosition += iFieldLength;
					}
				}
				drDirectoryContent.push_back(char(ISO8211_FLAG::FT));
				//===]

				//DR leader
				int iDRLeaderSize = sizeof(itDR->drLeader);
				int iDRRecordLength = iDRLeaderSize + drDirectoryContent.size() + drBinary.size();
				int iBaseAddressOfFieldArea = iDRLeaderSize + drDirectory.size() * (4 + iSizeOfFieldLengthField + iSizeOfFieldPositionField) + 1;
				std::string sDRRecordLength = Utils::intToString(iDRRecordLength, 5);
				std::string sBaseAddressOfFieldArea = Utils::intToString(iBaseAddressOfFieldArea, 5);
				{//DR Leader
					memcpy(&itDR->drLeader.recordLength, sDRRecordLength.c_str(), 5);
					memcpy(&itDR->drLeader.baseAddressOfFieldArea, sBaseAddressOfFieldArea.c_str(), 5);
				}

				std::vector<char> drLeaderBytes;
				drLeaderBytes.resize(iDRLeaderSize);
				memcpy(&drLeaderBytes[0], &itDR->drLeader, iDRLeaderSize);
				drBinary.insert(drBinary.begin(), drDirectoryContent.begin(), drDirectoryContent.end());//write dr directory from head
				drBinary.insert(drBinary.begin(), drLeaderBytes.begin(), drLeaderBytes.end());//write dr leader from head
				drsBinary.insert(drsBinary.end(), drBinary.begin(), drBinary.end());
			}
		}

		//=]

		//[==1、Create DDR Leader,write binary last
		{
			int iBaseAddressOfFieldArea = 24;
			iBaseAddressOfFieldArea += ddrFieldArea.size() * 16 + 1;//Field tag leader+end flag
			DDRLeader ddrLeader;
			ddrLeader.entryMap.sizeOfFieldLengthField = char(iSizeOfFieldLengthField) + '0';
			ddrLeader.entryMap.sizeOfFieldPositionField = char(iSizeOfFieldPositionField) + '0';
			//Calc
			int iDDRLeaderSize = sizeof(ddrLeader);//==24
			int iRecordLength = iDDRLeaderSize + binary.size();
			std::string sRecrodLength = Utils::intToString(iRecordLength, 5);
			memcpy(ddrLeader.recordLength, sRecrodLength.c_str(), 5);
			std::string sBaseAddressOfFieldArea = Utils::intToString(iBaseAddressOfFieldArea, 5);
			memcpy(ddrLeader.baseAddressOfFieldArea, sBaseAddressOfFieldArea.c_str(), 5);
			//write last
			std::vector<char> ddrLeaderBytes;
			ddrLeaderBytes.resize(iDDRLeaderSize);
			memcpy(&ddrLeaderBytes[0], &ddrLeader, iDDRLeaderSize);
			binary.insert(binary.begin(), ddrLeaderBytes.begin(), ddrLeaderBytes.end());//write from head
		}
		//==]
		//
		//[==2、Create DRs
		{
			binary.insert(binary.end(), drsBinary.begin(), drsBinary.end());
		}
		//==]
	}

	return binary;
}

void ISO8211::createDRFieldAreaBinary(std::vector<DR>::iterator& itDR, std::vector<char>& drBinary, std::vector<int>& drBinaryLens)
{
	int iDRBinaryLen = 0;
	for (auto itDRRecord = itDR->drFieldArea.drRecords.begin(); itDRRecord != itDR->drFieldArea.drRecords.end(); itDRRecord++)
	{
		std::string fieldTag = itDRRecord->fieldTag;
		std::string type = itDRRecord->type;
		std::vector<char> binary = itDRRecord->binary;
		ISO8211_FLAG emFlag = itDRRecord->emFlag;

		drBinary.insert(drBinary.end(), binary.begin(), binary.end());//subfield value
		iDRBinaryLen += binary.size();

		if (emFlag == ISO8211_FLAG::UT)
		{
			drBinary.push_back(char(ISO8211_FLAG::UT));
			iDRBinaryLen += 1;
		}
	}
	drBinary.push_back(char(ISO8211_FLAG::FT)); iDRBinaryLen += 1;
	drBinaryLens.push_back(iDRBinaryLen);

	for (auto itDRChild = itDR->drs.begin(); itDRChild != itDR->drs.end(); itDRChild++)
	{
		createDRFieldAreaBinary(itDRChild, drBinary, drBinaryLens);
	}
}



void ISO8211::createDRDirectory(const DR& dr, std::vector<std::string>& drDirectory)
{
	createDRDirectory(dr.drFieldArea.fieldTag, drDirectory);
	for (auto itDRChild = dr.drs.begin(); itDRChild != dr.drs.end(); itDRChild++)
	{
		std::vector<std::string> childs;
		createDRDirectory(itDRChild->drFieldArea.fieldTag, childs);
		for (auto itChild = childs.begin(); itChild != childs.end(); itChild++)
		{
			auto itFind = std::find(drDirectory.begin(), drDirectory.end(), *itChild);
			if (itFind == drDirectory.end())
			{
				drDirectory.push_back(*itChild);
			}
		}
	}
}

void ISO8211::createDRDirectory(const std::string& fieldTag, std::vector<std::string>& drDirectory)
{
	drDirectory.insert(drDirectory.begin(), fieldTag);
	auto itFind = mMetaDDRDirectoryParent.find(fieldTag);
	if (itFind != mMetaDDRDirectoryParent.end())
	{
		auto fieldTagParent = itFind->second;
		if (!fieldTagParent.empty())
		{
			createDRDirectory(fieldTagParent, drDirectory);//Parent
		}
	}
}



void ISO8211::ddrDirectoryFunction(const std::string& fieldTag, const std::string& fieldTagParent, std::function<void(const std::string&)> func, bool debugInfo, int iTabCount)
{

	if (debugInfo)
	{
		std::stringstream ss;
		for (int i = 0; i < iTabCount; i++)
		{
			ss << "\t";
		}
		ss << fieldTag;
		std::string s = ss.str();
		Utils::log(s.c_str());
	}

	auto itFind = mMetaDDRDirectory.find(fieldTag);
	if (itFind == mMetaDDRDirectory.end())
	{
		func(fieldTag);
		return;
	}

	for (auto it = itFind->second.begin(); it != itFind->second.end(); it++)
	{
		ddrDirectoryFunction(*it, fieldTag, func, debugInfo, iTabCount + 1);
	}
}


DRFieldArea ISO8211::createDRFieldArea(const std::string& fieldTag)
{
	DRFieldArea drFieldArea;
	drFieldArea.fieldTag = fieldTag;
	auto itFind = mMetaDDRFieldsTable.find(fieldTag);
	if (itFind != mMetaDDRFieldsTable.end())
	{
		int index = 0;
		for (auto it = itFind->second.mSubfieldInfos.begin(); it != itFind->second.mSubfieldInfos.end(); it++, index++)
		{
			std::string fieldTag = it->fieldTag;
			std::string ascType = it->ascFormat;
			std::string binType = it->binFormat;
			std::string type = GetType(ascType, binType);
			drFieldArea._subfieldIndics[fieldTag] = index;
			DRRecord drRecord(fieldTag, type, std::vector<char>(), ISO8211_FLAG::Unknown);
			drFieldArea.drRecords.push_back(drRecord);
		}
	}
	return drFieldArea;
}


DataFormat ISO8211::createSubfield(const TypeValue& typeValue, Subfield& buffer)
{
	DataFormat dataFormat = DataFormat::Parse(typeValue.type);
	switch (dataFormat.emDataFormatType)
	{
	case DataFormatType::UByte:
	{
		switch (dataFormat.Width)
		{
		case 1:buffer.setUInt8Value(typeValue.intValue); break;
		case 2:buffer.setUInt16Value(typeValue.intValue); break;
		case 4:buffer.setUInt32Value(typeValue.intValue); break;
		}
	}
	break;
	case DataFormatType::SByte:
	{
		switch (dataFormat.Width)
		{
		case 1:buffer.setInt8Value(typeValue.intValue); break;
		case 2:buffer.setInt16Value(typeValue.intValue); break;
		case 4:buffer.setInt32Value(typeValue.intValue); break;
		}
	}
	break;
	case DataFormatType::ASC:
	{
		int iLen = typeValue.binLength;
		if (iLen < 0)
		{
			if (typeValue.binValue)
			{
				iLen = strlen(typeValue.binValue);
				std::vector<char> bin;
				bin.resize(iLen);
				memcpy(&bin[0], typeValue.binValue, iLen);
				bin.push_back(char(ISO8211_FLAG::UT));
				buffer.setBinary(bin);
			}
			else
			{
				assert(false);
			}
		}
		else
			assert(false);
	}
	break;
	case DataFormatType::Bin:
	{
		if (dataFormat.Width > 0)
		{
			std::vector<char> bin;
			bin.resize(dataFormat.Width / 8);
			int iCopyLen = 0;
			if (typeValue.binLength < 0)
			{
				iCopyLen = strlen(typeValue.binValue);
			}
			if (iCopyLen < bin.size())
				iCopyLen = bin.size();
			memcpy(&bin[0], typeValue.binValue, iCopyLen);

			buffer.setBinary(bin);
		}
		else
			assert(false);
	}
	break;
	default:
		assert(false);
	}
	return dataFormat;
}

void ISO8211::writeToDRRecords(DRFieldArea& drFieldArea, const std::string& fieldTag, int value)
{
	auto itFindSubfield = drFieldArea._subfieldIndics.find(fieldTag);
	if (itFindSubfield != drFieldArea._subfieldIndics.end())
	{
		int iSubfield = itFindSubfield->second;
		auto itDRFieldTag = mMetaDDRFieldsTable.find(drFieldArea.fieldTag);
		auto subfieldInfo = itDRFieldTag->second.mSubfieldInfos[iSubfield];

		std::string ascType = subfieldInfo.ascFormat;
		std::string binType = subfieldInfo.binFormat;
		std::string type = GetType(ascType, binType);
		TypeValue typeValue(type, value);
		Subfield buffer;
		DataFormat dataFormat = createSubfield(typeValue, buffer);
		auto bytes = buffer.binary();
		//auto testValue = buffer.uintValue();
		std::string subfieldName = fieldTag;
		DRRecord subfieldBinary(subfieldName, type, bytes, dataFormat.emFlag);
		//update
		drFieldArea.drRecords[iSubfield] = subfieldBinary;
	}
}

void ISO8211::writeToDRRecords(DRFieldArea& drFieldArea, const std::string& fieldTag, const std::vector<TypeValue>& typeValues)
{
	auto itFindSubfield = drFieldArea._subfieldIndics.find(fieldTag);
	if (itFindSubfield != drFieldArea._subfieldIndics.end())
	{
		ISO8211_FLAG emISO8211Flag = ISO8211_FLAG::Unknown;
		if (fieldTag[0] == '*')
			emISO8211Flag = ISO8211_FLAG::FT;

		int iSubfield = itFindSubfield->second;
		auto itDRFieldTag = mMetaDDRFieldsTable.find(drFieldArea.fieldTag);
		auto subfieldInfo = itDRFieldTag->second.mSubfieldInfos[iSubfield];

		std::string ascType = subfieldInfo.ascFormat;
		std::string binType = subfieldInfo.binFormat;
		std::string type = GetType(ascType, binType);
		std::vector<char> bytes;
		for (auto itTypeValue = typeValues.begin(); itTypeValue != typeValues.end(); itTypeValue++)
		{
			Subfield buffer;
			DataFormat dataFormat = createSubfield(*itTypeValue, buffer);
			auto bin = buffer.binary();
			bytes.insert(bytes.end(), bin.begin(), bin.end());
		}
		std::string subfieldName = fieldTag;
		DRRecord subfieldBinary(subfieldName, type, bytes, emISO8211Flag);
		//update
		drFieldArea.drRecords[iSubfield] = subfieldBinary;
	}
}


std::string ISO8211::GetType(const std::string& ascType, const std::string& binType)
{
	std::string type = binType;
	if (type.size() == 0)
		type = ascType;
	return type;
}

void ISO8211::writeToDRRecords(DRFieldArea& drFieldArea, const std::string& fieldTag, const char* value, int valueLen)
{
	auto itFindSubfield = drFieldArea._subfieldIndics.find(fieldTag);
	if (itFindSubfield != drFieldArea._subfieldIndics.end())
	{
		int iSubfield = itFindSubfield->second;
		auto itDRFieldTag = mMetaDDRFieldsTable.find(drFieldArea.fieldTag);
		auto subfieldInfo = itDRFieldTag->second.mSubfieldInfos[iSubfield];
		std::string ascType = subfieldInfo.ascFormat;
		std::string binType = subfieldInfo.binFormat;
		auto type = GetType(ascType, binType);
		DataFormat dataFormat = DataFormat::Parse(type);

		std::vector<char> bytes;
		int len = valueLen;
		if (len < 0 && value)
		{
			len = strlen(value);
		}
		//
		if (len < 0)
			len = 0;
		bytes.resize(len);
		if (len > 0)
		{
			memcpy(&bytes[0], value, len);
		}
		DRRecord subfieldBinary(fieldTag, type, bytes, dataFormat.emFlag);
		drFieldArea.drRecords[iSubfield] = subfieldBinary;
	}
}



bool ISO8211::save(const std::string& path)
{
	auto binary = createBinary();
	bool bRet = Utils::saveBinary(path.c_str(), binary);
	return bRet;
}


