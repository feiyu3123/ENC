#ifndef S57TYPE_H
#define S57TYPE_H
#include "s57dataset_global.h"
#include <string>
#include <map>

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
//7.2.2.1 Data format
//Subfield data formats are specified by ISO / IEC 8211. The allowable data formats are as follows :
//Format Precision = w Data type
//A*) Character data
//I*) Implicit point representation
//R*) Explicit point representation
//B**) Bit string
//@ subfield label is a row heading for a
//2 - D array or table of known length
//b1w 1, 2, 4 * **) unsigned integer
//b2w 1, 2, 4 * **) signed integer
//table 7.2
//*) An extent of X(n) indicates a fixed length subfield of length n(in bytes).An extent of X() indicates a variable length subfield terminated by the appropriate delimiter(see clause 2.5).
//带"*)" 以分隔符结束
//**) The width of a fixed length bit subfield must be specified in bits.If necessary, the last byte of a fixed length bit subfield must be filled on the right with binary zero’s.
//带"**)" 固定长度不够时填0
//***) In the binary form, numerical data forms are constrained by the precision of the ISO / IEC 8211 binary format.
//带"***)"二进制格式
//Where: Precision is the width of the data items in bytes
//		w is a permitted value of precision
//		unsigned integer is a binary integer
//		signed integer is a two’s complement binary integer
//Binary values and multi - byte character codes(see clause 2.4 and Annex B) must be stored in the “least
//significant byte first”(LSBF or “little - endian”) order.LSBF is an ordering of bytes in which the least
//significant byte is placed closest to the beginning of a file.

//7.2.2.2 Permitted S - 57 (ASCII)Data domains
//The domain for ASCII data is specified by a domain code.The following domain codes are used in the
//field tables :
//Domain code Domain description
//bt Basic text(see clause 2.4)
//gt General text(see clause 2.4)
//dg digits; 0 - 9, right - adjusted and zero filled left(e.g.A(2) "03")
//date a date subfield in the form : YYYYMMDD(e.g. "19960101")
//int integer; ISO 6093 NR1, SPACE, "+", "-", 0 - 9, right - adjusted and zero filled left(e.g.I(5) “00015”)
//real real number; ISO 6093 NR2, SPACE, "+", "-", ".", 0 - 9
//an alphanumerics; A - Z, a - z, 0 - 9, "*", "?"
//hex hexadecimals; A - F, 0 - 9
typedef bool b1;
typedef unsigned char b11;
typedef unsigned short b12;
typedef unsigned int b14;

typedef short b2;
typedef int b24;
typedef float b3, b34;

//ASC
//变长
typedef char* A;
typedef char* B;
typedef char* R;
typedef char* I;
//固定
typedef char A_;
typedef char B_;
typedef char R_;
typedef char I_;

union S57DATASET_EXPORT UnionInt
{
	int intValue;
	struct
	{
		int byte0 : 8;
		int byte1 : 8;
		int byte2 : 8;
		int byte3 : 8;
	}u;
};




#include "s57meta.h"
#include <vector>
#include <string>

#ifndef byte
#define byte unsigned char
#endif

#ifndef uint16
#define uint16 unsigned short
#endif

#ifndef uint64
#define uint64 long long
#endif

#ifndef uint32
#define uint32 unsigned int
#endif

#ifndef ulong
#define ulong unsigned long
#endif





//S57 Field Tag
#define DSID "DSID"//Data Set Identification
#define DSSI "DSSI"//Data Set Structure information
#define DSPM "DSPM"//Data Set Parameter
#define DSPR "DSPR"//Data Set Projection
#define DSRC "DSRC"//Data Set Registration Control
#define DSHT "DSHT"//Data Set History
#define DSAC "DSAC"//Data Set Accuracy
#define CATD "CATD"//Catalogue Directory
#define CATX "CATX"//Catalogue Cross Reference
#define DDDF "DDDF"//Data Dictionary Definition
#define DDDR "DDDR"//Data Dictionary Definition Reference
#define DDDI "DDDI"//Data Dictionary Domain Identifier
#define DDOM "DDOM"//Data Dictionary Domain
#define DDRF "DDRF"//Data Dictionary Domain Reference
#define DDSI "DDSI"//Data Dictionary Schema Identifier
#define DDSC "DDSC"//Data Dictionary Schema
#define FRID "FRID"//Feature Record Identifier
#define FOID "FOID"//Feature Object Identifier
#define ATTF "ATTF"//Feature record attribute
#define NATF "NATF"//Feature record national attribute
#define FFPC "FFPC"//Feature Record to Feature Object Pointer Control
#define FFPT "FFPT"//Feature Record to Feature Object Pointer
#define FSPC "FSPC"//Feature Record to Spatial Record Pointer Control
#define FSPT "FSPT"//Feature Record to Spatial Record Pointer
#define VRID "VRID"//Vector Record Identifier
#define ATTV "ATTV"//Vector Record Attribute
#define VRPC "VRPC"//Vector Record Pointer Control
#define VRPT "VRPT"//Vector Record Pointer
#define SGCC "SGCC"//Coordinate control
#define SG2D "SG2D"//2-D Coordinate
#define SG3D "SG3D"//3-D Coordinate (Sounding Array)
#define ARCC "ARCC"//Arc/Curve definition
#define AR2D "AR2D"//Arc coordinate
#define EL2D "EL2D"//Ellipse coordinates
#define CT2D "CT2D"//Curve Coordinates

//RCNM ASC
#define RCNM_ASC_DS "DS" //Data Set General Information
#define RCNM_ASC_DP "DP" //Data Set Geographic Reference
#define RCNM_ASC_DH "DH" //Data Set History
#define RCNM_ASC_DA "DA" //Data Set Accuracy
#define RCNM_ASC_CD "CD" //Catalogue Directory
#define RCNM_ASC_CR "CR" //Catalogue Cross Reference
#define RCNM_ASC_ID "ID" //Data Dictionary Definition
#define RCNM_ASC_IO "IO" //Data Dictionary Domain
#define RCNM_ASC_IS "IS" //Data Dictionary Schema
#define RCNM_ASC_FE "FE" //Feature
#define RCNM_ASC_VI "VI" //Vector Isolated node
#define RCNM_ASC_VC "VC" //Vector Connected node
#define RCNM_ASC_VE "VE" //Vector Edge
#define RCNM_ASC_VF "VF" //Vector Face
//RCNM BIN
#define RCNM_DS 10 //Data Set General Information
#define RCNM_DP 20 //Data Set Geographic Reference
#define RCNM_DH 30 //Data Set History
#define RCNM_DA 40 //Data Set Accuracy
#define RCNM_CD "*)" //Catalogue Directory
#define RCNM_CR 60 //Catalogue Cross Reference
#define RCNM_ID 70 //Data Dictionary Definition
#define RCNM_IO 80 //Data Dictionary Domain
#define RCNM_IS 90 //Data Dictionary Schema
#define RCNM_FE 100 //Feature
#define RCNM_VI 110 //Vector Isolated node
#define RCNM_VC 120 //Vector Connected node
#define RCNM_VE 130 //Vector Edge
#define RCNM_VF 140 //Vector Face
//PRIM
#define PRIM_P          1       /* point feature */
#define PRIM_L          2       /* line feature */
#define PRIM_A          3       /* area feature */
#define PRIM_N          4       /* non-spatial feature  */





enum class S57DATASET_EXPORT S57DATASET_EXPORT ORNT
{
	ORNT_Forward = 1,//F
	ORNT_Reverse = 2,//R
	ORNT_NULL = 255,
};

enum class S57DATASET_EXPORT S57DATASET_EXPORT USAG
{
	USAG_ExteriorBoundaries = 1,//Exterior boundaries
	USAG_InteriorBoundaries = 2,//Interior boundaries
	USAG_ExteriorBoundaryTruncatedByTheDataLimit = 3,//Exterior boundary truncated by the data limit
	USAG_NULL = 255
};

enum class S57DATASET_EXPORT S57DATASET_EXPORT MASK
{
	MASK_Mask = 1,//Mask
	MASK_Show = 2,//Show
	MASK_MaskingIsNotRelevant = 255//Masking is not relevant
};

enum class S57DATASET_EXPORT S57DATASET_EXPORT TOPI
{
	TOPI_BeginningNode = 1,
	TOPI_EndNode = 2,
	TOPI_LeftFace = 3,
	TOPI_RightFace = 4,
	TOPI_ContainingFace = 5,
	TOPI_NULL = 255
};


class S57DATASET_EXPORT S57ExtRes//外部资源
{
public:
	std::string description;
	std::string acronym;
	int code;
	char type;//Attribute type
};

class S57DATASET_EXPORT S57ObjectClasses
{
public:
	bool load(const std::string& path);
	std::string acronym(int code);
	int code(const std::string& acronym);
public:
	std::vector<S57ExtRes> mObjectClasses;
	std::map<int, std::string> mAcronyms;
	std::map<std::string, int> mCodes;
};

class S57DATASET_EXPORT S57AttributesType
{
public:
	bool load(const std::string& path);
	std::map<int, char> mAttributesType;
};

class S57DATASET_EXPORT S57Attributes
{
public:
	bool load(const std::string& path, const S57AttributesType& attributesType);
	std::string acronym(int code);
	int code(const std::string& acronym);
public:
	std::vector<S57ExtRes> mAttributes;
	std::map<int, std::string> mAcronyms;
	std::map<std::string, int> mCodes;
};

class S57DATASET_EXPORT S57Field
{
public:
	std::string name;//字段名
	std::string value;//字段值文本
	char type;//Attribute 字段类型
};

#endif
