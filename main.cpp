#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
void help();
void calcPSF(Mat& outputImg, Size filterSize, int R);
void fftshift(const Mat& inputImg, Mat& outputImg);
void filter2DFreq(const Mat& inputImg, Mat& outputImg, const Mat& H);
void calcWnrFilter(const Mat& input_h_PSF, Mat& output_G, double nsr);
const String keys =
"{help h usage ? |             | print this message   }"
"{image          |original.JPG | input image name     }"
"{R              |53           | radius               }"
"{SNR            |5200         | signal to noise ratio}"
;
int main(int argc, char *argv[])
{
    help();
    CommandLineParser parser(argc, argv, keys);
    if (parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }
    int R = parser.get<int>("R");
    int snr = parser.get<int>("SNR");
    string strInFileName = parser.get<String>("image");
    if (!parser.check())
    {
        parser.printErrors();
        return 0;
    }
    Mat imgIn;
    imgIn = imread(strInFileName, IMREAD_GRAYSCALE);
    if (imgIn.empty()) //check whether the image is loaded or not
    {
        cout << "ERROR : Image cannot be loaded..!!" << endl;
        return -1;
    }
    Mat imgOut;
    // it needs to process even image only
    Rect roi = Rect(0, 0, imgIn.cols & -2, imgIn.rows & -2);
    //Hw calculation (start)
    Mat Hw, h;
    calcPSF(h, roi.size(), R);
    calcWnrFilter(h, Hw, 1.0 / double(snr));
    //Hw calculation (stop)
    // filtering (start)
    filter2DFreq(imgIn(roi), imgOut, Hw);
    // filtering (stop)
    imgOut.convertTo(imgOut, CV_8U);
    normalize(imgOut, imgOut, 0, 255, NORM_MINMAX);
    imwrite("result.jpg", imgOut);
    imshow("t", imgOut);
    waitKey(0);
    return 0;
}
void help()
{
    cout << "2018-07-12" << endl;
    cout << "DeBlur_v8" << endl;
    cout << "You will learn how to recover an out-of-focus image by Wiener filter" << endl;
}
void calcPSF(Mat& outputImg, Size filterSize, int R)
{
    Mat h(filterSize, CV_32F, Scalar(0));
    Point point(filterSize.width / 2, filterSize.height / 2);
    circle(h, point, R, 255, -1, 8);
    Scalar summa = sum(h);
    outputImg = h / summa[0];
}
void fftshift(const Mat& inputImg, Mat& outputImg)
{
    outputImg = inputImg.clone();
    int cx = outputImg.cols / 2;
    int cy = outputImg.rows / 2;
    Mat q0(outputImg, Rect(0, 0, cx, cy));
    Mat q1(outputImg, Rect(cx, 0, cx, cy));
    Mat q2(outputImg, Rect(0, cy, cx, cy));
    Mat q3(outputImg, Rect(cx, cy, cx, cy));
    Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}
void filter2DFreq(const Mat& inputImg, Mat& outputImg, const Mat& H)
{
    Mat planes[2] = { Mat_<float>(inputImg.clone()), Mat::zeros(inputImg.size(), CV_32F) };
    Mat complexI;
    merge(planes, 2, complexI);
    dft(complexI, complexI, DFT_SCALE);
    Mat planesH[2] = { Mat_<float>(H.clone()), Mat::zeros(H.size(), CV_32F) };
    Mat complexH;
    merge(planesH, 2, complexH);
    Mat complexIH;
    mulSpectrums(complexI, complexH, complexIH, 0);
    idft(complexIH, complexIH);
    split(complexIH, planes);
    outputImg = planes[0];
}
void calcWnrFilter(const Mat& input_h_PSF, Mat& output_G, double nsr)
{
    Mat h_PSF_shifted;
    fftshift(input_h_PSF, h_PSF_shifted);
    Mat planes[2] = { Mat_<float>(h_PSF_shifted.clone()), Mat::zeros(h_PSF_shifted.size(), CV_32F) };
    Mat complexI;
    merge(planes, 2, complexI);
    dft(complexI, complexI);
    split(complexI, planes);
    Mat denom;
    pow(abs(planes[0]), 2, denom);
    denom += nsr;
    divide(planes[0], denom, output_G);
}

/*

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
*/
