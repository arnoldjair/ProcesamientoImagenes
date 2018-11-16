#include <iostream>
#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>

#include "FilterFactory.h"

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
    
    cv::namedWindow("Input Image", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Output Image", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Blurred Image", cv::WINDOW_AUTOSIZE);

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
    
    cv::Mat magI = FilterFactory::FourierTransform(input);

    cv::Mat blurred;

    cv::GaussianBlur(input, blurred, cv::Size(3, 3), 10, 0);

    cv::imshow("Input Image", input);
    cv::imshow("Output Image", magI);
    cv::imshow("Blurred Image", blurred);
    cv::waitKey(0);

    return 0;

}

