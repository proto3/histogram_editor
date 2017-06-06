#include <HistogramEditor/HistogramMap.h>
#include <opencv2/imgproc.hpp>
#include <iostream>

//----------------------------------------------------------------------------//
HistogramMap::HistogramMap(Histogram hist_a, Histogram hist_b)
{
    if(hist_a.size() != hist_a.size())
        throw std::logic_error("Error : unable to map histograms of different data size.");


}
//----------------------------------------------------------------------------//
