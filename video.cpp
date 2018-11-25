#include "video.h"
#include <serialport.h>
//用于定时打印信息的计数值
int printflag = 0;
//获取速度
float GetSpeed(float angle,float last_angle,float t,float&v)
{
    v = (angle - last_angle)/(t)*1000;
    v = v>20?20:v;
    v = v<-20?-20:v;
    printflag++;
    if(printflag==100)
    {
        cout<<"v"<<endl;
        printflag = 0;
    }
}
video::video(string c)
{
    filename = c;
#ifdef CAMERA_DEBUG
    mode = 2;
#else
    mode = 0;
#endif
//    ismiddle = 0;
//    isfind = 0;
//    XY = Point2f(0,0);
}

video::video(int num,string c)
{
    n = num;
#ifdef CAMERA_DEBUG
    mode = 2;
#else
    mode = 0;
#endif
    filename = c;
}

#ifndef VIDEO_DEBUG
//open camera and write .avi file.
void video::camera_read_write()
{

    //solvePnP
    double camera_canshu[9] = {527.3444,0,337.5232,0,531.2206,254.4946,0,0,1};
    double dist_coeff[5] = {-0.4259,0.2928,-0.0106,-0.0031,0};
    Mat camera_matrix(3,3,CV_64FC1,camera_canshu);
    Mat dist_matrix(1,5,CV_64FC1,dist_coeff);
    AngleSolve ans(camera_matrix,dist_matrix,13.5,12.5,0,20,1000,1);
    double rot_c[] = {1,0,0,0,1,0,0,0,1};
    double tran_c[] = {0,0,0};
    Mat rot_martrix(3,3,CV_64FC1,rot_c);
    Mat tran_matrix(3,1,CV_64FC1,tran_c);
    ans.Relation_Camera_PTZ(rot_martrix,tran_matrix,0);
#ifdef KALMAN_OPEN
    //kalman
    const int stateNum = 4;
    const int measureNum = 4;
    KalmanFilter KF(stateNum,measureNum);   //初始化
    KF.transitionMatrix = (Mat_<float>(4,4)<<1,0,0.01,0,0,1,0,0.01,0,0,1,0,0,0,0,1);
    setIdentity(KF.measurementMatrix);   //H
    setIdentity(KF.processNoiseCov,Scalar(1));   //Q
    KF.measurementNoiseCov = (Mat_<float>(4,4)<<2000,0,0,0,0,2000,0,0,0,0,5000,0,0,0,0,5000);//R
    setIdentity(KF.errorCovPost,Scalar(1));//P
    //x(k-1)
    KF.statePost = (Mat_<float>(4,1)<<1,1,1,1);
    //z
    Mat measurement = Mat::zeros(measureNum,1,CV_32F);
#endif
    //各个类的初始化
#ifdef OPEN_SERIAL
    SerialPort sp;
    sp.initSerialPort();
#endif
    //初始化目标数据
    VisionData data = {0,0,0,0,0};
    RotatedRect RRect;
    find_armour f_armour(fs);

    //打开摄像头
    VideoCapture camera0(0);
    //设置摄像头分辨率为1280x720or640x480
    camera0.set(CV_CAP_PROP_FRAME_WIDTH,640);
    camera0.set(CV_CAP_PROP_FRAME_HEIGHT,480);
//    VideoWriter writer(filename, CV_FOURCC('M', 'J', 'P', 'G'), 10, Size(640, 480),true);
    if (!camera0.isOpened())
    {
        cout << "Failed!"<<endl;
    }


    float t=0;
    float last_time = 0;
    int numofpic = 0;
    Mat camera_location;
//    Mat pre_camera_location;
//    Mat now_camera_location;
    //初始化x、y方向的速度
    float vx=0,vy=0;
    KFparam kp = {0,0,0};   //初始化卡尔曼参数结构体
    //第一次找到的时刻、找到时现在的时刻、第一次没找到的时刻、没找到时现在的时刻。 主要用于400ms开预测和100ms消抖操作。
    float firstfind_t = 0,nowfind_t = 0,firnotfind_t = 0,nownotfind_t = 0;
    int isfirstfind = 1;   //是否第一次找到标帜位
    int isfirnotfind = 1;   //是否第一次没找到标帜位
    int isfind_flag = 0;    //是否找到标帜位
    while (1)
    {
        t = getTickCount();
        float delta_t = (t-last_time)/getTickFrequency()*1000;   //计算两帧之间的时间差
        cout<<delta_t<<"ms"<<endl;
        last_time = t;
        Mat frame;
//        double t1=0,t2=0;
//        t1 = getTickCount();
        camera0 >> frame;
        if (frame.empty()) break;
//        writer<<frame;
        imshow("src",frame);

        Mat dst = Mat::zeros(frame.size(), CV_8UC1);
/**
        //gamma
        vector<Mat> BGR;
        split(frame,BGR);
        for(int i = 0;i<3;i++)
        {

            Mat fI;
            BGR[i].convertTo(fI,CV_64F,1.0/255,0);
            pow(fI,0.5,BGR[i]);
            BGR[i].convertTo(BGR[i],CV_8U,255,0);
        }
        merge(BGR,frame);
*/
#ifdef OPEN_SERIAL
        sp.get_Mode(mode);
#endif
//        if(flag==2) dst = f_armour.find_blue1(frame,dst.clone());
//        if(mode==3) dst = f_armour.find_blue3(frame,dst.clone(),XY,ismiddle,isfind);
        if(mode==2) dst = f_armour.find_blue4(frame,dst.clone(),data,RRect);
//        if(flag==4) dst = f_armour.find_blue2(frame,dst.clone());
//        if(flag==5) dst = f_armour.find_red2(frame,dst.clone());
//        if(mode==1) dst = f_armour.find_red4(frame,dst.clone(),XY,ismiddle,isfind);

        isfind_flag = data.isfind;   //是否找到的标帜物位
        double xAngle=0,yAngle=0,dis=0;  //初始化角度和距离

//        double last_xAngle = 0,last_yAngle = 0;
//        double move_dis = 0;
//        double v = 0;

        if(mode == 0)
        {
            data.isfind = 0;
            isfirstfind = 1;

        }
        else if (ans.Rotated_SolveAngle(RRect,xAngle,yAngle,dis,camera_location,20,0,Point2f(0,0)))
        {
            if(isfind_flag!= 0)
            {
                nowfind_t = getTickCount();
                if (isfirstfind == 1)
                {
                    isfirnotfind = 1;
                    firstfind_t = getTickCount();
                    isfirstfind = 0;
                }
                if((nowfind_t-firstfind_t)/getTickFrequency()*1000>=380)
                {

                    now_camera_location = camera_location.clone();
                    double x1 = now_camera_location.at<double>(0,0);
                    double x2 = pre_camera_location.at<double>(0,0);
                    double y1 = now_camera_location.at<double>(1,0);
                    double y2 = pre_camera_location.at<double>(1,0);
                    double z1 = now_camera_location.at<double>(2,0);
                    double z2 = pre_camera_location.at<double>(2,0);
                    move_dis = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2));
                    t2 = getTickCount();
                    v = move_dis/((t2-t1)/getTickFrequency()*1000)*10;
                    pre_camera_location = now_camera_location.clone();
                    if(v>1) cout<<v<<"m/s"<<endl;
                }

//                cout<<"============"<<endl;
//                cout<<xAngle<<endl;
#ifdef KALMAN_OPEN
                    //1-2
                    Mat prediction = KF.predict();
                    float pre_xAngle = prediction.at<float>(0);
                    float pre_yAngle = prediction.at<float>(1);

                    //get speed
                    GetSpeed(pre_xAngle,kp.last_xAngle,delta_t,vx);
                    kp.last_xAngle = pre_xAngle;
                    GetSpeed(pre_yAngle,kp.last_yAngle,delta_t,vy);
                    kp.last_yAngle = pre_yAngle;
                    //3-4
                    measurement.at<float>(0) = (float)xAngle;
                    measurement.at<float>(1) = (float)yAngle;
                    measurement.at<float>(2) = vx;
                    measurement.at<float>(3) = vy;

                    //5
                    KF.correct(measurement);
                    //预测下一帧的位置
    //                Mat next_Angle;   //下一帧的角度
    //                gemm(KF.transitionMatrix,KF.statePost,1,NULL,0,next_Angle);

            //发送预测的下一帧位置
                data.pitch_angle.f = (next_Angle.at<float>(0)-KF.statePost.at<float>(0))*5+KF.statePost.at<float>(0);
                data.yaw_angle.f =  (next_Angle.at<float>(1)-KF.statePost.at<float>(1))*5+KF.statePost.at<float>(1);


                //发送本帧预测值
    //                data.pitch_angle.f = pre_xAngle;
    //                data.yaw_angle.f =  pre_yAngle;
                    data.dis.f = dis;
    //                cout<<dis<<endl;
    //                cout<<pre_xAngle<<endl;
    #else
                    data.pitch_angle.f = xAngle;
                    data.yaw_angle.f =  yAngle;
                    data.dis.f = dis;
    #endif
        //            cout<<xAngle<<","<<yAngle<<endl;
                }
                else{
                    data.pitch_angle.f = xAngle;
                    data.yaw_angle.f =  yAngle;
                    data.dis.f = dis;
                }
            }
            else
            {
                nownotfind_t = getTickCount();
                if(isfirnotfind == 1)
                {
                    firnotfind_t = getTickCount();
                    isfirnotfind = 0;
                }
                if ((nownotfind_t-firnotfind_t)/getTickFrequency()*1000<100)
                {
                    data.pitch_angle.f = 0;
                    data.yaw_angle.f = 0;
                    data.isfind = 1;
                }
                else
                {
                    numofpic = 0;
                    isfirstfind = 1;
                }
            }
        }

#ifdef OPEN_SERIAL
        sp.TransformData(data);
#endif

//        t2 = getTickCount();
//        double fps = (t2-t1)/getTickFrequency();
//        cout<<"time:"<<fps<<endl;
        int i = waitKey(1);
        if( i=='q') break;
    }
//    sp.Close();
    camera0.release();
}
#else
void video::file_read()
{
    find_armour f_armour(fs);
    VideoCapture camera0;

    VisionData data = {0,0,0,0,0};
    RotatedRect RRect;

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
        if(mode==2) dst = f_armour.find_blue4(frame,dst.clone(),data,RRect);
//        if(mode==2) dst = f_armour.find_blue4(frame,dst.clone(),data);
//        if(flag==4) dst = f_armour.find_blue2(frame,dst.clone());
//        if(flag==5) dst = f_armour.find_red2(frame,dst.clone());
//        if(flag==6) dst = f_armour.find_red4(frame,dst.clone(),XY,ismiddle,isfind);

        imshow("dst",dst);
        t2 = getTickCount();
//        double fps = (t2-t1)/getTickFrequency()*1000;
//        cout<<"time:"<<fps<<"ms"<<endl;
//        qDebug()<<"time:"<<time.elapsed()<<"ms";//输出计时

        int i = waitKey(0);
        if( i=='q') break;
    }
}
#endif //VIDEO_DEBUG
