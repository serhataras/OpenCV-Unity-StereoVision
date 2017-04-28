#pragma once

#include <iostream>
#include <fstream>
#include <string>

using namespace std;


//Class used to communicate with the unity simulation
class Controller
{
public:
	Controller(const string fileName) : m_fileName{ fileName } {}	//filname is the directory in wich unity and the controller will communicate

	//Commands used to pilot the unity robot
	void left() { generateCommand("left"); };
	void right() { generateCommand("right"); };
	void forward() { generateCommand("forward"); };
	void backward() { generateCommand("backward"); };
	void skip() { generateCommand("skip"); };
	void stop() { generateCommand("stop"); };

private:
	//Generate the command given by a string
	void generateCommand(const string command)
	{
		ofstream monFlux(m_fileName);
		if (monFlux)
		{
			monFlux << command;
			monFlux.close();
		}
		else
			cout << "Controller::Error: command generation failed." << endl;
	};

	const string m_fileName;	//directory in wich unity and the controller will communicate
};


