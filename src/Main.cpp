#include <exception>
#include <dirent.h>

#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>
#include "../lib/mapd/gen-cpp/MapD.h"

#include "Application.h"


namespace mapd {
    using std::make_shared;
    using std::shared_ptr;
}

using namespace Pistache;

int isEmpty = 0;

void runMorphing(int layerNumber, double maxLng, double minLng, double maxLat, double minLat)
{
	Application * app = new Application;
	try {
	    isEmpty = app->Run(layerNumber, maxLng, minLng, maxLat, minLat);      
	    delete app;
	} catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
	} catch (const char *e) {
            std::cerr << e << std::endl;
        } catch (...) {
            std::cerr << "(...) error" << std::endl;
        }
}


void loadOutputToDB()
{
    std::string command = "/home/yanlyu/Downloads/miniconda3/envs/py3/bin/python monitorFileSystemChange.py";
    int res = system(command.c_str());
}


int findFile(char* fileName) {

    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir("/home/hanyi/KDEEB/build/out")) != NULL){

        while ((ent = readdir(dir)) != NULL){

            if (!strcmp(ent->d_name, fileName)) {

                //std::cout << fileName <<" found" <<std::endl;
                closedir(dir);
                return 1;
            }
        }
        closedir(dir);
        return 0;

    }else {
        std::cout<<"can't open dir"<<std::endl;
        return -1;
    }
}


class StatsEndpoint {

public:

    StatsEndpoint(Address addr)
        : httpEndpoint(std::make_shared<Http::Endpoint>(addr))
    { }

    void init(size_t thr = 2) {
        auto opts = Http::Endpoint::options()
            .threads(thr)
            .flags(Tcp::Options::InstallSignalHandler);
            //.flags(Tcp::Options::InstallSignalHandler | Tcp::Options::ReuseAddr);
        httpEndpoint->init(opts);
        setupRoutes();
    }

    void start() {
        httpEndpoint->setHandler(router.handler());
        httpEndpoint->serve();
    }

    void shutdown() {
        httpEndpoint->shutdown();
    }

private:

    void setupRoutes() {

        using namespace Rest;

        Routes::Post(router, "/animation", Routes::bind(&StatsEndpoint::startAnimation, this));
        Routes::Post(router, "/morphing", Routes::bind(&StatsEndpoint::morphingInRealTime, this));
    }

    void startAnimation(const Rest::Request& request, Http::ResponseWriter response) {

        time_t t = time(NULL);
        tm* timePtr = localtime(&t);

        char buffer[100];
        sprintf(buffer, "%04d.%02d.%02d %02d:%02d:%02d", (timePtr->tm_year) + 1900, (timePtr->tm_mon) + 1, timePtr->tm_mday, (timePtr->tm_hour) + 1, (timePtr->tm_min), (timePtr->tm_sec));

        //std::cout << buffer << " startAnimation is called" << std::endl;

        auto request_body = request.body();
        std::cout<< request_body <<std::endl;

        std::vector<int> int_param;
        std::vector<double> double_param;
        std::string tempStr = "";
        std::stringstream buffer1(request_body);
        int tempCnt = 1;

        while (getline(buffer1, tempStr, '&')) {

            std::stringstream buffer2(tempStr);
            getline(buffer2, tempStr, '=');
            getline(buffer2, tempStr, '=');

            std::stringstream buffer3(tempStr);

            if (tempCnt == 1) {
                int x;
                buffer3 >> x;
                int_param.push_back(x);
                tempCnt += 1;
            }else if (tempCnt > 1) {
                double y;
                buffer3 >> y;
                double_param.push_back(y);
            }
        }

        char fileName[50];
        sprintf(fileName, "%.3f_%.3f_%d.csv", double_param[0], double_param[1], int_param[0]);

        int status = 0;
        do {
            status = findFile(fileName);
            std::cout << fileName << " ***** " << status << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }while (status != 1);

        std::this_thread::sleep_for(std::chrono::milliseconds(800));

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");       
        response.send(Http::Code::Ok, std::to_string(int_param[0]));
    }


    void morphingInRealTime(const Rest::Request& request, Http::ResponseWriter response) {

        time_t t = time(NULL);
        tm* timePtr = localtime(&t);

        char buffer[100];
        sprintf(buffer, "%04d.%02d.%02d %02d:%02d:%02d", (timePtr->tm_year) + 1900, (timePtr->tm_mon) + 1, timePtr->tm_mday, (timePtr->tm_hour) + 1, (timePtr->tm_min), (timePtr->tm_sec));

        std::cout << buffer << " morphingInRealTime is called" << std::endl;

        auto request_body = request.body();
        std::cout<< request_body <<std::endl;

        std::vector<int> int_param;
        std::vector<double> double_param;
        std::string tempStr = "";
        std::stringstream buffer1(request_body);
        int tempCnt = 1;

        while (getline(buffer1, tempStr, '&')) {

            std::stringstream buffer2(tempStr);
            getline(buffer2, tempStr, '=');
            getline(buffer2, tempStr, '=');

            std::stringstream buffer3(tempStr);

            if (tempCnt == 1) {
                int x;
                buffer3 >> x;
                int_param.push_back(x);
                tempCnt += 1;
            }else if (tempCnt > 1) {
                double y;
                buffer3 >> y;
                double_param.push_back(y);
            }
        }

        char fileName[50];
        sprintf(fileName, "%.3f_%.3f_0.csv", double_param[0], double_param[2]);

        int status = findFile(fileName);

        if (status == 0){

            std::thread th(runMorphing, int_param[0], double_param[0], double_param[1], double_param[2], double_param[3]);
            th.detach();
            //runMorphing(int_param[0], double_param[0], double_param[1], double_param[2], double_param[3]);

            do {
                status = findFile(fileName);
                std::cout << fileName << " ***** " << status << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                if (isEmpty == 1) {
                    break;
                }
            }while (status != 1);

        }else if (status == 1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }else {
            std::cerr << "findFile() status is -1" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(800));
        
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");       
        response.send(Http::Code::Ok, std::to_string(isEmpty));
        isEmpty = 0;
    }


    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;

};


int main(int argc, char *argv[])
{
/*
    //std::thread t(loadOutputToDB);
    //t.detach();

    Application * app = new Application;

    try {

    	isEmpty = app->Run(1, 104.30994964400492, 103.74911250978317, 30.73831426977732, 30.583625311807324);
    	//isEmpty = app->Run(1, 109.11305985813408, 108.0340504147245, 30.59039914741922, 30.55791924511665);
    	delete app;

    } catch (const std::exception &e) {
    	std::cerr << e.what() << std::endl;
    	return 1;
    } catch (const char * e) {
        std::cerr << e << std::endl;
     }
*/

    Port port(8889);

    int thr = 1;

    Address addr(Ipv4::any(), port);

    //cout << "Cores = " << hardware_concurrency() << endl;
    std::cout << "Using " << thr << " threads" << std::endl;

    StatsEndpoint stats(addr);

    stats.init(thr);

    std::thread t(loadOutputToDB);
    t.detach();

    std::cout << "Starting server..." << std::endl;
    stats.start();

    //std::cout << "Shutdowning server..." << std::endl;
    //stats.shutdown();

    return 0;
}

