#include "get_colors.h"

Mat get_colors::HSV_blue1(Mat img,Mat dst)
{


    Mat img_blue;
    int iLowH, iHighH, iLowS, iHighS, iLowV, iHighV;

    fs["iLowH"]>>iLowH;
    fs["iHighH"]>>iHighH;
    fs["iLowS"]>>iLowS;
    fs["iHighS"]>>iHighS;
    fs["iLowV"]>>iLowV;
    fs["iHighV"]>>iHighV;


    cvtColor(img, img_blue, COLOR_BGR2HSV);
    vector<Mat> planes;
    split(img_blue,planes);
    planes[2]-=70;
    merge(planes,img_blue);
    inRange(img_blue, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), dst);

    //open and close operation.
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    Mat kernel1 = getStructuringElement(MORPH_RECT, Size(3, 3));
//    Mat kernel2 = getStructuringElement(MORPH_RECT, Size(11,11));

//    morphologyEx(dst, dst, MORPH_CLOSE, kernel);

//    morphologyEx(dst, dst, MORPH_OPEN, kernel1);


//    morphologyEx(dst, dst, CV_MOP_OPEN, kernel1);
    dilate(dst,dst,kernel1);
//    morphologyEx(dst, dst, CV_MOP_CLOSE, kernel1);


//        erode(dst,dst,kernel);
    return dst;
}


//*********************************************************
//second red method
Mat get_colors::HSV_red1(Mat img,Mat dst)
{
    int i, j;
    Mat img_red;
    cvtColor(img, img_red, COLOR_BGR2HSV);
    vector<Mat> planes;
    split(img_red,planes);
    //planes[2]+=20;
    merge(planes,img_red);
    for (i = 0; i < img_red.rows; i++)
    {
        for (j = 0; j < img_red.cols ; j++)
        {
            if ((120 <= img_red.at<Vec3b>(i, j)[0] or img_red.at<Vec3b>(i, j)[0] <= 70) and img_red.at<Vec3b>(i, j)[1] >= 0
                and img_red.at<Vec3b>(i, j)[2] >= 40)
            {
                dst.at<uchar>(i, j) = 255;
            }
            else
                dst.at<uchar>(i, j) = 0;
        }
    }

    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    Mat kernel1 = getStructuringElement(MORPH_RECT, Size(3, 3));
//    morphologyEx(dst, dst, CV_MOP_OPEN, kernel);
//    erode(dst,dst,kernel1);
    dilate(dst,dst,kernel);
//    morphologyEx(dst, dst, CV_MOP_CLOSE, kernel);
//    morphologyEx(dst, dst, CV_MOP_CLOSE, kernel);


    return dst;
}




/*
Mat get_colors::HSV_red1(Mat img,Mat dst)
{
    int i, j;
    Mat img_red;
    cvtColor(img, img_red, COLOR_BGR2HSV);
    for (i = 0; i < img_red.rows; i++)
    {
        for (j = 0; j < img_red.cols ; j++)
        {
            if ((150 <= img_red.at<Vec3b>(i, j)[0] or img_red.at<Vec3b>(i, j)[0] <= 10) and img_red.at<Vec3b>(i, j)[1] >= 90
                and img_red.at<Vec3b>(i, j)[2] >= 40)
            {
                dst.at<uchar>(i, j) = 255;
            }
            else
                dst.at<uchar>(i, j) = 0;
        }
    }

    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    Mat kernel1 = getStructuringElement(MORPH_RECT, Size(3, 3));
    morphologyEx(dst, dst, CV_MOP_OPEN, kernel);
//    erode(dst,dst,kernel1);
    dilate(dst,dst,kernel);
    morphologyEx(dst, dst, CV_MOP_CLOSE, kernel);


    return dst;
}

*/






//原始
Mat get_colors::HSV_blue(Mat img,Mat dst)
{
    Mat img_blue;
    int iLowH = 100;
    int iHighH = 140;

    int iLowS = 0;
    int iHighS = 255;

    int iLowV = 63;
    int iHighV = 255;
    cvtColor(img, img_blue, COLOR_BGR2HSV);
    inRange(img_blue, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), dst);

    //open and close operation.
    Mat kernel = getStructuringElement(MORPH_RECT, Size(7, 7));
    Mat kernel1 = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(dst, dst, CV_MOP_OPEN, kernel);
    morphologyEx(dst, dst, CV_MOP_CLOSE, kernel);

//    erode(dst,dst,kernel);
//    dilate(dst,dst,kernel1);
    return dst;
}

Mat get_colors::red_hsv_inv(Mat img,Mat dst)
{
    //blur(img, img, Size(5, 5));
    Mat img_inv=~img;
    imshow("fan",img_inv);
    Mat imgHsv;
    cvtColor(img_inv, imgHsv, COLOR_BGR2HSV);


    inRange(imgHsv, Scalar(80 , 40 - 8, 100 - 18), Scalar(95, 255, 255), dst);
//    inRange(imgHsv, Scalar(80 , 40 - 8, 100 - 18), Scalar(120-20, 255, 255), mask);
    Mat element2 = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(dst, dst, MORPH_OPEN, element2);
    morphologyEx(dst, dst, MORPH_CLOSE, element2);
//    Mat element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
//    dilate(dst,dst,element1);

    return dst;
}


Mat get_colors::HSV_red(Mat img,Mat dst)
{
    int i, j;
    Mat img_red;
    cvtColor(img, img_red, COLOR_BGR2HSV);
    for (i = 0; i < img_red.rows; i++)
    {
        for (j = 0; j < img_red.cols / 2; j++)
        {
            if ((156 <= img_red.at<Vec3b>(i, j)[0] or img_red.at<Vec3b>(i, j)[0] <= 10) and img_red.at<Vec3b>(i, j)[1] >= 90
                and img_red.at<Vec3b>(i, j)[2] >= 90)
            {
                dst.at<uchar>(i, j) = 255;
            }
            else
                dst.at<uchar>(i, j) = 0;
        }

        for (j = img_red.cols/2; j < img_red.cols; j++)
        {
            if ((156 <= img_red.at<Vec3b>(i, j)[0] or img_red.at<Vec3b>(i, j)[0] <= 6) && img_red.at<Vec3b>(i, j)[1] >= 60
                && img_red.at<Vec3b>(i, j)[2] >= 40)
            {
                dst.at<uchar>(i, j) = 255;
            }
            else
                dst.at<uchar>(i, j) = 0;
        }
    }

    Mat kernel = getStructuringElement(MORPH_RECT, Size(7, 7));
    morphologyEx(dst, dst, CV_MOP_OPEN, kernel);
    morphologyEx(dst, dst, CV_MOP_CLOSE, kernel);


    return dst;
}
