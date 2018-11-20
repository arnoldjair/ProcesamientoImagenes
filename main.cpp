#include <iostream>
#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>
#include <stdint.h>

#include "FilterFactory.h"

using namespace cv;


float euclideanDistance(cv::Mat img1, cv::Mat img2) {

    int rows1 = img1.rows;
    int cols1 = img1.cols;

    int rows2 = img2.rows;
    int cols2 = img2.cols;

    if(rows1 != rows2 || cols1 != cols2) {
        std::cout<<"Different sizes"<<std::endl;
        return -1;
    }

    float sum = 0;

    for(int i = 0; i < rows1; i++) {
        for(int j = 0; j < cols1; j++) {
            sum += pow(img1.at<uint8_t>(i, j) - img2.at<uint8_t>(i, j), 2);
        }
    }

    sum = sqrt(sum);
    return sum;
}

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

    if (!original.data || !input.data) {
        std::cout<<"No image data"<<std::endl;
        return -1;
    }

    cv::Mat output = input.clone();

    //TODO: Verify filter name.
    int curr = 1;
    std::string outputName;
    for(auto filter: filters) {
        if(filter["name"] == "gaussian") {
            FilterFactory::gaussianBlur(output, filter["params"]);
        }
        if(filter["name"] == "fourier"){
            FilterFactory::fourierFilter(output, filter["params"]);
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
            cv::bitwise_not(output, output);
        }
        if(filter["name"] == "butterWorth") {
            FilterFactory::butterWorth(output, filter["params"]);
        }

        if(filter["params"].value("output", "") != "") {
            cv::imwrite(j["base"].get<std::string>() + filter["params"]["output"].get<std::string>(), output);
        }

        cv::imshow(filter["name"], output);
        cv::waitKey();
    }

    float distance = euclideanDistance(original, output);

    if(distance >= 0) {
        std::cout<<"Distance: "<<distance<<std::endl;
    }

    if(j.value("output", "") != "") {
        cv::imwrite(j["base"].get<std::string>() + j["output"].get<std::string>(), output);
    }

    return 0;

}
