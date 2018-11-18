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


    auto filters = j["filters"];

    if(filters == NULL) {
        std::cout<<"No filters"<<std::endl;
        return -1;
    }

    std::cout<<filters<<std::endl;
    std::cout<<originalPath<<std::endl;
    std::cout<<inputPath<<std::endl;

    if (!original.data || !input.data) {
        std::cout<<"No image data"<<std::endl;
        return -1;
    }

    cv::Mat output = input.clone();

    //TODO: Verify filter name.
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
    }

    //TODO: Get the file extension
    cv::imwrite(j["base"].get<std::string>() + "result.jpg", output);
    return 0;

}
