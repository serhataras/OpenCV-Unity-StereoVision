#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Controller
{
public:
	Controller(const string fileName) : m_fileName{ fileName } {}
	void left()		{ generateCommand("left");		};
	void right()	{ generateCommand("right");		};
	void forward()	{ generateCommand("forward");	};
	void backward() { generateCommand("backward");	};
	void stop()		{ generateCommand("stop");		};

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


int main()
{
	cout << "starting simulation." << endl;

	Controller controller("D:/command.txt");

	std::string line;

	bool playing = true;
	while (playing)
	{
		 std::getline(std::cin, line);
		 if (line == "stop")
		 {
			 playing = false;
			 controller.stop();
		 }
		 else if (line == "left" || line == "l")
			 controller.left();
		 else if (line == "right" || line == "r")
			 controller.right();
		 else if (line == "forward" || line == "f")
			 controller.forward();
		 else if (line == "backward" || line == "b")
			 controller.backward();
		 else
			 std::cout << "invalid command." << std::endl;
	}
	std::cout << "ending simulation." << std::endl;
	return 0;
}