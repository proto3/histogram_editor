#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

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

int main(int argc, char* argv[])
{
    // cv::Mat img_a = cv::imread("../../../datasets/inra/Phenomobile/1/0017_450nm.tif", CV_LOAD_IMAGE_ANYDEPTH);
    // cv::Mat img_b = cv::imread("../../../datasets/inra/Phenomobile/1/0017_675nm.tif", CV_LOAD_IMAGE_ANYDEPTH);
    cv::Mat img_a = cv::imread("../../../datasets/inra/Drone/10/0223_730nm.tif", CV_LOAD_IMAGE_ANYDEPTH);
    cv::Mat img_b = cv::imread("../../../datasets/inra/Drone/10/0223_675nm.tif", CV_LOAD_IMAGE_ANYDEPTH);

    process(img_b);
    process(img_a);

    cv::imshow("image_a", img_a);
    cv::imshow("image_b", img_b);
    cv::waitKey(0);

    return 0;
}
