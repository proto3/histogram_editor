#include <HistogramEditor/Histogram.h>
#include <opencv2/imgproc.hpp>
#include <iostream>

//----------------------------------------------------------------------------//
Histogram::Histogram(const cv::Mat img)
{
    CV_Assert(img.channels() == 1);

    switch (img.type())
    {
        case CV_8U:
        case CV_8S:
            hist = std::vector<int>(256, 0);
            break;
        case CV_16U:
        case CV_16S:
        // case CV_32S:
        // case CV_32F:
        // case CV_64F:
            hist = std::vector<int>(65536, 0);
            break;
        default:
            throw std::logic_error("Error : unable to generate histogram for this cv::Mat type (size > 16bits or floating points)");
    }

    switch (img.type())
    {
        case CV_8U:
            histgen<uint8_t>(img);
            break;
        case CV_8S:
            histgen<int8_t>(img);
            break;
        case CV_16U:
            histgen<uint16_t>(img);
            break;
        case CV_16S:
            histgen<int16_t>(img);
            break;
        // case CV_32S:
        //     histgen<int32_t>(img);
        //     break;
        // case CV_32F:
        //     histgen<float>(img);
        //     break;
        // case CV_64F:
        //     histgen<double>(img);
            // break;
    }
}
//----------------------------------------------------------------------------//
Histogram::Histogram(const Histogram &h)
{
    hist = h.hist;
}
//----------------------------------------------------------------------------//
Histogram* Histogram::clone()
{
    return new Histogram(*this);
}
//----------------------------------------------------------------------------//
template <typename T> void Histogram::histgen(const cv::Mat img)
{
    for(int i = 0; i < img.rows; i++)
    {
        for(int j = 0; j < img.cols; j++)
        {
            T pixval = img.at<T>(i,j);
            hist[pixval]++;
        }
    }
}
//----------------------------------------------------------------------------//
int Histogram::max()
{
    return *std::max_element(std::begin(hist), std::end(hist));
}
//----------------------------------------------------------------------------//
int Histogram::min()
{
    return *std::min_element(std::begin(hist), std::end(hist));
}
//----------------------------------------------------------------------------//
int Histogram::right()
{
    int i = hist.size() - 1;
    while(i >= 0 && hist[i] == 0)
        i--;

    return i;
}
//----------------------------------------------------------------------------//
int Histogram::left()
{
    int i = 0;
    while(i < hist.size() && hist[i] == 0)
        i++;

    return i;
}
//----------------------------------------------------------------------------//
int Histogram::width()
{
    return hist.size();
}
//----------------------------------------------------------------------------//
void Histogram::stretch(size_t width)
{
    if(width == 0)
        throw std::logic_error("Error : cannot strech histogram to 0");

    if(width == hist.size())
        return;

    std::vector<int> stretched_hist(width, 0);
    float ratio = (float)width / hist.size();

    for (int i=0;i<hist.size();i++)
    {
        stretched_hist[int(i*ratio)] += hist[i];
    }

    hist = stretched_hist;
}
//----------------------------------------------------------------------------//
void Histogram::info()
{
    std::cout << "min   :" << min() << std::endl;
    std::cout << "max   :" << max() << std::endl;
    std::cout << "left  :" << left() << std::endl;
    std::cout << "right :" << right() << std::endl;
    std::cout << "width :" << width() << std::endl;
}
//----------------------------------------------------------------------------//
void Histogram::crop(int left, int right)
{
    hist = std::vector<int>(hist.begin() + left, hist.begin() + right);
}
//----------------------------------------------------------------------------//
cv::Mat Histogram::draw(const cv::Size size)
{
    cv::Mat img = cv::Mat::zeros(size, CV_8UC3);

    Histogram* stretched;
    if(size.width != hist.size())
    {
        stretched = this->clone();
        stretched->stretch(size.width);
    }
    else
    {
        stretched = this;
    }

    int maxval = stretched->max();
    for(int i = 0; i < img.cols; i++)
    {
        cv::Point pt1(i, size.height);
        cv::Point pt2(i, size.height - stretched->hist[i] * size.height / maxval);
        cv::rectangle(img, pt1, pt2, cv::Scalar(255, 255, 255), CV_FILLED);
    }

    int right = stretched->right() + 1;
    int left = stretched->left() - 1;
    cv::line(img, cv::Point(left, 0), cv::Point(left, size.height), cv::Scalar(0, 0, 255));
    cv::line(img, cv::Point(right, 0), cv::Point(right, size.height), cv::Scalar(0, 0, 255));

    return  img;
}
//----------------------------------------------------------------------------//
