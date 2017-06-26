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
    // cv::Mat img_a = cv::imread("../../../datasets/inra/Drone/10/0223_730nm.tif", CV_LOAD_IMAGE_ANYDEPTH);
    // cv::Mat img_b = cv::imread("../../../datasets/inra/Drone/10/0223_675nm.tif", CV_LOAD_IMAGE_ANYDEPTH);

    cv::Mat img_a = cv::imread("../../../datasets/ima.tif", CV_LOAD_IMAGE_ANYDEPTH);
    cv::Mat img_b = cv::imread("../../../datasets/imb.tif", CV_LOAD_IMAGE_ANYDEPTH);
    // cv::Mat img_b = img_a.clone();
    // img_b = 65536 - img_b;

    // img_a = img_a(cv::Rect(20, 20, img_a.cols - 40, img_a.rows - 40));
    // img_b = img_b(cv::Rect(20, 20, img_b.cols - 40, img_b.rows - 40));
    img_a = img_a(cv::Rect(20, 20, 300, 300));
    img_b = img_b(cv::Rect(20, 20, 300, 300));
    // cv::Mat img = cv::imread("../../../datasets/lena/lena.jpg", CV_LOAD_IMAGE_GRAYSCALE);

    Histogram hist_a(img_a);
    Histogram hist_b(img_b);
    hist_a.stretchMax();
    hist_b.stretchMax();
    img_a = hist_a.image();
    img_b = hist_b.image();

    std::vector<uint16_t> map;
    hist_a.HistogramMap(hist_b, map);

    cv::Mat hista1 = hist_a.draw(cv::Size(500, 300));
    hist_a.applyMap(map);
    hist_a.stretchMax();
    cv::Mat img_map = hist_a.image();

    cv::imshow("img_a", img_a);
    cv::imshow("img_b", img_b);
    cv::imshow("img_map", img_map);


    // for(auto it = map.begin(); it < map.begin() + 1000; ++it)
    // {
    //     std::cout << *it << std::endl;
    // }
    // int left = h.left();
    // int right = h.right();
    // h.stretch(100, 250);

    // cv::Mat hist = h.draw(cv::Size((right-left)/2, 400));
    cv::Mat hista2 = hist_a.draw(cv::Size(500, 300));
    cv::Mat histb = hist_b.draw(cv::Size(500, 300));

    // hist_a.info();

    // cv::Mat back = hist_a.image();
    // cv::imshow("backgen", back);
    cv::imshow("hista1", hista1);
    cv::imshow("hista2", hista2);
    cv::imshow("histb", histb);
    cv::waitKey(0);

    return 0;
}
//----------------------------------------------------------------------------//
