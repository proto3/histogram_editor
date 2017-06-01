#ifndef HISTOGRAM_H
#define HISTOGRAM_H
#include <vector>
#include <opencv2/core.hpp>

class Histogram
{
public:
    Histogram(cv::Mat img);
    Histogram(const Histogram &h);
    Histogram* clone();
    int min();
    int max();
    int right();
    int left();
    int width();
    void crop(int left, int right);
    void stretch(size_t width);
    cv::Mat draw(const cv::Size size);
    void info();

private:
    std::vector<int> hist;

    template <typename T> void histgen(cv::Mat img);
};

#endif
