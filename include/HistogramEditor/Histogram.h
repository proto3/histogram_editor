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
    cv::Mat image();
    int min();
    int max();
    int right();
    int left();
    int size();
    int range();
    void crop(int left, int right);
    void stretch(int left, int right);
    cv::Mat draw(const cv::Size size);
    void info();
    int HistogramMap(Histogram &h);

private:
    typedef std::vector<std::vector<cv::Point>> hist_data;
    hist_data hist;

    cv::Size src_size;
    uint8_t  src_type;

    template <typename T> void generate(cv::Mat img);
};

#endif
