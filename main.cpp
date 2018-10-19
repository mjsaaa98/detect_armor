#include "video.h"
#include "v4l2_set.h"


FileStorage fs("canshu.yaml",FileStorage::READ);

int main()
{

    int fd = open("/dev/base0",O_RDWR);
    int fd1 = open("/dev/video0",O_RDWR);

    //save video's file path(读取视频文件时使用)
    string c2 ="/home/mjs/Videos/blue6.avi";

    int saturaion,exposure,contrast,gain,brightness,white;
    fs["saturation"] >> saturaion;
    fs["exposure"] >> exposure;
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
    string c1 = "/home/mjs/Videos/bubing0.avi";  //save video path
    int select;
    int flag;

    fs["select"]>>select;
    fs["flag"]>>flag;
    if (select == 1)
    {
    video vd(0,c1,flag);
    vd.camera_read_write();
    }
    else
    {
        video vd(c2,flag);
        vd.file_read();
    }
}
