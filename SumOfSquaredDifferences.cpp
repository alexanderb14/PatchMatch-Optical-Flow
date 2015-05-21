/*
 * Author: Alexander Brauckmann
 */

#include "SumOfSquaredDifferences.h"

namespace exercise {

//-------------------------------------------------------------------------------------------
SumOfSquaredDifferences::SumOfSquaredDifferences(int radius)
              : m_radius(radius) {

}

//-------------------------------------------------------------------------------------------
float SumOfSquaredDifferences::ComputeDifference(
          cv::Mat in1, 
          cv::Mat in2, 
          int r_1, 
          int c_1, 
          int r_2, 
          int c_2) {

  int dim_r = in1.rows;
  int dim_c = in1.cols;

  float sum = 0;
  int pixelCount = 0;

  for(int r_rad = -m_radius; r_rad <= m_radius; r_rad++) {
    for(int c_rad = -m_radius; c_rad <= m_radius; c_rad++) {
      if(r_1+r_rad <= dim_r && r_2+r_rad <= dim_r
          && c_1+c_rad <= dim_c && c_2+c_rad <= dim_c
          && r_1+r_rad >= 0 && r_2+r_rad >= 0
          && c_1+c_rad >= 0 && c_2+c_rad >= 0) {

        float diff = in1.at<cv::Vec3b>(r_1+r_rad, c_1+c_rad)[0] 
                        - in2.at<cv::Vec3b>(r_2+r_rad, c_2+c_rad)[0]
                      + in1.at<cv::Vec3b>(r_1+r_rad, c_1+c_rad)[1] 
                        - in2.at<cv::Vec3b>(r_2+r_rad, c_2+c_rad)[1]
                      + in1.at<cv::Vec3b>(r_1+r_rad, c_1+c_rad)[2] 
                        - in2.at<cv::Vec3b>(r_2+r_rad, c_2+c_rad)[2];

        sum = sum + diff*diff;
        pixelCount++;
      }
    }
  }

  if(pixelCount==0)
    return FLT_MAX;

  return sum/pixelCount;
}

} // namespace exercise
