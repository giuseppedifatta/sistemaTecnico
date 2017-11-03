/*
 * config.cpp
 *
 *  Created on: 03/nov/2017
 *      Author: giuseppe
 */
#include "conf.h"
#include <fstream>
#include <iostream>
using namespace std;

string getConfig(string value) {
	string founded;

	string line;
	ifstream myfile (".config.tecnico");
	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{
			size_t comment  = line.find("#");
			if(comment  == 0){
				//cout << "comment line" << endl;
				continue;
			}
			else{
				size_t startPos = line.find(value);
				if(startPos != std::string::npos){
					startPos = startPos + value.length() + 1;
					founded = line.substr(startPos);
					cout << founded << endl;
					break;
				}
			}
		}
		myfile.close();
	}

	else cout << "Unable to open file";

	return founded;
}
