#include "v4l2_set.h"

//a class to set camera parameter

v4l2_set::v4l2_set(int handle)
{
    h = handle;
    fd = h;
}

void v4l2_set::set_contrast(int c)
{

//set value
    struct v4l2_control ctrl0;

    ctrl0.id = V4L2_CID_EXPOSURE_AUTO;
    ctrl0.value = 1;
    ioctl(fd,VIDIOC_S_CTRL,&ctrl0);
//    cout<<ctrl0.value<<endl;

    ctrl0.id = V4L2_CID_EXPOSURE_ABSOLUTE;
    ctrl0.value = c;

    ioctl(fd,VIDIOC_S_CTRL,&ctrl0);
    cout<<ctrl0.value<<endl;
    //cout<<ctrl0.value<<endl;
}
void v4l2_set::set_exposure(int c)
{

    struct v4l2_control ctrl0;
    ctrl0.id = V4L2_CID_EXPOSURE_AUTO;
    ctrl0.value = 1;
    ioctl(fd,VIDIOC_S_CTRL,&ctrl0);
    cout<<ctrl0.value<<endl;
    ctrl0.id = V4L2_CID_EXPOSURE;
    int a = ioctl(fd,VIDIOC_G_CTRL,&ctrl0);
    cout<<"===="<<a<<endl;
    ctrl0.id = V4L2_CID_EXPOSURE_ABSOLUTE;
    ctrl0.value = c;
    ioctl(fd,VIDIOC_S_CTRL,&ctrl0);
    cout<<ctrl0.value<<endl;
}
void v4l2_set::set_gain(int c)
{
    struct v4l2_control ctrl0;
    ctrl0.id = V4L2_CID_GAIN;
    ctrl0.value = c;
    ioctl(fd,VIDIOC_S_CTRL,&ctrl0);
    //cout<<ctrl0.value<<endl;
}

void v4l2_set::set_white_balance(int c)
{
    struct v4l2_control ctrl0;
    ctrl0.id = V4L2_CID_WHITE_BALANCE_TEMPERATURE;
    ctrl0.value = c;
    ioctl(fd,VIDIOC_S_CTRL,&ctrl0);
    //cout<<ctrl0.value<<endl;

}

void v4l2_set::set_brightness(int c)
{
    struct v4l2_control ctrl0;
    ctrl0.id = V4L2_CID_BRIGHTNESS;
    ctrl0.value = c;
    ioctl(fd,VIDIOC_S_CTRL,&ctrl0);
    //cout<<ctrl0.value<<endl;


}

void v4l2_set::set_saturation(int c)
{
    struct v4l2_control ctrl0;
    ctrl0.id = V4L2_CID_SATURATION;
    ctrl0.value = c;
    ioctl(fd,VIDIOC_S_CTRL,&ctrl0);
    //cout<<ctrl0.value<<endl;
}
