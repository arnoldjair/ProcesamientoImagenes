#include <stdint.h>
#include "FilterFactory.h"



FilterFactory::FilterFactory() {

}

FilterFactory::~FilterFactory() {

}

void FilterFactory::getDFT(cv::Mat& input, cv::Mat& output){

    /*for(int i = 0; i < input.rows; i++) {
      for(int j = 0; j < input.cols; j++){
      input.at<uint8_t>(i, j) = input.at<uint8_t>(i,j) * pow(-1, (i + j));
      }
      }*/

    cv::Mat padded;                            
    int m = cv::getOptimalDFTSize( input.rows );
    int n = cv::getOptimalDFTSize( input.cols ); 
    cv::copyMakeBorder(input, padded, 0, m - input.rows, 0, n - input.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
    cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F)};
    cv::Mat complexI;
    cv::merge(planes, 2, complexI);         
    cv::dft(complexI, complexI);            
    output = complexI;
}

void FilterFactory::invertDFT(cv::Mat& source, cv::Mat& destination) {
    cv::Mat inverseTransform;
    cv::dft(source, inverseTransform, cv::DFT_INVERSE|cv::DFT_REAL_OUTPUT);
    destination = inverseTransform;
}

void FilterFactory::applyDFTFilter(cv::Mat& source, cv::Mat& filter) {
    cv::Mat complexSource;
    cv::Mat complexFilter;
    cv::Mat complexFiltered;
    
    FilterFactory::recenterDFT(filter);
    filter.convertTo(filter, CV_32F);
    cv::normalize(filter, filter, 0, 1, cv::NORM_MINMAX);
    
    cv::Mat planes[2] = {cv::Mat_<float>(source.clone()), cv::Mat::zeros(source.size(), CV_32F)  };

    FilterFactory::getDFT(source, complexSource);

    cv::Mat planesFilter[2] = { cv::Mat_<float>(filter.clone()), cv::Mat::zeros(filter.size(), CV_32F)  };
    cv::merge(planesFilter, 2, complexFilter);

    cv:: mulSpectrums(complexSource, complexFilter, complexFiltered, 0);

    FilterFactory::showDFT(complexFiltered);
    cv::Mat inverseTransform;
    FilterFactory::invertDFT(complexFiltered, inverseTransform);
    normalize(inverseTransform, inverseTransform, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    source = inverseTransform;
}

//TODO: Only on apply
void FilterFactory::applyButterWorthFilter(cv::Mat& source, int lowpass, float radius, float cuttingRadius, float order) {

    cv::Mat complexSource;
    cv::Mat complexFilter;
    cv::Mat complexFiltered;
    cv::Mat planes[2] = {cv::Mat_<float>(source.clone()), cv::Mat::zeros(source.size(), CV_32F)  };

    FilterFactory::getDFT(source, complexSource);

    complexFilter = complexSource.clone();

    FilterFactory::getButterWorth(complexFilter, lowpass, radius, cuttingRadius, order);
    FilterFactory::recenterDFT(complexSource);
    cv:: mulSpectrums(complexSource, complexFilter, complexFiltered, 0);
    FilterFactory::recenterDFT(complexFiltered);

    FilterFactory::showDFT(complexFiltered);

    cv::Mat inverseTransform;
    FilterFactory::invertDFT(complexFiltered, inverseTransform);
    normalize(inverseTransform, inverseTransform, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    source = inverseTransform;
}


void FilterFactory::showDFT(cv::Mat& source) {
    cv::Mat dftMagnitude;
    FilterFactory::getDFTImage(source, dftMagnitude);
    cv::imshow("como las yucas", dftMagnitude);
    cv::waitKey();
}

void FilterFactory::getDFTImage(cv::Mat& source, cv::Mat& destination) {
    cv::Mat splitArray[2] = {cv::Mat::zeros(source.size(), CV_32F), cv::Mat::zeros(source.size(), CV_32F)};
    cv::split(source, splitArray);
    cv::Mat dftMagnitude;
    cv::magnitude(splitArray[0], splitArray[1], dftMagnitude);
    dftMagnitude += cv::Scalar::all(1);
    cv::log(dftMagnitude, dftMagnitude);
    //TODO: Norm 0-255?
    cv::normalize(dftMagnitude, dftMagnitude, 0, 1, CV_MINMAX);
    FilterFactory::recenterDFT(dftMagnitude);
    destination = dftMagnitude;
}

void FilterFactory::saveDFTImage(cv::Mat& source, nlohmann::json params) {
    cv::Mat complexImage;
    cv::Mat dftImage;
    FilterFactory::getDFT(source, complexImage);
    FilterFactory::getDFTImage(complexImage, dftImage);
    normalize(dftImage, dftImage, 0, 255, CV_MINMAX, CV_8UC1);
    source = dftImage;
}

void FilterFactory::recenterDFT(cv::Mat& source) {
    source = source(cv::Rect(0, 0, source.cols & -2, source.rows & -2));

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



void FilterFactory::fourierFilter(cv::Mat& input, nlohmann::json params) {
    if(params["filterImage"] == NULL) {
        std::cout<<"Parameter filterImage is missing"<<std::endl;
        return;
    }

    std::string filterPath = params["base"].get<std::string>() + params["filterImage"].get<std::string>();

    //TODO: Change the name to layer.
    cv::Mat filterImage = cv::imread(filterPath, cv::IMREAD_GRAYSCALE);
    int rows = input.rows;
    int cols = input.cols;

    if (!filterImage.data) {
        std::cout<<"No filter image data"<<std::endl;
        return;
    }


    FilterFactory::applyDFTFilter(input, filterImage);

    //Region of interest
    cv::Rect roi(0, 0, cols, rows);

    input = input(roi);

    //normalize(inverseTransform, inverseTransform, 0, 255, CV_MINMAX, CV_8UC1);

}

void FilterFactory::gaussianBlur(cv::Mat& input, nlohmann::json params){
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

    cv::GaussianBlur(input, input, cv::Size(kernelSize, kernelSize), sigmaX, sigmaY);
}

void FilterFactory::denoising(cv::Mat& input, nlohmann::json params) {

    cv::imshow("Original", input);
    int templateWindowSize = params.value("templateWindowSize", 7);
    int searchWindowSize = params.value("searchWindowSize", 21);
    float h = params.value("h", 1);
    cv::Mat result;
    std::cout<<params<<std::endl;
    cv::fastNlMeansDenoising(input, result, h, templateWindowSize, searchWindowSize);
    input = result;
}

//TODO: size must not be divisble by 2 (?)
void FilterFactory::erode(cv::Mat& input, nlohmann::json params){
    int center = params.value("center", 1);
    int size = 2 * center + 1;
    std::string type = params.value("type", "ellipse");
    int erosionType;

    if(type == "rect") {
        erosionType = cv::MORPH_RECT;
    } else if (type == "cross") {
        erosionType = cv::MORPH_CROSS;
    } else {
        erosionType = cv::MORPH_ELLIPSE;
    }


    cv::Mat element = cv::getStructuringElement(erosionType,
            cv::Size( size, size),
            cv::Point( center, center) );
    cv::Mat result;
    cv::erode(input, result, element);
    input = result;
}

void FilterFactory::dilate(cv::Mat& input, nlohmann::json params) {
    int center = params.value("center", 1);
    int size = 2 * center + 1;
    std::string type = params.value("type", "ellipse");
    int erosionType;

    if(type == "rect") {
        erosionType = cv::MORPH_RECT;
    } else if (type == "cross") {
        erosionType = cv::MORPH_CROSS;
    } else {
        erosionType = cv::MORPH_ELLIPSE;
    }

    cv::Mat element = cv::getStructuringElement(erosionType,
            cv::Size( size, size),
            cv::Point( center, center) );
    cv::Mat result;
    cv::dilate(input, result, element);
    input = result;
}

void FilterFactory::invert(cv::Mat& input, nlohmann::json params) {
    cv::bitwise_not(input, input);

}

void FilterFactory::butterWorth(cv::Mat& input, nlohmann::json params){
    int lowpass = params.value("lowpass", 0);
    float radius = params.value("lowpass", 0.0);
    float cuttingRadius = params.value("cuttingRadius", 0.0);
    float order = params.value("order", 1.0);

    int rows = input.rows;
    int cols = input.cols;


    FilterFactory::applyButterWorthFilter(input, lowpass, radius, cuttingRadius, order);

    //Region of interest
    cv::Rect roi(0, 0, cols, rows);

    input = input(roi);
}

void FilterFactory::getButterWorth(cv::Mat& source, int lowpass, float radius, float cuttingRadius, float order) {
    cv::Mat imageFilter(source.rows,source.cols,CV_32FC2);
    cv::Point O;
    O.x = int(imageFilter.cols/2);
    O.y = int(imageFilter.rows/2);

    imageFilter.setTo(0);

    for(int i=0;i<imageFilter.rows;i++) {
        float* ptr = (float*)imageFilter.ptr<float>(i);
        for(int j=0;j<imageFilter.cols;j++) {
            float radius = sqrt(float((j-O.x)*(j-O.x)) + float((i-O.y)*(i-O.y)));
            if(lowpass == 1) {
                ptr[2*j]= 1/(1+pow(radius/cuttingRadius,2*order));
                ptr[2*j+1]=1/(1+pow(radius/cuttingRadius,2*order));

            } else {
                if(radius!=0) {
                    ptr[2*j]= 1/(1+pow(cuttingRadius/radius,2*order));
                    ptr[2*j+1]= 1/(1+pow(cuttingRadius/radius,2*order));
                } else {
                    ptr[2*j]=0;
                    ptr[2*j+1]=0;
                }
            }
        }
    }

    source = imageFilter;
}
