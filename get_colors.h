#ifndef GET_COLORS_H
#define GET_COLORS_H
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;
extern FileStorage fs;
/* save the method of descent colors*/


class get_colors
{
public:
    Mat HSV_blue(Mat img, Mat dst);
    Mat HSV_blue1(Mat img,Mat dst);
    Mat HSV_red(Mat img, Mat dst);
    Mat red_hsv_inv(Mat img,Mat dst);
    Mat HSV_red1(Mat img,Mat dst);
};

#endif // GET_COLORS_H
