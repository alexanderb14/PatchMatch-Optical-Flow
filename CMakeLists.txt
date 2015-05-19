cmake_minimum_required(VERSION 2.8)
project( cv2 )

# Libraries
find_package( OpenCV REQUIRED )

# Core
add_definitions( -D_DEBUG_PATCHMATCH )
#add_definitions( -D_CHANGECOUNTER )

set( PatchMatchCore_SRCS
  PatchMatch.cpp
  SumOfSquaredDifferences.cpp
  CrossCorrelation.cpp
)

# App
add_executable( PatchMatchApp
  PatchMatchApp.cpp
  ${PatchMatchCore_SRCS}
)

target_include_directories( PatchMatchApp
  PRIVATE .
          ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries( PatchMatchApp
	${OpenCV_LIBS} 
)

# Benchmark
add_executable( PatchMatchBenchmark
  patchMatchBenchmark.cpp
  ${PatchMatchCore_SRCS}
)

target_include_directories( PatchMatchBenchmark
  PRIVATE .
          ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries( PatchMatchBenchmark
	${OpenCV_LIBS} 
)

