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



#define PI 3.14159265f


#define UNITY_ENV	//Runs the program in unity simulation mode, comment this line for real robot application.

class robot_controller
{
public:
  /// Use the provided left and right images to update the desired speed
  /// - vx: speed forward [m/s]
  /// - vy: lateral speed [m/s] (+ is left)
  /// - omega: angular speed [rad/s] (+ is rotating to left)


  virtual void process(const cv::Mat & left_img, const cv::Mat & right_img, float * vx, float * vy, float * omega);
  robot_controller ();

  cv::Mat disparity(const cv::Mat & left_image, const cv::Mat & right_image);
  cv::Mat diff(const cv::Mat & image1, const cv::Mat & image2);
  cv::Mat depthMap(cv::Mat disp);

private:
	enum class Mode {capture, move };
	Mode _mode;

	bool _output;

	

    float calc_dist (const cv::Mat & depth_map, const cv::Mat & diff_map);
	std::vector<cv::Mat> getChessImages();

    float _last_dist;
    float _last_speed;

    const float INITIAL_SPEED = 4.0;
    const float DT = 0.5;

	#ifdef UNITY_ENV
		//Unity camera parameters
		const float CAMERA_SPACING = 0.1f;	//10cm
		const float FOCAL_LENGTH = 0.0035f;	//3.5mm
		const float FIELD_OF_VIEW = 40;		//40 degrees
		const float NEAR_PLANE_SIZE = FOCAL_LENGTH * tanf(FIELD_OF_VIEW * PI / 180.f);
		float PIXEL_SIZE;	//To be determined at runtime
	#else
		//Real robot parameters
		const float CAMERA_SPACING = 0.1f;	//10cm
		const float FOCAL_LENGTH = 0.0035f;
		const float PIXEL_SIZE = 0.00000375f;
	#endif 

    cv::Mat disp_last;
	cv::Mat leftG_img_last;
	cv::Mat rightG_img_last;
};


