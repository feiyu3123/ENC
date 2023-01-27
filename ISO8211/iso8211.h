#ifndef ISO8211_H
#define ISO8211_H

#include "iso8211_global.h"

#include <string>
#include <map>
#include <vector>
#include <tuple>
#include <functional>

#define ISO8211_0000 "0000"
#define ISO8211_0001 "0001" //ISO/IEC 8211 Record Identifier
//DDR 1600;&
//[
//Data structure code
#define DATA_STRUCTURE_CODE_0 '0' //single data item//
#define DATA_STRUCTURE_CODE_1 '1' //linear structure
#define DATA_STRUCTURE_CODE_2 '2' //multi-dimensional structure
//Data type code
#define DATA_TYPE_CODE_0 '0'//character string 字符串
#define DATA_TYPE_CODE_1 '1'//implicit point (integer) 整型
#define DATA_TYPE_CODE_2 '2'//explicit point (real) 浮点
#define DATA_TYPE_CODE_5 '5'//binary form 二进制
#define DATA_TYPE_CODE_6 '6'//mixed data types//混合数据类型
//Auxiliary controls
#define AUXILIARY_CONTROLS "00"
//Printable graphics
#define PRINTABLE_GRAPHICS ";&"
//Truncated escape sequence
//S57 B.1 Implementation of alternate character sets in S-57
//词汇级别-字符集,DDR Leader下填入Extended character set indicator中,默认" ! "
//Lexical level 0 (ASCII - ISO/IEC 646 IRV)			(2/0) (2/0) (2/0)
//Lexical level 1 (Latin 1 - ISO 8859)				(2/13) (4/1) (2/0)
//Lexical level 2 (Multilingual - ISO/IEC 10646)	(2/5) (2/15) (4/1)==Unicode 4.0
#define TRUNCATED_ESCAPE_SEQUENCE_Lexical_level_0 "   "//(SPACEs)
#define TRUNCATED_ESCAPE_SEQUENCE_Lexical_level_1 "-A "//(-,A,SPACE)
#define TRUNCATED_ESCAPE_SEQUENCE_Lexical_level_2 "%/A"//
//]


enum class ISO8211SHARED_EXPORT DataFormatType
{
	//BIN
	UByte,
	SByte,
	Double,
	//ASC
	INT,
	ASC,
	Real,//以ASC存储的浮点数
	//BIN
	Bin,
};

enum class ISO8211SHARED_EXPORT ISO8211_FLAG
{
	Unknown = 0,
	UT = 0x1F, //单元分隔符
	FT = 0x1E, //记录分隔符,Tail
};

struct ISO8211SHARED_EXPORT DataFormat
{
	DataFormatType emDataFormatType;
	int Width;
	ISO8211_FLAG emFlag;//ISO8211 flag
	static DataFormat Parse(const std::string& strType);
	static int CalcWidth(const char* str, int& width, bool hasBrackets = false);
};


enum class ISO8211SHARED_EXPORT RepeatedType
{
	RepeatedType_ISO8211,
	RepeatedType_S100,
};
//-------------------------------------------------------------------------------------------




struct ISO8211SHARED_EXPORT FieldControlField//现场控制字段
{
	FieldControlField();
	char dataStructureCode;
	char dataTypeCode;
	char auxiliaryControls[2];
	char printableGraphics[2];
	char truncatedEscapeSequence[3];
};

class ISO8211SHARED_EXPORT SubfieldInfo
{
public:
	SubfieldInfo();
	SubfieldInfo(const SubfieldInfo& subfieldInfo);
	SubfieldInfo(const char* subfieldName, const char* fieldTag, const char* ascFormat, const char* binFormat, bool isRepeated);
	std::string subfieldName;
	std::string fieldTag;
	std::string ascFormat;
	std::string binFormat;
	bool isRepeated;
	SubfieldInfo& operator= (const SubfieldInfo& subfieldInfo);
};


class ISO8211SHARED_EXPORT DDRField
{
public:
	//Meta
	FieldControlField mFieldControlField;
	std::string mFieldTag;
	std::string mFieldLabel;
	std::vector<SubfieldInfo> mSubfieldInfos;//Subfield name/Label/ASC Format/Bin Format or Load Format/is Rep
	//Extend
	std::string content();
	int subfieldInfoRepeatedIndex();//S57、S100(*YCOO,XCOO,ZC00) repeated index
	int subfieldInfoS100RepeatedIndex();//S100(\\*NATC) repeated index
};

struct ISO8211SHARED_EXPORT EntryMap
{
	char sizeOfFieldLengthField;//字段[长度字段]的大小'3'字节,按实际情况而定,Variable 1-9 (defined by encoder)
	char sizeOfFieldPositionField;//字段[位置字段]的大小'3'字节,按实际情况而定,Variable 1-9 (defined by encoder)
	char reserved;//保留'0'
	char sizeOfFieldTagField;//字段标记字段大小'4'字节
};

struct ISO8211SHARED_EXPORT DDRLeader
{
	DDRLeader();
	char recordLength[5];//DDR 记录共有**字节数
	char interChangeLevel;//交换级别
	char leaderIdentifier;//'L'
	char inLineCodeExtensionIndicator;//代码扩展指示符'E'
	unsigned char versionNumber;//版本号1
	unsigned char applicationIndicator;//' '
	char fieldControlLength[2];//字段长度'09'
	char baseAddressOfFieldArea[5];//字段区基地址
	char extendCharaterSetIndicator[3];//扩充字符集指示符' ! '
	EntryMap entryMap;
};

struct ISO8211SHARED_EXPORT Subfield
{
	Subfield();
	~Subfield();

	enum class DataType
	{
		Unknown,
		UInt8,
		UInt16,
		UInt32,
		Int8,
		Int16,
		Int32,
		Double,
		ASC,
		Real,
		Binary
	};

	std::string tagName;
	int arrayIndex;//重复时,在数组中的索引
	DataType emDataType = DataType::Unknown;
	std::vector<char> bytes;

	unsigned char uint8Value();
	void setUInt8Value(unsigned char value);

	unsigned short uint16Value();
	void setUInt16Value(unsigned short value);

	unsigned int uint32Value();
	void setUInt32Value(unsigned int value);

	char int8Value();
	void setInt8Value(char value);

	short int16Value();
	void setInt16Value(short value);

	int int32Value();
	void setInt32Value(int value);

	static int convert(const std::string& type, const char* binary, int binaryCount, Subfield& subfield, DataFormat& dataFormat, int iCharSize = 1);

	//Quick
	unsigned int uintValue();
	int intValue();
	double doubleValue();
	std::string stringValue();

	std::string toString();
	std::string typeString();

	std::vector<char> binary();
	void setBinary(const std::vector<char>& binary);

	void setDataTypeFromDataFormat(const DataFormat& dataFormat);

	bool operator==(const Subfield& subfield);
protected:
	void setIntBinary(int value, int width, std::vector<char>& binary);
	std::vector<char> getIntBinary(const char* bytes, int widdth);
};

struct ISO8211SHARED_EXPORT DRLeader
{
	DRLeader();
	char recordLength[5];//DR记录字节数
	char interChangeLevel;//交换级别，默认值:' '
	char leaderIdentifier;//头区标识符'D'
	char inLineCodeExtensionIndicator;//代码扩展指示符,默认值:' '
	unsigned char versionNumber;//版本号,默认值:' '
	unsigned char applicationIndicator;//应用指示器,默认值:' '
	char fieldControlLength[2];//字段长度,默认值:2个空格'  '
	char baseAddressOfFieldArea[5];//字段区基地址(头标+目录长度)
	char extendCharaterSetIndicator[3];//扩充字符集指示符,3个空格
	EntryMap entryMap;
};

struct ISO8211SHARED_EXPORT DRDirectory
{
	std::string fieldTag;
};

//typedef std::vector<std::tuple<std::string, std::vector<char>>> DRRecrods;

class DRRecord
{
public:
	DRRecord();
	DRRecord(const std::string& fieldTag_, const std::string& type_, const std::vector<char>& binary_, const ISO8211_FLAG emFlag_);
	std::string fieldTag;
	std::string type;
	std::vector<char> binary;
	ISO8211_FLAG emFlag;
	//
	int binaryOffset;//序列化文件中的位置
	void setBinaryOffset(int iBinaryOffset);
};

struct ISO8211SHARED_EXPORT DRFieldArea
{
	std::string fieldTag;
	std::vector<DRRecord> drRecords;//Field tag,type,binary,ISO8211 FLAG
	//
	std::map<std::string, int> _subfieldIndics;//sub field indics for write

	static void createSubfield(const DRRecord& drRecord, Subfield& subfield);
};

struct ISO8211SHARED_EXPORT DR
{
	DRLeader drLeader;
	DRDirectory drDirectory;
	DRFieldArea drFieldArea;
	std::vector<DR> drs;//child
	void setFieldTag(const std::string& fieldTag);
};


class ISO8211SHARED_EXPORT TypeValue//类型与值
{
public:
	TypeValue(const std::string& strType, int data);
	TypeValue(const std::string& strType, const char* data, int dataLen = -1);
	~TypeValue();

	std::string type;
	//int
	int intValue;
	//ASC/BIN
	const char* binValue;
	int binLength;
};


class ISO8211SHARED_EXPORT ISO8211
{
public:


	ISO8211();

	bool virtual save(const std::string& path);
	bool virtual load(const std::string& path);

	static std::string GetType(const std::string& ascType, const std::string& binType);
	static int CalcSize(int value);
	//
	static void SetName(unsigned char RCNM, unsigned RCID, char* bytes);
	static void GetNAME(const char* bytes, unsigned char& RCNM, unsigned int& RCID);

	virtual int charSize(const DDRField&);
	virtual void setRecordSubfield(const DRRecord& drRecord, Subfield& subfield);//处理在预加载过程中的变量,例如:S57 NALL,AALL
public://Quick
	void writeToDRRecords(DRFieldArea& drFieldArea, const std::string& fieldTag, int value);
	void writeToDRRecords(DRFieldArea& drFieldArea, const std::string& fieldTag, const char* value, int valueLen = -1);

	void writeToDRRecords(DRFieldArea& drFieldArea, const std::string& fieldTag, const std::vector<TypeValue>& typeValues);
	DRFieldArea createDRFieldArea(const std::string& fieldTag);
protected:
	//S57 meta
	void initMeta();
	std::vector<DDRField> mMetaDDRFields;//Ignore on read
	std::map<std::string, DDRField> mMetaDDRFieldsTable;
	//DDR Directory
	std::map<std::string, std::vector<std::string>> mMetaDDRDirectory;//Field tag,Field tag child list
	std::map<std::string, std::string> mMetaDDRDirectoryParent;//Field tag,Field tag parent
	//
	std::vector<DR> mDRs;
protected:
	//dr
	std::vector<char> createBinary();
	int createRecord(const std::vector<char>& binary, int index, const std::string& type, const std::string& sSubfieldName, DRRecord& drRecord, Subfield& subfield, DataFormat& dataFormat, int iCharSize = 1);
	bool loadBinary(const std::vector<char>& binary, std::string* firstFieldAreaFieldTag = nullptr);
	void outputRecord(int index, int iDRSubfield, int iDRSubfieldSize, int iLength, std::string& sSubfieldName, Subfield& subfield);
	//
	void createDRDirectory(const std::string& fieldTag, std::vector<std::string>& drDirectory);
	//
	void createDRDirectory(const DR& fieldTag, std::vector<std::string>& drDirectory);
	void createDRFieldAreaBinary(std::vector<DR>::iterator& itDR, std::vector<char>& drBinary, std::vector<int>& drBinaryLens);
	//ddr directory function
	void ddrDirectoryFunction(const std::string& fieldTag, const std::string& fieldTagParent, std::function<void(const std::string&)> func, bool debugInfo = false, int iTabCount = -1);
	void createDDRDirectory(const std::string& fieldTag, std::string& ddrDirectory);
	//
	DataFormat createSubfield(const TypeValue& typeValue, Subfield& buffer);
protected:
	std::vector<char> mISO8211Binary;
};


#endif // ISO8211_H
