/**
 * FilterFactory.h
 * Created on: 15/11/2018
 * Author: Arnold Jair Jiménez Vargas <arnoldjair at gmail dot com>
 */

#ifndef FILTERFACTORY_H_
#define FILTERFACTORY_H_

#include <iostream>
#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>

//TODO: Is this a factory?
class FilterFactory {
    public:
        FilterFactory();
        virtual ~FilterFactory();
        static void getDFT(cv::Mat& input, cv::Mat& output);
        static void invertDFT(cv::Mat& source, cv::Mat& destination);
        static void applyDFTFilter(cv::Mat& source, cv::Mat& filter);
        static void showDFT(cv::Mat& source);
        static void getDFTImage(cv::Mat& source, cv::Mat& destination);
        static void saveDFTImage(cv::Mat& source, nlohmann::json params);
        static void recenterDFT(cv::Mat& source);
        static void FourierFilter(cv::Mat& input, nlohmann::json params);
        static void GaussianBlur(cv::Mat input, nlohmann::json params);

};

#endif /* FILTERFACTORY_H_ */

