#include "Application.h"

#include <boost/tokenizer.hpp>
#include <omp.h>

// include files for Thrift and MapD Thrift Services
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>
#include "../lib/mapd/gen-cpp/MapD.h"



namespace mapd {
    using std::make_shared;
    using std::shared_ptr;
}



int Application::Input(int layerNum, double maxLng, double minLng, double maxLat, double minLat)
{

    using namespace ::apache::thrift;
    using namespace ::apache::thrift::protocol;
    using namespace ::apache::thrift::transport;

    using std::fstream;
    using std::string;
    using std::stringstream;
    using std::vector;


    std::string server_host("localhost");  // default to localohost
    int port = 9091;                       // default port number

    std::string db_name = "mapd";
    std::string user_name = "mapd";
    std::string passwd = "hyper";

    mapd::shared_ptr<TTransport> socket(new TSocket(server_host, port));
    mapd::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    mapd::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    MapDClient client(protocol);
    TSessionId session;


    // read nodes
    {
        char file1[100] = "/home/hanyi/KDEEB/others/new_dataset/Nodes.csv";
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

    int tripsNum = 0;

    try {

        transport->open();                                    // open transport
	client.connect(session, user_name, passwd, db_name);  // connect to mapd_server

        std::string query = "select sequence from Morphing_MotivationExample where (startLng <= "+std::to_string(maxLng)+" and startLng >= "+std::to_string(minLng)+" and startLat <= "+std::to_string(maxLat)+" and startLat >= "+std::to_string(minLat)+") and (endLng <= "+std::to_string(maxLng)+" and endLng >= "+std::to_string(minLng)+" and endLat <= "+std::to_string(maxLat)+" and endLat >= "+std::to_string(minLat)+") order by rowid";
        //std::string query = "select sequence from Mapped_TaxiNum14864_Aug3_Morning where rowid in (34, 55, 115, 157, 212, 356, 547, 582, 871, 941, 942, 975) order by rowid;";
        //std::cout << query << std::endl;
		
	TQueryResult results;
	client.sql_execute(results, session, query, true, "1", -1, -1);
	//std::cout << results;

	TRowSet rs= results.row_set;
	//std::cout << rs << std::endl;
	vector<TColumn> columns = rs.columns;

        TColumn nodeSequence = columns[0];
	vector<std::string> nodeSequence_vec = nodeSequence.data.str_col;
        tripsNum = nodeSequence_vec.size();
        std::cout << "trips num: " << nodeSequence_vec.size() << std::endl;

        this->dataSet.lines.resize(500000);
        this->dataSet.rawLines.resize(500000);
        this->dataSet.roadSegments.resize(500000);


        int trips_size = 0; // count of trips

        #pragma omp parallel
        {
            #pragma omp for ordered
            for (int i = 0; i < nodeSequence_vec.size(); i++) {

                #pragma omp ordered
                {
                    std::stringstream ss(nodeSequence_vec[i]);
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
            }
        }

	client.disconnect(session);  // disconnect from mapd_server
	transport->close();          // close transport

    } catch (TMapDException& e) {
	throw RenderException(e.error_msg);
    } catch (TException& te) {
	std::ostringstream buffer;
	buffer << "Thrift error: " << te.what();
	throw RenderException(buffer.str());
    }

/*
    {
        char file2[100] = "/home/hanyi/KDEEB/others/sample4.csv";
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
*/


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

    return tripsNum;



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
