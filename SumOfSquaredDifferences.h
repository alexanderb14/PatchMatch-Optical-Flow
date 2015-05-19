/*
 * Author: Alexander Brauckmann
 */

#ifndef SUMOFSQUAREDDIFFERENCES_H_
#define SUMOFSQUAREDDIFFERENCES_H_

#include "ISimilarityMeasure.h"
#include <opencv2/opencv.hpp>


namespace exercise {

/*
 * This class implements the Sum of Squared Differences similarity measure.
 */

class SumOfSquaredDifferences : 
        public ISimilarityMeasure {

public:
  SumOfSquaredDifferences(int radius);

//ISimilarityMeasure
public:
  float ComputeDifference(
          cv::Mat in1, 
          cv::Mat in2, 
          int r_1, 
          int c_1, 
          int r_2, 
          int c_2);

private:
  int m_radius;
};

} // namespace exercise

#endif //SUMOFSQUAREDDIFFERENCES_H_
