/*
 * Author: Alexander Brauckmann
 */

#include "PatchMatch.h"

#include <vector>
#include <string>


namespace exercise {

//-------------------------------------------------------------------------------------------
PatchMatch::PatchMatch(
            cv::Mat in1, 
            cv::Mat in2, 
            ISimilarityMeasurePtr similarityMeasure,
            std::string outPath)
              : m_rows(in1.rows),
                m_cols(in1.cols),
                m_similarityMeasure(similarityMeasure) {

  in1.copyTo(m_in1);
  in2.copyTo(m_in2);

  m_outPath = outPath;
}

//-------------------------------------------------------------------------------------------
PatchMatch::~PatchMatch() {

}

//-------------------------------------------------------------------------------------------
void PatchMatch::BasicApproach() {

  cv::Mat matInit = InitRandom(m_rows, m_cols);
  toFlowFile(matInit, "iteration0.flo");

  cv::Mat matFollow;
  matInit.copyTo(matFollow);

  for(int i=1; i<=5; i++) {
    matFollow = ComputeFlowField(m_in1, m_in2, (i%2!=0)?true:false, matFollow);
    toFlowFile(matFollow, "iteration" + std::to_string(i) + ".flo");
  }
}

//-------------------------------------------------------------------------------------------
void PatchMatch::PyramidApproach(
        int min_r, 
        int min_c) {

  // compute levels
  int s_r = m_rows;
  int s_c = m_cols;

  std::vector<cv::Mat> levels1, levels2;
  levels1.push_back(m_in1);
  levels2.push_back(m_in2);

  while(s_r>min_r && s_c>min_r) {
    s_r = s_r/2;
    s_c = s_c/2;

    cv::Mat level1;
    cv::resize(m_in1, level1, cv::Size(s_c, s_r), cv::INTER_LINEAR);
    levels1.push_back(level1);

    cv::Mat level2;
    cv::resize(m_in2, level2, cv::Size(s_c, s_r), cv::INTER_LINEAR);
    levels2.push_back(level2);
    
  }
  std::reverse(levels1.begin(), levels1.end());
  std::reverse(levels2.begin(), levels2.end());

  cv::Mat matPreviousLevel = InitRandom(levels1[0].rows, levels1[0].cols);
  for(int i=1; i<levels1.size(); i++) {
    // Propagate from previous level level
    cv::Mat matThisLevel(levels1[i].rows, levels1[i].cols, CV_32SC2, cv::Scalar::all(0));
    for(int r=0; r<levels1[i].rows; r++) {
      for(int c=0; c<levels1[i].cols; c++) {
        matThisLevel.at<cv::Vec2i>(r, c)[0] = matPreviousLevel.at<cv::Vec2i>(r/2, c/2)[0];
        matThisLevel.at<cv::Vec2i>(r, c)[1] = matPreviousLevel.at<cv::Vec2i>(r/2, c/2)[1];
      }
    }

    // PatchMatch on random Initialization
    cv::Mat matRandom = InitRandom(levels1[i].rows, levels1[i].cols);
    matRandom = ComputeFlowField(
                  levels1[i], 
                  levels2[i], 
                  (i%2!=0)?true:false, 
                  matRandom );

    cv::Mat flowFieldMin = MinFlowFields(
                            levels1[i], 
                            levels2[i], 
                            matThisLevel, 
                            matRandom );

    cv::Mat matFollow;
    flowFieldMin.copyTo(matFollow);
    for(int j=0; j<=3; j++) {
      matFollow = ComputeFlowField(
                    levels1[i], 
                    levels2[i], 
                    (j%2!=0)?true:false, 
                    matFollow );
    }

    matPreviousLevel = cv::Mat(levels1[i].rows, levels1[i].cols, CV_32SC2, cv::Scalar::all(0));
    matFollow.copyTo(matPreviousLevel); 

    #ifdef _DEBUG_PATCHMATCH
    toFlowFile(matPreviousLevel, std::to_string(i) + "matFollow" + ".flo");
    #endif //_DEBUG_PATCHMATCH
  }

  #ifndef _DEBUG_PATCHMATCH
  toFlowFile(matPreviousLevel, "Final.flo"); 
  #endif //_DEBUG_PATCHMATCH
}

//-------------------------------------------------------------------------------------------
float PatchMatch::ComputeSumSquaredDifferences(
            cv::Mat in1, 
            cv::Mat in2, 
            int radius, 
            int r_1, 
            int c_1, 
            int r_2, 
            int c_2) {

  int dim_r = in1.rows;
  int dim_c = in1.cols;

  float sum = 0;
  int pixelCount = 0;

  for(int r_rad = -radius; r_rad <= radius; r_rad++) {
    for(int c_rad = -radius; c_rad <= radius; c_rad++) {
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
    return INT_MAX;

  return sum/pixelCount;
}

//-------------------------------------------------------------------------------------------
cv::Mat PatchMatch::MinFlowFields(
            cv::Mat in1, 
            cv::Mat in2, 
            cv::Mat flowField1, 
            cv::Mat flowField2) {

  int dim_r = flowField1.rows;
  int dim_c = flowField1.cols;

  cv::Mat flowFieldMin(dim_r, dim_c, CV_32SC2, cv::Scalar::all(0));

  for(int r=0; r<dim_r; r++) {
    for(int c=0; c<dim_c; c++) {
       float ff1 = m_similarityMeasure->ComputeDifference(
                    in1,
                    in2,
                    r, 
                    c, 
                    r + flowField1.at<cv::Vec2i>(r, c)[0], 
                    c + flowField1.at<cv::Vec2i>(r, c)[1] );
       float ff2 = m_similarityMeasure->ComputeDifference(
                    in1,
                    in2,
                    r, 
                    c, 
                    r + flowField2.at<cv::Vec2i>(r, c)[0], 
                    c + flowField2.at<cv::Vec2i>(r, c)[1] );

      if(ff1<ff2) {
        flowFieldMin.at<cv::Vec2i>(r, c)[0] = flowField1.at<cv::Vec2i>(r, c)[0];
        flowFieldMin.at<cv::Vec2i>(r, c)[1] = flowField1.at<cv::Vec2i>(r, c)[1];
      } else {
        flowFieldMin.at<cv::Vec2i>(r, c)[0] = flowField2.at<cv::Vec2i>(r, c)[0];
        flowFieldMin.at<cv::Vec2i>(r, c)[1] = flowField2.at<cv::Vec2i>(r, c)[1];
      }
    }
  }

  return flowFieldMin;
}

//-------------------------------------------------------------------------------------------
cv::Mat PatchMatch::InitRandom(
            int dim_r, 
            int dim_c) {

  cv::Mat flowField(dim_r, dim_c, CV_32SC2, cv::Scalar::all(0));

  for(int r=0; r<dim_r; r++) {
    for(int c=0; c<dim_c; c++) {
      flowField.at<cv::Vec2i>(r, c)[0] = (rand() % dim_r) - r;
      flowField.at<cv::Vec2i>(r, c)[1] = (rand() % dim_c) - c;
    }
  }

  return flowField;
}

//-------------------------------------------------------------------------------------------
cv::Mat PatchMatch::ComputeFlowField(
            cv::Mat img1, 
            cv::Mat img2, 
            bool upLeftToDownRight, 
            cv::Mat matIn) {

  #ifdef _CHANGECOUNTER
  int change_counter=0;
  #endif // _CHANGECOUNTER

  int dim_r = matIn.rows;
  int dim_c = matIn.cols;

  cv::Mat flowField;
  matIn.copyTo(flowField);

    // in even iterations search goes from top left to buttom right
    if(upLeftToDownRight) {
      for(int r=1; r<dim_r; r++) {
        for(int c=1; c<dim_c; c++) {

          // Propagation phase
          float rc = m_similarityMeasure->ComputeDifference(
                        img1,
                        img2,
                        r, 
                        c, 
                        r + flowField.at<cv::Vec2i>(r, c)[0], 
                        c + flowField.at<cv::Vec2i>(r, c)[1] );
          float r1c = m_similarityMeasure->ComputeDifference(
                        img1,
                        img2,
                        r, 
                        c, 
                        r + flowField.at<cv::Vec2i>(r-1, c)[0], 
                        c + flowField.at<cv::Vec2i>(r-1, c)[1] );
          float rc1 = m_similarityMeasure->ComputeDifference(
                        img1,
                        img2,
                        r, 
                        c, 
                        r + flowField.at<cv::Vec2i>(r, c-1)[0], 
                        c + flowField.at<cv::Vec2i>(r, c-1)[1] );

          bool minFound = false;
          if(rc<=r1c && rc<=rc1 && !minFound) {
            // everything stays as it is
            minFound = true;
          }
          if(r1c<=rc && r1c<=rc1 && !minFound) {
            flowField.at<cv::Vec2i>(r, c)[0] = flowField.at<cv::Vec2i>(r-1, c)[0];
            flowField.at<cv::Vec2i>(r, c)[1] = flowField.at<cv::Vec2i>(r-1, c)[1];
            minFound = true;

            #ifdef _CHANGECOUNTER
            change_counter++;
            #endif // _CHANGECOUNTER
          }
          if(rc1<=rc && rc1<=r1c && !minFound) {
            flowField.at<cv::Vec2i>(r, c)[0] = flowField.at<cv::Vec2i>(r, c-1)[0];
            flowField.at<cv::Vec2i>(r, c)[1] = flowField.at<cv::Vec2i>(r, c-1)[1];
            minFound = true;

            #ifdef _CHANGECOUNTER
            change_counter++;
            #endif // _CHANGECOUNTER
          }

          // Random search phase
          int v_0[2];
          v_0[0] = flowField.at<cv::Vec2i>(r, c)[0];
          v_0[1] = flowField.at<cv::Vec2i>(r, c)[1];

          int w = std::max(dim_r, dim_c);

          float alpha = 0.5;

          int i = 0;
          while(w * pow(alpha, i) > 1) {
            float R[2];
            R[0] = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1+1))); 
            R[1] = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1+1))); 

            float rc = m_similarityMeasure->ComputeDifference(
                        img1,
                        img2,
                        r, 
                        c, 
                        r + flowField.at<cv::Vec2i>(r, c)[0], 
                        c + flowField.at<cv::Vec2i>(r, c)[1] );


            int u[2];
            u[0] = v_0[0] + w * pow(alpha, i) * R[0];
            u[1] = v_0[1] + w * pow(alpha, i) * R[1];

            float ssd = m_similarityMeasure->ComputeDifference(
                        img1,
                        img2,
                        r, 
                        c, 
                        r + flowField.at<cv::Vec2i>(r, c)[0] + u[0], 
                        c + flowField.at<cv::Vec2i>(r, c)[1] + u[1] );

            if(ssd < rc) {
              flowField.at<cv::Vec2i>(r, c)[0] = flowField.at<cv::Vec2i>(r, c)[0] + u[0];
              flowField.at<cv::Vec2i>(r, c)[1] = flowField.at<cv::Vec2i>(r, c)[1] + u[1];
            }

            i++;
          }
        }
      }

    } else {
      for(int r=dim_r-2; r>=0; r--) {
        for(int c=dim_c-2; c>=0; c--) {

          float rc = m_similarityMeasure->ComputeDifference(
                        img1,
                        img2,
                        r, 
                        c, 
                        r + flowField.at<cv::Vec2i>(r, c)[0], 
                        c + flowField.at<cv::Vec2i>(r, c)[1] );
          float r1c = m_similarityMeasure->ComputeDifference(
                        img1,
                        img2,
                        r, 
                        c, 
                        r + flowField.at<cv::Vec2i>(r+1, c)[0], 
                        c + flowField.at<cv::Vec2i>(r+1, c)[1] );
          float rc1 = m_similarityMeasure->ComputeDifference(
                        img1,
                        img2,
                        r, 
                        c, 
                        r + flowField.at<cv::Vec2i>(r, c+1)[0], 
                        c + flowField.at<cv::Vec2i>(r, c+1)[1] );

          bool minFound = false;
          if(rc<=r1c && rc<=rc1 && !minFound) {
            // everything stays as it is
            minFound = true;
          }
          if(r1c<=rc && r1c<=rc1 && !minFound) {
            flowField.at<cv::Vec2i>(r, c)[0] = flowField.at<cv::Vec2i>(r+1, c)[0];
            flowField.at<cv::Vec2i>(r, c)[1] = flowField.at<cv::Vec2i>(r+1, c)[1];
            minFound = true;

            #ifdef _CHANGECOUNTER
            change_counter++;
            #endif // _CHANGECOUNTER
          }
          if(rc1<=rc && rc1<=r1c && !minFound) {
            flowField.at<cv::Vec2i>(r, c)[0] = flowField.at<cv::Vec2i>(r, c+1)[0];
            flowField.at<cv::Vec2i>(r, c)[1] = flowField.at<cv::Vec2i>(r, c+1)[1];
            minFound = true;

            #ifdef _CHANGECOUNTER
            change_counter++;
            #endif // _CHANGECOUNTER
          }

          // Random search phase
          int v_0[2];
          v_0[0] = flowField.at<cv::Vec2i>(r, c)[0];
          v_0[1] = flowField.at<cv::Vec2i>(r, c)[1];

          int w = std::max(dim_r, dim_c);

          float alpha = 0.5;

          int i = 0;
          while(w * pow(alpha, i) > 1) {
            float R[2];
            R[0] = rand() % 2 + (-1);
            R[1] = rand() % 2 + (-1);

            float rc = m_similarityMeasure->ComputeDifference(
                        img1,
                        img2,
                        r, 
                        c, 
                        r + flowField.at<cv::Vec2i>(r, c)[0], 
                        c + flowField.at<cv::Vec2i>(r, c)[1] );


            int u[2];
            u[0] = v_0[0] + w * pow(alpha, i) * R[0];
            u[1] = v_0[1] + w * pow(alpha, i) * R[1];

            float ssd = m_similarityMeasure->ComputeDifference(
                        img1,
                        img2,
                        r, 
                        c, 
                        r + flowField.at<cv::Vec2i>(r, c)[0] + u[0], 
                        c + flowField.at<cv::Vec2i>(r, c)[1] + u[1] );

            if(ssd < rc) {
              flowField.at<cv::Vec2i>(r, c)[0] = flowField.at<cv::Vec2i>(r, c)[0] + u[0];
              flowField.at<cv::Vec2i>(r, c)[1] = flowField.at<cv::Vec2i>(r, c)[1] + u[1];

              #ifdef _CHANGECOUNTER
              change_counter++;
              #endif // _CHANGECOUNTER
            }

            i++;
          }
        }
      }
    }

  #ifdef _CHANGECOUNTER
  std::cout 
    << "dim_r:" << dim_r << ", "
    << "dim_c:" << dim_c << " -> "
    << change_counter << std::endl;
  #endif // _CHANGECOUNTER

  return flowField;
}

//-------------------------------------------------------------------------------------------
void PatchMatch::toFlowFile(
            cv::Mat flowField, 
            std::string filename) {

  int dim_r = flowField.rows;
  int dim_c = flowField.cols;

  // combine path + filename to an arg for fopen
  std::string combinedFn = m_outPath + "/" + filename;
  std::FILE *stream = std::fopen(combinedFn.c_str(), "wb");

  // write the header
  fprintf(stream, "PIEH");
  fwrite(&dim_c,  sizeof(int), 1, stream);
  fwrite(&dim_r, sizeof(int), 1, stream);

  for(int r=0; r<dim_r; r++) {
    for(int c=0; c<dim_c; c++) {
      float band0 = (float)flowField.at<cv::Vec2i>(r, c)[0];
      fwrite(&band0, sizeof(float), 1, stream);

      float band1 = (float)flowField.at<cv::Vec2i>(r, c)[1];
      fwrite(&band1, sizeof(float), 1, stream);
    }
  }

  // close the file
  fclose(stream);
}

} // namespace exercise
