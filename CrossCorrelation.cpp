/*
 * Author: Alexander Brauckmann
 */

#include "CrossCorrelation.h"

namespace exercise {

//-------------------------------------------------------------------------------------------
CrossCorrelation::CrossCorrelation(int radius, bool normalized)
        : m_radius(radius),
          m_normalized(normalized) {
}

//-------------------------------------------------------------------------------------------
float CrossCorrelation::ComputeDifference(
          cv::Mat in1, 
          cv::Mat in2, 
          int r_1, 
          int c_1, 
          int r_2, 
          int c_2) {

  int dim_r = in1.rows;
  int dim_c = in1.cols;

  // avg and pixelcount
  float sum_img1[3] = {0,0,0};
  float sum_img2[3] = {0,0,0};
  int pixelCount = 0;

  for(int r_rad = -m_radius; r_rad <= m_radius; r_rad++) {
    for(int c_rad = -m_radius; c_rad <= m_radius; c_rad++) {
      if(r_1+r_rad <= dim_r && r_2+r_rad <= dim_r
          && c_1+c_rad <= dim_c && c_2+c_rad <= dim_c
          && r_1+r_rad >= 0 && r_2+r_rad >= 0
          && c_1+c_rad >= 0 && c_2+c_rad >= 0) {

        pixelCount++;

        for(int rgb=0; rgb<3; rgb++) {
          sum_img1[rgb] += in1.at<cv::Vec3b>(r_1+r_rad, c_1+c_rad)[rgb];
          sum_img2[rgb] += in2.at<cv::Vec3b>(r_2+r_rad, c_2+c_rad)[rgb];
        }
      }
    }
  }

  float avg_img1[3];
  float avg_img2[3];
  for(int rgb=0; rgb<3; rgb++) {
    avg_img1[rgb] = sum_img1[rgb] / pixelCount;
    avg_img2[rgb] = sum_img2[rgb] / pixelCount;
  }

  // standard deviations
  float std_deviation_img1[3] = {0,0,0};
  float std_deviation_img2[3] = {0,0,0};

  if(m_normalized) {  
    for(int r_rad = -m_radius; r_rad <= m_radius; r_rad++) {
      for(int c_rad = -m_radius; c_rad <= m_radius; c_rad++) {
        if(r_1+r_rad <= dim_r && r_2+r_rad <= dim_r
            && c_1+c_rad <= dim_c && c_2+c_rad <= dim_c
            && r_1+r_rad >= 0 && r_2+r_rad >= 0
            && c_1+c_rad >= 0 && c_2+c_rad >= 0) {
  
          for(int rgb=0; rgb<3; rgb++) {
            std_deviation_img1[rgb] += std::pow(in1.at<cv::Vec3b>(r_1+r_rad, c_1+c_rad)[rgb] - avg_img1[rgb], 2);
            std_deviation_img2[rgb] += std::pow(in2.at<cv::Vec3b>(r_2+r_rad, c_2+c_rad)[rgb] - avg_img2[rgb], 2);
          }
        }
      }
    }
  
    for(int rgb=0; rgb<3; rgb++) {
      std_deviation_img1[rgb] = std::sqrt(std_deviation_img1[rgb] / pixelCount);
      std_deviation_img2[rgb] = std::sqrt(std_deviation_img2[rgb] / pixelCount);

      if(std_deviation_img1[rgb] == 0)
        std_deviation_img1[rgb] = FLT_MIN;
      if(std_deviation_img2[rgb] == 0)
        std_deviation_img2[rgb] = FLT_MIN;
    }
  }

  // cross correlation
  float crossCorrelation[3] = {0,0,0};

  for(int r_rad = -m_radius; r_rad <= m_radius; r_rad++) {
    for(int c_rad = -m_radius; c_rad <= m_radius; c_rad++) {
      if(r_1+r_rad <= dim_r && r_2+r_rad <= dim_r
          && c_1+c_rad <= dim_c && c_2+c_rad <= dim_c
          && r_1+r_rad >= 0 && r_2+r_rad >= 0
          && c_1+c_rad >= 0 && c_2+c_rad >= 0) {

        for(int rgb=0; rgb<3; rgb++) {
          crossCorrelation[rgb] +=  ( (in1.at<cv::Vec3b>(r_1+r_rad, c_1+c_rad)[rgb]
                                        - avg_img1[rgb])
                                    * (in2.at<cv::Vec3b>(r_2+r_rad, c_2+c_rad)[rgb]
                                        - avg_img2[rgb]) )
                                    / (m_normalized ? ( std_deviation_img1[rgb] * std_deviation_img2[rgb] ) : 1);
        }
      }
    }
  }

  for(int rgb=0; rgb<3; rgb++) {
    crossCorrelation[rgb] += crossCorrelation[rgb] / pixelCount;
  } 


  if(pixelCount==0)
    return FLT_MAX;

  return (-1) * (crossCorrelation[0] + crossCorrelation[1] + crossCorrelation[2]);
}

} // namespace exercise
