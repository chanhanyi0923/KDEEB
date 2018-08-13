#include "DataSet.h"


DataSet::DataSet()
{
}


DataSet::DataSet(size_t size)
{
	this->data.resize(size);
}


DataSet::~DataSet()
{
}


istream & operator >> (istream & input, DataSet & dataSet)
{
	using std::string;
	using std::istringstream;

	size_t countLine = 0;
	for (int c; !input.eof(); ) {
		c = input.get();
		if (c == '\n') {
			countLine++;
		}
	}

	// alloc memory
	dataSet.data.resize(countLine);

	// return to the beginning of input file.
	input.clear();
	input.seekg(0, std::ios::beg);

	// read each line
	for (size_t lineNum = 0; lineNum < countLine; lineNum ++) {
		string line;
		getline(input, line);
		istringstream lineBuffer(line);
		string token;

		double number[4] = { 0.0, 0.0, 0.0, 0.0 };
		// split by ','
		for (int i = 0; i < 4; i++) {
			getline(lineBuffer, token, ',');
			istringstream buffer(token);
			buffer >> number[i];
		}
		vector<Record> & records = dataSet.data[lineNum];
		records.push_back(Record(number[1], number[0], 1.0f));
		records.push_back(Record(number[3], number[2], 1.0f));
	}
	return input;
}


ostream & operator << (ostream & output, const DataSet & dataSet)
{
	output << std::setprecision(15);
	for (size_t i = 0; i < dataSet.data.size(); i ++) {
		const vector<Record> & records = dataSet.data[i];
		for (const Record & record : records) {
			output << record.x << "," << record.y << "," << (i + 1) << std::endl;
		}
	}
	output << std::resetiosflags(std::ios::showbase);

	return output;
}


void DataSet::AddRemovePoints(double removeDist, double splitDist)
{
	const double removeDist2 = removeDist * removeDist;
	const double splitDist2 = splitDist * splitDist;

    DataSet temp = DataSet(this->data.size());

	#pragma omp parallel for
	for (size_t i = 0; i < this->data.size(); i ++) {
		const vector<Record> & records = this->data[i];
        if (records.size() >= 2) {

		    vector<Record> source;
		    Record item = records.front();
		    source.push_back(item);

		    for (size_t j = 1; j < records.size(); j ++) {
			    const Record & record = records[j];

			    const double dist = (item.x - record.x) * (item.x - record.x) + (item.y - record.y) * (item.y - record.y);
			    if (dist > splitDist2) {
				    source.push_back(Record(
					    (item.x + record.x) / 2.0,
					    (item.y + record.y) / 2.0,
					    (item.z + record.z) / 2.0,
					    record.timeNormelized,
					    record.timeInt
				    ));
				    source.push_back(record);
				    item = record;
			    } else if (dist > removeDist2) {
				    source.push_back(record);
				    item = record;
			    }
		    }

		    if (records.back() != source.back()) {
			    source.push_back(records.back());
		    }
		    const double timeInt = source.front().timeInt;
	    	const double num5 = (source.back().timeInt - timeInt) / (double)(source.size() - 1);
    		for (size_t k = 0; k < source.size(); k++) {
			    Record record3 = source[k];
			    record3.timeInt = timeInt + num5 * k;
		    }

            temp.data[i] = source;
        }
	}
    this->data = temp.data;
}


void DataSet::SmoothTrails(const double interp)
{
    using std::pair;

    #pragma omp parallel for
    for (size_t i = 0; i < this->data.size(); i ++) {
        vector<Record> & recordArray = this->data[i];
        if (recordArray.size() >= 2) {
            const size_t length = recordArray.size();
            vector< pair<float, float> > vectorArray(length);
            for (int index = 1; index < length - 1; index ++) {
                vectorArray[index] = std::make_pair(recordArray[index].x, recordArray[index].y);
            }

            for (int j = 0; j < 10; j++) {
                for (int index = 1; index < length - 1; index++) {
                    double num4 = 0.0;
                    double num5 = 0.0;
                    int num6 = 0;
                    for (int k = -4; k <= 4; k++) {
                        int num8 = index + k;
                        if ((num8 >= 0) && (num8 < length)) {
                            num4 += recordArray[num8].x;
                            num5 += recordArray[num8].y;
                            num6++;
                        }
                    }
                    num4 /= (double)num6;
                    num5 /= (double)num6;
                    vectorArray[index].first = vectorArray[index].first * (1.0 - interp) + num4 * interp;
                    vectorArray[index].second = vectorArray[index].second * (1.0 - interp) + num5 * interp;
                }
                for (int index = 1; index < length - 1; index++) {
                    recordArray[index].x = vectorArray[index].first;
                    recordArray[index].y = vectorArray[index].second;
                }
            }
        }
    }
}


