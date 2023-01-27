#ifndef UTILS_H
#define UTILS_H

#include "utils_global.h"

#include <string>
#include <vector>
#include <array>
#include <list>
#include <sstream>
#include <algorithm>
#include <map>
#include <set>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <functional>


#ifndef delptr
#define delptr(o) if(o){delete o;o=nullptr;}
#endif

template<class T>
void removeAt(std::vector<T>& array, int removeIndex, int removeCount)
{
	int index = 0;
	for (auto it = array.begin(); it != array.end(); index++)
	{
		if (index == removeIndex)
		{
			for (int i = 0; i < removeCount; i++)
			{
				it = array.erase(it);
			}
		}
		else
			++it;  //指向下一个位置
	}
}

template<class T>
void freeVector(std::vector<T*>& list)
{
	for (int i = 0; i < list.size(); i++)
	{
		delete list[i];
	}
	list.clear();
}

template<class T1, class T2>
void vectorConvert(const std::vector<T1>& vec1, std::vector<T2>& vec2)
{
	for (auto it = vec1.begin(); it != vec1.end(); it++)
	{
		T2 item = static_cast<T2>(*it);
		vec2.push_back(item);
	}
}


class UTILSSHARED_EXPORT Utils
{
public:

	static void log(const std::string& msg);
	static bool saveBinary(const char* path, const std::vector<char>& bytes);
	static bool loadBinary(const char* path, std::vector<char>& bytes);
	static bool loadText(const char* path, std::string& text);




	static bool isNumberChar(char ch);
	static bool isNumberAsc(const char* data, int dataLen);
	static bool isNumberAsc(const std::string& data);
	static bool isHexChar(char ch);
	static bool isHexString(const std::string& data, int iFrom = -1);
	static std::string hexToString(const std::vector<char>& data, char splitChar = ' ');
	static std::vector<char> stringToHex(const std::string& data);
	static std::vector<char> compressHex(const std::vector<char>& data);

	static bool parseYYYYMMDD(const std::string& str, time_t& datetime);
	static int secondsToDay(int s);

	static std::string generateKey(int keyLen);

	static std::string formatDir(const std::string& dir, bool appendFlag = false, char split = '/');
	static void extractPath(const std::string& path, std::string& dir, std::string& filename, std::string& extName);
	static std::string getPathDir(const std::string& filePath);
	static void createDir(const std::string& dir);
	static bool copyFile(const std::string& copyFrom, const std::string& copyTo);
	static bool fileExists(const std::string& path);
	static std::vector<std::string> getDirectoryFiles(const std::string& dir);

	static std::vector<char> convertToIntList(const std::string& inst);

	static std::string replace(std::string& str, const std::string& replaceString, const std::string& newString);
	static std::vector<std::string> split(const std::string& str, const char* spliter);
	static std::string formatString(const char* format, ...);
	static int indexOf(const std::string& str, const char* findstr, int index = 0);

	template<class T>
	static int indexOf(const std::vector<T>& bytes, const std::vector<T>& find, int index = 0)
	{
		int iFind = -1;
		for (int iChar = index; iChar < bytes.size() - find.size(); iChar++)
		{
			bool isFind = true;
			for (int iFind = 0; iFind < find.size(); iFind++)
			{
				T item = *&bytes[iChar + iFind];
				T findItem = *&find[0];
				if (item != findItem)
				{
					isFind = false;
					break;
				}
			}
			if (isFind)
			{
				iFind = iChar;
				break;
			}
		}
		return iFind;
	}

	static bool endsWith(const std::string& str, const char* tail);
	static bool startWith(const std::string& str, const char* head);
	static std::string leftTrim(const std::string& str, const char* trimstr = " ");
	static std::string rightTrim(const std::string& str, const char* trimstr = " ");
	static std::string rightTrimZero(const std::string& str);
	static std::string trim(const std::string& str, const char* trimstr = " ");

	static std::string intToString(int value, int intStringLen);
	static int stingToInt(const char* bytes, int len);
	//编码转换
	static void encodeConvert(const char* fromCode, const char* fromCodeString, int iFromCodeStringSize, const char* toCode, std::vector<char>& toCodeString);
	static int stringCharCount(const char* str, int charSize = 1);

	//点与面相交
	static bool intersect(double x, double y, double* points, int pointsCount, int doubleCount = 2);
	//面与面相交
	static bool intersect(double* points1, int points1Count, int points1DoubleCount, double* points2, int points2Count, int points2DoubleCount);
	//求重心
	static void gravity(double* points, int pointsCount, double& x, double& y, int doubleCount = 2);
	//求面积
	static double area(double* points, int pointsCount, int doubleCount = 2);
	//找最近的点
	static int findNearPoint(double* points, int pointsCount, double x, double y, int doubleCount = 2);

	inline static double div(double a, double b);
};

#endif // UTILS_H
