/**
 * FilterFactory.h
 * Created on: 15/11/2018
 * Author: Arnold Jair Jiménez Vargas <arnoldjair at gmail dot com>
 */

#ifndef FILTERFACTORY_H_
#define FILTERFACTORY_H_

#include <opencv2/opencv.hpp>

class FilterFactory {
    public:
        FilterFactory();
        virtual ~FilterFactory();
        static cv::Mat FourierTransform(cv::Mat input);
};

#endif /* FILTERFACTORY_H_ */

