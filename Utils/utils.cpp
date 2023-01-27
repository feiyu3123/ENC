#include "utils.h"
#include <ctime>
//#include "iconv/iconv.h"

#include <QTextCodec>

#ifdef WIN32
#include <io.h>
#include <direct.h>
#include <stdarg.h>
#include <Windows.h>
#else
#include <sys/stat.h>
#endif // WIN32

#include <QDebug>

void Utils::log(const std::string& msg)
{
	std::string line = msg;
	line += "\n";
#ifdef WIN32
	OutputDebugStringA(line.c_str());
#endif
}

std::vector<char> Utils::convertToIntList(const std::string& inst)//逗号分隔整形数组
{
	std::string strInst = inst;
	strInst = Utils::trim(strInst);
	std::vector<char> bytes;
	std::vector<std::string> lines = Utils::split(strInst, ",");
	for (auto itLine = lines.begin(); itLine != lines.end(); itLine++)
	{
		char value = stoi(*itLine);
		bytes.push_back(value);
	}
	bytes.push_back('\0');
	return bytes;
}

std::string Utils::replace(std::string& str, const std::string& replaceString, const std::string& newString)
{
	for (std::string::size_type pos(0); pos != std::string::npos; pos += newString.length())
	{
		pos = str.find(replaceString, pos);
		if (pos != std::string::npos)
			str.replace(pos, replaceString.length(), newString);
		else
			break;
	}
	return str;
}

bool Utils::saveBinary(const char* path, const std::vector<char>& bytes)
{
	bool bRet = false;
	std::ofstream out(path, std::ios::out | std::ios::binary);
	if (out.is_open())
	{
		out.write(&bytes[0], bytes.size());
		out.flush();
		bRet = true;
	}
	return bRet;
}

bool Utils::loadBinary(const char* path, std::vector<char>& bytes)
{
	bool bRet = false;
	std::ifstream in(path, std::ios::in | std::ios::binary | std::ios::ate);
	if (in.is_open())
	{
		long size = in.tellg();
		in.seekg(0, std::ios::beg);
		char* buffer = new char[size];
		if (buffer != nullptr)
		{
			in.read(buffer, size);
			bytes.resize(size);
			memcpy(&bytes[0], buffer, size);
		}
		in.close();
		delete[] buffer;
		bRet = true;
	}
	return bRet;
}

bool Utils::loadText(const char* path, std::string& text)
{
	text.clear();
	std::vector<char> bytes;
	bool bRet = loadBinary(path, bytes);
	if (bRet)
	{
		text = std::string(&bytes[0], bytes.size());
	}
	return bRet;
}

std::string Utils::formatString(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	char buffer[2048];
	memset(buffer, 0, sizeof(buffer));
	vsprintf(buffer, format, ap);
	va_end(ap);
	std::string str(buffer);
	return str;
}

int Utils::indexOf(const std::string& str, const char* findstr, int index)
{
	int iRet = -1;
	std::string::size_type find_index = str.find(findstr, index);
	if (find_index != std::string::npos) {
		iRet = find_index;
	}
	return iRet;
}





std::vector<std::string> Utils::split(const std::string& str, const char* spliter)
{
	std::vector<std::string> lines;

	if ("" == str) return lines;

	std::string strSpliter(spliter);
	std::string::size_type pos2 = str.find(strSpliter);
	std::string::size_type pos1 = 0;
	while (std::string::npos != pos2)
	{
		lines.push_back(str.substr(pos1, pos2 - pos1));

		pos1 = pos2 + strSpliter.size();
		pos2 = str.find(strSpliter, pos1);
	}
	if (pos1 != str.length())
		lines.push_back(str.substr(pos1));
	return lines;
}

bool Utils::endsWith(const std::string& str, const char* tail) {
	int iTailLen = strlen(tail);
	if (str.length() == 0 || iTailLen == 0 || iTailLen > str.length())
		return false;

	return str.compare(str.size() - iTailLen, iTailLen, tail) == 0;
}

bool Utils::startWith(const std::string& str, const char* head) {
	int iHeadLen = strlen(head);
	if (str.length() == 0 || iHeadLen == 0 || iHeadLen > str.length())
		return false;
	return str.compare(0, iHeadLen, head) == 0;
}

std::string Utils::leftTrim(const std::string& s, const char* trimstr)
{
	std::string str = s;
	for (;;)
	{
		if (str.length() == 0)
			break;
		int len = strlen(trimstr);
		size_t iFind = str.find(trimstr);
		if (iFind == std::string::npos || iFind != 0)
		{
			break;
		}
		str = str.substr(len, str.size() - len);
	}
	return str;
}

std::string Utils::rightTrimZero(const std::string& str)
{
	std::string s(str);
	s = Utils::rightTrim(s, "0");
	s = Utils::rightTrim(s, ".");
	return s;
}

std::string Utils::rightTrim(const std::string& s, const char* trimstr)
{
	std::string str = s;
	int trimstrLen = strlen(trimstr);
	for (;;)
	{
		if (endsWith(str, trimstr))
		{
			int len = trimstrLen;
			int iTail = str.length() - len;
			str = str.substr(0, iTail);
		}
		else
		{
			break;
		}
	}
	return str;
}

std::string Utils::trim(const std::string& s, const char* trimstr)
{
	std::string str = rightTrim(s, trimstr);
	str = leftTrim(str, trimstr);
	return str;
}

std::string Utils::intToString(int value, int intStringLen)
{
	std::stringstream ssValue; ssValue << value;
	std::string sValue = ssValue.str();
	for (int i = sValue.size(); i < intStringLen; i++)
	{
		sValue.insert(0, "0");
	}
	return sValue;
}

int Utils::stingToInt(const char* bytes, int len)
{
	std::string str(bytes, len);
	int iRet = stoi(str);
	return iRet;
}

bool Utils::isNumberAsc(const std::string& data)
{
	bool bRet = isNumberAsc(data.c_str(), data.size());
	return bRet;
}

bool Utils::isNumberChar(char ch)
{
	bool bRet = false;
	if (ch >= '0' && ch <= '9')
	{
		bRet = true;
	}
	return bRet;
}

bool Utils::isNumberAsc(const char* data, int dataLen)
{
	bool bRet = true;
	for (int i = 0; i < dataLen; i++)
	{
		char ch = data[i];
		bRet = isNumberChar(ch);
		if (!bRet)
			break;
	}
	return bRet;
}



bool Utils::isHexChar(char ch)
{
	bool bRet = false;
	if ((ch >= '0' && ch <= '9')
		|| (ch >= 'A' && ch <= 'F')
		|| (ch >= 'a' && ch <= 'f'))
	{
		bRet = true;
	}
	return bRet;
}

bool Utils::isHexString(const std::string& data, int iFrom)
{
	if (data.empty())
		return false;

	int iStart = 0;
	if (iFrom > -1)
		iStart = iFrom;

	for (int i = iStart; i < data.size(); i++)
	{
		char ch = data[i];
		bool check = isHexChar(ch);
		if (!check)
			return false;
	}
	return true;
}

std::string Utils::hexToString(const std::vector<char>& data, char splitChar)
{
	std::stringstream ss;
	for (int i = 0; i < data.size(); i++)
	{
		char buffer[255] = { 0 };
		char ch = data[i];
		sprintf(buffer, "%02X", (unsigned char)ch);
		ss << buffer;
		if (splitChar)
			ss << splitChar;
	}
	std::string s = ss.str();
	return s;
}

std::vector<char> Utils::stringToHex(const std::string& data)
{
	std::vector<char> buffer(data.begin(), data.end());
	auto ret = compressHex(buffer);
	return ret;
}

std::vector<char> Utils::compressHex(const std::vector<char>& data)
{
	std::vector<char> hex;
	for (int i = 0; i < data.size(); i += 2)
	{
		char buffer[4] = { 0 };
		memcpy(buffer, &data[0] + i, 2);
		std::string s(buffer);
		char ch = stoi(s, 0, 16);
		hex.push_back(ch);
	}
	return hex;
}

bool Utils::parseYYYYMMDD(const std::string& str, time_t& datetime)
{
	if (str.length() >= 8)
	{
		int year = stoi(str.substr(0, 4));
		if (year <= 0) {
			return false;
		}
		tm date{};
		date.tm_year = year - 1900;
		int month = stoi(str.substr(4, 2));
		if (month <= 0) {
			return false;
		}
		date.tm_mon = month - 1;
		int day = stoi(str.substr(6, 2));
		if (day <= 0) {
			return false;
		}
		date.tm_mday = day;
		datetime = std::mktime(&date);
		if (datetime < 0)
			return false;
		return true;
	}
	return false;
};

void Utils::extractPath(const std::string& path, std::string& dir, std::string& fileName, std::string& extName)
{
	dir = "";
	fileName = "";
	extName = "";
	//
	std::string path_ = path;
	path_ = formatDir(path_);
	int iDot = path_.find_last_of('.');
	std::string file_ = path_;
	if (iDot > -1)
	{
		extName = path_.substr(iDot + 1, path_.size() - iDot - 1);
		file_ = path_.substr(0, iDot);
	}

	int iSplit = file_.find_last_of('/');
	if (iSplit == (file_.size() - 1))
	{
		dir = file_;
	}
	else
	{
		dir = file_.substr(0, iSplit);
		fileName = file_.substr(iSplit + 1, file_.size() - iSplit - 1);
	}
}

std::string Utils::getPathDir(const std::string& filePath)
{
	std::string dirPath = filePath;
	size_t p = filePath.find_last_of('/');
	if (p != -1)
	{
		dirPath.erase(p);
	}
	return dirPath;
}

bool Utils::fileExists(const std::string& path)
{
#ifdef WIN32
	if (_access(path.c_str(), 0) != -1) { //io.h
		return true;
	}
	else {
		return false;
	}
#else
	struct stat buf;
	errno_t err = 0;

	if (stat(path, &buf) == 0) {
		return buf.st_mode & S_IFDIR;
	}
	else {
		return 0;
	}
#endif

}

int Utils::secondsToDay(int s)
{
	int day = s / 86400;
	return day;
}

std::string Utils::generateKey(int keyLen)
{
	char ascii[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
					 'A', 'B', 'C', 'D', 'E', 'F', 'G',
					 'H', 'I', 'J', 'K', 'L', 'M', 'N',
					 'O', 'P', 'Q', 'R', 'S', 'T',
					 'U', 'V', 'W', 'X', 'Y', 'Z'
	};
	int asciiSize = sizeof(ascii);
	srand(time(NULL));
	std::string strRet;

	for (int i = 0; i < keyLen; i++)
	{

		int idx = rand() % (asciiSize - 1);
		char buffer[16] = { 0 };
		sprintf_s(buffer, "%c", ascii[idx]);
		strRet.push_back(*buffer);
	}
	return strRet;
}

void Utils::createDir(const std::string& dir_)
{
	std::string dir = dir_;
#ifdef WIN32
	if (_access(dir.c_str(), 00) == 0)
	{
		return;
	}

	std::list <std::string> dirList;
	dirList.push_front(dir);

	std::string curDir = getPathDir(dir);
	while (curDir != dir)
	{
		if (_access(curDir.c_str(), 00) == 0)
		{
			break;
		}

		dirList.push_front(curDir);

		dir = curDir;
		curDir = getPathDir(dir);
	}

	for (auto it : dirList)
	{
		_mkdir(it.c_str());
	}
#else
	assert(0);
#endif // WIN32


	}

std::string Utils::formatDir(const std::string& dir, bool appendFlag, char split)
{
	std::string dir_ = dir;
	std::replace(dir_.begin(), dir_.end(), '\\', split);
	std::replace(dir_.begin(), dir_.end(), '/', split);
	if (appendFlag && dir_.size() > 0 && dir_[dir_.size() - 1] != split)
	{
		dir_ += split;
	}
	return dir_;
}

bool Utils::copyFile(const std::string& copyFrom, const std::string& copyTo)
{
	FILE* fileFrom = fopen(copyFrom.c_str(), "rb");
	if (!fileFrom)
	{
		fclose(fileFrom);
		return false;
	}
	FILE* fileTo = fopen(copyTo.c_str(), "wb");
	if (!fileTo)
	{
		fclose(fileTo);
		return false;
	}

	unsigned char* bytes;
	unsigned int length;
	fseek(fileFrom, 0, SEEK_END);
	length = ftell(fileFrom);
	bytes = new unsigned char[length + 1];
	memset(bytes, 0, length + 1);
	fseek(fileFrom, 0, SEEK_SET);
	fread(bytes, length, 1, fileFrom);

	fwrite(bytes, length, 1, fileTo);

	fclose(fileFrom);
	fclose(fileTo);
	delete[] bytes;
	return true;
}

std::vector<std::string> Utils::getDirectoryFiles(const std::string& dir)
{
	std::string path = dir;
	std::vector<std::string> list;
	//文件句柄
	intptr_t hFile = 0;
	//文件信息，_finddata_t需要io.h头文件
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之
			//如果不是,加入列表
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					p = formatDir(p.assign(path).append("\\").append(fileinfo.name));
					auto files = getDirectoryFiles(p);
					list.insert(list.end(), files.begin(), files.end());
				}
			}
			else
			{
				p = formatDir(p.assign(path).append("\\").append(fileinfo.name));
				list.push_back(p);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	return list;
}

void Utils::encodeConvert(const char* fromCode, const char* fromCodeString, int iFromCodeStringSize, const char* toCode, std::vector<char>& toCodeString)
{
	QByteArray fromBytes(fromCodeString, iFromCodeStringSize);
	QTextCodec* fromCodec = QTextCodec::codecForName(fromCode);
	QString strUnicode = fromCodec->toUnicode(fromBytes);
	QTextCodec* toCodec = QTextCodec::codecForName(toCode);
	QByteArray toBytes = toCodec->fromUnicode(strUnicode);
	toCodeString.clear();
	if (toBytes.size() > 0)
	{
		toCodeString.resize(toBytes.size());
		memcpy(toCodeString.data(), toBytes.data(), toBytes.size());
	}
}

int Utils::stringCharCount(const char* str, int charSize)
{
	char* buffer = (char*)str;
	int iCount = 0;
	while (true)
	{
		bool end = true;
		for (int i = 0; i < charSize; i++)
		{
			char ch = buffer[i];
			if (ch != 0)
			{
				end = false;
				break;
			}
		}
		if (end)
			break;
		buffer += charSize;
		iCount++;
	}
	return iCount;
}


bool Utils::intersect(double x, double y, double* points, int pointsCount, int doubleCount)//
{
	bool odd = false;
	int j = pointsCount - 1;
	for (int i = 0; i < pointsCount; i++)
	{
		if (points[i * doubleCount + 1] < y && points[j * doubleCount + 1] >= y || points[j * doubleCount + 1] < y && points[i * doubleCount + 1] >= y)
		{
			if (points[i * doubleCount] + (y - points[i * doubleCount + 1]) / (points[j * doubleCount + 1] - points[i * doubleCount + 1]) * (points[j * doubleCount] - points[i * doubleCount]) < x)
			{
				odd = !odd;
			}
		}
		j = i;
	}
	return odd;
}

bool Utils::intersect(double* points1, int points1Count, int points1DoubleCount, double* points2, int points2Count, int points2DoubleCount)
{
	for (int iPoint = 0; iPoint < points1Count; iPoint++)
	{
		double x = points1[iPoint * points1DoubleCount];
		double y = points1[iPoint * points1DoubleCount + 1];
		bool isIntersect = intersect(x, y, points2, points2Count, points2DoubleCount);
		if (isIntersect)
			return true;
	}

	for (int iPoint = 0; iPoint < points2Count; iPoint++)
	{
		double x = points2[iPoint * points2DoubleCount];
		double y = points2[iPoint * points2DoubleCount + 1];
		bool isIntersect = intersect(x, y, points1, points1Count, points1DoubleCount);
		if (isIntersect)
			return true;
	}
	return false;
}


int Utils::findNearPoint(double* points, int pointsCount, double x, double y, int doubleCount)
{
	int iIndex = -1;
	double distance = 0;
	for (int i = 0; i < pointsCount; i++)
	{
		double ax = points[i * doubleCount];
		double ay = points[i * doubleCount + 1];
		double a = ax - x;
		double b = ay - y;
		double d = sqrt(a * a + b * b);
		if (i == 0)
		{
			distance = d;
			iIndex = i;
		}
		else
		{
			if (d < distance)
				iIndex = i;
		}
	}
	return iIndex;
}

void Utils::gravity(double* points, int pointsCount, double& x, double& y, int doubleCount)
{
	double px, py, sx, sy;
	double tp, area = 0, tpx = 0, tpy = 0;
	px = points[0]; py = points[1];
	for (int i = 1; i <= pointsCount; i++)
	{
		int iPoint = (i == pointsCount) ? 0 : i;
		sx = points[iPoint * doubleCount];
		sy = points[iPoint * doubleCount + 1];
		tp = px * sy - sx * py;
		area += tp / 2.0;
		tpx += (px + sx) * tp;
		tpy += (py + sy) * tp;
		px = sx;
		py = sy;
	}
	sx = tpx / (6 * area);
	sy = tpy / (6 * area);
	x = sx;
	y = sy;
}

//求面积
double Utils::area(double* points, int pointsCount, int doubleCount)
{
	if (pointsCount < 3) return 0;

	double dArea(0.0);
	for (int i = 0; i < pointsCount; ++i)
	{
		//dArea += polygon[i].y * (polygon[(i - 1 + size) % size].x - polygon[(i + 1) % size].x);
		dArea += points[i * doubleCount + 1] * (points[((i - 1 + pointsCount) % pointsCount) * doubleCount] - points[((i + 1) % pointsCount) * doubleCount]);
	}

	return fabs(dArea / 2);
}

double Utils::div(double a, double b)
{
	double c = a / b;
	return c;
}
