/*
 * Author: Alexander Brauckmann
 */

#ifndef ISIMILARITYMEASURE_H_
#define ISIMILARITYMEASURE_H_

#include <opencv2/opencv.hpp>


namespace exercise {

/*
 * A interface representing a similarity measure.
 */
class ISimilarityMeasure {
public:
  virtual float ComputeDifference(
            cv::Mat in1, 
            cv::Mat in2, 
            int r_1, 
            int c_1, 
            int r_2, 
            int c_2) = 0;
};

typedef std::shared_ptr<ISimilarityMeasure> ISimilarityMeasurePtr;

} // namespace exercise

#endif //ISIMILARITYMEASURE_H_
