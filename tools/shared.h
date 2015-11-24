#include <cstdlib>
#include <algorithm>
#include <limits>
#include <sys/stat.h>
#include "serializer/Serializer.h"

#ifndef NEW_SERIALIZER_COMPARE_SHARED
#define NEW_SERIALIZER_COMPARE_SHARED

struct Bounds {
	int lower, upper;
};

struct IJKLBounds {
	int iLower, iUpper;
	int jLower, jUpper;
	int kLower, kUpper;
	int lLower, lUpper;
};

IJKLBounds getIJKLBounds(const ser::DataFieldInfo& info,
                         const Bounds& iBounds, const Bounds& jBounds,
                         const Bounds& kBounds, const Bounds& lBounds)
{
	IJKLBounds bounds;
	int iSize = info.iSize();
	bounds.iLower = std::max(0, iBounds.lower);
	bounds.iUpper = std::min(iSize - 1, iBounds.upper);
	int jSize = info.jSize();
	bounds.jLower = std::max(0, jBounds.lower);
	bounds.jUpper = std::min(jSize - 1, jBounds.upper);
	int kSize = info.kSize();
	bounds.kLower = std::max(0, kBounds.lower);
	bounds.kUpper = std::min(kSize - 1, kBounds.upper);
	int lSize = info.lSize();
	bounds.lLower = std::max(0, lBounds.lower);
	bounds.lUpper = std::min(lSize - 1, lBounds.upper);
	return bounds;
}

std::ostream& operator << (std::ostream &o, const Bounds &bounds)
{
	o << "(" << bounds.lower << ", " << bounds.upper << ")";
	return o;
}

Bounds string2bounds(std::string boundString)
{
	Bounds bounds = { 0, std::numeric_limits<int>::max() };

	if (boundString[0] != ':')
	{
		const char *s = boundString.c_str();
		char *t;
		bounds.lower = strtol(s, &t, 10);
		boundString = std::string(t);
		if (boundString.length() == 0)
		{
			bounds.upper = bounds.lower;
		}
		else
		{
			boundString = boundString.substr(1);
		}
	}
	else
	{
		boundString = boundString.substr(1);
	}

	if (boundString.length() > 0)
	{
		const char *s = boundString.c_str();
		char *t;
		bounds.upper = strtol(s, &t, 10);
	}

	if (bounds.upper < bounds.lower)
	{
		std::swap(bounds.upper, bounds.lower);
	}

	return bounds;
}

bool fileExists(const std::string& path)
{
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

bool splitFilePath(const std::string& path, std::string& directory, std::string& basename, std::string& field)
{
	if (!fileExists(path))
	{
		return false;
	}

	int size = path.length();

	if (path.substr(size - 5, 5) == ".json")
	{
		int lastSlash = path.find_last_of("/\\");

		if (lastSlash == std::string::npos)
			directory = ".";
		else
			directory = path.substr(0, lastSlash);

		basename = path.substr(lastSlash + 1, size - lastSlash - 6);

		return true;
	}


	if (size < 7)
	{
		return false;
	}

	if (path.substr(size - 4, 4) != ".dat")
	{
		return false;
	}

	int last_ = path.find_last_of("_");
	if (last_ == std::string::npos || last_ > size - 6)
	{
		return false;
	}

	std::string jsonPath;
	do
	{
		jsonPath = path.substr(0, last_) + ".json";
	}
	while (!fileExists(jsonPath) && (last_ = path.find_last_of("_", last_ - 1)) != std::string::npos);

	if (last_ == std::string::npos)
	{
		return false;
	}

	int lastSlash = path.find_last_of("/\\");
	std::string file;

	if (lastSlash == std::string::npos)
	{
		directory = ".";
		file = path;
	}
	else
	{
		directory = path.substr(0, lastSlash);
		file = path.substr(lastSlash + 1);
	}

	basename = path.substr(lastSlash + 1, (last_ - lastSlash) - 1);
	field = path.substr(last_ + 1, size - last_ - 5);

	return true;
}

void readInfo(const std::string& directory, const std::string& basename, const std::string& field, ser::Serializer& serializer, ser::DataFieldInfo& info)
{
	serializer.Init(directory, basename, ser::SerializerOpenModeRead);
	info = serializer.FindField(field);
}

template <typename T>
void readData(const ser::Serializer& serializer, const ser::DataFieldInfo& info, const ser::Savepoint& savepoint, T*& data)
{
	int iSize = info.iSize();
	int jSize = info.jSize();
	int kSize = info.kSize();
	int lSize = info.lSize();
	int fieldLength = info.bytesPerElement();
	std::string fieldName = info.name();

	int lStride = fieldLength;
	int kStride = lSize * lStride;
	int jStride = kSize * kStride;
	int iStride = jSize * jStride;

	data = new T[iSize * jSize * kSize * lSize];
	serializer.ReadField(fieldName, savepoint, data, iStride, jStride, kStride, lStride);
}

#endif
