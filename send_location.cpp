#include "send_location.h"

void send_location::send_coordinate(cv::Point center)
{
    SerialPort sp;
    sp.Open();
    char locationXY[9] = {'X','0','0','0','0','Y','0','0','0'};
    int x = int(center.x);
    int y = int(center.y);

    for(i=8;i>0;i--)
    {
        if(i>5)
        {
            locationXY[i] = y%10+'0';
            y = y-y%10/10;
        }
        if(i<5)
        {
            locationXY[i] = x%10+'0';
            x = x-x%10/10;
        }
    }
    cout<<locationXY;
    port.Send(coordinate);
}

