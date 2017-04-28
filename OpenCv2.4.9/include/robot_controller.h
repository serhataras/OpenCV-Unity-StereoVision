#pragma once

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "Controller.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>


#define PI 3.14159265f		//Pi 
#define UNITY_ENV			//Runs the program in unity simulation mode, comment this line for real robot application.
#define CV_VIDEO_OUTPUT		//Show video output, comment this line for no video output

//Class used to control a Unity or a real robot using stereo vision.
class robot_controller
{
public:
	robot_controller();
	virtual void process(const cv::Mat & left_img, const cv::Mat & right_img, float * vx, float * vy, float * omega);

private:
	enum class Mode {capture, move };
	Mode _mode;

	cv::Mat disparity(const cv::Mat & left_image, const cv::Mat & right_image);	//Returns a disparity map given two stereo images
	cv::Mat diff(const cv::Mat & image1, const cv::Mat & image2);				//Returns the difference between two images (motion detection)
	cv::Mat depthMap(cv::Mat disp);												//Returns a depth map given a disparity map

	float pixel_distance(float disparity);							//return a distance given a disparity value 
	float motion_distance(cv::Mat motion, cv::Mat disparity_map);	//return a distance given a motion map and a disparity_map

	std::vector<cv::Mat> getChessImages();		//Return a vector of chess images used for calibration

    float _last_dist;		//distance on last step
    float _last_speed;		//speed on last step

    const float INITIAL_SPEED = 4.0;	//initial speed value
    const float DT = 0.5;				//initial values

	#ifdef UNITY_ENV
		//Unity camera parameters
		const float CAMERA_SPACING = 0.1f;	//10cm
		const float FOCAL_LENGTH = 0.0035f;	//3.5mm
		const float FIELD_OF_VIEW = 40;		//40 degrees
		const float NEAR_PLANE_SIZE = FOCAL_LENGTH * tanf(FIELD_OF_VIEW * PI / 180.f);
		float PIXEL_WIDTH =0;	//To be determined at runtime
		float PIXEL_HEIGHT = 0;	//To be determined at runtime
	#else
		//Real robot parameters
		const float CAMERA_SPACING = 0.1f;	//10cm
		const float FOCAL_LENGTH = 0.0035f;
		const float PIXEL_SIZE = 0.00000375f;
	#endif 

	//Camera data from the last capture step
    cv::Mat disp_last;
	cv::Mat leftG_img_last;
	cv::Mat rightG_img_last;
};


