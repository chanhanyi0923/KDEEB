#include "Application.h"


void Application::Input()
{
    using std::fstream;
    using std::string;
    using std::stringstream;




    char file1[100];
    sprintf(file1, "%snodes.txt", this->inputPath);
    fstream nodeIn;
    nodeIn.open(file1, fstream::in);
    this->refPoints.resize(500000);
    for (string line; getline(nodeIn, line); ) {
        stringstream buffer(line);
        int index;
        double x1, y1;
        buffer >> index >> x1 >> y1;
        Point p(x1, y1, 0);
        p.fixed = true;
        p.id = index - 1;
        this->refPoints[index - 1] = p;
    }
    nodeIn.close();




    char file2[100];
    sprintf(file2, "%strip.txt", this->inputPath);
    //const char * file2 = "others/test2lines/trip.txt";
    fstream lineIn;
    lineIn.open(file2, fstream::in);

    this->dataSet.lines.resize(500000);

    for (string stringLine; getline(lineIn, stringLine); ) {
        stringstream buffer(stringLine);
        if (stringLine == "" || stringLine == " ") {
            this->dataSet.lines.push_back(Line());
            continue;
        }

        int id, o, d;
        buffer >> id >> o >> d;
        Line line;
        line.AddPoint(this->refPoints[o - 1]);
        line.AddPoint(this->refPoints[d - 1]);
        line.id = id - 1;

        //std::cout << line.id << std::endl;
        this->dataSet.lines[id - 1] = line;
    }

    lineIn.close();



    char file3[100];
    sprintf(file3, "%smore.txt", this->inputPath);
    fstream output_kdeeb;
    output_kdeeb.open(file3, fstream::in);

    for (string stringLine; getline(output_kdeeb, stringLine); ) {
        stringstream buffer(stringLine);
        if (stringLine == "" || stringLine == " ") {
            continue;
        }

        int id;
        double x, y;
        buffer >> id >> x >> y;
        Point p(x, y, 0);
        this->dataSet.lines[id - 1].AddSecondLastPoint(p);
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


