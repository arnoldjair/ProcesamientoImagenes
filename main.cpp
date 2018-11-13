#include <iostream>
#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>

int main(int argc, char* argv[]) {

    if(argc == 1) {
        std::cout<<"usage: ./ProcesamientoImagenes <archivo.json>"<<std::endl;
        return -1;
    }

    std::ifstream i(argv[1]);
    nlohmann::json j;  // @suppress("Type cannot be resolved")
    i >> j;

    //TODO: Json well formed.
    std::string originalPath = j["base"].get<std::string>() + j["original"].get<std::string>();
    std::string inputPath = j["base"].get<std::string>() + j["input"].get<std::string>();
    cv::Mat original = cv::imread(originalPath, cv::IMREAD_GRAYSCALE);
    cv::Mat input = cv::imread(inputPath, cv::IMREAD_GRAYSCALE);
    std::cout<<originalPath<<std::endl;
    std::cout<<inputPath<<std::endl;

    if (!original.data || !input.data) {
        std::cout<<"No image data"<<std::endl;
        return -1;
    }

    cv::Mat padded;
    int m = cv::getOptimalDFTSize(input.rows);
    int n = cv::getOptimalDFTSize(input.cols);
    cv::copyMakeBorder(input, padded, 0, m - input.rows, 0, n - input.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
    cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F)};
    cv::Mat complexI;
    cv::merge(planes, 2, complexI);
    cv::dft(complexI, complexI);
    cv:: split(complexI, planes);  
    cv::magnitude(planes[0], planes[1], planes[0]);
    cv::Mat magI = planes[0];
    magI += cv::Scalar::all(1);  
    cv::log(magI, magI);
    magI = magI(cv::Rect(0, 0, magI.cols & -2, magI.rows & -2));

    int cx = magI.cols/2;
    int cy = magI.rows/2;
    cv::Mat q0(magI, cv::Rect(0, 0, cx, cy));
    cv::Mat q1(magI, cv::Rect(cx, 0, cx, cy));
    cv::Mat q2(magI, cv::Rect(0, cy, cx, cy));
    cv::Mat q3(magI, cv::Rect(cx, cy, cx, cy));

    cv::Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);

    cv::normalize(magI, magI, 0, 1, CV_MINMAX);

    cv::namedWindow("Input Image", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Output Image", cv::WINDOW_AUTOSIZE);

    //Channels (BGR)
    /*
       std::vector<cv::Mat> channels(3);
       cv::split(original, channels);

       for(auto& channel: channels) {
       cv::imshow("Display Image", channel);
       cv::waitKey(0);
       }
       */

    //cv::Mat output;
    //cv::boxFilter(original, output, -1, cv::Size(16, 16));

    cv::imshow("Input Image", input);
    cv::imshow("Output Image", magI);
    cv::waitKey(0);

    return 0;

}

