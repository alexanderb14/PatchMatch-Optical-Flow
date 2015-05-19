/*
 * Author: Alexander Brauckmann
 */

#include <stdio.h>
#include <opencv2/opencv.hpp>

#include "patchmatch.h"
#include "SumOfSquaredDifferences.h"
#include "CrossCorrelation.h"

int radius;

int r_1;
int c_1;
int r_2;
int c_2;
cv::Mat img1, img2;

exercise::PatchMatchPtr _pm;
exercise::ISimilarityMeasurePtr _sm;

//-----------------------------------------------------------------------------------------------
int main(int argc, char** argv) {
    std::string outPath;
    std::string similarityMeasure;

    // read image from file + error handling
    if (argc < 6) {
        std::cout << "No sufficient args provided!" << std::endl
          << "Usage: ./cv2 [path to image1] [path to image 2] [path to output flow files] [similarity measure] [radius of similarity measure]" << std::endl;

        return 0;
    } else {
        img1 = cv::imread(argv[1]);
        img2 = cv::imread(argv[2]);
        outPath = argv[3];
        similarityMeasure.append(argv[4]);
        radius = atoi(argv[5]);
    }

    if(similarityMeasure == "CrossCorrelation")
        _sm.reset(new exercise::CrossCorrelation(radius, false)); 
    if(similarityMeasure == "CrossCorrelationNormalized")
        _sm.reset(new exercise::CrossCorrelation(radius, true));       
    if(similarityMeasure == "SumOfSquaredDifferences")
        _sm.reset(new exercise::SumOfSquaredDifferences(radius));
    if(similarityMeasure == "")
        _sm.reset(new exercise::SumOfSquaredDifferences(radius));

    _pm.reset(new exercise::PatchMatch(
                    img1, 
                    img2, 
                    _sm, 
                    outPath) );

    _pm->PyramidApproach(2, 4);

    return 0;
}
