#ifndef FIND_ARMOUR_H
#define FIND_ARMOUR_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "serialport.h"

#define KALMANXY_OPEN
using namespace cv;
using namespace std;
extern FileStorage fs;
class find_armour
{
public:
    find_armour(FileStorage);
    Mat find_red(Mat Img,Mat dst);
    Mat find_blue(Mat ,Mat dst);
    Mat find_blue1(Mat,Mat);
    Mat find_red1(Mat img,Mat dst);
    Mat find_blue2(Mat,Mat);
    Mat find_red2(Mat,Mat);
    Mat find_red3(Mat,Mat);
    Mat roi(Mat,Point,double,int,int);
    Mat find_blue3(Mat,Mat,Point &,int&,int&);
    void search_armour(Mat img,Mat dst,vector<Point2f> & armour_center,vector<double> & diameters,int flags,vector<double>&,vector<double>&Heights);
    Mat find_blue4(Mat,Mat,VisionData&,RotatedRect&,float);
    Mat find_red4(Mat,Mat,Point&,int&,int&);
private:
    FileStorage fs;
    double x1,x2,y1,y2;
    double y_dist_wucha_ROI,height_d_wucha_ROI;
    double a1,a2,area_min,min_rate,max_rate,height_d_wucha,y_dist_wucha;
    int ismiddle;
    int isfind;
};

#endif // FIND_ARMOUR_H
