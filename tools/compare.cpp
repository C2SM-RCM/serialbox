//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include <unistd.h>    /* for getopt */
#include <algorithm>   /* for max, min */
#include <cmath>       /* for abs */
#include <iostream>    /* for cout */
#include <iomanip>     /* for std::setw */

#include "serializer/Serializer.h"
#include "shared.h"

using std::vector;
using std::string;
using namespace ser;

bool compareInfo(const DataFieldInfo& info1, const DataFieldInfo& info2)
{
	bool equal = true;
	if (info1.type() != info2.type())
	{
		equal = false;
		std::cout << "Type: " << info1.type() << " != " << info2.type() << std::endl;
	}
	if (info1.rank() != info2.rank())
	{
		equal = false;
		std::cout << "Rank: " << info1.rank() << " != " << info2.rank() << std::endl;
	}
	if (info1.bytesPerElement() != info2.bytesPerElement())
	{
		equal = false;
		std::cout << "Bytes per Element: " << info1.bytesPerElement() << " != " << info2.bytesPerElement() << std::endl;
	}
	if (info1.iSize() != info2.iSize())
	{
		equal = false;
		std::cout << "iSize: " << info1.iSize() << " != " << info2.iSize() << std::endl;
	}
	if (info1.jSize() != info2.jSize())
	{
		equal = false;
		std::cout << "jSize: " << info1.jSize() << " != " << info2.jSize() << std::endl;
	}
	if (info1.kSize() != info2.kSize())
	{
		equal = false;
		std::cout << "kSize: " << info1.kSize() << " != " << info2.kSize() << std::endl;
	}
	if (info1.lSize() != info2.lSize())
	{
		equal = false;
		std::cout << "lSize: " << info1.lSize() << " != " << info2.lSize() << std::endl;
	}

	return equal;
}

template <typename T>
bool compareData(const Serializer& serializer1, const Serializer& serializer2,
                 const Savepoint& savepoint,
                 const DataFieldInfo& info1, const DataFieldInfo& info2,
                 const IJKLBounds& bounds,
                 double tolerance, vector<bool>& failed)
{
	T* data1;
	readData(serializer1, info1, savepoint, data1);

	T* data2;
	readData(serializer2, info2, savepoint, data2);

	int iSize = info1.iSize();
	int jSize = info1.jSize();
	int kSize = info1.kSize();
	int lSize = info1.lSize();

	bool notequal = false;
	failed.resize(iSize * jSize * kSize * lSize);

	for (int i = bounds.iLower; i <= bounds.iUpper; ++i)
		for (int j = bounds.jLower; j <= bounds.jUpper; ++j)
			for (int k = bounds.kLower; k <= bounds.kUpper; ++k)
				for (int l = bounds.lLower; l <= bounds.lUpper; ++l)
				{
					int index = i * jSize * kSize * lSize + j * kSize * lSize + k * lSize + l;

					const double val = static_cast<double>(data1[index]);
					const double ref = static_cast<double>(data2[index]);
					const double err = std::fabs(ref) > 1. ?
					                 std::fabs((ref - val) / ref) : std::fabs(ref - val);

					const bool f = err > tolerance                     // Error is too large (e.g. val is infinite, ref is not)
					               || ((val != val) != (ref != ref))       // Exactly one is NaN
					               || (err != err && ref != val)           // ref is infinite, val different
					               ;
					failed[index] = f;
					notequal = notequal || f;
				}

	delete [] data1;
	delete [] data2;

	return !notequal;
}

template <typename T, typename U>
void printDifference(const Serializer& serializer1, const Serializer& serializer2,
                     const Savepoint& savepoint,
                     const DataFieldInfo& info1, const DataFieldInfo& info2,
                     const IJKLBounds& bounds,
                     const vector<bool>& failed)
{
	T* data1;
	readData(serializer1, info1, savepoint, data1);

	T* data2;
	readData(serializer2, info2, savepoint, data2);

	int iSize = info1.iSize();
	int jSize = info1.jSize();
	int kSize = info1.kSize();
	int lSize = info1.lSize();

	int nValues = (bounds.iUpper - bounds.iLower + 1) * (bounds.jUpper - bounds.jLower + 1) *
	              (bounds.kUpper - bounds.kLower + 1) * (bounds.lUpper - bounds.lLower + 1);
	int nErrors = 0;
	U maxRelError = 0;
	U maxAbsError = 0;

	for (int i = bounds.iLower; i <= bounds.iUpper; ++i)
		for (int j = bounds.jLower; j <= bounds.jUpper; ++j)
			for (int k = bounds.kLower; k <= bounds.kUpper; ++k)
				for (int l = bounds.lLower; l <= bounds.lUpper; ++l)
				{
					int index = i * jSize * kSize * lSize + j * kSize * lSize + k * lSize + l;
					if (failed[index])
					{
						++nErrors;

						const double val = static_cast<double>(data1[index]);
						const double ref = static_cast<double>(data2[index]);
						maxAbsError = std::max(maxAbsError, std::fabs(val - ref));
						maxRelError = std::max(maxRelError, std::fabs((val - ref) / ref));
					}
				}

	std::cout << " | Number of values: " << std::setw(6) << nValues << "\n";
	std::cout << " | Number of errors: " << std::setw(6) << nErrors << "\n";
	std::cout << " | Percentuage of errors: " << std::setprecision(2) << std::fixed << (100.*nErrors) / nValues << " %\n";
	std::cout << " | Maximum absolute error: " << std::setw(17) << std::setfill(' ') << std::scientific << std::setprecision(10) << maxAbsError  << "\n";
	std::cout << " | Maximum relative error: " << std::setw(17) << std::setfill(' ') << std::scientific << std::setprecision(10) << maxRelError  << "\n";

	delete [] data1;
	delete [] data2;

}

int compare(const std::string& directory1, const std::string& basename1,
            const std::string& directory2, const std::string& basename2,
            const Bounds& iBounds, const Bounds& jBounds,
            const Bounds& kBounds, const Bounds& lBounds,
            double tolerance, bool infoOnly, const vector<string>& specificFields)

{
	Serializer serializer1;
	serializer1.Init(directory1, basename1, SerializerOpenModeRead);

	Serializer serializer2;
	serializer2.Init(directory2, basename2, SerializerOpenModeRead);

	vector<Savepoint> savepoints = serializer1.savepoints();

	for (int i = 0; i < savepoints.size(); i++)
	{
		if (savepoints[i].name().substr(savepoints[i].name().size() - 4, 4) != "-out")
			continue;

		std::cout << "---------------------------------" << std::endl;
		std::cout << savepoints[i].name()                << std::endl;
		std::cout << savepoints[i].metainfo().ToString() << std::endl;

		vector<string> fieldsAtSavepoint = serializer1.FieldsAtSavepoint(savepoints[i]);
		vector<string> fields;

		if (specificFields.empty())
			fields = serializer1.FieldsAtSavepoint(savepoints[i]);
		else
		{
			fields.resize(std::min(specificFields.size(), fieldsAtSavepoint.size()));
			auto it = std::set_intersection(fieldsAtSavepoint.begin(), fieldsAtSavepoint.end(),
			                                specificFields.begin(), specificFields.end(), fields.begin());
			fields.resize(it - fields.begin());
		}

		for (auto const& field : fields)
		{
			std::cout << '\t' << field << std::endl;

			DataFieldInfo info1 = serializer1.FindField(field);
			DataFieldInfo info2 = serializer2.FindField(field);

			IJKLBounds bounds = getIJKLBounds(info1, iBounds, jBounds, kBounds, lBounds);

			bool equal = compareInfo(info1, info2);
			if (!equal)
			{
				return 1;
			}
			else if (infoOnly)
			{
				continue;
			}

			vector<bool> failed;

			if (info1.type() == "integer" || info1.type() == "int")
			{
				equal = compareData<int>(serializer1, serializer2, savepoints[i], info1, info2, bounds, tolerance, failed);
				if (!equal)
					printDifference<int,double>(serializer1, serializer2, savepoints[i], info1, info2, bounds, failed);
			}
			else if (info1.type() == "double")
			{
				equal = compareData<double>(serializer1, serializer2, savepoints[i], info1, info2, bounds, tolerance, failed);
				if (!equal)
					printDifference<double,double>(serializer1, serializer2, savepoints[i], info1, info2, bounds, failed);
			}
			else if (info1.type() == "float")
			{
				equal = compareData<float>(serializer1, serializer2, savepoints[i], info1, info2, bounds, tolerance, failed);
				if (!equal)
					printDifference<float,float>(serializer1, serializer2, savepoints[i], info1, info2, bounds, failed);
			}
			else if (info1.type() == "float")
			{
				equal = compareData<float>(serializer1, serializer2, savepoints[i], info1, info2, bounds, tolerance, failed);
				if (!equal)
					printDifference<float>(serializer1, serializer2, savepoints[i], info1, info2, bounds, failed);
			}
			else
			{
				std::cerr << "Unsupported type: " << info1.type() << std::endl;
				return 2;
			}
		}
	}

	// return (int) (!equal);
	return 0;
}

void printHelp()
{
	std::cout << "Usage:" << std::endl;
	std::cout << "  compare [options] field_file1 field_file2" << std::endl;
	std::cout << "  compare [options] json_file1 json_file2" << std::endl;
	std::cout << "  compare -h" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "  -h                       show this screen" << std::endl;
	std::cout << "  -q                       compare field description only" << std::endl;
	std::cout << "  -t <tol>                 tolerance, default 1e-12" << std::endl;
	std::cout << "  -i <iDim>                compare <iDim> for i dimension" << std::endl;
	std::cout << "  -i <iLower>:<iUpper>     compare range <iLower>:<iUpper> for i dimension" << std::endl;
	std::cout << "  -j <jDim>                compare <jDim> for j dimension" << std::endl;
	std::cout << "  -j <jLower>:<jUpper>     compare range <jLower>:<jUpper> for j dimension" << std::endl;
	std::cout << "  -k <kDim>                compare <kDim> for k dimension" << std::endl;
	std::cout << "  -k <kLower>:<kUpper>     compare range <kLower>:<kUpper> for k dimension" << std::endl;
	std::cout << "  -l <lDim>                compare <lDim> for l dimension" << std::endl;
	std::cout << "  -l <lLower>:<lUpper>     compare range <lLower>:<lUpper> for l dimension" << std::endl;

}

int main (int argc, char **argv)
{
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
	double tolerance = 1e-12;
	std::string savepointName2 = "";
	bool infoOnly = false;
	while ( (opt = getopt(argc, argv, "i:j:k:l:t:q:h")) != -1) {
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
		case 't':
			tolerance = strtod(optarg, NULL);
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

	std::string filepath1 = argv[optind++];
	std::string filepath2 = argv[optind++];

	vector<string> specificFields;

	std::string directory1;
	std::string basename1;
	std::string field1;
	if (!splitFilePath(filepath1, directory1, basename1, field1))
	{
		std::cerr << "Invalid file 1: " << filepath1 << std::endl;
		return 2;
	}

	std::string directory2;
	std::string basename2;
	std::string field2;
	if (!splitFilePath(filepath2, directory2, basename2, field2))
	{
		std::cerr << "Invalid file 2: " << filepath2 << std::endl;
		return 2;
	}

	if (field1 != field2)
	{
		std::cerr << "inconsistent fields" << std::endl;
		return 2;
	}

	if (field1 != "")
		specificFields.push_back(field1);

	std::cout << basename1 << std::endl << basename2 << std::endl;

	return compare(directory1, basename1,
	               directory2, basename2,
	               iBounds, jBounds, kBounds, lBounds, tolerance,
	               infoOnly, specificFields);

}
