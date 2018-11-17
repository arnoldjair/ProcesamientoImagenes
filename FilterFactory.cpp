#include <stdint.h>
#include "FilterFactory.h"



FilterFactory::FilterFactory() {

}

FilterFactory::~FilterFactory() {

}

void FilterFactory::getDFT(cv::Mat& input, cv::Mat& output){

    for(int i = 0; i < input.rows; i++) {
        for(int j = 0; j < input.cols; j++){
            input.at<float>(i,j) * pow(-1, (i + j));
        }
    }

    cv::Mat padded;
    int m = cv::getOptimalDFTSize(input.rows);
    int n = cv::getOptimalDFTSize(input.cols);
    cv::copyMakeBorder(input, padded, 0, m - input.rows, 0, n - input.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
    cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F)};
    cv::Mat complexI;
    cv::merge(planes, 2, complexI);
    cv::dft(complexI, complexI, cv::DFT_COMPLEX_OUTPUT, input.rows);

    output = complexI;
}

void FilterFactory::invertDFT(cv::Mat& source, cv::Mat& destination) {
    cv::Mat inverse;
    cv::dft(source, inverse, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);
    destination = inverse;
}

void FilterFactory::showDFT(cv::Mat& source) {
    cv::Mat splitArray[2] = {cv::Mat::zeros(source.size(), CV_32F), cv::Mat::zeros(source.size(), CV_32F)};

    cv::split(source, splitArray);

    cv::Mat dftMagnitude;

    cv::magnitude(splitArray[0], splitArray[1], dftMagnitude);

    dftMagnitude += cv::Scalar::all(1);

    cv::log(dftMagnitude, dftMagnitude);

    cv::normalize(dftMagnitude, dftMagnitude, 0, 1, CV_MINMAX);

    cv::waitKey();
    cv::imshow("como las yucas", dftMagnitude);
    cv::waitKey();
}

void FilterFactory::recenterDFT(cv::Mat& source) {
    int centerX = source.cols / 2;
    int centerY = source.rows / 2;

    cv::Mat q1(source, cv::Rect(0, 0, centerX, centerY));
    cv::Mat q2(source, cv::Rect(centerX, 0, centerX, centerY));
    cv::Mat q3(source, cv::Rect(0, centerY, centerX, centerY));
    cv::Mat q4(source, cv::Rect(centerX, centerY, centerX, centerY));

    cv::Mat swapMap;

    q1.copyTo(swapMap);
    q4.copyTo(q1);
    swapMap.copyTo(q4);

    q2.copyTo(swapMap);
    q3.copyTo(q2);
    swapMap.copyTo(q3);
}



void FilterFactory::FourierFilter(cv::Mat input, nlohmann::json params) {
    if(params["filterImage"] == NULL) {
        std::cout<<"Parameter filterImage is missing"<<std::endl;
        return;
    }

    std::string filterPath = params["base"].get<std::string>() + params["filterImage"].get<std::string>();

    std::cout<<filterPath<<std::endl;
    cv::namedWindow("Borrar", CV_WINDOW_AUTOSIZE);

    //TODO: Change the name to layer.
    cv::Mat filterImage = cv::imread(filterPath, cv::IMREAD_GRAYSCALE);

    if (!filterImage.data) {
        std::cout<<"No filter image data"<<std::endl;
        return;
    }

    filterImage.convertTo(filterImage, CV_32F);
    cv::imshow("Borrar", filterImage);
    cv::waitKey(0);

    cv::Mat complexImage;

    FilterFactory::getDFT(input, complexImage);



    FilterFactory::showDFT(complexImage);

    //normalize(inverseTransform, inverseTransform, 0, 255, CV_MINMAX, CV_8UC1);

}

void FilterFactory::GaussianBlur(cv::Mat input, nlohmann::json params){
    if(params["kernelSize"] == NULL) {
        std::cout<<"Parameter kernelSize is missing"<<std::endl;
        return;
    }

    int kernelSize = params["kernelSize"];
    double sigmaX = 0;
    double sigmaY = 0;

    if(params["sigmaX"] == NULL){
        sigmaX = params["sigmaX"];
    }

    if(params["sigmaY"] == NULL){
        sigmaY = params["sigmaY"];
    }

    std::cout<<params<<std::endl;

    cv::GaussianBlur(input, input, cv::Size(kernelSize, kernelSize), sigmaX, sigmaY);
}

