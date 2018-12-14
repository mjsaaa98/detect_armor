#ifndef IMGFACTORY_H
#define IMGFACTORY_H
#include <find_armour.h>
#include <anglesolve.h>
#include <thread>
#include <mutex>

#define BUFF_SIZE 1


//#define OPEN_SERIAL
//#define VIDEO
#define CAMERA_DEBUG
//#define F640
class ImgFactory
{
    bool handle_flag;
    std::mutex Lock;

private:
    string filename;
    bool stop_pro;
    int mode;
    Mat frame;
    list<Mat> list_of_frame;
public:
    ImgFactory();
    void Img_read();
    void Img_handle();
};

#endif // IMGFACTORY_H
