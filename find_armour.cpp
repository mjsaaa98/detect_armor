#include "find_armour.h"

/**
 * @brief find_armour::find_armour  构造函数 完成一些从yaml读取数据的操作
 * @param f  写入yaml文件的类
 */
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
    Clear();
    clear_data();

}

/**
 * @brief find_armour::Clear  清空所有数据
 */
void find_armour::Clear()
{
    diameters.clear();
    armour_center.clear();
    fir_armor.clear();
    result_armor.clear();
    Armorlists.clear();
    diameters.clear();
    armour_center.clear();
    Rotate_Point.clear();
    Rotate_Points.clear();
    contours_para.clear();
    CellMaxs.clear();
    isfind = 0;
    ismiddle =0;
}
/**
 * @brief find_armour::clear_data  切换指令时清空所有数据
 */
void find_armour::clear_data()
{
    last_angle = 0;
    last_d = 0;
    last_angle = 0;
    last_size = Size(0,0);
    isROIflag = 0;
    last_center = Point2f(0,0);
    x1 = 0;
    x2 = 0;
    y1 = 0;
    y2 = 0;
}

/**
 * @brief find::Get_Rotated_param  得到构造Rotated的参数   not used
 * @param x1
 * @param x2
 * @param y1
 * @param y2
 * @param Rotated_angle
 */
void find_armour::Get_Rotated_param(float x1,float x2,float y1,float y2,float &Rotated_angle)
{
    if(x1<x2)
    {
        if(y1 == y2)
        {
            Rotated_angle = 0;
        }
        else
        {
            Rotated_angle = atan((y2-y1)/(x2-x1))*180/3.14159;
        }
    }
    else
    {
        if(y1 == y2)
        {
            Rotated_angle = 0;
        }
        else
        {
            Rotated_angle = atan((y1-y2)/(x1-x2))*180/3.14159;;
        }
    }
}


/**
 * @brief find_armour::roi   截图
 * @param img
 * @param center
 * @param d
 * @param cols
 * @param rows
 * @return
 */
Mat find_armour::roi(Mat img,Point center,float d)
{
    Mat roi;
    int cols = img.cols;
    int rows = img.rows;
    x1 = center.x-d*1;
    x2 = center.x+d*1;
    y1 = center.y-d*0.8;
    y2 = center.y+d*0.8;
    if(x1<=0) x1 = 1;
    if(x2>cols) x2 = cols-1;
    if(y1<=0) y1 = 1;
    if(y2>=rows) y2 = rows-1;
    roi = img(Range(y1,y2),Range(x1,x2));
    return roi;
}

float find_armour::Point_Angle(const Point2f &p1,const Point2f &p2){
    return fabs(atan2(p2.y-p1.y,p2.x-p1.x)*180.0/CV_PI);
}

/**
 * @brief find_armour::image_preprocess  图像预处理
 * @param mode  识别模式
 * @param src  input  输入图像
 * @param dst  output 输出图像
 */
void find_armour::image_preprocess(int mode,Mat src,Mat &dst)
{
    Mat gray;
    Mat k = getStructuringElement(MORPH_RECT,Size(3,3));
    Mat k1 = getStructuringElement(MORPH_RECT,Size(7,7));
    cvtColor(src,gray,CV_BGR2GRAY);
    threshold(gray,gray,50,255,THRESH_BINARY);
    if(mode==2) dst = gc.HSV_blue1(src,dst.clone());
    else if(mode==1) dst = gc.HSV_red1(src,dst.clone());
    dst = dst&gray;
    dilate(dst,dst,k1);
    ////顶帽操作
    //    Mat k = getStructuringElement(MORPH_RECT,Size(3,3));
    //    morphologyEx(img,dst,MORPH_TOPHAT,k,Point(-1,-1),1);
    //    imshow("dst",dst);
    //    cvtColor(dst,dst,CV_BGR2GRAY);
    //    imshow("dst1",dst);
    //    threshold(dst,dst,50,255,THRESH_BINARY);
    //    dilate(dst,dst,k);
    //    imshow("dst2",dst);


    ////    蓝色通道减去红色通道
    //    Mat gray;
    //    vector <Mat> planne;
    //    Mat k = getStructuringElement(MORPH_RECT,Size(11,11));
    //    Mat k1 = getStructuringElement(MORPH_RECT,Size(7,7));

    //    split(img,planne);
    //    cvtColor(img,gray,CV_BGR2GRAY);
    //    threshold(gray,gray,50,255,THRESH_BINARY);
    //    dilate(gray,gray,k);

    //    subtract(planne[0],planne[2],dst);
    //    imshow("planne",dst);

    //    threshold(dst,dst,50,255,THRESH_BINARY);
    //    dilate(dst,dst,k);
    //    imshow("planne_a",dst);
    //    dst = dst&gray;
    //    dilate(dst,dst,k1);
}


/**
 * @brief find_armour::find_blue4
 * @param img  input  输入相机原始图像
 * @param dst  output 输出图像
 * @param RRect output 输出旋转矩形，用于解算出角度
 * @param mode  input 输入指令
 * @return Mat
 */
Mat find_armour::find_blue4(Mat img,Mat dst,RotatedRect&RRect,int mode)
{
    Clear();

    //判断是否切换命令了
    if(last_mode!=mode)
    {
        clear_data();
        last_mode = mode;
        dstROI = Mat::zeros(img.size(),img.type());
    }

    image_preprocess(mode,img,dst);  //图片预处理

    if(isROIflag == 0)
    {
        clear_data();
        search_armour(img,dst);

        if(armour_center.size()==0)
        {
            isROIflag = 0;
//            dstROI = roi(dst,Point(img.cols/2,img.rows/2),100,img.cols,img.rows);
            isfind = 0;
        }
        else if(armour_center.size()==1)
        {
            last_center = armour_center[0];
            last_d = diameters[0];
            circle(img,last_center,last_d/2,Scalar(0,255,0));
            Rotate_Point = Rotate_Points[0];
            isROIflag = 1;
            isfind = 1;
        }
        else
        {
            int n = 0;
            double d1 = diameters[0];
            vector<Point2f> temp_center = armour_center;
            sort(temp_center.begin(),temp_center.end(),SortArmorCenterY);  //降序
            sort(temp_center.begin(),temp_center.end(),SortArmorCenterX);
            for (int i = 1;i<armour_center.size();i++)
            {
                if(temp_center[0]==armour_center[i])
                {\
                    n = i;
                    break;
                }
            }
//            for (int i = 1;i<armour_center.size();i++)
//            {
//                double d2 = diameters[i];
//                if(d1>d2)
//                {
//                    d1 = d2;
//                    n = i;
//                }
//            }
            last_center = armour_center[n];
            last_d = diameters[n];
            Rotate_Point = Rotate_Points[n];
            circle(img,last_center,last_d/2,Scalar(0,255,0));
            isROIflag = 1;
            isfind = 1;
        }
    }
    else
    {
        //截取本帧图片，只对截图操作
        dstROI = roi(dst,last_center,last_d);
        search_armour(img,dstROI);
        //当前截图区域没找到，以半径扩展
        while(armour_center.size()==0)
        {
            Clear();
            last_d = last_d*1.3;
            dstROI = roi(dst,last_center,last_d);
            search_armour(img,dstROI);
            if(x1==1||x2==img.cols-1||y1==1||y2==img.rows-1) break;
        }
        //识别到的装甲板个数
        if(armour_center.size()==0)
        {
            isROIflag = 0;
            isfind = 0;
        }
        else if(armour_center.size()==1)
        {
            last_center = armour_center[0];
            last_d = diameters[0];
            Rotate_Point = Rotate_Points[0];
            circle(img,last_center,last_d/2,Scalar(0,255,0));
            isROIflag = 1;
            isfind = 1;
        }
        else
        {
            int n = 0;
            vector<Point2f> temp_center = armour_center;
            sort(temp_center.begin(),temp_center.end(),SortArmorCenterY);  //降序
            sort(temp_center.begin(),temp_center.end(),SortArmorCenterX);
            for (int i = 1;i<armour_center.size();i++)
            {
                if(temp_center[0]==armour_center[i])
                {\
                    n = i;
                    break;
                }
            }
            last_center = armour_center[n];
            last_d = diameters[n];
            Rotate_Point = Rotate_Points[n];
            circle(img,last_center,last_d/2,Scalar(0,255,0));
            isROIflag = 1;
            isfind = 1;
        }
        imshow("ROI",dstROI);
    }
    if(isfind==1)
    {
        RRect = minAreaRect(Rotate_Point);
#ifdef SHOWDEBUG
        Mat vertice;
        boxPoints(RRect,vertice);
        for(int k = 0;k<4;k++)
        {
            Point p1 = Point(vertice.row(k));
            int n = (k+1)%4;
            Point p2 = Point(vertice.row(n));
            line(img,p1,p2,Scalar(255,255,255),2);
        }
#endif
    }
    imshow("find",img);
    return dst;
}

/**
 * @brief find_armour::get_Light   剔出领近的灯柱
 */
void find_armour::get_Light()
{
    size_t size = fir_armor.size();
    vector<RotatedRect> Groups;
    int cellmaxsize;
    Groups.push_back(fir_armor[0]);
    cellmaxsize = fir_armor[0].size.height * fir_armor[0].size.width;
    if(cellmaxsize > 2500) cellmaxsize = 0;
    int maxsize;
    for(int i=1;i<size;i++){
        if(fir_armor[i].center.x - fir_armor[i-1].center.x <10){
            maxsize = fir_armor[i].size.height * fir_armor[i].size.width;
            if(maxsize > 2500) continue;
            if(maxsize > cellmaxsize) cellmaxsize = maxsize;
            Groups.push_back(fir_armor[i]);
        }else{
            Armorlists.push_back(Groups);\
            CellMaxs.push_back(cellmaxsize);
            cellmaxsize = 0;
            maxsize = 0;
            Groups.clear();
            //if(fir_armor[i].size.height * fir_armor[i].size.width > 2500) continue;
            Groups.push_back(fir_armor[i]);
            cellmaxsize = fir_armor[i].size.height * fir_armor[i].size.width;
        }
        //std::cout<<"max:"<<cellmaxsize<<std::endl;
        //sizescale = (float)fir_armor[i].size.height/(float)fir_armor[i].size.width;
        //std::cout<<"scale:"<<sizescale<<" width:"<<fir_armor[i].size.width<<std::endl;
    }
    Armorlists.push_back(Groups);\
    CellMaxs.push_back(cellmaxsize);
    size = Armorlists.size();
    for(int i=0;i<size;i++){
        int Gsize = Armorlists[i].size();
        int GroupMax = CellMaxs[i];
        if(GroupMax > 5){
            for(int j=0;j<Gsize;j++){
                maxsize = Armorlists[i][j].size.height * Armorlists[i][j].size.width;
                if(maxsize == GroupMax){
                    result_armor.push_back(Armorlists[i][j]);
                    RotatedRect r = Armorlists[i][j];
                    double real_h = r.size.height < r.size.width ? r.size.width : r.size.height;
                    Vec4f contour_para(0,0,0,0);
                    if(isROIflag==0){
                        contour_para[0] = real_h;
                        contour_para[1] = r.center.x;
                        contour_para[2] = r.center.y;
                        contour_para[3] = r.angle;
                    }else{
                        contour_para[0] = real_h;
                        contour_para[1] = r.center.x+x1;
                        contour_para[2] = r.center.y+y1;
                        contour_para[3] = r.angle;
                        cout<<"1:"<<real_h<<endl;
                        cout<<"para:"<<contour_para[1]<<endl;\
                        cout<<"========="<<endl;
                    }
                    contours_para.push_back(contour_para);
                }
            }
        }
    }

}

/**
 * @brief find_armour::src_get_armor  在原图（截图）中找装甲板
 */
void find_armour::src_get_armor()
{
    vector<Point2f> VecPoint;
    int has_armor_flag = 0;  //是否有装甲板标志位
    int size = result_armor.size();
//    ArmorOldCenters = ArmorCenters;
//    ArmorCenters.clear();
    if(size < 2){
        return;
    }
    Point2f center_point1,center_point2;
    Point2f _pt[4],pt[4];
    float height1,height2;
    float angle1,angle2;
    float area1,area2,area_rate;
    float x1,x2;
    float y1,y2;
    float y_dist,x_dist,min_h,height_d,K,x2h_rate,angle_diff,max_h;
    float angle_of_Rotated,height_of_Rotated;
    for(int i=0;i<size-1;i++)
    {
        height1 = contours_para[i][0];
        x1 = contours_para[i][1];
        y1 = contours_para[i][2];
        angle1 = fabs(contours_para[i][3]);

        center_point1 = result_armor[i].center;
        area1 = result_armor[i].size.height * result_armor[i].size.width;

        result_armor[i].points(_pt);
         /**
          * pt
          * 0 2
          * 1 3
        */
        if(angle1 > 50.0)
        {
            pt[0] = _pt[3];
            pt[1] = _pt[0];
        }
        else
        {
            pt[0] = _pt[2];
            pt[1] = _pt[3];
        }
        for(int j = i+1;j<size;j++)
        {
            height2 = contours_para[j][0];
            x2 = contours_para[j][1];
            y2 = contours_para[j][2];
            angle2 = fabs(contours_para[j][3]);

            center_point2 = result_armor[j].center;
            area2 = result_armor[j].size.height * result_armor[j].size.width;

            float angle_d = fabs(angle2-angle1);
            y_dist = fabs(y2-y1);
            if(y1>y2) y_dist = y1-y2;
            else y_dist = y2-y1;
            x_dist = x2-x1;
            min_h = min(height1,height2);
            max_h = max(height1,height2);
            height_d = fabs(height2-height1);
            if(center_point1.x != center_point2.x)
            {
                K = (center_point1.y - center_point2.y) / (center_point1.x - center_point2.x);

                //装甲板右边的点
                result_armor[j].points(_pt);
                if(angle2 > 50.0)
                {
                    pt[2] = _pt[2];
                    pt[3] = _pt[1];
                }else{
                    pt[2] = _pt[1];
                    pt[3] = _pt[0];
                }
                //面积比
                if(area1 > area2){
                    area_rate = area1 / area2;
                }else{
                    area_rate = area2 / area1;
                }
                angle_of_Rotated = MAX(Point_Angle(pt[0],pt[2]),Point_Angle(pt[1],pt[3]));//旋转矩形的角度

                height_of_Rotated = MAX(MAX(result_armor[i].size.width,result_armor[j].size.width),
                                        MAX(result_armor[i].size.height,result_armor[j].size.height));
                x2h_rate = x_dist/height_of_Rotated;
                //get circle diameter
                double d=sqrt(pow(contours_para[i][1]-contours_para[j][1],2)
                        +pow(contours_para[i][2]-contours_para[j][2],2));
                if(isROIflag==0)
                {
                    if(y_dist<0.4*(height1+height2)&&(angle_d<20||angle_d>50)
                           &&fabs(K)<0.5&&angle_of_Rotated<20&&area_rate<3.0&&x2h_rate>=0.8&&x2h_rate<=2.5&&height_d<0.5*max_h)
                    {
                        has_armor_flag = 1;
                    }
                }
                else
                {
                    if(y_dist<0.5*(height1+height2)
                           &&fabs(K)<0.5&&angle_of_Rotated<30&&area_rate<3.5&&x2h_rate>=0.8&&x2h_rate<=2.5&&height_d<0.5*max_h)
                    {
                        has_armor_flag = 1;
                    }
                }
                if(has_armor_flag == 1 )
                {
                    diameters.push_back(d);
                    Point center=Point2f((x1+x2)*0.5,(y1+y2)*0.5);
                    armour_center.push_back(center);
                    VecPoint.push_back(pt[0]+Point2f(find_armour::x1,find_armour::y1));
                    VecPoint.push_back(pt[1]+Point2f(find_armour::x1,find_armour::y1));
                    VecPoint.push_back(pt[2]+Point2f(find_armour::x1,find_armour::y1));
                    VecPoint.push_back(pt[3]+Point2f(find_armour::x1,find_armour::y1));
                    Rotate_Points.push_back(VecPoint);
                }
            }
        }
    }
}

/**
 * @brief find_armour::search_armour  寻找装甲板的方法。分为截图和不截图两种情况
 * @param img input img
 * @param dst input dst
 * @param armour_center  output save center of armor
 * @param diameters      output save diameter of armor
 * @param flags          input  is a flag to get ROI or not
 * @param Rotated_angles    output save RotatedRect's Angle
 * @param Heights        output save Height
 */
void find_armour::search_armour(Mat img,Mat dst)
{
    vector<vector<Point> > contours;

    findContours(dst,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);

    int num = contours.size();   //contour's amount

    if(isROIflag == 0)
    {
        for(int i = 0;i<num;i++)
        {
            RotatedRect r = minAreaRect(contours[i]);
            double real_h = r.size.height < r.size.width ? r.size.width : r.size.height;
            double real_w = r.size.height < r.size.width ? r.size.height : r.size.width;
            if(real_h<1.1*real_w) continue;
            //长宽分明时。筛去平躺的矩形
            if((r.size.height>r.size.width&&r.angle>-30)
                ||(r.size.height<r.size.width&&r.angle<-60))
            {
                Mat vertice;
                boxPoints(r,vertice);
                for(int k = 0;k<4;k++)
                {
                    Point p1 = Point(vertice.row(k));
                    int n = (k+1)%4;
                    Point p2 = Point(vertice.row(n));
                    line(img,p1,p2,Scalar(0,255,0),2);
                }
                fir_armor.push_back(r);
            }
        }
        sort(fir_armor.begin(),fir_armor.end(),Sort_RotatedRect);
        get_Light();
        sort(result_armor.begin(),result_armor.end(),Sort_RotatedRect);
        src_get_armor();
    }
    else
    {
        for(int i = 0;i<num;i++)
        {
            RotatedRect r = minAreaRect(contours[i]);
            double real_h = r.size.height < r.size.width ? r.size.width : r.size.height;
            double real_w = r.size.height < r.size.width ? r.size.height : r.size.width;
            if(real_h<1.1*real_w) continue;
            //长宽分明时。筛去平躺的矩形
            if((r.size.height>r.size.width&&r.angle>-30)
                ||(r.size.height<r.size.width&&r.angle<-60))
            {
                Mat vertice;
                boxPoints(r,vertice);
                for(int k = 0;k<4;k++)
                {
                    Point p1 = Point(Point(vertice.row(k)).x+x1,Point(vertice.row(k)).y+y1);
                    int n = (k+1)%4;
                    Point p2 = Point(Point(vertice.row(n)).x+x1,Point(vertice.row(n)).y+y1);
                    line(img,p1,p2,Scalar(0,255,0),2);
                }
                fir_armor.push_back(r);
            }
        }
        sort(fir_armor.begin(),fir_armor.end(),Sort_RotatedRect);
        get_Light();
        if(result_armor.size()>=2)
        sort(result_armor.begin(),result_armor.end(),Sort_RotatedRect);
        src_get_armor();
    }
}


