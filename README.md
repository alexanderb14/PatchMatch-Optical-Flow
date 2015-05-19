PatchMatch algorithm
======================

Implementation of the PatchMatch algorithm proposed by Barnes, Shechtman, Finkelstein and Goldman.
SOURCE:  http://gfx.cs.princeton.edu/gfx/pubs/Barnes_2009_PAR/patchmatch.pdf 

Prerequisites
----------

Software
--------
- OpenCV [http://opencv.org/]
- CMake [http://www.cmake.org/install/]

Datasets
--------
- Middlebury dataset [http://vision.middlebury.edu/flow/data/comp/zip/other-color-twoframes.zip
]
- Ground-truth dataset [http://vision.middlebury.edu/flow/data/comp/zip/other-gt-flow.zip]
- Evaluation tool [http://vision.middlebury.edu/flow/code/flow-code.zip]

Build instructions
----------

PatchMatch algorithm
--------
- git-clone the repo: 
> git clone https://github.com/alexanderb14/PatchMatch-Optical-Flow.git
- cd into it: 
> cd PatchMatch-Optical-Flow
- create a build directory and cd into it: 
> mkdir build && cd build
- run cmake and initiate the build process: 
> cmake .. && make

Evaluation tool
--------
- build imageLib
> cd imageLib
> make
- build evaluation tool
> cd ..
> make

Usage
----------
- two executables will be generated
  - PatchMatchApp
    - gui program, intended to test similarity measures
    - user can click to a location in each picture. Then, the similarity is calculated.
  - PatchMatchBenchmark
    - headless program, intended to work with gen_dataset.sh:
      - If you configure gen_dataset.sh in the right way, it will generate you all flow images (PatchMatch+Ground-truth) for the whole dataset, using two similarity measures. To do this, modify the following 3 variables in the header of the file:
        - FLOWCODE_EXEC=<location of color_flow>

        - DATASET_DIR=<directory of middlebury dataset (where folders like Dimetrodon, Grove2, ... are located)>
        - GROUNDTRUTH_DIR=<directory of ground-truth (where folders like Dimetrodon, Grove2, ... are located)>
