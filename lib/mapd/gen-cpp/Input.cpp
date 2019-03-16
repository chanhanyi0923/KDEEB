/*
 * Copyright 2017 MapD Technologies, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file    StreamInsertSimple.cpp
 * @author  Liu Xu <liuxu726@gmail.com>
 * @brief   Sample MapD Client code for inserting a stream of rows from stdin
 * to a MapD table.
 *
 * Copyright (c) 2014 MapD Technologies, Inc.  All rights reserved.
 **/
#include "Application.h"

#include <boost/tokenizer.hpp>
#include <cstring>
#include <iostream>
//#include <iomanip>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <time.h>

// include files for Thrift and MapD Thrift Services
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>
#include "gen-cpp/MapD.h"



namespace mapd {
using std::make_shared;
using std::shared_ptr;
}  // namespace mapd



void Application::Input()
{
	using namespace ::apache::thrift;
	using namespace ::apache::thrift::protocol;
	using namespace ::apache::thrift::transport;


	std::string server_host("localhost");  // default to localohost
	int port = 9091;                       // default port number
	const char* delimiter = "\t";          // only support tab delimiter for now

	std::string db_name = "mapd";
	std::string user_name = "mapd";
	std::string passwd = "hyper";

	mapd::shared_ptr<TTransport> socket(new TSocket(server_host, port));
	mapd::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	mapd::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	MapDClient client(protocol);
	TSessionId session;
	try {
		clock_t start1, end1, start2, end2;
		start1 = clock();
		transport->open();                                    // open transport
		client.connect(session, user_name, passwd, db_name);  // connect to mapd_server
		//TTableDetails table_details;
		//client.get_table_details(table_details, session, table_name);
		//std::cout << table_details;
		std::string query = "select rowid, mapd_geo from geojson_test4 order by rowid limit 3;";
		TQueryResult results;
		client.sql_execute(results, session, query, true, "1", -1, -1);
		//std::cout << results;

		TRowSet rs= results.row_set;
		//std::cout << rs << std::endl;

		std::vector<TColumn> columns = rs.columns;

		TColumn linestring =  columns[1];
		std::vector<std::string> str_cols = linestring.data.str_col;

		int size = str_cols.size();
		std::cout << size << std::endl;
		std::vector<std::vector<double>> data;
		int cnt = 0;
		start2 = clock();
		for (auto line: str_cols){
			//std::cout << line << std::endl;
			line.pop_back();
			line = line.erase(0, 12);
			replace(line.begin(), line.end(), ' ', ',');
			//std::cout << line << std::endl;

			std::istringstream iss(line);
			std::vector<double> tempData;
			int innerCnt = 0;
			while (iss.good()){
				std::string substr;
				//std::string::size_type sz;
				getline(iss, substr, ',');
				std::istringstream buffer(substr);
				double dd;
				buffer >> dd;
				tempData.push_back(dd);
				//std::cout<< substr << std::endl;
				innerCnt += 1;
			}
			data.push_back(tempData);
			//for (int i = 0;i < innerCnt; i++){
			//data[cnt][i] = tempData
			//}
			cnt += 1;
		}
		end2 = clock();
		std::cout<< "parser time: " << (double)(end2-start2)/CLOCKS_PER_SEC << std::endl;
		//stream_insert(client, session, table_name, table_details.row_desc, delimiter);
		client.disconnect(session);  // disconnect from mapd_server
		transport->close();          // close transport
		end1 = clock();
		std::cout<< "total time: " << (double)(end1-start1)/CLOCKS_PER_SEC << std::endl;
	} catch (TMapDException& e) {
		std::cerr << e.error_msg << std::endl;
		return 1;
	} catch (TException& te) {
		std::cerr << "Thrift error: " << te.what() << std::endl;
		return 1;
	}

	return 0;
}
