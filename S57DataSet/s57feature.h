#ifndef S57FEATURE_H
#define S57FEATURE_H

#include "s57dataset_global.h"
#include "../ISO8211/iso8211.h"
#include "s57type.h"
#include "s57geometry.h"



class S57DATASET_EXPORT S57Feature
{
public:
	S57Feature();
	~S57Feature();

public://S57
	std::string name();
	uint32 rcid();
	S57GeometryType geometryType();
	std::string geometryTypeString();

	void setObjectClasses(S57ObjectClasses* objectClasses);

	void setGeometry(S57Geometry* geometry);
	S57Geometry* geometry();

	std::map<std::string, S57Field> mFields;
protected:
	S57ObjectClasses* mObjectClasses;
	S57Geometry* mGeometry;
public://ISO8211 Attributes
	//ATTF
	std::vector<ATTFRecord> mATTFs;
	//NATF
	std::vector<NATFRecord> mNATFs;
	//FRID
	FRIDRecord mFRID;
	//FOID
	FOIDRecord mFOID;

	uint64 LNAM();

	//FFPC
	FFPCRecord mFFPC;
	//FFPT
	std::vector<FFPTRecord> mFFPTs;
	//FSPC
	FSPCRecord mFSPC;
	//FSPT
	std::vector<FSPTRecord> mFSPTs;
};

#endif // S57FEATURE_H
