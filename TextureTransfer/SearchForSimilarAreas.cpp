#include "stdafx.h"

typedef struct minimumDifferenceLocation
{
	int row_num = 0;
	int col_num = 0;
	int pixelDifference = 0;
}MinimumDifferenceLocation;

bool compare_minimumDifferenceLocationArray(MinimumDifferenceLocation const struct1, MinimumDifferenceLocation const struct2);

Mat SearchForSimilarAreas(Mat currentPatch, Mat texturePic)
{
	int patchCols = currentPatch.cols; // the cols number of the patch is currently being compared
	int patchRows = currentPatch.rows; // the rows number of the patch is currently being compared
	int textureCols = texturePic.cols; // the cols number of texture picture
	int textureRows = texturePic.rows; // the rows number of texture picture
	int rows_need_be_compared = textureRows - patchRows; // max rows number of pixel rows pointer can reach
	int cols_need_be_compared = textureCols - patchCols; // max cols number of pixel cols pointer can reach
	

	int **gray_scale_difference_sum;  // contains the result of gray scale difference for each possible reached pixel
	gray_scale_difference_sum = (int**)calloc(rows_need_be_compared, sizeof(int*)); // calloc memory space for gray_scale_difference array
	for (int i = 0; i < rows_need_be_compared; i++)
	{
		gray_scale_difference_sum[i] = (int*)calloc(cols_need_be_compared, sizeof(int));
	}
	for (int i = 0; i < rows_need_be_compared; i++) // initialize gray_scale_difference array
	{
		for (int j = 0; j < cols_need_be_compared; j++)
			gray_scale_difference_sum[i][j] = 0;
	}

	Mat grayCurrentPatch, grayTexturePic;
	cvtColor(currentPatch, grayCurrentPatch, CV_BGR2GRAY, 0);  // colored currently being compared patch is transferred to gray patch
	cvtColor(texturePic, grayTexturePic, CV_BGR2GRAY, 0);  // colored texture picture is transferred to gray picture


	MinimumDifferenceLocation* minimumDifferenceLocationArray = (MinimumDifferenceLocation*)calloc(cols_need_be_compared, sizeof(MinimumDifferenceLocation));
	int minPixelRowPositon = 0;
	int minPixelColPosition = 0;
	int minimumDifferenceSum = INT_MAX;
	for (int i = 0; i < rows_need_be_compared; i++)
	{
		for (int j = 0; j < cols_need_be_compared; j++)
		{
			for (int k = 0; k < patchRows; k++)  // calculate the sum of the gray scale difference for each possible similar patch
			{
				for (int l = 0; l < patchCols; l++)
				{
					gray_scale_difference_sum[i][j] += abs(grayCurrentPatch.at<uchar>(k, l) - grayTexturePic.at<uchar>(i + k, j + l)); // the gray scale difference
				}
			}
			minimumDifferenceLocationArray[j].row_num = i;
			minimumDifferenceLocationArray[j].col_num = j;
			minimumDifferenceLocationArray[j].pixelDifference = gray_scale_difference_sum[i][j];
		}

		sort(minimumDifferenceLocationArray, minimumDifferenceLocationArray + cols_need_be_compared, compare_minimumDifferenceLocationArray);
		if (minimumDifferenceLocationArray[0].pixelDifference < minimumDifferenceSum)
		{
			 minPixelRowPositon = minimumDifferenceLocationArray[0].row_num;
			 minPixelColPosition = minimumDifferenceLocationArray[0].col_num;
			 minimumDifferenceSum = minimumDifferenceLocationArray[0].pixelDifference;
		}
	}

	/*
	for (int i = 0; i < rows_need_be_compared; i++) // find the most similar patch in texture picture using Brute force
	{
		for (int j = 0; j < cols_need_be_compared; j++)
		{
			if (*min_element(gray_scale_difference_sum[i], gray_scale_difference_sum[i] + cols_need_be_compared) < minimumDifferenceSum)  // update currently the max similar patch coordinate
			{
				minimumDifferenceSum = *min_element(gray_scale_difference_sum[i], gray_scale_difference_sum[i] + cols_need_be_compared); // update minimum difference sum

				minPixelRowPositon = i;
				minPixelColPosition = distance(gray_scale_difference_sum[i], min_element(gray_scale_difference_sum[i], gray_scale_difference_sum[i] + cols_need_be_compared));
			}
		}
	}
	*/

	printf("rows at %d\n", minPixelRowPositon);
	printf("cols at %d\n", minPixelColPosition);

	Mat theSimilarPath;
	theSimilarPath.create(patchRows, patchCols, CV_8UC3);
	for (int i = 0; i < patchRows; i++) // create the most similar patch
	{
		for (int j = 0; j < patchCols; j++)
		{
			Vec3b RGB = texturePic.at<Vec3b>(minPixelRowPositon + i, minPixelColPosition + j);
			theSimilarPath.at<Vec3b>(i, j) = RGB;
		}
	}

	return theSimilarPath;
}

bool compare_minimumDifferenceLocationArray(MinimumDifferenceLocation const struct1, MinimumDifferenceLocation const struct2)
{
	return struct1.pixelDifference < struct2.pixelDifference;
}