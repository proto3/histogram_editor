#include <HistogramEditor/Histogram.h>
#include <opencv2/imgproc.hpp>
#include <iostream>

//----------------------------------------------------------------------------//
Histogram::Histogram(const cv::Mat img)
: src_size(img.size()),
  src_type(img.type())
{
    CV_Assert(img.channels() == 1);

    switch (img.type())
    {
        case CV_8U:
        case CV_8S:
            hist = hist_data(256);
            break;
        case CV_16U:
        case CV_16S:
            hist = hist_data(65536);
            break;
        default:
            throw std::logic_error("Error : unable to generate histogram for this cv::Mat type (size > 16bits or floating points)");
    }

    switch (img.type())
    {
        case CV_8U:
            generate<uint8_t>(img);
            break;
        case CV_8S:
            generate<int8_t>(img);
            break;
        case CV_16U:
            generate<uint16_t>(img);
            break;
        case CV_16S:
            generate<int16_t>(img);
            break;
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
cv::Mat Histogram::image()
{
    cv::Mat img = cv::Mat::zeros(src_size, src_type);
    for(auto gray_it = hist.begin(); gray_it != hist.end(); ++gray_it)
    {
        for(auto pix_it = gray_it->begin(); pix_it != gray_it->end(); ++pix_it)
        {
            switch (src_type)
            {
                case CV_8U:
                    img.at<uint8_t>(pix_it->x, pix_it->y) = gray_it - hist.begin();
                    break;
                case CV_8S:
                    img.at<int8_t>(pix_it->x, pix_it->y) = gray_it - hist.begin();
                    break;
                case CV_16U:
                    img.at<uint16_t>(pix_it->x, pix_it->y) = gray_it - hist.begin();
                    break;
                case CV_16S:
                    img.at<int16_t>(pix_it->x, pix_it->y) = gray_it - hist.begin();
                    break;
            }
        }
    }
    return img;
}
//----------------------------------------------------------------------------//
template <typename T> void Histogram::generate(const cv::Mat img)
{
    for(int i = 0; i < img.rows; i++)
    {
        for(int j = 0; j < img.cols; j++)
        {
            T pixval = img.at<T>(i,j);
            hist[pixval].push_back(cv::Point(i,j));
        }
    }
}
//----------------------------------------------------------------------------//
bool size_comp(std::vector<cv::Point> a, std::vector<cv::Point> b)
{
    return a.size() < b.size();
}
//----------------------------------------------------------------------------//
//Return largest value in the histogram
int Histogram::max()
{
    return std::max_element(std::begin(hist), std::end(hist), size_comp)->size();
}
//----------------------------------------------------------------------------//
//Return smallest value in the histogram
int Histogram::min()
{
    return std::min_element(std::begin(hist), std::end(hist), size_comp)->size();
}
//----------------------------------------------------------------------------//
//Return index of first non zero value of the histogram
//Return histogram size if all values are 0
int Histogram::left()
{
    int i = 0;
    while(i < size() && hist[i].size() == 0)
        i++;

    return i;
}
//----------------------------------------------------------------------------//
//Return index of last non zero value of the histogram
//Return -1 if all values are 0
int Histogram::right()
{
    int i = size() - 1;
    while(i >= 0 && hist[i].size() == 0)
        i--;

    return i;
}
//----------------------------------------------------------------------------//
//Return size of the histogram data array
int Histogram::size()
{
    return hist.size();
}
//----------------------------------------------------------------------------//
//Return length of the histogram from first to last non zero values
int Histogram::range()
{
    int r = right() - left() + 1;
    if(r < 0)
        return 0;
    else
        return r;
}
//----------------------------------------------------------------------------//
void Histogram::stretch(int left, int right)
{
    right++;
    if(right - left <= 0)
        throw std::logic_error("Error : cannot stretch histogram to null width.");

    hist_data stretched_hist(size());
    float a = (float)(right-left) / size();
    float b = left;

    for (int i=0;i<size();i++)
    {
        std::vector<cv::Point> &v = stretched_hist[int(i*a + b)];
        v.insert(v.end(), hist[i].begin(), hist[i].end());
    }

    hist = stretched_hist;
}
//----------------------------------------------------------------------------//
void Histogram::info()
{
    std::cout << "min   :" << min()   << std::endl;
    std::cout << "max   :" << max()   << std::endl;
    std::cout << "left  :" << left()  << std::endl;
    std::cout << "right :" << right() << std::endl;
    std::cout << "size  :" << size()  << std::endl;
    std::cout << "range :" << range() << std::endl;
}
//----------------------------------------------------------------------------//
void Histogram::crop(int left, int right)
{
    if(left >= right)
        throw std::logic_error("Error : cannot crop histogram, left >= right.");

    for(auto it = hist.begin(); it < hist.begin() + left; ++it)
    {
        it->clear();
    }

    for(auto it = hist.begin() + right + 1; it < hist.end(); ++it)
    {
        it->clear();
    }
}
//----------------------------------------------------------------------------//
cv::Mat Histogram::draw(const cv::Size img_size)
{
    cv::Mat img = cv::Mat::zeros(img_size, CV_8UC3);

    Histogram* stretched;
    // if(img_size.width != this->size())
    // {
    //     stretched = this->clone();
    //     stretched->stretch(0, img_size.width);
    // }
    // else
    {
        stretched = this;
    }

    int maxval = stretched->max();
    if(maxval == 0)
        return img;
    for(int i = 0; i < img.cols; i++)
    {
        cv::Point pt1(i, img_size.height);
        cv::Point pt2(i, img_size.height - stretched->hist[i].size() * img_size.height / maxval);
        cv::rectangle(img, pt1, pt2, cv::Scalar(255, 255, 255), CV_FILLED);
    }

    int right = stretched->right() + 1;
    int left  = stretched->left()  - 1;
    cv::line(img, cv::Point(left, 0), cv::Point(left, img_size.height), cv::Scalar(0, 0, 255));
    cv::line(img, cv::Point(right, 0), cv::Point(right, img_size.height), cv::Scalar(0, 0, 255));

    return  img;
}
//----------------------------------------------------------------------------//
