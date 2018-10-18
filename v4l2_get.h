#ifndef V4L2_GET_H
#define V4L2_GET_H


class v4l2_get
{
public:
    void contrast(int fd,int c);
    void gain(int fd);
    void exposure(int fd);
    void white_balance(int fd);
    void brightness(int fd);
    void saturation(int fd);
};

#endif // V4L2_GET_H
