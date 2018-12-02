#ifndef FIND_ARMOUR_H
#define FIND_ARMOUR_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "serialport.h"
#include "get_colors.h"
//#define KALMANXY_OPEN
#define SHOWDEBUG
using namespace cv;
using namespace std;
extern FileStorage fs;

static inline bool Sort_RotatedRect(RotatedRect r1,RotatedRect r2)
{
    return r1.center.x<r2.center.x;
}

class find_armour
{
public:
    find_armour(FileStorage);
    Mat find_red(Mat Img,Mat dst);
    Mat find_red1(Mat img,Mat dst);
    Mat find_red2(Mat,Mat);
    Mat find_red3(Mat,Mat);
    Mat roi(Mat,Point,double);
    float Point_Angle(const Point2f &p1,const Point2f &p2);
    void image_preprocess(int mode,Mat src,Mat &);
    void search_armour(Mat img,Mat dst,int flags);
    void get_Light();
    void src_get_armor();
    void Get_Rotated_param(double x1,double x2,double y1,double y2,double &Rotated_angle);
    void find_blue4(Mat,Mat &dst,RotatedRect&,int);
    Mat find_red4(Mat,Mat,Point&,int&,int&);
public:
    //与串口通信有关的数据
    int ismiddle;
    int isfind;
private:
    void clear_data();   //切换指令时清空所有数据
    void Clear();   //清空所有数据结构
    FileStorage fs;
    get_colors gc;
    vector<double> diameters;
    vector<Point2f> armour_center;
    vector<vector<RotatedRect> > Armorlists;
    vector<vector<Point2f> > Rotate_Points;
    vector<RotatedRect> fir_armor,result_armor;
    vector<Vec4f> contours_para;
    vector<int> CellMaxs;
    double x1,x2,y1,y2;
    double y_dist_wucha_ROI,height_d_wucha_ROI;
    double a1,a2,area_min,min_rate,max_rate,height_d_wucha,y_dist_wucha;
private:
    //与截图有关的参数
    int isROIflag;
    int last_mode;
    Point2f last_center;
    Mat dstROI;
    double last_d;
    double last_angle;
    Size2f last_size;
};

#endif // FIND_ARMOUR_H
