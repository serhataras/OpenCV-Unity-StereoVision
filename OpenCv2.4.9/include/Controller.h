#pragma once

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Controller
{
public:
	Controller(const string fileName) : m_fileName{ fileName } {}
	void left() { generateCommand("left"); };
	void right() { generateCommand("right"); };
	void forward() { generateCommand("forward"); };
	void backward() { generateCommand("backward"); };
	void skip() { generateCommand("skip"); };
	void stop() { generateCommand("stop"); };

private:
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
	const string m_fileName;
};


