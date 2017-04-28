#include <iostream>

#include "robot_controller.h"
#include "Controller.h"

#include <stdio.h>

using namespace std;

#define AUTO_PILOT


#ifdef AUTO_PILOT

int main()
{
	cout << "starting simulation." << endl;

	//Create the controller and the needed values
	Controller controller("D:/command.txt");
	string image_file = "D:/camera.png";
	robot_controller rc;
	std::string line;
	bool playing = true;
	float vx = 0, vy = 0, omega = 0;

	controller.skip();	//First step do nothing

	while (playing)
	{
		cv::Mat fullImage;
		//Wait for simulator output
		do
		{
			fullImage = cv::imread(image_file, cv::IMREAD_COLOR);
		} while (fullImage.empty());

		remove(image_file.c_str());

		//Crop the simulator output
		cv::Rect leftRoi(0, 0, fullImage.cols / 2, fullImage.rows);
		cv::Rect rightRoi(fullImage.cols / 2, 0, fullImage.cols / 2, fullImage.rows);
		cv::Mat left = fullImage(leftRoi);
		cv::Mat right = fullImage(rightRoi);

		//Run the robot controller 
		rc.process(left, right, &vx, &vy, &omega);
	}

	std::cout << "ending simulation." << std::endl;

	return 0;
}

#else// AUTO_PILOT

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

#endif // AUTO_PILOT