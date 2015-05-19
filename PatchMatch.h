/*
 * Author: Alexander Brauckmann
 */

#ifndef PATCHMATCH_H_
#define PATCHMATCH_H_

#include "ISimilarityMeasure.h"
#include <opencv2/opencv.hpp>


namespace exercise {

/*
 * This class implements the patchmatch algorithm proposed by Barnes, Shechtman,
 * Finkelstein, Goldman.
 * SOURCE:  http://gfx.cs.princeton.edu/gfx/pubs/Barnes_2009_PAR/patchmatch.pdf
 */

class PatchMatch {
public:
  PatchMatch(
            cv::Mat in1, 
            cv::Mat in2, 
            ISimilarityMeasurePtr similarityMeasure,
            std::string outPath);
  ~PatchMatch();

  void BasicApproach();
  void PyramidApproach(
        int min_r, 
        int min_c);

private:
  float ComputeSumSquaredDifferences(
            cv::Mat in1, 
            cv::Mat in2, 
            int radius, 
            int r_1, 
            int c_1, 
            int r_2, 
            int c_2);
  cv::Mat MinFlowFields(
            cv::Mat in1, 
            cv::Mat in2, 
            cv::Mat flowField1, 
            cv::Mat flowField2);
  cv::Mat InitRandom(
            int dim_r, 
            int dim_c);
  cv::Mat ComputeFlowField(
            cv::Mat img1, 
            cv::Mat img2, 
            bool upLeftToDownRight, 
            cv::Mat matIn);

  void toFlowFile(
            cv::Mat flowField, 
            std::string filename);

private:
  int m_rows;
  int m_cols;

  cv::Mat m_in1;
  cv::Mat m_in2;

  ISimilarityMeasurePtr m_similarityMeasure;

  std::string m_outPath;
};

typedef std::shared_ptr<PatchMatch> PatchMatchPtr;

} // namespace exercise

#endif // PATCHMATCH_H_
