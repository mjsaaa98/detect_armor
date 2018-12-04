#ifndef VIDEO_H
#define VIDEO_H
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
//用于计时
#include<time.h>
#include<QDebug>
#include<QTime>
#include "find_armour.h"
#include <anglesolve.h>
#include <opencv2/video/tracking.hpp>
#define VIDEO_DEBUG   //使用视频测试
#define CAMERA_DEBUG   //不接受数据自己测试
//#define KALMAN_OPEN     //打开卡尔曼
//#define KALMAN_2    //进行第二次卡尔曼
#define OPEN_SERIAL     //打开串口
//#define F640
#define SHOW_DEBUG   //调试时显示数据
using namespace std;
using namespace cv;
struct KFparam
{
    float last_time;
    float last_xAngle;
    float last_yAngle;
};

class video
{
public:
    video(string c);   //read video file
    video(int n,string filename);   //open camera
    void camera_read_write();
    void file_read();
private:
    int n;   //open camera num
    string filename;
    int mode;
//    int ismiddle;
//    int isfind;
//    Point XY;
};

#endif // VIDEO_H
