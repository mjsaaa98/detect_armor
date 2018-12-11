#include "ImgFactory.h"


ImgFactory::ImgFactory(){
    filename = "/home/mjs/Videos/blue.avi";
    stop_pro = false;
    handle_flag = false;
    read_num = 0;
    process_num = 0;
#ifdef CAMERA_DEBUG
    mode = 2;
#else
    mode = 0;
#endif
    cout<<filename<<endl;
}

/**
 * @brief ImgFactory::Img_read  读取图像的一个线程
 */
void ImgFactory::Img_read(){
#ifdef VIDEO
    VideoCapture camera0;
    camera0.open(filename);
#else
    VideoCapture camera0(0);
#ifdef F640
    //设置摄像头分辨率为1280x720
    camera0.set(CV_CAP_PROP_FRAME_WIDTH,640);
    camera0.set(CV_CAP_PROP_FRAME_HEIGHT,480);
#else
    camera0.set(CV_CAP_PROP_FRAME_WIDTH,1280);
    camera0.set(CV_CAP_PROP_FRAME_HEIGHT,720);
#endif
#endif
    if (!camera0.isOpened())
    {
        cout << "Failed!"<<endl;
    }
    cout<<"摄像头设置打开成功！"<<endl;

    while (1)
    {
        double t1=0,t3 = 0;
        t1 = getTickCount();
        camera0 >> frame;
        if (frame.empty()) break;
        t3 = getTickCount();
        int fps_read = (t3-t1)/getTickFrequency()*1000;
        cout<<"time_read:"<<fps_read<<"ms"<<endl;

        while(handle_flag==true);  //可以处理标志位还为真，说明图片还没被传进去处理，一直等待。
        //加锁,在列表插入值，同时禁止读取
        Lock.lock();
//        std::lock_guard<std::mutex> guard(Lock);
        list_of_frame.push_back(frame);
        handle_flag = true;
        Lock.unlock();
    }
}


/**
 * @brief ImgFactory::Img_handle  处理图像线程
 */
void ImgFactory::Img_handle(){
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
#ifdef OPEN_SERIAL
    SerialPort sp;
    sp.initSerialPort();
#endif
    VisionData data = {0,0,0,0,0};
    RotatedRect RRect;
    find_armour f_armour(fs);
    float t=0;
    int numofpic = 0;
    Mat camera_location;
    float firstfind_t = 0,nowfind_t = 0,firnotfind_t = 0,nownotfind_t = 0;
    int isfirstfind = 1;
    int isfirnotfind = 0;
    int isreceiveflag = 0;   //是否接收到数据
    while(1)
    {
#ifdef OPEN_SERIAL
        sp.get_Mode(mode,receive_data,isreceiveflag);
#endif
        data.isfind = f_armour.isfind;
        Mat src;

        while(handle_flag==false);

        Lock.lock();
        src = list_of_frame.back();
        list_of_frame.clear();
        handle_flag = false;
        Lock.unlock();
        imshow("src",src);
        Mat dst = Mat::zeros(src.size(), CV_8UC1);
        if(mode == 0)
        {
            data.isfind = 0;
            data.pitch_angle.f = 0;
            data.yaw_angle.f = 0;
            isfirstfind = 1;
            isfirnotfind = 0;
        }
        else if (mode==3){
            Mat img = Mat::zeros(src.size(), src.type());
            img = f_armour.camshift_findarmor(src,dst);
            imshow("img",img);
            continue;
        }
        else dst = f_armour.find_blue4(src,dst,RRect,mode);

        double xAngle=0,yAngle=0,dis=0;
        if( data.isfind == 1)
        {
            if (ans.Rotated_SolveAngle(RRect,xAngle,yAngle,dis,camera_location,20,0,Point2f(0,0)))  //结算角度
            {
                //第一次接收数据标志
                if (isreceiveflag == 1)
                {
                    isreceiveflag = 0;
                }
                //第一次找到目标标志
                if (isfirstfind == 1)
                {
                    firstfind_t = getTickCount();
                    isfirstfind = 0;
                    isfirnotfind = 1;
                }
                nowfind_t = getTickCount();
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
                measurement.at<float>(1) = (float)yAngle+sum_yaw_offset;
                measurement.at<float>(2) = 0;
                measurement.at<float>(3) = vy;
                //5
                KF.correct(measurement);
                if((nowfind_t-firstfind_t)/getTickFrequency()*1000>=1000)   //1000ms之后再发送预测值
                {
                    data.pitch_angle.f = xAngle;//-KF.statePost.at<float>(0))*10+KF.statePost.at<float>(0);
                    data.yaw_angle.f = KF.statePost.at<float>(1)-sum_yaw_offset+KF.statePost.at<float>(3)*delta_t/1000;
                    data.dis.f = yAngle;
                }
                else
                {
#ifdef SHOW_DEBUG
                    cout<<"发送当前值的帧数:"<<num++<<endl;
#endif
                    data.pitch_angle.f = xAngle;//-KF.statePost.at<float>(0))*10+KF.statePost.at<float>(0);
                    data.yaw_angle.f = yAngle;
                    data.dis.f = yAngle;
                }
                history_yaw_offset.erase(history_yaw_offset.begin());   //清除已经被使用的第一个数据
                history_yaw_offset.push_back(data.yaw_angle.f);    //加入新传输的数据
#ifdef SHOW_DEBUG
                cout<<"当前历史数据中的第一个值:"<<history_yaw_offset[0]<<endl;
#endif
#else
                data.pitch_angle.f = xAngle;
                data.yaw_angle.f =  yAngle;
                data.dis.f = dis;
#endif
#ifdef KALMAN_2
                //1-2
                Mat prediction2 = KF2.predict();
                //3-4
                measurement2.at<float>(0) = data.yaw_angle.f;
                //5
                KF2.correct(measurement2);
                data.yaw_angle.f = KF2.statePost.at<float>(0);
#endif
            }
            else
            {
                data.pitch_angle.f = 0;
                data.yaw_angle.f =  0;
                data.dis.f = 0;
            }
        }
        else if(data.isfind==0)
        {
            if(isfirnotfind == 1)
            {
                firnotfind_t = getTickCount();
                isfirnotfind = 0;
                nowfind_t = 0;
            }
            nownotfind_t = getTickCount();
            numofpic = 0;
            isfirstfind = 1;
            data.pitch_angle.f = 0;
            data.yaw_angle.f = 0;
            data.dis.f = 0;
        }
        else
        {
            data.pitch_angle.f = 0;
            data.yaw_angle.f = 0;\
            data.dis.f = 0;
        }
#ifdef OPEN_SERIAL
        sp.TransformData(data);
#endif
        int i = waitKey(1);
        if( i=='q') break;
    }
#ifdef OPEN_SERIAL
    sp.Close();
#endif
}
