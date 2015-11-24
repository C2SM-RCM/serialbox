#include <unistd.h>    /* for getopt */
#include <algorithm>   /* for max, min */
#include <iostream>    /* for cout */
#include "serializer/Serializer.h"
#include "shared.h"

using namespace ser;

void dumpInfo(const DataFieldInfo& info, Bounds iBounds, Bounds jBounds, Bounds kBounds, Bounds lBounds)
{
	iBounds.upper = std::min(iBounds.upper, info.iSize() - 1);
	jBounds.upper = std::min(jBounds.upper, info.jSize() - 1);
	kBounds.upper = std::min(kBounds.upper, info.kSize() - 1);
	lBounds.upper = std::min(lBounds.upper, info.lSize() - 1);

	std::cout << "Field: " << info.name() << std::endl;
	std::cout << "Type: " << info.type() << std::endl;
	std::cout << "Rank: " << info.rank() << std::endl;
	std::cout << "Bytes per Element: " << info.bytesPerElement() << std::endl;
	std::cout << "iSize: " << info.iSize() << " " << iBounds << std::endl;
	std::cout << "jSize: " << info.jSize() << " " << jBounds << std::endl;
	std::cout << "kSize: " << info.kSize() << " " << kBounds << std::endl;
	std::cout << "lSize: " << info.lSize() << " " << lBounds << std::endl;
}

template <typename T>
void dumpData(const Serializer& serializer, const DataFieldInfo& info, const Savepoint& savepoint,
              const Bounds& iBounds, const Bounds& jBounds, const Bounds& kBounds, const Bounds& lBounds)
{
	T* data;
	readData(serializer, info, savepoint, data);

	int iSize = info.iSize();
	int iLower = std::max(0, iBounds.lower);
	int iUpper = std::min(iSize - 1, iBounds.upper);
	int jSize = info.jSize();
	int jLower = std::max(0, jBounds.lower);
	int jUpper = std::min(jSize - 1, jBounds.upper);
	int kSize = info.kSize();
	int kLower = std::max(0, kBounds.lower);
	int kUpper = std::min(kSize - 1, kBounds.upper);
	int lSize = info.lSize();
	int lLower = std::max(0, lBounds.lower);
	int lUpper = std::min(lSize - 1, lBounds.upper);

	for (int i = iLower; i <= iUpper; ++i)
	{
		for (int j = jLower; j <= jUpper; ++j)
		{
			if (kSize > 1) std::cout << "[ ";
			for (int k = kLower; k <= kUpper; ++k)
			{
				if (lSize > 1) std::cout << "( ";
				for (int l = lLower; l <= lUpper; ++l)
				{
					int index = i * jSize * kSize * lSize + j * kSize * lSize + k * lSize + l;
					std::cout << data[index];
					if (l < lUpper) std::cout << ", ";
				}
				if (lSize > 1) std::cout << " )";
				if (k < kUpper) std::cout << ", ";
			}
			if (kSize > 1) std::cout << " ]";
			if (j < jUpper) std::cout << ", ";
		}
		std::cout << std::endl;
	}
}

int dump(const std::string& directory, const std::string& basename,
         const std::string& field, const std::string& savepointId,
         const Bounds& iBounds, const Bounds& jBounds, const Bounds& kBounds, const Bounds& lBounds, bool infoOnly)
{
	Serializer serializer;
	DataFieldInfo info;
	readInfo(directory, basename, field, serializer, info);

	dumpInfo(info, iBounds, jBounds, kBounds, lBounds);

	if (infoOnly)
	{
		return 0;
	}

	std::cout << std::endl;

	Savepoint savepoint = serializer.savepoints()[stoi(savepointId)];

	std::cout << "Savepoint: " << savepoint.name() << savepoint.metainfo().ToString() << std::endl;

	if (info.type() == "integer" || info.type() == "int")
	{
		dumpData<int>(serializer, info, savepoint, iBounds, jBounds, kBounds, lBounds);
	}
	else if (info.type() == "double")
	{
		dumpData<double>(serializer, info, savepoint, iBounds, jBounds, kBounds, lBounds);
	}
	else
	{
		std::cerr << "Unsupported type: " << info.type();
		return 2;
	}
}

void printHelp()
{
	std::cout << "Usage:" << std::endl;
	std::cout << "  dump [options] field_file savepointId" << std::endl;
	std::cout << "  dump -h" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "  -h                       show this screen" << std::endl;
	std::cout << "  -q                       dump field description only" << std::endl;
	std::cout << "  -i <iDim>                dump <iDim> for i dimension" << std::endl;
	std::cout << "  -i <iLower>:<iUpper>     dump range <iLower>:<iUpper> for i dimension" << std::endl;
	std::cout << "  -j <jDim>                dump <jDim> for j dimension" << std::endl;
	std::cout << "  -j <jLower>:<jUpper>     dump range <jLower>:<jUpper> for j dimension" << std::endl;
	std::cout << "  -k <kDim>                dump <kDim> for k dimension" << std::endl;
	std::cout << "  -k <kLower>:<kUpper>     dump range <kLower>:<kUpper> for k dimension" << std::endl;
	std::cout << "  -l <lDim>                dump <lDim> for l dimension" << std::endl;
	std::cout << "  -l <lLower>:<lUpper>     dump range <lLower>:<lUpper> for l dimension" << std::endl;

}

int main (int argc, char **argv) {

	if (argc < 2)
	{
		printHelp();
		return 0;
	}

	int opt;
	std::string i = ":";
	std::string j = ":";
	std::string k = ":";
	std::string l = ":";
	bool infoOnly = false;
	while ( (opt = getopt(argc, argv, "i:j:k:l:q:h")) != -1) {
		switch (opt)
		{
		case 'i':
			i = optarg;
			break;
		case 'j':
			j = optarg;
			break;
		case 'k':
			k = optarg;
			break;
		case 'l':
			l = optarg;
			break;
		case 'q':
			infoOnly = true;
			break;
		case 'h':
			printHelp();
			return 0;
		}
	}

	Bounds iBounds = string2bounds(i);
	Bounds jBounds = string2bounds(j);
	Bounds kBounds = string2bounds(k);
	Bounds lBounds = string2bounds(l);

	std::string filepath = argv[optind++];
	std::string savepointId = argv[optind++];

	std::string directory;
	std::string basename;
	std::string field;
	if (!splitFilePath(filepath, directory, basename, field))
	{
		std::cerr << "Invalid file: " << filepath << std::endl;
		return 2;
	}

	std::cout << "Directory: " << directory << std::endl;
	std::cout << "Basename: " << basename << std::endl;
	std::cout << "SavepointId: " << savepointId << std::endl;

	return dump(directory, basename, field, savepointId,
	            iBounds, jBounds, kBounds, lBounds, infoOnly);
}
