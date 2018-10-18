#include "find_armour.h"
#include "get_colors.h"

find_armour::find_armour(FileStorage f)
{
    fs = f;
    fs["area_min"] >> area_min;
    fs["angle1"] >> a1;
    fs["angle2"] >> a2;
    fs["y_dist_wucha"] >> y_dist_wucha;
    fs["height_d_wucha"] >> height_d_wucha;
    fs["min_rate"] >> min_rate;
    fs["max_rate"] >> max_rate;
    cout<<min_rate<<endl;
}

//use method HSV_blue
Mat find_armour::find_blue(Mat img,Mat dst)
{
    get_colors gc;      //class get_colors

    //get_red and find contours
    vector<vector<Point> > contours;

    //
    dst = gc.HSV_blue(img,dst.clone());

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

        //长宽分明时。
        if(areas[i]>100&&((rRect[i].size.height>rRect[i].size.width&&rRect[i].angle>-20)
                          ||(rRect[i].size.height<rRect[i].size.width&&rRect[i].angle<-60)))
        {
            //if Yes,draw rect and save pram.
            for(int k = 0;k<4;k++)
            {
                Point p1 = Point(vertices[i].row(k));
                int j = (k+1)%4;
                Point p2 = Point(vertices[i].row(j));
                line(dst,p1,p2,Scalar(255));
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
            double y_dist_wucha = 20;
            double angle_wucha = 30;
            double max_rate = 1.5;
            double min_rate = 0.7;

            // Y and Y 's distance must less wucha.and area's rate must bettwen min and max
            if(y_dist<y_dist_wucha&&area_rate>min_rate&&area_rate<max_rate&&abs(angle1-angle2)<angle_wucha)
            {
                //get circle diameter(直径）
                double diameter=sqrt(pow(con_prams[i][1]-con_prams[j][1],2)
                        +pow(con_prams[i][2]-con_prams[j][2],2));


                if(diameter<200&&diameter>10)    //diameter can't be length.
                {
                //get armour's center.
                Point armour_center=Point((con_prams[i][1]+con_prams[j][1])*0.5,
                                     (con_prams[i][2]+con_prams[j][2])*0.5);
                // draw circle
                circle(dst,armour_center,diameter*0.5,Scalar(255),1);
                //cout<<"d: "<<diameter<<endl;
                }
            }

        }
    }
    return dst;
}



//--------------------------------------------------------------------------
/*
//use method HSV_blue
Mat find_armour::find_blue1(Mat img,Mat dst)
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
                int n = (k+1)%4;
                Point p2 = Point(vertices[i].row(n));
                line(img,p1,p2,Scalar(255,0,0),1);
            }
            double h = rRect[i].size.height;
            if(rRect[i].size.height<rRect[i].size.width)
            {
                h = rRect[i].size.width;
            }
            cout<<h<<'\t'<< rRect[i].size.width<<'\t'<< rRect[i].size.height<<endl;
            //cout<<i<<'\t'<<rRect[i].center<<endl;
            Vec4f con_pram(h,rRect[i].center.x,rRect[i].center.y,rRect[i].angle);
            area.push_back(areas[i]);
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
            double y_dist_wucha = 30;
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
                diameter.push_back(d);
                Point center=Point((con_prams[i][1]+con_prams[j][1])*0.5,
                        (con_prams[i][2]+con_prams[j][2])*0.5);
                armour_center.push_back(center);

            }
        }
    }
    //cout<<diameter.size()<<endl;
    if (diameter.size()!=0)
    {
        double d = diameter[0];
        Point center = armour_center[0];   //中心
        //cout<<"d: "<<d<<endl;

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
        circle(dst,center,d*0.5,Scalar(255),1);
        //cout<<"d: "<<d<<endl;
    }
    imshow("i",img);
    return dst;
}
*/

//---------------------------------------------------------------
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

/*
//在blue2的基础上添加胃角度比较信息
//--------------------------------------------------------------------
Mat find_armour::find_blue1(Mat img,Mat dst)
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

    Point2f last_center = Point(640,360);

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
        //cout<<"x:"<<rRect[i].center.x<<endl;
        //长宽分明时。
        if(areas[i]>50&&((rRect[i].size.height>rRect[i].size.width&&rRect[i].angle>-20)
                 ||(rRect[i].size.height<rRect[i].size.width&&rRect[i].angle<-60)))
        {
            //判断是顺时针还是逆时针
            //if(rRect[i].angle=-90) rRect[i].angle+=180;
            if(rRect[i].size.height>rRect[i].size.width&&rRect[i].angle!=-90) rRect[i].angle+=90;

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
            double angle_d = abs(angle2-angle1);
            double angle_a = angle1+angle2;
            double y_dist = abs(y2-y1);
            double y_dist_wucha = 30;
            double angle_wucha = 20;
            double max_rate = 2.8;
            double min_rate = 0.2;

            double height_d_wucha = 10;

            // Y and Y 's distance must less wucha.and area's rate must bettwen min and max
            if(y_dist<y_dist_wucha&&(angle_d<20||angle_d>50)&&height_d<height_d_wucha
                    &&area_rate>min_rate&&area_rate<max_rate)
            {
                if(angle_a>0)
                {
                    if(angle_d<15)
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
                else
                {

                   if(angle_d<15)
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
    }
    else
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
        cout<<last_center<<endl;
        cout<<last_d<<endl;
    }
    last_center = center;
    last_d = diameter;
    // draw circle
    circle(dst,center,diameter*0.5,Scalar(255),1);
    //cout<<"d: "<<d<<endl;

    imshow("i",img);
    return dst;
}
*/

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
    cout<<"int"<<endl;
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

    static Point2f last_center;

    static Mat dstROI;

    static double last_d;

    double isinside=0;

    static int flags = 0;
//    cout<<flags<<endl;

    static double x , y , x1,x2=1200,x3=0,y1,y2,y3;

//    imshow("ROI",dstROI);
    //traverse all contours
    for(int i = 0;i<num;i++)
    {

        rRect[i] = minAreaRect(contours[i]);
        areas[i] =  contourArea(contours[i]);


        //find four top_points
        boxPoints(rRect[i],vertices[i]);
        for(int n = 0;n<vertices.size();n++)
        {
            if(Point(vertices[i].row(n)).x>x3||(Point(vertices[i].row(n)).x<x2))
            {
                isinside = 1;
                break;
            }
            else isinside = 0;
        }

        //filter small rectangles,and list two conditions of rect.
        //cout<<"x:"<<rRect[i].center.x<<endl;
        //长宽分明时。
        if(isinside == 1)
        {
            if(areas[i]>50&&((rRect[i].size.height>rRect[i].size.width&&rRect[i].angle>-20)
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
                //cout<<i<<'\t'<<rRect[i].center<<endl;
                Vec4f con_pram(h,rRect[i].center.x,rRect[i].center.y,rRect[i].angle);
                area.push_back(areas[i]);
                con_prams.push_back(con_pram);
                cout<<con_prams.size()<<endl;

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
            double y_dist_wucha = 30;
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
        circle(dst,center,diameter*0.5,Scalar(255),1);
        x = last_center.x-last_d/2;
        y = last_center.y-last_d/2;
        x1 = last_center.x+last_d/2;
        y1 = last_center.y+last_d/2;
        x2 = x1+200;
        y2 = y1+100;
        x3 = x-200;
        y3 = y-100;
        if(x2>1280) x2 = 1279;
        if(y2>720) y2 = 719;
        if(x3<0) x3 = 1;
        if(y3<0) y3 = 1;
        cout<<x3<<"-"<<y3<<endl;
        dstROI = dst(Range(y3,y2),Range(x3,x2));

        cout<<"a"<<endl;
        imshow("ROI",dstROI);

    }
    else
    {
        if(diameters.size()!=0)
        {

            diameter = diameters[0];
            center = armour_center[0];   //中心
            cout<<center<<endl;
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
//            circle(dst,center,diameter*0.5,Scalar(255),1);
//            x = last_center.x-last_d/2;
//            y = last_center.y-last_d/2;
//            x1 = last_center.x+last_d/2;
//            y1 = last_center.y+last_d/2;
//            x2 = x1+200;
//            y2 = y1+100;
//            x3 = x-200;
//            y3 = y-100;
//            if(x2>720) x2 = 720;
//            if(y2>1280) y2 = 1280;
//            if(x3<0) x3 = 0;
//            if(y3<0) y3 = 0;

//            cout<<dst.size<<endl;
//            dstROI = dst(Rect(x,y,x1,y1));

//            imshow("ROI",dstROI);
        }
        else
        {
            flags = 0;
        }
    }
    cout<<center<<endl;
//    imshow("i",img);
    return dst;
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
    cout<<last_d<<endl;

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

