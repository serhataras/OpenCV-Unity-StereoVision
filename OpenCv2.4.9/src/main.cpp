#include <iostream>

#include "robot_controller.h"
#include "Controller.h"

#include <stdio.h>

using namespace std;

int main()
{

	cout << "starting simulation." << endl;

	Controller controller("D:/command.txt");
	string image_file = "D:/camera.png";
	robot_controller rc;
	std::string line;
	bool playing = true;
	float vx = 0, vy = 0, omega = 0;

	controller.skip();

	while (playing)
	{
		cv::Mat fullImage;
		//Wait for simulator output
		do
		{
			fullImage = cv::imread(image_file, cv::IMREAD_COLOR);
		} while ( fullImage.empty() );

		remove(image_file.c_str());

		//Crop the simulator output
		cv::Rect leftRoi ( 0,					0, fullImage.cols/2, fullImage.rows);
		cv::Rect rightRoi( fullImage.cols/2,	0, fullImage.cols/2, fullImage.rows);
		cv::Mat left = fullImage( leftRoi);
		cv::Mat right = fullImage(rightRoi);

		//Run the robot controller 
		rc.process(left, right, &vx, &vy, &omega);
	}

	std::cout << "ending simulation." << std::endl;

	return 0;


	/*
	std::string right_im = "img/damier_10cm/right_10.png";
	std::string left_im = "img//damier_10cm//left_10.png";
	std::string right_im2 = "img/damier_10cm/right_11.png";
	std::string left_im2 = "img/damier_10cm/left_11.png";

	float vx=0, vy=0, omega=0;

	cv::Mat right = cv::imread(right_im, cv::IMREAD_COLOR);
	if (right.empty()) {
		std::cout << "Cannot read image file: " << right_im;
		return 0;
	}
	cv::Mat left = cv::imread(left_im, cv::IMREAD_COLOR);
	if (left.empty()) {
		std::cout << "Cannot read image file: " << left_im;
		return 0;
	}

	cv::Mat right2 = cv::imread(right_im2, cv::IMREAD_COLOR);
	if (right2.empty()) {
		std::cout << "Cannot read image file: " << right_im2;
		return 0;
	}
	cv::Mat left2 = cv::imread(left_im2, cv::IMREAD_COLOR);
	if (left2.empty()) {
		std::cout << "Cannot read image file: " << left_im2;
		return 0;
	}

	robot_controller rc;
	rc.process(left, right, &vx, &vy, &omega);
	rc.process(left2, right2, &vx, &vy, &omega);*/
	return 0;
}

/*int main()
{
	cout << "starting simulation." << endl;

	

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
}*/