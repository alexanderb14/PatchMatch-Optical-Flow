/*
 * Author: Alexander Brauckmann
 */

#ifndef CROSSCORRELATION_H_
#define CROSSCORRELATION_H_

#include "ISimilarityMeasure.h"
#include <opencv2/opencv.hpp>


namespace exercise {

/*
 * This class implements the cross-correlation similarity measure.
 * SOURCE:  http://en.wikipedia.org/wiki/Cross-correlation
 * LATEX:   \frac{1}{n} \sum_{x,y}
 *          \frac{(f(x,y) - \overline{f})(t(x,y) - \overline{t})}
 *          {\sigma_f \sigma_t}
 */

class CrossCorrelation : 
        public ISimilarityMeasure {

public:
  CrossCorrelation(int radius, bool normalized);

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
  bool m_normalized;
};

} // namespace exercise

#endif //CROSSCORRELATION_H_
