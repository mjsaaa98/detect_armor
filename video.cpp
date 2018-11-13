#include "video.h"
#include <serialport.h>
video::video(string c)
{
    filename = c;
//    flag = f;
    mode = 2;
//    ismiddle = 0;
//    isfind = 0;
//    XY = Point2f(0,0);
}

video::video(int num,string c)
{
    n = num;
    mode = 2;
    filename = c;
//    flag = f;
}

//open camera and write .avi file.
//
void video::camera_read_write()
{
    VisionData data = {0,0,0,0,0};
    find_armour f_armour(fs);
    VideoCapture camera0(0);
    //设置摄像头分辨率为1280x720
    camera0.set(CV_CAP_PROP_FRAME_WIDTH,640);
    camera0.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    VideoWriter writer(filename, CV_FOURCC('M', 'J', 'P', 'G'), 10, Size(640, 480),true);
    if (!camera0.isOpened())
    {
        cout << "Failed!"<<endl;
    }

//    SerialPort sp;
//    sp.initSerialPort();


    while (1)
    {
        Mat frame;
//        double t1=0,t2=0;
//        t1 = getTickCount();
        camera0 >> frame;
        if (frame.empty()) break;
        writer<<frame;
        imshow("src",frame);

        Mat dst = Mat::zeros(frame.size(), CV_8UC1);

//        //gamma
//        vector<Mat> BGR;
//        split(frame,BGR);
//        for(int i = 0;i<3;i++)
//        {

//            Mat fI;
//            BGR[i].convertTo(fI,CV_64F,1.0/255,0);
//            pow(fI,0.5,BGR[i]);
//            BGR[i].convertTo(BGR[i],CV_8U,255,0);
//        }
//        merge(BGR,frame);

    //flag == 1 -- red
    //flag == 2 -- blue

//        sp.get_Mode(mode);
//        if(flag==2) dst = f_armour.find_blue1(frame,dst.clone());
//        if(mode==3) dst = f_armour.find_blue3(frame,dst.clone(),XY,ismiddle,isfind);
        if(mode==2) dst = f_armour.find_blue4(frame,dst.clone(),data);
//        if(flag==4) dst = f_armour.find_blue2(frame,dst.clone());
//        if(flag==5) dst = f_armour.find_red2(frame,dst.clone());
//        if(mode==1) dst = f_armour.find_red4(frame,dst.clone(),XY,ismiddle,isfind);

//        cout<<isfind<<endl;
//        VisionData data = {XY.x,XY.y,0,ismiddle,isfind};
//        sp.TransformData(data);


//        t2 = getTickCount();
//        double fps = (t2-t1)/getTickFrequency();
//        cout<<"time:"<<fps<<endl;
        int i = waitKey(30);
        if( i=='q') break;
    }
//    sp.Close();
    camera0.release();
}

void video::file_read()
{
    find_armour f_armour(fs);
    VideoCapture camera0;

    camera0.open(filename);
    if (!camera0.isOpened())
    {
        cout << "Failed!"<<endl;
    }


    while (1)
    {
        QTime time;
        time.start();
        Mat frame;
        double t1=0,t2=0;
        t1 = getTickCount();
        camera0 >> frame;
        if (frame.empty()) break;

//        imshow("src",frame);

        int ismiddle,isfind;
        Point XY;
        Mat dst = Mat::zeros(frame.size(), CV_8UC1);
//        if(flag==1) dst = f_armour.find_blue1(frame,dst.clone());
//        if(flag==2) dst = f_armour.find_blue3(frame,dst.clone(),XY,ismiddle,isfind);
//        if(mode==2) dst = f_armour.find_blue4(frame,dst.clone(),data);
//        if(flag==4) dst = f_armour.find_blue2(frame,dst.clone());
//        if(flag==5) dst = f_armour.find_red2(frame,dst.clone());
//        if(flag==6) dst = f_armour.find_red4(frame,dst.clone(),XY,ismiddle,isfind);

        imshow("dst",dst);
        t2 = getTickCount();
//        double fps = (t2-t1)/getTickFrequency()*1000;
//        cout<<"time:"<<fps<<"ms"<<endl;
        qDebug()<<"time:"<<time.elapsed()<<"ms";//输出计时

        int i = waitKey(10);
        if( i=='q') break;
    }
}
