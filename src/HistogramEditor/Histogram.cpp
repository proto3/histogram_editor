#include <HistogramEditor/Histogram.h>
#include <opencv2/imgproc.hpp>
#include <chrono>
#include <iostream>

using namespace std::chrono;

//----------------------------------------------------------------------------//
//TODO not uint8_t compatible yet
int Histogram::HistogramMap(Histogram &h, std::vector<uint16_t> &map)
{
    if(hist.size() != h.hist.size())
        throw std::logic_error("Error : unable to map histograms of different data size.");

    cv::Mat img = h.image();

    for(auto gray_it = hist.begin(); gray_it < hist.end(); ++gray_it)
    {
        // uint64_t mean = 0;
        // for(auto pix_it = gray_it->begin(); pix_it < gray_it->end(); ++pix_it)
        // {
        //     mean += img.at<uint16_t>(pix_it->x, pix_it->y);
        // }
        // if(gray_it->size() > 0)
        //     mean /= gray_it->size();
        //
        // map.push_back(mean);

        std::vector<int> v(65536);
        for(auto pix_it = gray_it->begin(); pix_it < gray_it->end(); ++pix_it)
        {
            v[img.at<uint16_t>(pix_it->x, pix_it->y)]++;
        }


        int max = 0;
        int med = 0;
        for(int i=0+5; i < v.size()-5; ++i)
        {
            int c = 0;
            for(int j=i-5;j<i+5;j++)
                c+=v[j];
            if(max < c)
            {
                max = c;
                med = i;
            }
        }
        map.push_back(med);

        // uint16_t i = 0;
        // int count = 0;
        // int median_pos = gray_it->size()/2;
        // while(count < median_pos)
        // {
        //     count += v[i];
        //     i++;
        // }
        // if(i!=0)
        //     map.push_back(i-1);
        // else
        //     map.push_back(0);
    }
}
//----------------------------------------------------------------------------//
//TODO not uint8_t compatible yet
int Histogram::applyMap(std::vector<uint16_t> &map)
{
    hist_data mapped_hist(65536);
    for(int i = 0; i < hist.size(); i++)
    // for(auto it = hist.begin(); it < hist.end(); ++it)
    {
        mapped_hist[map[i]].insert(mapped_hist[map[i]].end(), hist[i].begin(), hist[i].end());
    }

    hist = mapped_hist;
}
//----------------------------------------------------------------------------//
uint32_t ms_elapsed(steady_clock::time_point timestamp)
{
    steady_clock::time_point now = steady_clock::now();
    milliseconds elapsed_time = duration_cast<milliseconds>(now - timestamp);
    return elapsed_time.count();
}
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
    std::chrono::steady_clock::time_point start = steady_clock::now();

    for(int i = 0; i < img.rows; i++)
    {
        for(int j = 0; j < img.cols; j++)
        {
            T pixval = img.at<T>(i,j);
            hist[pixval].push_back(cv::Point(i,j));
        }
    }

    uint32_t elapsed_time = ms_elapsed(start);
    std::cout << "generate : " << elapsed_time << "ms" << std::endl;
}
//----------------------------------------------------------------------------//
bool size_comp(std::vector<cv::Point> a, std::vector<cv::Point> b)
{
    return a.size() < b.size();
}
//----------------------------------------------------------------------------//
//Return smallest value in the histogram
int Histogram::min()
{
    int min = 0;
    for(auto it = hist.begin(); it < hist.end(); ++it)
    {
        min = std::min((int)it->size(), min);
    }
    return min;
}
//----------------------------------------------------------------------------//
//Return largest value in the histogram
int Histogram::max()
{
    int max = 0;
    for(auto it = hist.begin(); it < hist.end(); ++it)
    {
        max = std::max((int)it->size(), max);
    }
    return max;
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
void Histogram::stretchMax()
{
    float ratio = (float)size() / range();

    hist_data stretched_hist(size());
    for(int i=0 ; i<range() ; i++)
    {
        std::vector<cv::Point> &v = stretched_hist[int(i*ratio)];
        v.insert(v.end(), hist[i+left()].begin(), hist[i+left()].end());
    }

    hist = stretched_hist;
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

    for(int i=0 ; i<size() ; i++)
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
    if(img_size.width != this->size())
    {
        stretched = this->clone();
        stretched->stretch(0, img_size.width);
    }
    else
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
