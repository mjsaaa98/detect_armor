#include "video.h"
#include <serialport.h>

video::video(string c)
{
    filename = c;
//    flag = f;
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
//    flag = f;
}

#ifndef VIDEO_DEBUG
//open camera and write .avi file.
//
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
    const int measureNum = 2;
    KalmanFilter KF(stateNum,measureNum);   //初始化

    KF.transitionMatrix = (Mat_<float>(4,4)<<1,0,1,0,0,1,0,1,0,0,1,0,0,0,0,1);
    setIdentity(KF.measurementMatrix);
    setIdentity(KF.measurementNoiseCov,Scalar::all(1));  //R
    setIdentity(KF.processNoiseCov,Scalar(10e-3));   //Q
    setIdentity(KF.errorCovPost,Scalar(10e-2));
    KF.statePost = (Mat_<float>(4,1)<<1,1,1,1);
//    cout<<KF.statePost<<endl;
    Mat measurement = Mat::zeros(measureNum,1,CV_32F);
#endif
    //各个类的初始化
#ifdef OPEN_SERIAL
    SerialPort sp;
    sp.initSerialPort();
#endif
    VisionData data = {0,0,0,0,0};
    RotatedRect RRect;
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



//    int FirstPic = 1;
    int numofpic = 0;
    Mat camera_location;
    Mat pre_camera_location;
    Mat now_camera_location;
    while (1)
    {
        Mat frame;
        double t1=0,t2=0;
        t1 = getTickCount();
        camera0 >> frame;
        if (frame.empty()) break;
        writer<<frame;
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

//        cout<<isfind<<endl;

        double xAngle=0,yAngle=0,dis=0;
//        double last_xAngle = 0,last_yAngle = 0;
        double move_dis = 0;
        double v = 0;
        if(mode == 0)
        {
            data.isfind = 0;
        }
        else if (ans.Rotated_SolveAngle(RRect,xAngle,yAngle,dis,camera_location,20,0,Point2f(0,0)))
        {
            if(data.isfind!= 0)
            {
//                FirstPic = 0;
                numofpic++;
                if (numofpic==1)
                {
                    pre_camera_location = camera_location.clone();
                }
                else
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
                    if(v>1)                    cout<<v<<"m/s"<<endl;
                }
//                cout<<"============"<<endl;
//                cout<<xAngle<<endl;
#ifdef KALMAN_OPEN
                //1-2
                Mat prediction = KF.predict();
//                float pre_xAngle = prediction.at<float>(0);
//                float pre_yAngle = prediction.at<float>(1);

                //3-4
                measurement.at<float>(0) = (float)xAngle;
                measurement.at<float>(1) = (float)yAngle;

                //5
                KF.correct(measurement);
                //预测下一帧的位置
                Mat next_Angle;   //下一帧的角度
                gemm(KF.transitionMatrix,KF.statePost,1,NULL,0,next_Angle);

            //发送预测的下一帧位置
                data.pitch_angle.f = next_Angle.at<float>(0);
                data.yaw_angle.f =  next_Angle.at<float>(1);

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
            else
            {
//                FirstPic = 1;
                numofpic = 0;
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
