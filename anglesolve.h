#ifndef ANGLESOLVE_H
#define ANGLESOLVE_H
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
//#include <opencv2/calib3d.hpp>
#include <algorithm>


using namespace cv;
using namespace std;

class AngleSolve
{
private:
    cv::Mat cammatrix;  //相机内参
    cv::Mat discoeff;  //畸变参数
    double width_target;    //目标宽
    double height_target;   //目标高

    cv::Mat position_in_camera;  //相机坐标
    cv::Mat position_in_ptz;     //云台坐标

    cv::Mat trans_camera2ptz;    //相机到云台平移向量
    cv::Mat rot_camera2ptz;     //相机到云台旋转矩阵

    // offset between barrel and ptz on y axis (cm)
    double offset_y_barrel_ptz;   //枪管相对于云台在Y轴上的偏移

    // scope of detection distance (cm)
    double min_distance;      //最大距离
    double max_distance;      //最小距离
    double scale_z;           //Z轴的比例

public:
    double armor_width;       //大装甲板
    double armor_height;
    double small_armor_width;   //小装甲板
    double small_armor_height;
    double rune_width;          //神符
    double rune_height;
    AngleSolve(const Mat &cam_matrix,const Mat &dis_coeff,double width,double height,double offset_y,double min,double max,double scale_);
    void GetPosition2d(RotatedRect rect,vector<Point2f> & position2d,Point2f offset = Point2f(0,0));
    //    void SetCameraParam(Mat cammatrix,Mat discoeff);
    void Rect_GetPosition2d(Rect rect,vector<Point2f> & position2d,Point2f offset);

    void SolvepnpPro(const vector<Point2f> &position2d,Mat &rot,Mat &tran);
    void Relation_Camera_PTZ(const cv::Mat & rot_camera_ptz, const cv::Mat & trans_camera_ptz, double y_offset_barrel_ptz);
    void Camera2PTZ(const cv::Mat & pos, cv::Mat & transed_pos);
    void PTZ2Barrel(const cv::Mat & pos_in_ptz,double & angle_P,double & angle_Y,double speed_bullet);
//    bool SolveAngle(const vector<Point2f>, double & angle_P, double & angle_Y, double speed_bullet, double current_ptz_angle, const cv::Point2f & offset);
    bool Rotated_SolveAngle(const RotatedRect, double & angle_P, double & angle_Y, double &dis,  Mat& camera_location,double speed_bullet, double current_ptz_angle, const cv::Point2f & offset);

    //    void SetTargetSize();




};

#endif // ANGLESOLVE_H
