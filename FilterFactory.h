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
        
        /**
         * Frequency
         */
        static void getDFT(cv::Mat& input, cv::Mat& output);
        static void invertDFT(cv::Mat& source, cv::Mat& destination);
        static void applyDFTFilter(cv::Mat& source, cv::Mat& filter);
        static void applyButterWorthFilter(cv::Mat& source, int lowpass, float radius, float cuttingRadius, float order);
        static void showDFT(cv::Mat& source);
        static void getDFTImage(cv::Mat& source, cv::Mat& destination);
        static void saveDFTImage(cv::Mat& source, nlohmann::json params);
        static void recenterDFT(cv::Mat& source);
        static void fourierFilter(cv::Mat& input, nlohmann::json params);
        static void butterWorth(cv::Mat& input, nlohmann::json params);
        static void getButterWorth(cv::Mat& source, int lowpass, float radius, float cuttingRadius, float order);
        
        /*
         * Spatial
         */
        static void gaussianBlur(cv::Mat& input, nlohmann::json params);
        static void denoising(cv::Mat& input, nlohmann::json params);
        static void erode(cv::Mat& input, nlohmann::json params);
        static void dilate(cv::Mat& input, nlohmann::json params);
        static void invert(cv::Mat& input, nlohmann::json params);
        

};

#endif /* FILTERFACTORY_H_ */

