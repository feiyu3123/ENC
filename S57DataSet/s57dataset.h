#ifndef S57SERIALIZE_H
#define S57SERIALIZE_H

#include "s57dataset_global.h"
#include "../ISO8211/iso8211.h"

#include <map>
#include "s57type.h"
#include "s57meta.h"
#include "s57feature.h"
#include "s57spatial.h"

//000数据集
class S57DATASET_EXPORT S57DataSet :public ISO8211
{
public:
	S57DataSet();
	void initMeta();
	void initMeta000();
	void initMetaUpdate();
	std::string createCode(std::string* metaStructs = nullptr, std::string* metaCreateInterfaces = nullptr, std::string* metaCreateInterfaceImplements = nullptr);//C++ struct
	static void CreateCodeOutput();
public:
	bool load(const std::string& path) override;
	bool save(const std::string& path) override;
public://save used
	static void Sample();
	DR createDSID(const DSIDRecord& record);
	DR createDSSI(const DSSIRecord& record);
	DR createDSPM(const DSPMRecord& record);
	DR createDSPR(const DSPRRecord& record);
	DR createDSRC(const std::vector<DSRCRecord>& records);
	DR createDSAC(const DSACRecord& record);
	DR createCATD(const CATDRecord& record);
	DR createCATX(const std::vector<CATXRecord>& records);
	DR createDDDF(const DDDFRecord& record);
	DR createDDDR(const std::vector<DDDRRecord>& records);
	DR createDDDI(const DDDIRecord& record);
	DR createDDOM(const DDOMRecord& record);
	DR createDDRF(const std::vector<DDRFRecord>& records);
	DR createDDSI(const DDSIRecord& record);
	DR createDDSC(const std::vector<DDSCRecord>& records);
	DR createFRID(const FRIDRecord& record);
	DR createFOID(const FOIDRecord& record);
	DR createATTF(const std::vector<ATTFRecord>& records);
	DR createNATF(const std::vector<NATFRecord>& records);
	DR createFFPC(const FFPCRecord& record);
	DR createFFPT(const std::vector<FFPTRecord>& records);
	DR createFSPC(const FSPCRecord& record);
	DR createFSPT(const std::vector<FSPTRecord>& records);
	DR createVRID(const VRIDRecord& record);
	DR createATTV(const std::vector<ATTVRecord>& records);
	DR createVRPC(const VRPCRecord& record);
	DR createVRPT(const std::vector<VRPTRecord>& records);
	DR createSGCC(const SGCCRecord& record);
	DR createSG2D(const std::vector<SG2DRecord>& records);
	DR createSG3D(const std::vector<SG3DRecord>& records);
	DR createARCC(const ARCCRecord& record);
	//DR createAR2D(const AR2DRecord& record);
	//DR createEL2D(const EL2DRecord& record);
	DR createCT2D(const std::vector<CT2DRecord>& records);

public://load used
	DSIDRecord mDSID;//DSID
	DSSIRecord mDSSI;//DSSI
	DSPMRecord mDSPM;//DSPM
	DSACRecord mDSAC;//DSAC
	CATDRecord mCATD;//CATD
	std::vector<S57Feature*> mFeatures;
	void iso8211ConvertToS57Buffer();//转换至 S57 ISO8211 Buffer
	void s57BufferMerge(std::vector<S57DataSet>& dataSets);//S57 ISO8211 Buffer 合并更新数据集
	void createS57Features();
	void createEdges(S57Feature * feature, std::vector<S57Ring2D> &edges);
	//生成最终的S57数据
	std::map<ulong, S57Feature*> mS57FeaturesBuffer;
	std::map<std::tuple<unsigned char, ulong>, S57Spatial*> mS57SpatialsBuffer;//RCNM/RCID/SPATIAL
	void clear();
public://S57 Object classes
	S57ObjectClasses* mS57ObjectClasses;
public://S57 Attributes
	S57Attributes* mS57Attributes;
public:
	void setS57ObjectClasses(S57ObjectClasses* s57ObjectClasses);
	void setS57Attributes(S57Attributes* s57Attributes);
protected://下面是文本的处理
	void setRecordSubfield(const DRRecord& drRecord, Subfield& subfield) override;
	int charSize(const DDRField& ddrField) override;
	int charSize(bool isNATF);
	int textBytesLength(const char* str, int charSize);//字符串字节长度
	std::string textValue(bool isNATF, const char* text, int textBytesLength);
	int mNALL = -1;
	int mAALL = -1;
protected://通用函数
	inline S57Spatial* getSpatial(unsigned char rcnm, ulong rcid);
	inline bool getSG2DPoint(S57Spatial* spatial, double& x, double& y);
	inline bool createRings(const std::vector<S57Ring2D>& edges, S57Ring2D& exteriorRing, std::vector<S57Ring2D>& innerRings);
};

#endif // S57SERIALIZE_H
