#include "video.h"
#include "v4l2_set.h"
//#define DEBUG

FileStorage fs("canshu.yaml",FileStorage::READ);

int main()
{

    int fd = open("/dev/base0",O_RDWR);
    int fd1 = open("/dev/video0",O_RDWR);

    //save video's file path(读取视频文件时使用)
    string c2 ="/home/mjs/Videos/blue.avi";

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


    //open camera and write video or read .avi file
    //find color and armous's center;
    //select == 1--camera
    //select == 2--.avi
    //flag == 1--find red armour
    //flag == 2--find blue armour
//    string c1 = "/home/mjs/Videos/bubing0.avi";  //save video path

string c1 ="bubing.avi";
//    fs["select"]>>select;   //camera or video_file
//    fs["flag"]>>flag;   //  which mode.
#ifndef VIDEO_DEBUG
//    video vd(0,c1,flag);
    video vd(0,c1);
    vd.camera_read_write();
#else
    video vd(c2);
    vd.file_read();
#endif
}
