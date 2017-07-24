// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

typedef struct textureFingerprintStruct 
{
	int coordinate_x = 0;
	int coordinate_y = 0;

	//char fingerprint[64];
	//double fingerprint = 0.0;
	unsigned long long int fingerprint = 0;
}TextureFingerprintStruct;

Mat SearchForSimilarAreas(Mat currentPatch, Mat texturePic);
Mat Vertical(Mat rock1_, Mat rock2_, int cutRows);
Mat Horizontal(Mat rock1_, Mat rock2_, int cutCols);
// TODO: reference additional headers your program requires here
