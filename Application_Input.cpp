#include "Application.h"


void Application::Input()
{
    using std::fstream;
    using std::string;
    using std::stringstream;

    //const char * file2 = "others/out/trip.txt";
    const char * file2 = "others/out/trip.txt";
    fstream lineIn;
    lineIn.open(file2, fstream::in);
    for (string stringLine; getline(lineIn, stringLine); ) {
        stringstream buffer(stringLine);
        int id;
        double x1, y1, x2, y2;
        buffer >> id >> x1 >> y1 >> x2 >> y2;
        Line line;
        Point p1(x1, y1, 0);
        Point p2(x2, y2, 0);
        p1.fixed = true;
        p2.fixed = true;
        line.AddPoint(p1);
        line.AddPoint(p2);
        this->dataSet.lines.push_back(line);
    }

    lineIn.close();
}


/*
void Application::Input()
{
    using std::fstream;
    using std::string;
    using std::stringstream;

    const char * file1 = "others/Morphing/data/morphingTest_normalizedNodeLocations.txt";

    fstream nodeIn;
    nodeIn.open(file1, fstream::in);
    for (string line; getline(nodeIn, line); ) {
        stringstream buffer(line);
        double x1, y1;
        buffer >> x1 >> y1;
        this->refPoints.push_back(Point(x1, y1, 0));
    }
    nodeIn.close();

    const char * file2 = "others/Morphing/data/13/morphingTest_normalizedLines.txt";
    fstream lineIn;
    lineIn.open(file2, fstream::in);
    for (string stringLine; getline(lineIn, stringLine); ) {
        stringstream buffer(stringLine);
        double x1, y1, x2, y2;
        buffer >> x1 >> y1 >> x2 >> y2;
        Line line;
        Point p1(x1, y1, 0);
        Point p2(x2, y2, 0);
        p1.fixed = true;
        p2.fixed = true;
        line.AddPoint(p1);
        line.AddPoint(p2);
        this->dataSet.lines.push_back(line);
    }

    lineIn.close();
}
*/


