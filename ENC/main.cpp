#include "mainwindow.h"
#include <QApplication>

#include <array>

#include "../S57DataSet/s57dataset.h"
#include "../Utils/utils.h"

int main(int argc, char *argv[])
{
	//加载外部资源
	S57ObjectClasses objectClasses;
	objectClasses.load("D:/opensource/ENC/bin/debug/script/Object Classes.txt");

	S57AttributesType attributesType;
	attributesType.load("D:/opensource/ENC/bin/debug/script/AttributesType.txt");

	S57Attributes attributes;
	attributes.load("D:/opensource/ENC/bin/debug/script/Attributes.txt", attributesType);

	S57DataSet dataSetS57;
	dataSetS57.setS57ObjectClasses(&objectClasses);
	dataSetS57.setS57Attributes(&attributes);

	//加载000
	bool ok = dataSetS57.load("D:/opensource/ENC/bin/debug/C1313100.000");

	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
