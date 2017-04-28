#include "robot_controller.h"

using namespace std;

robot_controller::robot_controller() : _mode{ Mode::capture }, _last_dist(-1.0) {}

cv::Mat robot_controller::disparity(const cv::Mat & left_image, const cv::Mat & right_image)
{
	//Resize and copy the image
    float downSize = 0.8f;
    cv::Mat disp, disp8;
    cv::Mat left_image_cpy, right_image_cpy;
    cv::resize(left_image, left_image_cpy, cv::Size(), downSize,downSize);
    cv::resize(right_image, right_image_cpy, cv::Size(), downSize,downSize);

	//Set sBM parameters
	cv::StereoBM sbm;
	sbm.state->SADWindowSize = 7;
	sbm.state->numberOfDisparities = 112;
	sbm.state->preFilterSize = 5;
	sbm.state->preFilterCap = 61;
	sbm.state->minDisparity = -50;
	sbm.state->textureThreshold = 507;
	sbm.state->uniquenessRatio = 0;
	sbm.state->speckleWindowSize = 0;
	sbm.state->speckleRange = 8;
	sbm.state->disp12MaxDiff = 1;

	sbm(left_image_cpy, right_image_cpy, disp);
    normalize(disp, disp8, 0, 255, CV_MINMAX, CV_8U);

    return disp8;
}

float robot_controller::pixel_distance(float disparity)
{
	#ifdef UNITY_ENV
		//Compute Unity world pixel depth
	if (disparity != 0)
		return (FOCAL_LENGTH * CAMERA_SPACING) / (2 * PIXEL_WIDTH * disparity);
	else
		return 0;

	#else
		//Compute real world pixel depth
		if (disparity != 0)
			distance = (FOCAL_LENGTH * CAMERA_SPACING) / (2 * PIXEL_SIZE * disparity);
		else
			return 0;
	#endif 
}

cv::Mat robot_controller::depthMap( cv::Mat disparity_map)
{
    cv::Mat depth_map(disparity_map.size(), CV_32F, 0.2f);

	//Process the depth for every pixel
    for(int y=0;y<depth_map.rows;++y)
    {
		for (int x = 0; x < depth_map.cols; ++x)
		{
			float disparity = disparity_map.at<uchar>(cv::Point(x, y));
			float distance = pixel_distance(disparity);
			depth_map.at<float>(cv::Point(x, y)) = distance;
		}
    }
    return depth_map;
}

float robot_controller::motion_distance(cv::Mat motion, cv::Mat disparity_map)
{
	float distance = 0;
	float pixel_count = 0;

	for (int y = 0; y<motion.rows; ++y)
	{
		for (int x = 0; x < motion.cols; ++x)
		{
			if (motion.at<uchar>(cv::Point(x, y)) > 0)
			{
				float disparity = disparity_map.at<uchar>(cv::Point(x, y));
				distance += pixel_distance(disparity);

				++pixel_count;
			}
		}
	}

	if (pixel_count < 100)
		return 0;
	else
	{
		cout << "distance: " << distance / pixel_count << " count: " << pixel_count << endl;
		return distance / pixel_count;
	}
		
}

cv::Mat robot_controller::diff(const cv::Mat & image1, const cv::Mat & image2)
{
    using namespace cv;

    Mat motion;
    absdiff(image1, image2, motion);

	/// Apply erosion to eliminate noise
	int erosion_size = 3;
	Mat element = getStructuringElement(MORPH_RECT,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));
	erode(motion, motion, element);

	for (int y = 0; y<motion.rows; ++y)
	{
		for (int x = 0; x < motion.cols; ++x)
		{
			if (motion.at<uchar>(cv::Point(x, y)) > 10)
				motion.at<uchar>(cv::Point(x, y)) = 255;
			else
				motion.at<uchar>(cv::Point(x, y)) = 0;
		}
	}

    return motion;
}

void robot_controller::process(const cv::Mat & left_img, const cv::Mat & right_img, float * vx, float * vy, float * omega)
{
	if (_mode == Mode::capture)
	{
		//Save pictures for later use
		cv::cvtColor(left_img, leftG_img_last, CV_BGR2GRAY);
		cv::cvtColor(right_img, rightG_img_last, CV_BGR2GRAY);

		#ifdef UNITY_ENV
				PIXEL_WIDTH = NEAR_PLANE_SIZE / left_img.cols;
				PIXEL_HEIGHT = NEAR_PLANE_SIZE / left_img.rows;
		#endif

		disp_last = disparity(leftG_img_last, rightG_img_last);
		_mode = Mode::move;
	}
	else if (_mode == Mode::move)
	{
		//Get pictures
		cv::Mat leftG_img, rightG_img;
		cv::cvtColor(left_img, leftG_img, CV_BGR2GRAY);
		cv::cvtColor(right_img, rightG_img, CV_BGR2GRAY);

		//Generate difference map
		cv::Mat disp = disparity(leftG_img, rightG_img);
		cv::Mat diff_map = diff(disp_last, disp);
		cv::Mat depth_map = depthMap(disp);
		float distance = motion_distance(diff_map, disp);

		#ifdef CV_VIDEO_OUTPUT
			cv::imshow("disp", disp);
			cv::imshow("disp_last", disp_last);
			cv::imshow("movement detection", diff_map);
			cv::imshow("depth_map", depth_map);
			cv::waitKey(1);
		#endif

		disp_last = disp;
		_mode = Mode::capture;
	}

	Controller controller("D:/command.txt");
	controller.skip();	//Do nothing

   /* Old command function not usable yet
   
   float dist (calc_dist(left_img, right_img));     // Distance from the object.
    // Case of the first call.
    if (_last_dist == -1)
    {
        *vx = INITIAL_SPEED;
        _last_speed = INITIAL_SPEED;
        _last_dist = dist;
    }
    else
    {
        float delta_d (dist - _last_dist);

        float delta_v (delta_d / DT);
        *vx = _last_speed + delta_v;
        _last_speed = *vx;
        _last_dist = dist;
    }*/
}

std::vector<cv::Mat> robot_controller::getChessImages()
{

	std::vector<cv::Mat> chessImages;

	//Right
	for (unsigned i = 0; i < 20; ++i)
	{
		std::stringstream ss;
		ss << "img/damier_10cm/right_" << i << ".png";
        chessImages.push_back(cv::imread(ss.str()));
		assert(!chessImages.at(i).empty());
	}

	//Left
	for (unsigned i = 0; i < 0; ++i)
	{
		std::stringstream ss;
		ss << "img/damier_10cm/left_" << i << ".png";
        chessImages.push_back(cv::imread(ss.str()));
		assert(!chessImages.at(i).empty());
	}

	return chessImages;
}
