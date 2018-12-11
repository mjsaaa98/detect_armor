#ifndef IMGFACTORY_H
#define IMGFACTORY_H
#include <find_armour.h>
#include <anglesolve.h>
#include <thread>
#include <mutex>

#define BUFF_SIZE 1

#define VIDEO
#define CAMERA_DEBUG
//#define F640
class ImgFactory
{
    bool handle_flag;
    std::mutex Lock;
    long long read_num;
    long long process_num;
private:
    string filename;
    bool stop_pro;
    int mode;
    Mat frame;
    Mat src;
    list<Mat> list_of_frame;
public:
    ImgFactory();
    void Img_read();
    void Img_handle();
};

#endif // IMGFACTORY_H
