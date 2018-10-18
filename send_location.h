#ifndef SEND_LOCATION_H
#define SEND_LOCATION_H
#include <opencv2/imgproc.hpp>
#include "serialport.h"

class send_location
{
public:
    void send_coordinate(cv::Point);
};

#endif // SEND_LOCATION_H
