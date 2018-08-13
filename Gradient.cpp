#include "Gradient.h"


Gradient::Gradient()
{
}


Gradient::~Gradient()
{
}


void Gradient::Resize(size_t size)
{
	this->gradientX.resize(size);
	this->gradientY.resize(size);
}


void Gradient::ComputeGradient(const vector<float> & accMap, int gradientW, float attractionFactor)
{
	#pragma omp parallel for
	for (int y = 0; y < gradientW - 1; y++) {
		const int curLineOffs = y * gradientW;

		#pragma omp parallel for
		for (int x = 0; x < gradientW - 1; x++) {
			int index = x + curLineOffs;
			float num2 = accMap[index];
			float num3 = num2 - accMap[index + 1];
			float num4 = num2 - accMap[index + gradientW];
			glm::vec2 vec2d = glm::vec2(num3, num4);
			if (glm::length(vec2d) > 5E-06f) {
				vec2d = glm::normalize(vec2d);
				this->gradientX[index] = vec2d.x * attractionFactor;
				this->gradientY[index] = vec2d.y * attractionFactor;
			}
		}
	}
}


void Gradient::ApplyGradient(DataSet &dataSet, const vector<float> & accMap, int gradientW, double attractionFactor, double obstacleRadius)
{
	this->ComputeGradient(accMap, gradientW, attractionFactor / 100.0f);

	#pragma omp parallel for
	for (int i = 0; i < dataSet.data.size(); i ++) {
		vector<Record> & records = dataSet.data[i];
        if (records.size() >= 2) {
		Record & recordFront = records.front();
		size_t idx = this->GetAccMapIndexNormalized(recordFront.x, recordFront.y, gradientW);
		recordFront.z = accMap[idx];
		for (int j = 1; j < records.size() - 1; j++) {
			Record & record = records[j];
			int index = this->GetAccMapIndexNormalized(record.x, record.y, gradientW);
			float num3 = accMap[index];
			double x = record.x - this->gradientX[index];
			double y = record.y - this->gradientY[index];
			if (x < 0.0 || x > 1.0 || y < 0.0 || y > 1.0) {
				x = record.x;
				y = record.y;
			}
			record.x = x;
			record.y = y;
			record.z = num3 + 0.02f;
		}

		Record & recordBack = records.back();
		size_t idx2 = this->GetAccMapIndexNormalized(recordBack.x, recordBack.y, gradientW);
		recordBack.z = accMap[idx2];
        }
	}
}


int Gradient::GetAccMapIndexNormalized(double x, double y, int width)
{
	int num1 = x * (width - 1);
	int num2 = y * (width - 1);
	return num1 + num2 * width;
}

