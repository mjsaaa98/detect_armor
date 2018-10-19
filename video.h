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

using namespace std;
using namespace cv;

class video
{
public:
    video(string c,int);   //read video file
    video(int n,string filename,int);   //open camera
    void camera_read_write();
    void file_read();
private:
    int n;   //open camera num
    string filename;
    int flag;   //select
};

#endif // VIDEO_H
