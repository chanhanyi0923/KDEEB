#include "Application.h"


void Application::Input()
{
    using std::fstream;
    using std::string;
    using std::stringstream;

    // read nodes
    {
        char file1[100] = "/home/hanyi/KDEEB/others/test9/Nodes.csv";
        //sprintf(file1, "/home/hanyi/KDEEB/others/test7/nodes.txt", this->inputPath);
        fstream nodeIn;
        nodeIn.open(file1, fstream::in);
        this->refPoints.resize(500000);

        int index;
        double x, y;
        std::string str, str2, str3;
        while (std::getline(nodeIn, str)) {
            std::stringstream ss(str);
            {
                std::getline(ss, str2, ',');
                std::stringstream ss2(str2);
                ss2 >> index;
                index--;
            }
            {
                std::getline(ss, str2, ',');
                std::stringstream ss2(str2);
                ss2 >> x;
            }
            {
                std::getline(ss, str2, ',');
                std::stringstream ss2(str2);
                ss2 >> y;
            }

            Point p(x, y, 0);
            p.fixed = true;
            p.id = index;
            this->refPoints[index] = p;
        }
        nodeIn.close();
    }

    this->dataSet.xMin = this->dataSet.yMin = std::numeric_limits<double>::max();
    this->dataSet.xMax = this->dataSet.yMax = std::numeric_limits<double>::min();
    {
        char file2[100] = "/home/hanyi/KDEEB/others/test9/sample4.csv";
        //sprintf(file2, "%strip.txt", this->inputPath);
        //const char * file2 = "others/test2lines/trip.txt";
        fstream lineIn;
        lineIn.open(file2, fstream::in);

        this->dataSet.lines.resize(500000);
        this->dataSet.rawLines.resize(500000);

        std::string buffer, buffer2;

        int trips_size = 0; // count of trips

        while (std::getline(lineIn, buffer)) {
            std::stringstream ss(buffer);
            std::vector<Point> rawLine;
            for (int index = 0; ss >> index; ) {
                index --;
                Point p = this->refPoints[index];
                p.fixed = false;
                rawLine.push_back(p);
                this->dataSet.xMin = std::min(this->dataSet.xMin, p.x);
                this->dataSet.yMin = std::min(this->dataSet.yMin, p.y);
                this->dataSet.xMax = std::max(this->dataSet.xMax, p.x);
                this->dataSet.yMax = std::max(this->dataSet.yMax, p.y);

            }
            rawLine.front().fixed = true;
            rawLine.back().fixed = true;
            this->dataSet.rawLines[trips_size] = rawLine;

            Line line;
            line.AddPoint(rawLine.front());
            line.AddPoint(rawLine.back());
            line.id = trips_size;
            this->dataSet.lines[trips_size] = line;
            trips_size ++;
        }
        lineIn.close();
    }

    // normalize
    for (size_t i = 0; i < 500000; i ++) {
        double x = this->refPoints[i].x;
        double y = this->refPoints[i].y;
        x = (x - this->dataSet.xMin) / (this->dataSet.xMax - this->dataSet.xMin);
        y = (y - this->dataSet.yMin) / (this->dataSet.yMax - this->dataSet.yMin);
        this->refPoints[i].x = x;
        this->refPoints[i].y = y;
    }

    for (size_t i = 0; i < 500000; i ++) {
        Line & line = this->dataSet.lines[i];
        for (size_t j = 0; j < line.GetPointSize(); j ++) {
            Point p = this->refPoints[line.GetPoint(j).id];
            line.SetPoint(j, p);
        }
    }
    // char file3[100];
    // sprintf(file3, "%smore.txt", this->inputPath);
    // fstream output_kdeeb;
    // output_kdeeb.open(file3, fstream::in);

    // for (string stringLine; getline(output_kdeeb, stringLine); ) {
        // stringstream buffer(stringLine);
        // if (stringLine == "" || stringLine == " ") {
            // continue;
        // }

        // int id;
        // double x, y;
        // buffer >> id >> x >> y;
        // Point p(x, y, 0);
        // this->dataSet.lines[id - 1].AddSecondLastPoint(p);
    // }

    // lineIn.close();
}
