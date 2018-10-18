#include "v4l2_get.h"
#include<opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;
using namespace cv;


//a class to get camera parameter


void v4l2_get::contrast(int fd,int c)
{
//get max min step default_value
    struct v4l2_queryctrl qc;
    qc.id = V4L2_CID_CONTRAST;
    ioctl(fd,VIDIOC_QUERYCTRL,&qc);
    cout<<"Name:"<<qc.name<<'\t'<<"step:"<<qc.step<<endl;
    cout<<"default_value:"<<qc.default_value<<endl;
    cout<<"Min:"<<qc.minimum<<'\t'<<"Max："<<qc.maximum<<endl;
//set value
    struct v4l2_control ctrl0;
    ctrl0.id = V4L2_CID_CONTRAST;
    ctrl0.value = c;
    ioctl(fd,VIDIOC_S_CTRL,&ctrl0);
    cout<<ctrl0.value<<endl;
}
void v4l2_get::exposure(int fd)
{

    struct v4l2_queryctrl qc;
    qc.id = V4L2_CID_EXPOSURE_ABSOLUTE;
    ioctl(fd,VIDIOC_QUERYCTRL,&qc);
    cout<<"Name:"<<qc.name<<'\t'<<"step:"<<qc.step<<endl;
    cout<<"default_value:"<<qc.default_value<<endl;
    cout<<"Min:"<<qc.minimum<<'\t'<<"Max："<<qc.maximum<<endl;
}
void v4l2_get::gain(int fd)
{
    struct v4l2_queryctrl qc;
    qc.id = V4L2_CID_GAIN;
    ioctl(fd,VIDIOC_QUERYCTRL,&qc);
    cout<<"Name:"<<qc.name<<'\t'<<"step:"<<qc.step<<endl;
    cout<<"default_value:"<<qc.default_value<<endl;
    cout<<"Min:"<<qc.minimum<<'\t'<<"Max："<<qc.maximum<<endl;


}

void v4l2_get::white_balance(int fd)
{
    struct v4l2_queryctrl qc;
    qc.id = V4L2_CID_WHITE_BALANCE_TEMPERATURE;
    ioctl(fd,VIDIOC_QUERYCTRL,&qc);
    cout<<"Name:"<<qc.name<<'\t'<<"step:"<<qc.step<<endl;
    cout<<"default_value:"<<qc.default_value<<endl;
    cout<<"Min:"<<qc.minimum<<'\t'<<"Max："<<qc.maximum<<endl;


}

void v4l2_get::brightness(int fd)
{
    struct v4l2_queryctrl qc;
    qc.id = V4L2_CID_BRIGHTNESS;
    ioctl(fd,VIDIOC_QUERYCTRL,&qc);
    cout<<"Name:"<<qc.name<<'\t'<<"step:"<<qc.step<<endl;
    cout<<"default_value:"<<qc.default_value<<endl;
    cout<<"Min:"<<qc.minimum<<'\t'<<"Max："<<qc.maximum<<endl;


}

void v4l2_get::saturation(int fd)
{
    struct v4l2_queryctrl qc;
    qc.id = V4L2_CID_SATURATION;
    ioctl(fd,VIDIOC_QUERYCTRL,&qc);
    cout<<"Name:"<<qc.name<<'\t'<<"step:"<<qc.step<<endl;
    cout<<"default_value:"<<qc.default_value<<endl;
    cout<<"Min:"<<qc.minimum<<'\t'<<"Max："<<qc.maximum<<endl;


}
