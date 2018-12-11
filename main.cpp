#include "video.h"
#include "v4l2_set.h"
#include <thread>
#include <mutex>
#include <ImgFactory.h>
//#define DEBUG

FileStorage fs("canshu.yaml",FileStorage::READ);

void open_and_set_camera_para(int fd1)
{

    int saturaion,exposure,contrast,gain,brightness,white;
    fs["saturation"] >> saturaion;
    cout<<saturaion<<endl;
    fs["exposure"] >> exposure;
    cout<<exposure<<endl;
    fs["contrast"] >> contrast;
    fs["gain"] >> gain;
    fs["brightness"] >> brightness;
    fs["white"] >> white;
    //set camera's param
    v4l2_set vs(fd1);
    vs.set_saturation(saturaion);      //饱和度
    vs.set_exposure(exposure);     //曝光
//    vs.set_contrast(contrast);    //对比度
//    vs.set_gain(gain);         //增益
//    vs.set_brightness(brightness);   //亮度
//    vs.set_white_balance(white);    //白平衡
}

int main()
{

//    int fd = open("/dev/base0",O_RDWR);
    int fd1 = open("/dev/video0",O_RDWR);
    open_and_set_camera_para(fd1);

    cout<<"摄像头设置成功！"<<endl;

    ImgFactory imgfactory;
    imgfactory.Img_handle();


    return 0;

//#ifndef VIDEO_DEBUG
//    string c1 ="bubing.avi";
//    video vd(0,c1);
//    vd.camera_read_write();
//#else
//    //save video's file path(读取视频文件时使用)
//        string c2 ="/home/mjs/Videos/blue.avi";
//        video vd(c2);
//    vd.file_read();
//#endif
}
