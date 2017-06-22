#include <HistogramEditor/Histogram.h>

#include <boost/filesystem.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <regex>

using namespace std;
using namespace boost::filesystem;
//----------------------------------------------------------------------------//
int main(int argc, char* argv[])
{
    std::vector<path> va, vb;
    regex format_in("[0-9]+_[0-9]+nm\\.[a-zA-Z]+");
    regex format_out("[0-9]+_[0-9]+nm_reg\\.[a-zA-Z]+");
    for(directory_iterator d_it(argv[2]); d_it != directory_iterator(); ++d_it)
    {
        path cur_path = d_it->path();
        if(!regex_match(cur_path.filename().string(), format_in))
        {
            cerr << "\"" << cur_path.filename().string() << "\" file ignored." << endl;
        }
        else
        {
            va.push_back(cur_path);
        }
    }
    std::sort(va.begin(), va.end());

    for(directory_iterator d_it(argv[3]); d_it != directory_iterator(); ++d_it)
    {
        path cur_path = d_it->path();
        if(!regex_match(cur_path.filename().string(), format_out))
        {
            cerr << "\"" << cur_path.filename().string() << "\" file ignored." << endl;
        }
        else
        {
            vb.push_back(cur_path);
        }
    }
    std::sort(vb.begin(), vb.end());

    int i = 99*6;
    int j = 99*6;
    while(i < va.size() && j < vb.size())
    {
        string fa = va[i].filename().string();
        string fb = vb[j].filename().string();
        string pref_a = fa.substr(0, fa.find('.'));
        string pref_b = fb.substr(0, fb.find('.') - 4);

        if(pref_a != pref_b)
        {
            if(pref_a < pref_b)
            {
                std::cout << "missing " << pref_a << ".tif" << std::endl;
                i++;
            }
            else
            {
                std::cout << "missing " << pref_b << "_reg.tif" << std::endl;
                j++;
            }
            continue;
        }
        std::cout << va[i].filename().string() << " " << vb[j].filename().string() << std::endl;

        cv::Mat img_a = cv::imread(va[i].string(), cv::IMREAD_ANYDEPTH);
        cv::Mat img_b = cv::imread(vb[j].string(), cv::IMREAD_ANYDEPTH);
        img_a = img_a(cv::Rect(50, 50, img_a.cols - 100, img_a.rows - 100));
        img_b = img_b(cv::Rect(50, 50, img_b.cols - 100, img_b.rows - 100));

        int border = min(img_a.cols, img_a.rows);
        img_a = img_a(cv::Rect(0, 0, border, border));
        img_b = img_b(cv::Rect(0, 0, border, border));

        cv::Mat img_a_fp,img_b_fp;
        img_a.convertTo(img_a_fp, CV_32FC1);
        img_b.convertTo(img_b_fp, CV_32FC1);

        cv::Point2f shift = phaseCorrelate(img_a_fp, img_b_fp);
        float dist = sqrt(shift.x*shift.x + shift.y*shift.y);
        if(dist > 1.0)
        {
            std::cout << shift << std::endl;
            string ref_name = pref_a + ".tif";
            ref_name[ref_name.size() - 9] = '5';
            ref_name[ref_name.size() - 8] = '3';
            ref_name[ref_name.size() - 7] = '0';
            cout << ref_name<< endl;

            cv::Mat img_ref = cv::imread(string(argv[1]) + "/" + ref_name, cv::IMREAD_ANYDEPTH);
            img_ref = img_ref(cv::Rect(50, 50, img_ref.cols - 100, img_ref.rows - 100));
            img_ref = img_ref(cv::Rect(0, 0, border, border));

            uint16_t max_ref = *max_element(img_ref.begin<uint16_t>(), img_ref.end<uint16_t>());
            uint16_t max_a = *max_element(img_a.begin<uint16_t>(), img_a.end<uint16_t>());
            uint16_t max_b = *max_element(img_b.begin<uint16_t>(), img_b.end<uint16_t>());

            img_ref = img_ref * (65536.0 / max_ref);
            img_a = img_a * (65536.0 / max_a);
            img_b = img_b * (65536.0 / max_b);

            bool toggle;
            while(cv::waitKey(300) != 27)
            {
                if(toggle)
                {
                    imshow("a", img_ref);
                    imshow("b", img_ref);
                }
                else
                {
                    imshow("a", img_a);
                    imshow("b", img_b);
                }
                toggle = !toggle;
            }
        }

        i++; j++;
    }

    return 0;
}
//----------------------------------------------------------------------------//
