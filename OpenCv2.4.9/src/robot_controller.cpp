#include "robot_controller.h"

using namespace std;

robot_controller::robot_controller() : _mode{ Mode::capture }, _output { true }, _last_dist(-1.0) {}

float robot_controller::calc_dist(const cv::Mat & depth_map, const cv::Mat & diff_map)
{
	
    return 0;
}


cv::Mat robot_controller::disparity(const cv::Mat & left_image, const cv::Mat & right_image)
{
    float downSize = 0.8f;

    cv::Mat disp, disp8;
    cv::Mat left_image_cpy, right_image_cpy;

    cv::resize(left_image, left_image_cpy, cv::Size(), downSize,downSize);
    cv::resize(right_image, right_image_cpy, cv::Size(), downSize,downSize);

    //cv::GaussianBlur(left_image_cpy, left_image_cpy, cv::Size(21,21),5);
    //cv::GaussianBlur(right_image_cpy, right_image_cpy, cv::Size(21,21),5);

    //On entre les paramètres pour la carte de disparité
   /* cv::StereoSGBM sgbm;
    int sadSize = 5;
    sgbm.SADWindowSize = 5;
    sgbm.numberOfDisparities = 192;
    sgbm.preFilterCap = 4;
    sgbm.minDisparity = -52;
    sgbm.uniquenessRatio = 5;
    sgbm.speckleWindowSize = 150;
    sgbm.speckleRange = 2;
    sgbm.disp12MaxDiff = 10;
    sgbm.fullDP = false;
    sgbm.P1 = sadSize*sadSize*8*3;
    sgbm.P2 = sadSize*sadSize*32*3;*/

	cv::StereoSGBM sgbm;
	int sadSize = 7;
	sgbm.SADWindowSize =7;
	sgbm.numberOfDisparities = 112;
	sgbm.preFilterCap = 61;
	sgbm.minDisparity = -50;
	sgbm.uniquenessRatio = 1;
	sgbm.speckleWindowSize = 150;
	sgbm.speckleRange = 2;
	sgbm.disp12MaxDiff = 10;
	//sgbm.fullDP = false;
	//sgbm.P1 = sadSize*sadSize * 8 * 3;
	//sgbm.P2 = sadSize*sadSize * 32 * 3;

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

cv::Mat robot_controller::depthMap( cv::Mat disp )
{
    cv::Mat depth_map( disp.size(), CV_32F, 0.2f);

    for(int y=0;y<depth_map.rows;y++)
    {
		for (int x = 0; x < depth_map.cols; x++)
		{
			//Compute pixel depth
			float distance = 1;
			float disparity = disp.at<uchar>(cv::Point(x, y));
			if (disparity != 0)
				distance = (FOCAL_LENGTH * CAMERA_SPACING) / (2 * PIXEL_SIZE * disparity);
			depth_map.at<float>(cv::Point(x, y)) = distance;
		}
    }
    return depth_map;
}

cv::Mat robot_controller::diff(const cv::Mat & image1, const cv::Mat & image2)
{
    using namespace cv;

    Mat motion;
    absdiff(image1, image2, motion);

	int erosion_size = 3;
	Mat element = getStructuringElement(MORPH_RECT,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));

	/// Apply the erosion operation
	erode(motion, motion, element);

    motion = motion > 50;

    return motion;
}

void robot_controller::process(const cv::Mat & left_img, const cv::Mat & right_img, float * vx, float * vy, float * omega)
{
	Controller controller("D:/command.txt");

	if (_mode == Mode::capture)
	{
		cerr << "Capturing picture...\t";

		//Save pictures for later use
		cv::cvtColor(left_img, leftG_img_last, CV_BGR2GRAY);
		cv::cvtColor(right_img, rightG_img_last, CV_BGR2GRAY);
		disp_last = disparity(leftG_img_last, rightG_img_last);
		controller.skip();
		_mode = Mode::move;

		cerr << "Done" << endl;
	}
	else if (_mode == Mode::move)
	{
		cerr << "Generating command...\t";

		//Get pictures
		cv::Mat leftG_img, rightG_img;
		cv::cvtColor(left_img, leftG_img, CV_BGR2GRAY);
		cv::cvtColor(right_img, rightG_img, CV_BGR2GRAY);

		//Generate difference map
		cv::Mat disp = disparity(leftG_img, rightG_img);
		cv::Mat diff_map = diff(disp_last, disp);


		cv::imshow("disp", disp);
		cv::imshow("disp_last", disp_last);
		cv::imshow("diff_map", diff_map);
		cv::waitKey(1);


		controller.skip();
		disp_last = disp;
		_mode = Mode::capture;

		cerr << "Done" << endl;
		
	}

	//cin.ignore();

	//std::string line;
	//std::getline(std::cin, line);


	/*
	cv::Mat leftG_img, rightG_img;

	cv::cvtColor(left_img, leftG_img, CV_BGR2GRAY);
	cv::cvtColor(right_img, rightG_img, CV_BGR2GRAY);
	
	//First process call = setup
	if (last_disp.empty())
	{
		cout << "Initialisation" << endl;
		leftG_img_last = leftG_img;
		rightG_img_last = rightG_img;
		last_disp = disparity(leftG_img, rightG_img);
		_last_speed = *vx;
		
	}
	else
	{
		cerr << "Processing... ";
		//cv::Mat disp = disparity(left_img, right_img);
		cv::Mat diff_map = diff(leftG_img, leftG_img_last);
		//cv::Mat depth_map = depthMap(disp);


		if (_output)
		{
			cv::imshow("diff_map", diff_map);
			cv::waitKey(0);

			//cv::imshow("diff", diff_map);
			//cv::imshow("depth", depth_map);
			//cv::waitKey();
		}

		leftG_img_last = leftG_img;
		//last_right = right_img;
		//last_disp = disp;

		cerr << "Done" << endl;
	}
	*/


   /* float dist (calc_dist(left_img, right_img));     // Distance from the object.
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
