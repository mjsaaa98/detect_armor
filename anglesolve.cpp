#include "anglesolve.h"

/**
 * @brief AngleSolve::AngleSolve
 * @param cammatrix    内参
 * @param ditcoeff     畸变向量
 * @param width
 * @param height
 * @param offset_y_barrel_ptz
 * @param min_distance
 * @param max_distance
 * @param scale_
 */
AngleSolve::AngleSolve(const Mat &cam_matrix,const Mat &dis_coeff,double width,double height,double offset_y,double min,double max,double scale_)
{
    cammatrix = cam_matrix.clone();
    discoeff = dis_coeff.clone();
    width_target = width;
    height_target = height;
//    offset_y_barrel_ptz = offset_y_barrel_ptz;
    min_distance = min;
    max_distance = max;
    armor_width = 21.6;
    armor_height = 5.4;
    small_armor_width = 12.4;
    small_armor_height = 5.4;
    rune_width = 28;
    rune_height = 16;
//    scale_z = scale_;
//    r_camera2ptz =
}

inline bool cmp(const Point2f & p1, const Point2f & p2)
{
    return p1.x<p2.x;
}

/**
 * @brief AngleSolve::GetPosition2d
 * @param rect      input     armor
 * @param position2d   input   vector of vertices
 * @param offset      intput    偏移
 */
void AngleSolve::GetPosition2d(RotatedRect rect,vector<Point2f> & position2d,Point2f offset)
{
    Point2f vertices[4];
    rect.points(vertices);
    Point2f lu, ld, ru, rd;
    //sort 按X排序
    sort(vertices, vertices + 4, cmp);
    //按Y 排序
    if (vertices[0].y < vertices[1].y){
        lu = vertices[0];
        ld = vertices[1];
    }
    else{
        lu = vertices[1];
        ld = vertices[0];
    }
    if (vertices[2].y < vertices[3].y)	{
        ru = vertices[2];
        rd = vertices[3];
    }
    else {
        ru = vertices[3];
        rd = vertices[2];
    }

    //坐标入栈
    position2d.clear();
    position2d.push_back(lu + offset);
    position2d.push_back(ru + offset);
    position2d.push_back(rd + offset);
    position2d.push_back(ld + offset);
}



void AngleSolve::Rect_GetPosition2d(Rect rect,vector<Point2f> & position2d,Point2f offset)
{
//    Point2f vertices[4];
    Point2f lu, ld, ru, rd;
    lu = rect.tl();
    ld = Point2f(rect.tl())+Point2f(0,rect.height);
    ru = Point2f(rect.tl())+Point2f(rect.width,0);
    rd = rect.br();
    position2d.clear();
    position2d.push_back(lu + offset);
    position2d.push_back(ru + offset);
    position2d.push_back(rd + offset);
    position2d.push_back(ld + offset);
}


//获取世界坐标，并求解pnp
void AngleSolve::SolvepnpPro(const vector<Point2f> &position2d,Mat &rot,Mat &tran)
{
    std::vector<cv::Point3f> position3d;
    double half_x = width_target / 2.0;
    double half_y = height_target / 2.0;

    //世界坐标
    position3d.push_back(Point3f(-half_x, -half_y, 0));
    position3d.push_back(Point3f(half_x, -half_y, 0));
    position3d.push_back(Point3f(half_x, half_y, 0));
    position3d.push_back(Point3f(-half_x, half_y, 0));

    Mat r;

    solvePnP(position3d, position2d, cammatrix, discoeff, r, tran);
    Rodrigues(r, rot);    //将旋转向量转换为旋转矩阵
}

//相机转换到云台的旋转和平移矩阵
void AngleSolve::Relation_Camera_PTZ(const cv::Mat & rot_camera_ptz, const cv::Mat & trans_camera_ptz, double y_offset_barrel_ptz)
{
    rot_camera_ptz.copyTo(rot_camera2ptz);
    trans_camera_ptz.copyTo(trans_camera2ptz);
    offset_y_barrel_ptz = y_offset_barrel_ptz;
}

void AngleSolve::Camera2PTZ(const cv::Mat & pos, cv::Mat & transed_pos)
{
    transed_pos = rot_camera2ptz * pos - trans_camera2ptz;
}


void AngleSolve::PTZ2Barrel(const cv::Mat & pos_in_ptz,double & angle_P,double & angle_Y,double speed_bullet)
{
    double* _xyz = (double*)pos_in_ptz.data;
    double down_time;
    //子弹飞行时间
    if (speed_bullet> 0.1)
        down_time = _xyz[1]/100/speed_bullet;
    double offset_gravity = 0.5 * 9.8 * down_time * down_time * 100;  //  0.5*g*t^2
//    double xyz[3] = {_xyz[0], _xyz[1] - offset_gravity, _xyz[2]};
    double xyz[3] = {_xyz[0], _xyz[1], _xyz[2]};

    double alpha = 0.0, theta = 0.0;
    //数学关系
    alpha = asin(offset_y_barrel_ptz/sqrt(xyz[1]*xyz[1] + xyz[2]*xyz[2]));
    if(xyz[1] < 0){
        theta = atan(-xyz[1]/xyz[2]);
        angle_P = -(alpha+theta);
    }
    else{
        theta = atan(xyz[1]/xyz[2]);
        angle_P = (theta-alpha);
    }
    angle_Y = atan2(xyz[0], xyz[2]);
    //cout << "angle_P: " << angle_P << "\tangle_Y: " << angle_Y <<  "\talpha: " << alpha << "\ttheta: " << theta << endl;
    ///结算最终的角度
    angle_P = angle_P * 180 / 3.1415926;
    angle_Y = angle_Y * 180 / 3.1415926;

}

//bool AngleSolve::SolveAngle(const cv::RotatedRect & rect, double & angle_P, double & angle_Y, double speed_bullet, double current_ptz_angle, const cv::Point2f & offset)
//{
//    vector<Point2f> target2d;
//    GetPosition2d(rect,target2d,offset);

//    Mat r;
//    SolvepnpPro(target2d,r,position_in_camera);

//    position_in_camera.at<double>(2, 0) = scale_z * position_in_camera.at<double>(2, 0);
//    if (position_in_camera.at<double>(2, 0) < min_distance || position_in_camera.at<double>(2, 0) > max_distance){
//        cout << "out of range: [" << min_distance << ", " << max_distance << "]\n";
//        return false;
//    }

//    Camera2PTZ(position_in_camera,position_in_ptz);

//    PTZ2Barrel(position_in_ptz,angle_P,angle_Y,speed_bullet);
//    return true;
//}

//bool AngleSolve::SolveAngle(const vector<Point2f> target2d, double & angle_P, double & angle_Y, double speed_bullet, double current_ptz_angle, const cv::Point2f & offset)
//{
////    vector<Point2f> target2d;
////    Rect_GetPosition2d(rect,target2d,offset);

//    Mat r;
//    SolvepnpPro(target2d,r,position_in_camera);

////    position_in_camera.at<double>(2, 0) = scale_z * position_in_camera.at<double>(2, 0);
//    if (position_in_camera.at<double>(2, 0) < min_distance || position_in_camera.at<double>(2, 0) > max_distance){
//        cout << "out of range: [" << min_distance << ", " << max_distance << "]\n";
//        return false;
//    }

//    Camera2PTZ(position_in_camera,position_in_ptz);

//    PTZ2Barrel(position_in_ptz,angle_P,angle_Y,speed_bullet);
//    return true;
//}


bool AngleSolve::Rotated_SolveAngle(const RotatedRect rect, double & angle_P, double & angle_Y, double speed_bullet, double current_ptz_angle, const cv::Point2f & offset)
{
    vector<Point2f> target2d;
//    Rect_GetPosition2d(rect,target2d,offset);
    GetPosition2d(rect,target2d,offset);

    Mat r;
    SolvepnpPro(target2d,r,position_in_camera);

//    position_in_camera.at<double>(2, 0) = scale_z * position_in_camera.at<double>(2, 0);
    if (position_in_camera.at<double>(2, 0) < min_distance || position_in_camera.at<double>(2, 0) > max_distance){
        cout << "out of range: [" << min_distance << ", " << max_distance << "]\n";
        return false;
    }

    Camera2PTZ(position_in_camera,position_in_ptz);

    PTZ2Barrel(position_in_ptz,angle_P,angle_Y,speed_bullet);
    return true;
}


