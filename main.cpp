#include <iostream>
#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>

#include "FilterFactory.h"

using namespace cv;
int main(int argc, char* argv[]) {

    /*const char* filename = argc >=2 ? argv[1] : "2.jpg";

    Mat I = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
    if( I.empty())
        return -1;

    Mat complexI;
    FilterFactory::getDFT(I, complexI);

    cv::Mat planes[] = {Mat_<float>(complexI), Mat::zeros(complexI.size(), CV_32F)};
    split(complexI, planes);                  
    
    cv::Mat inverseTransform;
    FilterFactory::invertDFT(complexI, inverseTransform);
    normalize(inverseTransform, inverseTransform, 0, 255, CV_MINMAX, CV_8UC1);
    imshow("Reconstructed", inverseTransform);
    waitKey();

    return 0;*/
    
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


    auto filters = j["filters"];

    if(filters == NULL) {
        std::cout<<"No filters"<<std::endl;
        return -1;
    }

    if (!original.data || !input.data) {
        std::cout<<"No image data"<<std::endl;
        return -1;
    }

    cv::Mat output = input.clone();

    //TODO: Verify filter name.
    int curr = 1;
    for(auto filter: filters) {
        if(filter["name"] == "gaussian") {
            FilterFactory::GaussianBlur(output, filter["params"]);
        }
        if(filter["name"] == "fourier"){
            FilterFactory::FourierFilter(output, filter["params"]);
        }
        if(filter["name"] == "dftImage") {
            FilterFactory::saveDFTImage(output, filter["params"]);
        }
        if(filter["name"] == "denoising") {
            FilterFactory::denoising(output, filter["params"]);
        }
        if(filter["name"] == "erode") {
            FilterFactory::erode(output, filter["params"]);
        }
        if(filter["name"] == "dilate") {
            FilterFactory::dilate(output, filter["params"]);
        }
        if(filter["name"] == "invert") {
            //FilterFactory::invert(output, filter["params"]);
            cv::bitwise_not(output, output);
        }
    }

    if(j.value("output", "") != "") {
        cv::imwrite(j["base"].get<std::string>() + j["output"].get<std::string>(), output);
    }
    
    return 0;

}
