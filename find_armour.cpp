#include "find_armour.h"
#include "get_colors.h"
#include "send_location.h"
#include <anglesolve.h>
find_armour::find_armour(FileStorage f)
{
    fs = f;
    fs["y_dist_wucha_ROI"] >> y_dist_wucha_ROI;
    fs["height_d_wucha_ROI"] >> height_d_wucha_ROI;
    fs["area_min"] >> area_min;
    fs["angle1"] >> a1;
    fs["angle2"] >> a2;
    fs["y_dist_wucha"] >> y_dist_wucha;
    fs["height_d_wucha"] >> height_d_wucha;
    fs["min_rate"] >> min_rate;
    fs["max_rate"] >> max_rate;
    isfind = 0;
    ismiddle =0;
}


Mat find_armour::find_blue2(Mat img,Mat dst)
{
    get_colors gc;      //class get_colors

    //get_red and find contours
    vector<vector<Point> > contours;


    //
    //dst = gc.HSV_blue(img,dst.clone());
    //1
    dst = gc.HSV_blue1(img,dst.clone());


    findContours(dst,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);

    int num = contours.size();   //contour's amount

    vector<RotatedRect> rRect(num);    //save rotatedrect

    vector<double> areas(num);    //save area

    vector<double> area;

    vector<Mat> vertices(num);    //find point to line rect.

    vector<Vec4f> con_prams;    //save contuor's pram,X Y and area

//    Point2f last_center = Point(640,360);

//    static double last_d;

    static int flags = 0;
//    cout<<flags<<endl;


    for(int i = 0;i<num;i++)
    {
        //traverse all contours

        rRect[i] = minAreaRect(contours[i]);
        areas[i] =  contourArea(contours[i]);


        //find four top_points
        boxPoints(rRect[i],vertices[i]);

        cout<<vertices.size()<<endl;

        //filter small rectangles,and list two conditions of rect.
        //cout<<"x:"<<rRect[i].center.x<<endl;
        //长宽分明时。

        if(areas[i]>area_min&&((rRect[i].size.height>rRect[i].size.width&&rRect[i].angle>a1)
                 ||(rRect[i].size.height<rRect[i].size.width&&rRect[i].angle<a2)))
        {
            //if Yes,draw rect and save pram.
            for(int k = 0;k<4;k++)
            {
                Point p1 = Point(vertices[i].row(k));
                int n = (k+1)%4;
                Point p2 = Point(vertices[i].row(n));
                line(img,p1,p2,Scalar(255,0,0),1);
            }
            double h = rRect[i].size.height;
            if(rRect[i].size.height<rRect[i].size.width)
            {
                h = rRect[i].size.width;
            }
            //cout<<h<<'\t'<< rRect[i].size.width<<'\t'<< rRect[i].size.height<<endl;
            //cout<<i<<'\t'<<rRect[i].center<<endl;
            Vec4f con_pram(h,rRect[i].center.x,rRect[i].center.y,rRect[i].angle);
            area.push_back(areas[i]);
            con_prams.push_back(con_pram);
//            cout<<con_prams.size()<<endl;


//        //长宽差不多
//        if(areas[i]>100)
//        {
//            cout<<areas[i]<<endl;
//            //if Yes,draw rect and save pram.
//            for(int k = 0;k<4;k++)
//            {
//                Point p1 = Point(vertices[i].row(k));
//                int j = (k+1)%4;
//                Point p2 = Point(vertices[i].row(j));
//                line(dst,p1,p2,Scalar(255));
//            }
//            //cout<<i<<'\t'<<rRect[i].center<<endl;
//            Vec4f con_pram(areas[i],rRect[i].center.x,rRect[i].center.y,rRect[i].angle);
//            con_prams.push_back(con_pram);
        }
    }

    vector<double> diameters;
    vector<Point2f> armour_center;
    for(int i=0;i<con_prams.size();i++)
    {
        for(int j = i+1;j<con_prams.size();j++)
        {
            double height1 = con_prams[i][0];
            double height2 = con_prams[j][0];

            double y1 = con_prams[i][2];
            double y2 = con_prams[j][2];

            double angle1 = con_prams[i][3];
            double angle2 = con_prams[j][3];

            double area_rate = area[i]/area[j];
            double height_d = abs(height2-height1);
//            double angle_d = abs(angle2-angle1);
//            double angle_a = abs(angle1+angle2);
            double y_dist = abs(y2-y1);

            // Y and Y 's distance must less wucha.and area's rate must bettwen min and max
            if(y_dist<y_dist_wucha&&height_d<height_d_wucha
                    &&area_rate>min_rate&&area_rate<max_rate)
            {


                //get circle diameter
                double d=sqrt(pow(con_prams[i][1]-con_prams[j][1],2)
                        +pow(con_prams[i][2]-con_prams[j][2],2));

                //cout<<d<<endl;
                diameters.push_back(d);
                Point center=Point((con_prams[i][1]+con_prams[j][1])*0.5,
                        (con_prams[i][2]+con_prams[j][2])*0.5);
                armour_center.push_back(center);

            }
        }
    }
    //cout<<diameter.size()<<endl;
    double d,diameter;
    Point2f center;
    if(flags == 1)
    {

        if (diameters.size()!=0)
        {
            diameter = diameters[0];
            center = armour_center[0];
            if(diameters.size() != 1)
            {
                for(int i = 0;i<diameters.size()-1;i++)
                {
                    if (diameters[i]>diameters[i+1])
                    {
                        d = diameters[i+1];
                        diameters[i+1] = diameters[i];
                        diameters[i] = d;
                        center = armour_center[i+1];
                        armour_center[i+1] = armour_center[i];
                        armour_center[i] = center;
                    }

                }
                diameter = diameters[0];
                center = armour_center[0];
//                d1 = diameters[0];
//                d2 = diameters[1];
//                d_d1 = abs(d1-last_d);
//                d_d2 = abs(d2-last_d);
//                cout<<d_d1<<endl;
//                cout<<last_d<<endl;
//                cout<<endl;
//                double cen_d1 = (last_center.x-armour_center[0].x)+abs(last_center.y-armour_center[0].y);
//                double cen_d2 = (last_center.x-armour_center[1].x)+abs(last_center.y-armour_center[1].y);
//                if((d_d2)<(d_d1))
//                {
//                    center = armour_center[1];
//                    diameter = diameters[1];
//                }
//                else
//                {
//                    center = armour_center[0];
//                    diameter = diameters[0];
//                }
            }
        }
        circle(dst,center,diameter*0.5,Scalar(255),1);

    }
    else
    {
        if(diameters.size()!=0)
        {
            diameter = diameters[0];
            center = armour_center[0];   //中心
            //cout<<"d: "<<d<<endl;

            for (int i = 1;i<diameters.size();i++)
            {
                d = diameters[i];
                if(diameter>d)
                {
                    diameter = d;
                    center = armour_center[i];
                }
            }
            flags = 1;


//            last_center = center;
//            last_d = diameter;

            //        cout<<last_center<<endl;
            //        cout<<last_d<<endl;
            circle(dst,center,diameter*0.5,Scalar(255),1);
        }
        else
        {
            flags = 0;
        }
    }

//    imshow("i",img);
    return dst;
}

//-----------------------------------------------------------------------------------

Mat find_armour::find_red1(Mat img,Mat dst)
{
    get_colors gc;      //class get_colors
    //get_red and find contours
    vector<vector<Point> > contours;

    //
    //dst = gc.HSV_blue(img,dst.clone());
    //1
    dst = gc.HSV_blue1(img,dst.clone());

    dst = gc.HSV_red1(img,dst.clone());
    findContours(dst,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);


    int num = contours.size();   //contour's amount

    vector<RotatedRect> rRect(num);    //save rotatedrect

    vector<double> areas(num);    //save area

    vector<Mat> vertices(num);    //find point to line rect.

    vector<Vec4f> con_prams;    //save contuor's pram,X Y and area

    for(int i = 0;i<num;i++)
    {
        //traverse all contours

        rRect[i] = minAreaRect(contours[i]);
        areas[i] =  contourArea(contours[i]);


        //find four top_points
        boxPoints(rRect[i],vertices[i]);

        //filter small rectangles,and list two conditions of rect.
        //cout<<"x:"<<rRect[i].center.x<<endl;
        //长宽分明时。
        if(areas[i]>50&&((rRect[i].size.height>rRect[i].size.width&&rRect[i].angle>-20)
                 ||(rRect[i].size.height<rRect[i].size.width&&rRect[i].angle<-60)))
        {
            //if Yes,draw rect and save pram.
            for(int k = 0;k<4;k++)
            {
                Point p1 = Point(vertices[i].row(k));
                int j = (k+1)%4;
                Point p2 = Point(vertices[i].row(j));
                line(dst,p1,p2,Scalar(255),1);
            }
            //cout<<i<<'\t'<<rRect[i].center<<endl;
            Vec4f con_pram(areas[i],rRect[i].center.x,rRect[i].center.y,rRect[i].angle);
            con_prams.push_back(con_pram);

//        //长宽差不多
//        if(areas[i]>100)
//        {
//            cout<<areas[i]<<endl;
//            //if Yes,draw rect and save pram.
//            for(int k = 0;k<4;k++)
//            {
//                Point p1 = Point(vertices[i].row(k));
//                int j = (k+1)%4;
//                Point p2 = Point(vertices[i].row(j));
//                line(dst,p1,p2,Scalar(255));
//            }
//            //cout<<i<<'\t'<<rRect[i].center<<endl;
//            Vec4f con_pram(areas[i],rRect[i].center.x,rRect[i].center.y,rRect[i].angle);
//            con_prams.push_back(con_pram);
        }
    }

    vector<double> diameter;
    vector<Point2f> armour_center;
    for(int i=0;i<con_prams.size();i++)
    {
        for(int j = i+1;j<con_prams.size();j++)
        {


            double area1 = con_prams[i][0];
            double area2 = con_prams[j][0];

            double y1 = con_prams[i][2];
            double y2 = con_prams[j][2];

            double angle1 = con_prams[i][3];
            double angle2 = con_prams[j][3];

            double area_rate = area1/area2;
            double y_dist = abs(y2-y1);
            double y_dist_wucha = 30;
            double angle_wucha = 20;
            double max_rate = 2;
            double min_rate = 0.5;

            // Y and Y 's distance must less wucha.and area's rate must bettwen min and max
            if(y_dist<y_dist_wucha&&area_rate>min_rate&&area_rate<max_rate&&abs(angle1-angle2)<angle_wucha)
            {
                //get circle diameter
                double d=sqrt(pow(con_prams[i][1]-con_prams[j][1],2)
                        +pow(con_prams[i][2]-con_prams[j][2],2));


                diameter.push_back(d);
                Point center=Point((con_prams[i][1]+con_prams[j][1])*0.5,
                        (con_prams[i][2]+con_prams[j][2])*0.5);
                armour_center.push_back(center);

            }
        }
    }
    if (diameter.size()!=0)
    {
        double d = diameter[0];
        Point center = armour_center[0];   //中心
        cout<<"d: "<<d<<endl;

        for (int i = 1;i<diameter.size();i++)
        {
            double d1 = diameter[i];
            if(d>d1)
            {
                d = d1;
                center = armour_center[i];
            }
        }
        // draw circle
        circle(dst,center,100,Scalar(255),1);
        cout<<"d: "<<d<<endl;
    }

    return dst;
}



Mat find_armour::find_red2(Mat img,Mat dst)
{
    get_colors gc;      //class get_colors

    //get_red and find contours
    vector<vector<Point> > contours;


    //
    //dst = gc.HSV_blue(img,dst.clone());
    //1
    dst = gc.HSV_red1(img,dst.clone());


    findContours(dst,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);

    int num = contours.size();   //contour's amount

    vector<RotatedRect> rRect(num);    //save rotatedrect

    vector<double> areas(num);    //save area

    vector<double> area;

    vector<Mat> vertices(num);    //find point to line rect.

    vector<Vec4f> con_prams;    //save contuor's pram,X Y and area

    Point2f last_center;

    double last_d;

    static int flags = 0;
//    cout<<flags<<endl;


    for(int i = 0;i<num;i++)
    {
        //traverse all contours

        rRect[i] = minAreaRect(contours[i]);
        areas[i] =  contourArea(contours[i]);


        //find four top_points
        boxPoints(rRect[i],vertices[i]);

        //filter small rectangles,and list two conditions of rect.
//        cout<<"x:"<<rRect[i].center.x<<endl;
        //长宽分明时。
        if(areas[i]>10&&((rRect[i].size.height>rRect[i].size.width&&rRect[i].angle>-20)
                 ||(rRect[i].size.height<rRect[i].size.width&&rRect[i].angle<-60)))
        {
            //if Yes,draw rect and save pram.
            for(int k = 0;k<4;k++)
            {
                Point p1 = Point(vertices[i].row(k));
                int n = (k+1)%4;
                Point p2 = Point(vertices[i].row(n));
                line(img,p1,p2,Scalar(255,0,0),1);
            }
            double h = rRect[i].size.height;
            if(rRect[i].size.height<rRect[i].size.width)
            {
                h = rRect[i].size.width;
            }
            //cout<<h<<'\t'<< rRect[i].size.width<<'\t'<< rRect[i].size.height<<endl;
            cout<<i<<'\t'<<rRect[i].center.y<<endl;
            Vec4f con_pram(h,rRect[i].center.x,rRect[i].center.y,rRect[i].angle);
            area.push_back(areas[i]);
            con_prams.push_back(con_pram);

//        //长宽差不多
//        if(areas[i]>10)
//        {
//            //cout<<areas[i]<<endl;
//            //if Yes,draw rect and save pram.
//            for(int k = 0;k<4;k++)
//            {
//                Point p1 = Point(vertices[i].row(k));
//                int j = (k+1)%4;
//                Point p2 = Point(vertices[i].row(j));
//                line(dst,p1,p2,Scalar(255));
//            }
//            double h = rRect[i].size.height;
//            if(rRect[i].size.height<rRect[i].size.width)
//            {
//                h = rRect[i].size.width;
//            }
//            //cout<<h<<'\t'<< rRect[i].size.width<<'\t'<< rRect[i].size.height<<endl;
//            cout<<i<<'\t'<<rRect[i].center.y<<endl;
//            Vec4f con_pram(h,rRect[i].center.x,rRect[i].center.y,rRect[i].angle);
//            area.push_back(areas[i]);
//            con_prams.push_back(con_pram);
        }
    }

    vector<double> diameters;
    vector<Point2f> armour_center;
    for(int i=0;i<con_prams.size();i++)
    {
        for(int j = i+1;j<con_prams.size();j++)
        {
            double height1 = con_prams[i][0];
            double height2 = con_prams[j][0];

            double y1 = con_prams[i][2];
            double y2 = con_prams[j][2];

            double angle1 = con_prams[i][3];
            double angle2 = con_prams[j][3];

            double area_rate = area[i]/area[j];
            double height_d = abs(height2-height1);
            double angle_d = abs(angle2-angle1);
            double angle_a = abs(angle1+angle2);
            double y_dist = abs(y2-y1);
            double y_dist_wucha = 20;
            double angle_wucha = 20;
            double max_rate = 2.8;
            double min_rate = 0.2;

            double height_d_wucha = 10;

            // Y and Y 's distance must less wucha.and area's rate must bettwen min and max
            if(y_dist<y_dist_wucha&&(angle_d<20||angle_d>50)&&height_d<height_d_wucha
                    &&area_rate>min_rate&&area_rate<max_rate)
            {


                //get circle diameter
                double d=sqrt(pow(con_prams[i][1]-con_prams[j][1],2)
                        +pow(con_prams[i][2]-con_prams[j][2],2));

                //cout<<d<<endl;
                diameters.push_back(d);
                Point center=Point((con_prams[i][1]+con_prams[j][1])*0.5,
                        (con_prams[i][2]+con_prams[j][2])*0.5);
                armour_center.push_back(center);

            }
        }
    }
    //cout<<diameter.size()<<endl;
    double d,d1,d2,diameter,d_d1,d_d2;
    Point2f center;
    if(flags == 1)
    {

        if (diameters.size()!=0)
        {
            diameter = diameters[0];
            center = armour_center[0];
            if(diameters.size() != 1)
            {
                for(int i = 0;i<diameters.size()-1;i++)
                {
                    if (diameters[i]>diameters[i+1])
                    {
                        d = diameters[i+1];
                        diameters[i+1] = diameters[i];
                        diameters[i] = d;
                        center = armour_center[i+1];
                        armour_center[i+1] = armour_center[i];
                        armour_center[i] = center;
                    }

                }

                d1 = diameters[0];
                d2 = diameters[1];
                d_d1 = abs(d1-last_d);
                d_d2 = abs(d2-last_d);
//                cout<<d_d1<<endl;
//                cout<<d_d2<<endl;
//                cout<<endl;
                double cen_d1 = (last_center.x-armour_center[0].x)+abs(last_center.y-armour_center[0].y);
                double cen_d2 = (last_center.x-armour_center[1].x)+abs(last_center.y-armour_center[1].y);
                if((d_d2)<(d_d1))
                {
                    center = armour_center[1];
                    diameter = diameters[1];
                }
                else
                {
                    center = armour_center[0];
                    diameter = diameters[0];
                }
            }
        }
        circle(dst,center,diameter*0.5,Scalar(255),1);
    }
    else
    {
        if(diameters.size()!=0)
        {
            diameter = diameters[0];
            center = armour_center[0];   //中心
            //cout<<"d: "<<d<<endl;

            for (int i = 1;i<diameters.size();i++)
            {
                d = diameters[i];
                if(diameter>d)
                {
                    diameter = d;
                    center = armour_center[i];
                }
            }
            flags = 1;


            last_center = center;
            last_d = diameter;

            //        cout<<last_center<<endl;
            //        cout<<last_d<<endl;
            circle(dst,center,diameter*0.5,Scalar(255),1);
        }
        else
        {
            flags = 0;
        }

    }
//    imshow("i",img);
    return dst;
}






Mat find_armour::find_blue1(Mat img,Mat dst)
{

    get_colors gc;      //class get_colors

    //get_red and find contours
    vector<vector<Point> > contours;

    dst = gc.HSV_blue1(img,dst.clone());


    findContours(dst,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);

    int num = contours.size();   //contour's amount

    vector<RotatedRect> rRect(num);    //save rotatedrect

    vector<double> areas(num);    //save area

    vector<double> area;

    vector<Mat> vertices(num);    //find point to line rect.

    vector<Vec4f> con_prams;    //save contuor's pram,X Y and area

    static Point2f last_center;

    static Mat dstROI;

    static double last_d;

    static int flags =0;

    int isfind = 0;

    //traverse all contours
    for(int i = 0;i<num;i++)
    {

        rRect[i] = minAreaRect(contours[i]);
        areas[i] =  contourArea(contours[i]);


        //find four top_points
        boxPoints(rRect[i],vertices[i]);
        //长宽分明时。

            if(areas[i]>50&&((rRect[i].size.height>rRect[i].size.width&&rRect[i].angle>-20)
                     ||(rRect[i].size.height<rRect[i].size.width&&rRect[i].angle<-60)))
            {
                //if Yes,draw rect and save pram.
                for(int k = 0;k<4;k++)
                {
                    Point p1 = Point(vertices[i].row(k));
                    int n = (k+1)%4;
                    Point p2 = Point(vertices[i].row(n));
                    line(img,p1,p2,Scalar(0,255,0),2);
                }
                double h = rRect[i].size.height;
                if(rRect[i].size.height<rRect[i].size.width)
                {
                    h = rRect[i].size.width;
                }
                //cout<<h<<'\t'<< rRect[i].size.width<<'\t'<< rRect[i].size.height<<endl;
                //cout<<i<<'\t'<<rRect[i].center<<endl;
                Vec4f con_pram(h,rRect[i].center.x,rRect[i].center.y,rRect[i].angle);
                area.push_back(areas[i]);
                con_prams.push_back(con_pram);
            }

    }

    vector<double> diameters;
    vector<Point2f> armour_center;
    for(int i=0;i<con_prams.size();i++)
    {
        for(int j = i+1;j<con_prams.size();j++)
        {
            double height1 = con_prams[i][0];
            double height2 = con_prams[j][0];

            double y1 = con_prams[i][2];
            double y2 = con_prams[j][2];

//            double angle1 = con_prams[i][3];
//            double angle2 = con_prams[j][3];

            double area_rate = area[i]/area[j];
            double height_d = abs(height2-height1);
//            double angle_d = abs(angle2-angle1);
//            double angle_a = abs(angle1+angle2);
            double y_dist = abs(y2-y1);

            double height_d_wucha = 10;

            // Y and Y 's distance must less wucha.and area's rate must bettwen min and max
            if(y_dist<y_dist_wucha&&height_d<height_d_wucha
                    &&area_rate>min_rate&&area_rate<max_rate)
            {


                //get circle diameter
                double d=sqrt(pow(con_prams[i][1]-con_prams[j][1],2)
                        +pow(con_prams[i][2]-con_prams[j][2],2));

                //cout<<d<<endl;
                diameters.push_back(d);
                Point center=Point((con_prams[i][1]+con_prams[j][1])*0.5,
                        (con_prams[i][2]+con_prams[j][2])*0.5);
                armour_center.push_back(center);
            }
        }
    }

    double d;
    Point center;
    vector<Point> last_center_v;
    vector<double> last_d_v;
    //no target
    if(flags == 1)
    {
        if(armour_center.size()==0)
        {
            flags = 0;
        }
        else if(armour_center.size()==1)
        {
            last_center = armour_center[0];
            last_d = diameters[0];
            dstROI = roi(img,last_center,last_d,img.cols,img.rows);
            circle(img,last_center,last_d/2,Scalar(0,255,0));
        }
        else
        {
            for (int i = 0;i<diameters.size();i++)
            {
                if(armour_center[i].x>=x1&&armour_center[i].x<=x2
                        &&armour_center[i].y>=y1&&armour_center[i].x<=y2)
                {
                    last_center_v.push_back(armour_center[i]);
                    last_d_v.push_back(diameters[i]);
                    isfind += 1;
                }

            }
            if(isfind == 0)
            {
                flags = 0;
            }
            else
            {
                for (int j = 0;j<last_center_v.size();j++)
                {
                    if((last_d_v[j]*last_d_v[j]/((x2-x1)*(y2-y1)))<1)
                    {
                        last_center = last_center_v[j];
                        last_d = last_d_v[j];
                        dstROI = roi(img,last_center,last_d,img.cols,img.rows);
                        circle(img,last_center,last_d/2,Scalar(0,255,0));
                        flags = 1;
                        break;
                    }
                    flags = 0;
                }

            }
        }
    }
    if(flags == 0)
    {
        if(armour_center.size()==0){
            flags =  0;
            dstROI = Mat::zeros(dst.size(),dst.type());
        }
        else if(armour_center.size()==1){
            flags = 1;
            last_center = armour_center[0];
            last_d = diameters[0];
            dstROI = roi(img,last_center,last_d,img.cols,img.rows);
            circle(img,last_center,last_d/2,Scalar(255,0,0));
        }
        else
        {
            for (int i = 0;i<diameters.size()-1;i++)
            {
                if(diameters[i]>diameters[i+1])
                {
                    d = diameters[i+1];
                    diameters[i+1] = diameters[i];
                    diameters[i] = d;
                    center = armour_center[i+1];
                    armour_center[i+1] = armour_center[i];
                    armour_center[i] = center;
                }
            }
            last_center = armour_center[0];
            last_d = diameters[0];
            flags = 1;
            dstROI = roi(img,last_center,last_d,img.cols,img.rows);
            circle(img,last_center,last_d/2,Scalar(255,0,0));
        }
    }
    send_location send_l;
    send_l.send_coordinate(last_center);
    imshow("ROI",dstROI);
    imshow("find",img);
    return dst;
}

//find roi
Mat find_armour::roi(Mat img,Point center,double d,int cols,int rows)
{
    Mat roi;
    x1 = center.x-d*0.8;
    x2 = center.x+d*0.8;
    y1 = center.y-d*1.0;
    y2 = center.y+d*1.0;
    if(x1<=0) x1 = 1;
    if(x2>cols) x2 = cols-1;
    if(y1<=0) y1 = 1;
    if(y2>=rows) y2 = rows-1;
    roi = img(Range(y1,y2),Range(x1,x2));
    return roi;
}

Mat find_armour::find_red3(Mat img,Mat dst)
{
    get_colors gc;      //class get_colors

    //get_red and find contours
    vector<vector<Point> > contours;


    //
    //dst = gc.HSV_blue(img,dst.clone());
    //1
    dst = gc.HSV_red1(img,dst.clone());


    findContours(dst,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);

    int num = contours.size();   //contour's amount

    vector<RotatedRect> rRect(num);    //save rotatedrect

    vector<double> areas(num);    //save area

    vector<double> area;

    vector<Mat> vertices(num);    //find point to line rect.

    vector<Vec4f> con_prams;    //save contuor's pram,X Y and area

    static Point2f last_center;

    static double last_d;

    static int flags = 0;
//    cout<<flags<<endl;


    for(int i = 0;i<num;i++)
    {
        //traverse all contours

        rRect[i] = minAreaRect(contours[i]);
        areas[i] =  contourArea(contours[i]);


        //find four top_points
        boxPoints(rRect[i],vertices[i]);

        //filter small rectangles,and list two conditions of rect.
//        cout<<"x:"<<rRect[i].center.x<<endl;
        //长宽分明时。
        if(areas[i]>10&&((rRect[i].size.height>rRect[i].size.width&&rRect[i].angle>-20)
                 ||(rRect[i].size.height<rRect[i].size.width&&rRect[i].angle<-60)))
        {
            //if Yes,draw rect and save pram.
            for(int k = 0;k<4;k++)
            {
                Point p1 = Point(vertices[i].row(k));
                int n = (k+1)%4;
                Point p2 = Point(vertices[i].row(n));
                line(img,p1,p2,Scalar(255,0,0),1);
            }
            double h = rRect[i].size.height;
            if(rRect[i].size.height<rRect[i].size.width)
            {
                h = rRect[i].size.width;
            }
            //cout<<h<<'\t'<< rRect[i].size.width<<'\t'<< rRect[i].size.height<<endl;
//            cout<<i<<'\t'<<rRect[i].center.y<<endl;
            Vec4f con_pram(h,rRect[i].center.x,rRect[i].center.y,rRect[i].angle);
            area.push_back(areas[i]);
            con_prams.push_back(con_pram);

//        //长宽差不多
//        if(areas[i]>10)
//        {
//            //cout<<areas[i]<<endl;
//            //if Yes,draw rect and save pram.
//            for(int k = 0;k<4;k++)
//            {
//                Point p1 = Point(vertices[i].row(k));
//                int j = (k+1)%4;
//                Point p2 = Point(vertices[i].row(j));
//                line(dst,p1,p2,Scalar(255));
//            }
//            double h = rRect[i].size.height;
//            if(rRect[i].size.height<rRect[i].size.width)
//            {
//                h = rRect[i].size.width;
//            }
//            //cout<<h<<'\t'<< rRect[i].size.width<<'\t'<< rRect[i].size.height<<endl;
//            cout<<i<<'\t'<<rRect[i].center.y<<endl;
//            Vec4f con_pram(h,rRect[i].center.x,rRect[i].center.y,rRect[i].angle);
//            area.push_back(areas[i]);
//            con_prams.push_back(con_pram);
        }
    }

    vector<double> diameters;
    vector<Point2f> armour_center;
    for(int i=0;i<con_prams.size();i++)
    {
        for(int j = i+1;j<con_prams.size();j++)
        {
            double height1 = con_prams[i][0];
            double height2 = con_prams[j][0];

            double y1 = con_prams[i][2];
            double y2 = con_prams[j][2];

            double angle1 = con_prams[i][3];
            double angle2 = con_prams[j][3];

            double area_rate = area[i]/area[j];
            double height_d = abs(height2-height1);
            double angle_d = abs(angle2-angle1);
            double angle_a = abs(angle1+angle2);
            double y_dist = abs(y2-y1);
            double y_dist_wucha = 20;
            double angle_wucha = 20;
            double max_rate = 2.8;
            double min_rate = 0.2;

            double height_d_wucha = 10;

            // Y and Y 's distance must less wucha.and area's rate must bettwen min and max
            if(y_dist<y_dist_wucha&&(angle_d<20||angle_d>50)&&height_d<height_d_wucha
                    &&area_rate>min_rate&&area_rate<max_rate)
            {


                //get circle diameter
                double d=sqrt(pow(con_prams[i][1]-con_prams[j][1],2)
                        +pow(con_prams[i][2]-con_prams[j][2],2));

                //cout<<d<<endl;
                diameters.push_back(d);
                Point center=Point((con_prams[i][1]+con_prams[j][1])*0.5,
                        (con_prams[i][2]+con_prams[j][2])*0.5);
                armour_center.push_back(center);

            }
        }
    }
    //cout<<diameter.size()<<endl;
    double d,d1,d2,diameter,d_d1,d_d2;
    Point2f center;
    if(flags == 1)
    {

        if (diameters.size()!=0)
        {
            last_d=diameter = diameters[0];
            last_center=center = armour_center[0];
            if(diameters.size() != 1)
            {
                for(int i = 0;i<diameters.size()-1;i++)
                {
                    if (diameters[i]>diameters[i+1])
                    {
                        d = diameters[i+1];
                        diameters[i+1] = diameters[i];
                        diameters[i] = d;
                        center = armour_center[i+1];
                        armour_center[i+1] = armour_center[i];
                        armour_center[i] = center;
                    }

                }

                d1 = diameters[0];
                d2 = diameters[1];
                d_d1 = abs(d1-last_d);
                d_d2 = abs(d2-last_d);
//                cout<<d_d1<<endl;
//                cout<<d_d2<<endl;
//                cout<<endl;
                double cen_d1 = (last_center.x-armour_center[0].x)+abs(last_center.y-armour_center[0].y);
                double cen_d2 = (last_center.x-armour_center[1].x)+abs(last_center.y-armour_center[1].y);
                if(cen_d2<cen_d1)
                {
                    last_center = armour_center[1];
                    last_d = diameters[1];
                }
                else
                {
                    last_center = armour_center[0];
                    last_d = diameters[0];
                }
            }
        }
        circle(dst,last_center,last_d*0.5,Scalar(255),1);
    }
    else
    {
        if(diameters.size()!=0)
        {
            diameter = diameters[0];
            center = armour_center[0];   //中心
            //cout<<"d: "<<d<<endl;

            for (int i = 1;i<diameters.size();i++)
            {
                d = diameters[i];
                if(diameter>d)
                {
                    diameter = d;
                    center = armour_center[i];
                }
            }
            flags = 1;


            last_center = center;
            last_d = diameter;

            //        cout<<last_center<<endl;
            //        cout<<last_d<<endl;
            circle(dst,center,diameter*0.5,Scalar(255),1);
        }
        else
        {
            flags = 0;
        }

    }
    imshow("i",img);
    return dst;
}


Mat find_armour::find_blue3(Mat img,Mat dst,Point& XY,int& ismiddle,int& isfind)
{

    get_colors gc;      //class get_colors

    vector<double> area;

    vector<Vec4f> con_prams;    //save contuor's pram,X Y and area

    vector<double> diameters;

    vector<Point2f> armour_center;

    static Point2f last_center;

    static Mat dstROI;

    static double last_d;

    static int flags =0;

    vector<vector<Point> > contours;

    dst = gc.HSV_blue1(img,dst.clone());

    if(flags == 0)
    {
        findContours(dst,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);

        int num = contours.size();   //contour's amount

        vector<RotatedRect> rRect(num);    //save rotatedrect

        vector<double> areas(num);    //save area

        vector<Mat> vertices(num);    //find point to line rect.

        for(int i = 0;i<num;i++)
        {
            rRect[i] = minAreaRect(contours[i]);
            areas[i] =  contourArea(contours[i]);
            boxPoints(rRect[i],vertices[i]);

            //长宽分明时。
            if(areas[i]>50&&((rRect[i].size.height>rRect[i].size.width&&rRect[i].angle>-20)
                     ||(rRect[i].size.height<rRect[i].size.width&&rRect[i].angle<-60)))
            {

                //if Yes,draw rect and save pram.
                for(int k = 0;k<4;k++)
                {
                    Point p1 = Point(vertices[i].row(k));
                    int n = (k+1)%4;
                    Point p2 = Point(vertices[i].row(n));
                    line(img,p1,p2,Scalar(0,255,0),2);
                }
                double h = rRect[i].size.height;
                if(rRect[i].size.height<rRect[i].size.width)
                {
                    h = rRect[i].size.width;
                }
                Vec4f con_pram(h,rRect[i].center.x,rRect[i].center.y,rRect[i].angle);
                area.push_back(areas[i]);
                con_prams.push_back(con_pram);
            }
        }
        //delect some bad.
        for(int i=0;i<con_prams.size();i++)
        {
            for(int j = i+1;j<con_prams.size();j++)
            {
                double height1 = con_prams[i][0];
                double height2 = con_prams[j][0];

                double y1 = con_prams[i][2];
                double y2 = con_prams[j][2];

    //            double angle1 = con_prams[i][3];
    //            double angle2 = con_prams[j][3];

                double area_rate = area[i]/area[j];
                double height_d = abs(height2-height1);
    //            double angle_d = abs(angle2-angle1);
    //            double angle_a = abs(angle1+angle2);
                double y_dist = abs(y2-y1);

                double height_d_wucha = 10;

                // Y and Y 's distance must less wucha.and area's rate must bettwen min and max
                if(y_dist<y_dist_wucha&&height_d<height_d_wucha
                        &&area_rate>min_rate&&area_rate<max_rate)
                {
                    //get circle diameter
                    double d=sqrt(pow(con_prams[i][1]-con_prams[j][1],2)
                            +pow(con_prams[i][2]-con_prams[j][2],2));

                    diameters.push_back(d);
                    Point center=Point((con_prams[i][1]+con_prams[j][1])*0.5,
                            (con_prams[i][2]+con_prams[j][2])*0.5);
                    armour_center.push_back(center);
                }
            }
        }

        if(armour_center.size()==0)
        {
            flags = 0;
            dstROI = roi(dst,Point(img.cols/2,img.rows/2),100,img.cols,img.rows);
            isfind = 0;
        }
        else if(armour_center.size()==1)
        {
            last_center = armour_center[0];
            last_d = diameters[0];
            dstROI = roi(dst,last_center,last_d,img.cols,img.rows);
            circle(img,last_center,last_d/2,Scalar(0,255,0));
            flags = 1;
            isfind = 1;
        }
        else
        {
            int n = 0;
            double d1 = diameters[0];
            for (int i = 1;i<armour_center.size();i++)
            {
                double d2 = diameters[i];
                if(d1>d2)
                {
                    d1 = d2;
                    n = i;
                }
            }

            last_center = armour_center[n];
            last_d = d1;
            dstROI = roi(dst,last_center,last_d,img.cols,img.rows);
            circle(img,last_center,last_d/2,Scalar(0,255,0));
            flags = 1;
            isfind = 1;
        }
    }

//
    else
    {
        //截取本阵图片，只对截图操作
        dstROI = roi(dst,last_center,last_d,img.cols,img.rows);

        findContours(dstROI,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
        int num = contours.size();   //contour's amount

        vector<RotatedRect> rRect(num);    //save rotatedrect

        vector<double> areas(num);    //save area

        vector<Mat> vertices(num);    //find point to line rect.

        for(int i = 0;i<num;i++)
        {

            //traverse all contours
            rRect[i] = minAreaRect(contours[i]);
            areas[i] =  contourArea(contours[i]);

            //find four top_points
            boxPoints(rRect[i],vertices[i]);
            //长宽分明时。
            if(((rRect[i].size.height>rRect[i].size.width&&rRect[i].angle>-20)
                     ||(rRect[i].size.height<rRect[i].size.width&&rRect[i].angle<-60)))
            {

                //if Yes,draw rect and save pram.
                for(int k = 0;k<4;k++)
                {
                    Point p1 = Point(Point(vertices[i].row(k)).x+x1,Point(vertices[i].row(k)).y+y1);
                    int n = (k+1)%4;
                    Point p2 = Point(Point(vertices[i].row(n)).x+x1,Point(vertices[i].row(n)).y+y1);
                    line(img,p1,p2,Scalar(0,255,0),2);
                }


                double h = rRect[i].size.height;

                if(rRect[i].size.height<rRect[i].size.width) h = rRect[i].size.width;

                Vec4f con_pram(h,rRect[i].center.x+x1,rRect[i].center.y+y1,rRect[i].angle);

                area.push_back(areas[i]);

                con_prams.push_back(con_pram);
            }

        }
        for(int i=0;i<con_prams.size();i++)
        {
            for(int j = i+1;j<con_prams.size();j++)
            {
                double height1 = con_prams[i][0];
                double height2 = con_prams[j][0];

                double y1 = con_prams[i][2];
                double y2 = con_prams[j][2];

                double height_d = abs(height2-height1);

                double y_dist = abs(y2-y1);

                // Y and Y 's distance must less wucha.and area's rate must bettwen min and max
                if(y_dist<y_dist_wucha_ROI&&height_d<height_d_wucha_ROI)
                {
                    //get circle diameter
                    double d=sqrt(pow(con_prams[i][1]-con_prams[j][1],2)
                            +pow(con_prams[i][2]-con_prams[j][2],2));

                    diameters.push_back(d);
                    Point center=Point((con_prams[i][1]+con_prams[j][1])*0.5,
                            (con_prams[i][2]+con_prams[j][2])*0.5);
                    armour_center.push_back(center);
                }
            }
        }


        //识别到的装甲板个数
        if(armour_center.size()==0)
        {
            flags = 0;
            isfind = 0;
        }
        else if(armour_center.size()==1)
        {
            last_center = armour_center[0];
            last_d = diameters[0];
            circle(img,last_center,last_d/2,Scalar(0,255,0));
            flags = 1;
            isfind = 1;
        }
        else
        {

            int n = 0;
            double d1 = diameters[0];
            for (int i = 1;i<armour_center.size();i++)
            {
                double d2 = diameters[i];
                if(abs(d1-last_d)>abs(d2-last_d))
                {
                    d1 = d2;
                    n = i;
                }
            }
            last_center = armour_center[n];
            last_d = diameters[n];
            circle(img,last_center,last_d/2,Scalar(0,255,0));
            flags = 1;
            isfind = 1;
        }
    }
//    send_location send_l;
//    send_l.send_coordinate(last_center);
//    cout<<last_center<<endl;
    XY = last_center;
    imshow("ROI",dstROI);
    imshow("find",img);
    return dst;
}



Mat find_armour::find_blue4(Mat img,Mat dst,VisionData &data)
{

    /* 在Blue3的基础上，把寻找装甲板的方法写成一个函数search_armour().接着根据装甲板的数量进行不同的操作
     */

    get_colors gc;      //class SR

//    double camera_canshu[9] = {527.3444,0,337.5232,0,531.2206,254.4946,0,0,1};
    double camera_canshu[9] = {400,0,337,0,500,200,0,0,1};

    double dist_coeff[5] = {-0.4259,0.2928,-0.0106,-0.0031,0};
    Mat camera_matrix(3,3,CV_64FC1,camera_canshu);
    Mat dist_matrix(1,5,CV_64FC1,dist_coeff);
    AngleSolve ans(camera_matrix,dist_matrix,21.6,5.4,0,20,1000,1);
    double rot_c[] = {1,0,0,0,1,0,0,0,1};
    double tran_c[] = {0,0,0};
    Mat rot_martrix(3,3,CV_64FC1,rot_c);
    Mat tran_matrix(3,1,CV_64FC1,tran_c);
    ans.Relation_Camera_PTZ(rot_martrix,tran_matrix,0);

    vector<double> diameters;

    vector<Point2f> armour_center;

    vector<double> Heights;

    vector<double> Rotated_angles;

    vector<Point2f> XY;

    static Point2f last_center;

    static Mat dstROI;

    static double last_d;

    static double last_angle;

    static Size2f last_size;

    static int flags =0;

    dst = gc.HSV_blue1(img,dst.clone());

    if(flags == 0)
    {
        search_armour(img,dst,armour_center,diameters,flags,Rotated_angles,Heights);

        if(armour_center.size()==0)
        {
            flags = 0;
//            dstROI = roi(dst,Point(img.cols/2,img.rows/2),100,img.cols,img.rows);
            isfind = 0;
        }
        else if(armour_center.size()==1)
        {
            last_center = armour_center[0];
            last_d = diameters[0];
            if (last_angle<=0)
                last_size = Size2f(last_d,Heights[0]);
            else
                last_size = Size2f(Heights[0],last_d);
            circle(img,last_center,last_d/2,Scalar(0,255,0));
            flags = 1;
            isfind = 1;

        }
        else
        {
            int n = 0;
            double d1 = diameters[0];
            for (int i = 1;i<armour_center.size();i++)
            {
                double d2 = diameters[i];
                if(d1>d2)
                {
                    d1 = d2;
                    n = i;
                }
            }

            last_center = armour_center[n];
            last_d = d1;
            last_angle = Rotated_angles[n];
            if (last_angle<=0)
                last_size = Size2f(last_d,Heights[n]);
            else
                last_size = Size2f(Heights[n],last_d);
            circle(img,last_center,last_d/2,Scalar(0,255,0));
            flags = 1;
            isfind = 1;
        }
    }

//
    else
    {
        //截取本阵图片，只对截图操作
        dstROI = roi(dst,last_center,last_d,img.cols,img.rows);

        search_armour(img,dstROI,armour_center,diameters,flags,Rotated_angles,Heights);

        //识别到的装甲板个数
        while(armour_center.size()==0)
        {
//            cout<<"a"<<endl;
            last_d = last_d*1.3;
            dstROI = roi(dst,last_center,last_d,img.cols,img.rows);
            search_armour(img,dstROI,armour_center,diameters,flags,Rotated_angles,Heights);
            if(x1==1||x2==img.cols-1||y1==1||y2==img.rows-1) break;
        }
        if(armour_center.size()==0)
        {
            flags = 0;
            isfind = 0;
        }
        else if(armour_center.size()==1)
        {
            last_center = armour_center[0];
            last_d = diameters[0];
            if (last_angle<=0)
                last_size = Size2f(last_d,Heights[0]);
            else
                last_size = Size2f(Heights[0],last_d);
            circle(img,last_center,last_d/2,Scalar(0,255,0));
            flags = 1;
            isfind = 1;
        }
        else
        {
            int n = 0;
            double d1 = diameters[0];
            for (int i = 1;i<armour_center.size();i++)
            {
                double d2 = diameters[i];
                if(d1>d2)
                {
                    n = i;
                }
            }
            last_center = armour_center[n];
            last_d = diameters[n];
            if (last_angle<=0)
                last_size = Size2f(last_d,Heights[n]);
            else
                last_size = Size2f(Heights[n],last_d);
            circle(img,last_center,last_d/2,Scalar(0,255,0));
            flags = 1;
            isfind = 1;
        }
        imshow("ROI",dstROI);

    }
//    send_location send_l;
//    send_l.send_coordinate(last_center);
//    cout<<last_center<<endl;

//    Point2f xy1 = last_center+Point2f(-0.5*last_d,-0.2*last_d);
//    Point2f xy2 = last_center+Point2f(0.5*last_d,-0.2*last_d);

//    Point2f xy3 = last_center-Point2f(-0.5*last_d,0.2*last_d);
//    Point2f xy4 = last_center-Point2f(0.5*last_d,0.2*last_d);
//    XY.clear();
//    XY.push_back(xy1);
//    XY.push_back(xy2);
//    XY.push_back(xy3);
//    XY.push_back(xy4);
    RotatedRect RRect = RotatedRect(last_center,last_size,last_angle);
    double xAngle=0,yAngle=0;
    if (ans.Rotated_SolveAngle(RRect,xAngle,yAngle,20,0,Point2f(0,0)))
        cout<<xAngle<<","<<yAngle<<endl;
    data = {last_center.x,last_center.y,0,ismiddle,isfind};
    imshow("find",img);

    return dst;
}


//Mat find_armour::find_red4(Mat img,Mat dst,Point &XY,int& ismiddle,int& isfind)
//{

//    /* 在Blue3的基础上，把寻找装甲板的方法写成一个函数search_armour().接着根据装甲板的数量进行不同的操作
//     */

//    get_colors gc;      //class get_colors

//    vector<double> diameters;

//    vector<Point2f> armour_center;

//    static Point2f last_center;

//    static Mat dstROI;

//    static double last_d;

//    static int flags = 0;

//    dst = gc.HSV_red1(img,dst.clone());

//    if(flags == 0)
//    {
//        search_armour(img,dst,armour_center,diameters,flags);

//        if(armour_center.size()==0)
//        {
//            flags = 0;
//            dstROI = roi(dst,Point(img.cols/2,img.rows/2),100,img.cols,img.rows);
//            isfind = 0;
//        }
//        else if(armour_center.size()==1)
//        {
//            last_center = armour_center[0];
//            last_d = diameters[0];
//            dstROI = roi(dst,last_center,last_d,img.cols,img.rows);
//            circle(img,last_center,last_d/2,Scalar(0,255,0));
//            flags = 1;
//            isfind = 1;
//        }
//        else
//        {
//            int n = 0;
//            double d1 = diameters[0];
//            for (int i = 1;i<armour_center.size();i++)
//            {
//                double d2 = diameters[i];
//                if(d1>d2)
//                {
//                    d1 = d2;
//                    n = i;
//                }
//            }

//            last_center = armour_center[n];
//            last_d = d1;
//            dstROI = roi(dst,last_center,last_d,img.cols,img.rows);
//            circle(img,last_center,last_d/2,Scalar(0,255,0));
//            flags = 1;
//            isfind = 1;
//        }
//    }

////
//    else
//    {
//        //截取本阵图片，只对截图操作
//        dstROI = roi(dst,last_center,last_d,img.cols,img.rows);

//        search_armour(img,dstROI,armour_center,diameters,flags);

//        //识别到的装甲板个数
//        while(armour_center.size()==0)
//        {
//            cout<<"a"<<endl;
//            last_d = last_d*1.3;
//            dstROI = roi(dst,last_center,last_d,img.cols,img.rows);
//            search_armour(img,dstROI,armour_center,diameters,flags);
//            if(x1==1||x2==img.cols-1||y1==1||y2==img.rows-1) break;
//        }
//        if(armour_center.size()==0)
//        {
//            flags = 0;
//            isfind = 0;
//        }
//        else if(armour_center.size()==1)
//        {
//            last_center = armour_center[0];
//            last_d = diameters[0];
//            circle(img,last_center,last_d/2,Scalar(0,255,0));
//            flags = 1;
//            isfind = 1;
//        }
//        else
//        {
//            int n = 0;
//            double d1 = diameters[0];
//            for (int i = 1;i<armour_center.size();i++)
//            {
//                double d2 = diameters[i];
//                if(d1>d2)
//                {
//                    n = i;
//                }
//            }
//            last_center = armour_center[n];
//            last_d = diameters[n];
//            circle(img,last_center,last_d/2,Scalar(0,255,0));
//            flags = 1;
//            isfind = 1;
//        }
//    }
////    send_location send_l;
////    send_l.send_coordinate(last_center);
////    cout<<last_center<<endl;
//    XY = last_center;
//    imshow("ROI",dstROI);
//    imshow("find",img);
//    return dst;
//}


void find_armour::search_armour(Mat img,Mat dst,vector<Point2f> & armour_center,vector<double> & diameters,int flags,vector<double> &Rotated_angles,vector<double>&Heights)
{

    /* 功能：寻找装甲板的方法。分为截图和不截图两种情况
     */
    vector<double> area;

    vector<Vec4f> con_prams;    //save contuor's pram,X Y and area

    vector<vector<Point> > contours;

    if(flags == 0)
    {
        findContours(dst,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);

        int num = contours.size();   //contour's amount

        vector<RotatedRect> rRect(num);    //save rotatedrect

        vector<double> areas(num);    //save area

        vector<Mat> vertices(num);    //find point to line rect.

        for(int i = 0;i<num;i++)
        {
            rRect[i] = minAreaRect(contours[i]);

            areas[i] =  contourArea(contours[i]);
            boxPoints(rRect[i],vertices[i]);

            //长宽分明时。
            if(areas[i]>30&&((rRect[i].size.height>rRect[i].size.width&&rRect[i].angle>-20)
                     ||(rRect[i].size.height<rRect[i].size.width&&rRect[i].angle<-60)))
            {

                //if Yes,draw rect and save pram.
                for(int k = 0;k<4;k++)
                {
                    Point p1 = Point(vertices[i].row(k));
                    int n = (k+1)%4;
                    Point p2 = Point(vertices[i].row(n));
                    line(img,p1,p2,Scalar(0,255,0),2);
                }
                double h = rRect[i].size.height;
                if(rRect[i].size.height<rRect[i].size.width)
                {
                    h = rRect[i].size.width;
                }
                Vec4f con_pram(h,rRect[i].center.x,rRect[i].center.y,rRect[i].angle);
                area.push_back(areas[i]);
                con_prams.push_back(con_pram);

            }
        }
        //delect some bad.
        for(int i=0;i<con_prams.size();i++)
        {
            for(int j = i+1;j<con_prams.size();j++)
            {
                double height1 = con_prams[i][0];
                double height2 = con_prams[j][0];

                double x1 = con_prams[i][1];
                double x2 = con_prams[j][1];

                double y1 = con_prams[i][2];
                double y2 = con_prams[j][2];

                double area_rate = area[i]/area[j];
                double height_d = abs(height2-height1);

                double y_dist = abs(y2-y1);

                // Y and Y 's distance must less wucha.and area's rate must bettwen min and max
                if(y_dist<y_dist_wucha&&height_d<height_d_wucha
                        &&area_rate>min_rate&&area_rate<max_rate)
                {
                    //get circle diameter
                    double d=sqrt(pow(con_prams[i][1]-con_prams[j][1],2)
                            +pow(con_prams[i][2]-con_prams[j][2],2));

                    diameters.push_back(d);
                    Point center=Point((con_prams[i][1]+con_prams[j][1])*0.5,
                            (con_prams[i][2]+con_prams[j][2])*0.5);
                    armour_center.push_back(center);

                    if (x1<x2)
                    {
                        double Rotated_angle = atan((y2-y1)/(x2-x1));
                        Rotated_angles.push_back(Rotated_angle);
                        double h = con_prams[i][0]>con_prams[j][0]?con_prams[i][0]:con_prams[j][0];
                        Heights.push_back(h);
                    }
                    else
                    {
                        double Rotated_angle = atan((y2-y1)/(x1-x2));
                        Rotated_angles.push_back(Rotated_angle);
                        double h = con_prams[i][0]>con_prams[j][0]?con_prams[i][0]:con_prams[j][0];
                        Heights.push_back(h);
                    }
                }
            }
        }
    }
    else
    {
        findContours(dst,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
        int num = contours.size();   //contour's amount

        vector<RotatedRect> rRect(num);    //save rotatedrect

        vector<double> areas(num);    //save area

        vector<Mat> vertices(num);    //find point to line rect.

        for(int i = 0;i<num;i++)
        {

            //traverse all contours
            rRect[i] = minAreaRect(contours[i]);
            areas[i] =  contourArea(contours[i]);

            //find four top_points
            boxPoints(rRect[i],vertices[i]);
            //长宽分明时。
            if(((rRect[i].size.height>rRect[i].size.width&&rRect[i].angle>-20)
                     ||(rRect[i].size.height<rRect[i].size.width&&rRect[i].angle<-60)))
            {

                //if Yes,draw rect and save pram.
                for(int k = 0;k<4;k++)
                {
                    Point p1 = Point(Point(vertices[i].row(k)).x+x1,Point(vertices[i].row(k)).y+y1);
                    int n = (k+1)%4;
                    Point p2 = Point(Point(vertices[i].row(n)).x+x1,Point(vertices[i].row(n)).y+y1);
                    line(img,p1,p2,Scalar(0,255,0),2);
                }


                double h = rRect[i].size.height;

                if(rRect[i].size.height<rRect[i].size.width) h = rRect[i].size.width;

                Vec4f con_pram(h,rRect[i].center.x+x1,rRect[i].center.y+y1,rRect[i].angle);

                area.push_back(areas[i]);

                con_prams.push_back(con_pram);
            }

        }
        for(int i=0;i<con_prams.size();i++)
        {
            for(int j = i+1;j<con_prams.size();j++)
            {
                double height1 = con_prams[i][0];
                double height2 = con_prams[j][0];

                double x1 = con_prams[i][1];
                double x2 = con_prams[j][1];

                double y1 = con_prams[i][2];
                double y2 = con_prams[j][2];

                double height_d = abs(height2-height1);

                double y_dist = abs(y2-y1);

                // Y and Y 's distance must less wucha.and area's rate must bettwen min and max
                if(y_dist<y_dist_wucha_ROI&&height_d<height_d_wucha_ROI)
                {
                    //get circle diameter
                    double d=sqrt(pow(con_prams[i][1]-con_prams[j][1],2)
                            +pow(con_prams[i][2]-con_prams[j][2],2));

                    diameters.push_back(d);
                    Point center=Point((con_prams[i][1]+con_prams[j][1])*0.5,
                            (con_prams[i][2]+con_prams[j][2])*0.5);
                    armour_center.push_back(center);
                    if (x1<x2)
                    {
                        double Rotated_angle = atan((y2-y1)/(x2-x1));
                        Rotated_angles.push_back(Rotated_angle);
                        double h = con_prams[i][0]>con_prams[j][0]?con_prams[i][0]:con_prams[j][0];
                        Heights.push_back(h);
                    }
                    else
                    {
                        double Rotated_angle = atan((y2-y1)/(x1-x2));
                        Rotated_angles.push_back(Rotated_angle);
                        double h = con_prams[i][0]>con_prams[j][0]?con_prams[i][0]:con_prams[j][0];
                        Heights.push_back(h);
                    }
                }
            }
        }
    }
}

