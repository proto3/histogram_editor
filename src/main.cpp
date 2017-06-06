#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

#include <HistogramEditor/Histogram.h>
#include <HistogramEditor/HistogramMap.h>
//----------------------------------------------------------------------------//
void process(cv::Mat &img)
{
    // img.convertTo(img, CV_32FC1);
    cv::normalize(img, img, 0, 255, cv::NORM_MINMAX);
    img.convertTo(img, CV_8UC1);

    // cv::threshold(img, img, 20, 0, cv::THRESH_TOZERO);

    cv::equalizeHist(img, img);
    cv::Mat tmp;
    cv::Laplacian(img, tmp, CV_16UC1, 7);

    cv::normalize(tmp, tmp, 0, 255, cv::NORM_MINMAX);
    tmp.convertTo(img, CV_8UC1);

    cv::threshold(img, img, 40, 0, cv::THRESH_TOZERO);
}
//----------------------------------------------------------------------------//
void histogram_generate(cv::Mat &img, int* hist)
{
    for(int i = 0; i < 256; i++)
        hist[i] = 0;

    for(int i = 0; i < img.rows; i++)
    {
        for(int j = 0; j < img.cols; j++)
        {
            uint8_t &pix = img.at<uint8_t>(i,j);
            hist[pix]++;
        }
    }
}
//----------------------------------------------------------------------------//
void histogram_draw(cv::Mat &img, int* hist)
{
    const int height = 500;

    float max_val = 0;
    for(int i = 0; i < 256; i++)
        if(hist[i] > max_val)
            max_val = hist[i];

    float divider = max_val / height;

    img = cv::Mat::zeros(height, 256, CV_8UC1);

    for(int i = 0; i < 256; i++)
        cv::line(img, cv::Point(i, height), cv::Point(i, height - (float)hist[i] / divider), 255);
}
//----------------------------------------------------------------------------//
int main(int argc, char* argv[])
{
    // cv::Mat img = cv::imread("../../../datasets/inra/Phenomobile/1/0017_530nm.tif", CV_LOAD_IMAGE_ANYDEPTH);
    // cv::Mat img = cv::imread("../../../datasets/inra/Phenomobile/1/0017_675nm.tif", CV_LOAD_IMAGE_ANYDEPTH);
    // cv::Mat img = cv::imread("../../../datasets/inra/Drone/10/0223_730nm.tif", CV_LOAD_IMAGE_ANYDEPTH);
    cv::Mat img = cv::imread("../../../datasets/inra/Drone/10/0223_675nm.tif", CV_LOAD_IMAGE_ANYDEPTH);
    // cv::Mat img = cv::imread("../../../datasets/lena/lena.jpg", CV_LOAD_IMAGE_GRAYSCALE);

    Histogram h(img);
    int left = h.left();
    int right = h.right();
    // h.stretch(100, 250);

    // cv::Mat hist = h.draw(cv::Size((right-left)/2, 400));
    cv::Mat hist = h.draw(cv::Size(1800, 600));
    h.info();

    cv::Mat back = h.image();
    cv::imshow("backgen", back);
    cv::imshow("hist", hist);
    cv::waitKey(0);

    return 0;
}
//----------------------------------------------------------------------------//
