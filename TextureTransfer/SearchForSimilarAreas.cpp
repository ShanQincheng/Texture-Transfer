#include "stdafx.h"

Mat SearchForSimilarAreas(Mat currentPatch, Mat texturePic)
{
	int patchCols = currentPatch.cols; // the cols number of the patch is currently being compared
	int patchRows = currentPatch.rows; // the rows number of the patch is currently being compared
	int textureCols = texturePic.cols; // the cols number of texture picture
	int textureRows = texturePic.rows; // the rows number of texture picture
	int rows_need_be_compared = textureRows - patchRows; // max rows number of pixel rows pointer can reach
	int cols_need_be_compared = textureCols - patchCols; // max cols number of pixel cols pointer can reach\

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
		}
	}

	int minPixelRowPositon = 0;
	int minPixelColPosition = 0;
	int minimumDifferenceSum = INT_MAX;
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


TextureFingerprintStruct** GenerateTextureFingerprint(Mat texturePic, int patchRows, int patchCols)
{
	int textureCols = texturePic.cols; // the cols number of texture picture
	int textureRows = texturePic.rows; // the rows number of texture picture
	int rows_need_be_compared = textureRows - patchRows; // max rows number of pixel rows pointer can reach
	int cols_need_be_compared = textureCols - patchCols; // max cols number of pixel cols pointer can reach

	TextureFingerprintStruct **fingerprintStruct;  // contains the texture patch fingerprint for each possible reached pixel
	fingerprintStruct = (TextureFingerprintStruct**)calloc(rows_need_be_compared, sizeof(TextureFingerprintStruct*)); // calloc memory space for gray_scale_difference array
	for (int i = 0; i < rows_need_be_compared; i++)
	{
		fingerprintStruct[i] = (TextureFingerprintStruct*)calloc(cols_need_be_compared, sizeof(TextureFingerprintStruct));
	}
	for (int i = 0; i < rows_need_be_compared; i++) // initialize gray_scale_difference array
	{
		for (int j = 0; j < cols_need_be_compared; j++)
		{
			fingerprintStruct[i][j].coordinate_x = 0;
			fingerprintStruct[i][j].coordinate_y = 0;
			fingerprintStruct[i][j].fingerprint = 0.0;
		}
	}

	for (int i = 0; i < rows_need_be_compared; i++)
	{
		for (int j = 0; j < cols_need_be_compared; j++)
		{
			Mat currentPatch, currentPatchResize, currentPatchResizeGray;
			currentPatch.create(patchRows, patchCols, CV_8UC3);
			currentPatchResize.create(8, 8, CV_8UC3);
			for (int k = 0; k < patchRows; k++)  // calculate the sum of the gray scale difference for each possible similar patch
			{
				for (int l = 0; l < patchCols; l++)
				{
					Vec3b RGB = texturePic.at<Vec3b>(k + i, l + j);
					currentPatch.at<Vec3b>(i, j) = RGB;
				}
			}
			resize(currentPatch, currentPatchResize, currentPatchResize.size(), 0, 0, CV_INTER_LINEAR); // resize to 8 * 8 picture
			cvtColor(currentPatchResize, currentPatchResizeGray, CV_BGR2GRAY, 0);  // colored currentPatchResize patch was transferred to gray patch

			int pixelsValue = 0, averagePixelsValue = 0;;
			for (int y = 0; y < 8; y++)
			{
				for (int z = 0; z < 8; z++)
				{
					pixelsValue += currentPatchResizeGray.at<uchar>(y, z);
				}
			}
			averagePixelsValue = pixelsValue / 64;
			//char fingerprint[64];
			double fingerprint = 0.0;
			for (int y = 0; y < 8; y++)
			{
				for (int z = 0; z < 8; z++)
				{
					if (currentPatchResizeGray.at<uchar>(y, z) > averagePixelsValue)
						//fingerprint[y * 8 + z] = 1;
						fingerprint = fingerprint * 2 + 1;
					else
						//fingerprint[y * 8 + z] = 0;
						fingerprint = fingerprint * 2 + 0;
				}
			}
			fingerprintStruct[i][j].coordinate_x = i;
			fingerprintStruct[i][j].coordinate_y = j;
			//strcpy(fingerprintStruct[i][j].fingerprint, fingerprint);
			fingerprintStruct[i][j].fingerprint = fingerprint;
		}
	}

	sort(fingerprintStruct, (fingerprintStruct + rows_need_be_compared * cols_need_be_compared), compare_fingerprint);

	return fingerprintStruct;
}

bool compare_fingerprint(TextureFingerprintStruct* const struct1, TextureFingerprintStruct* const struct2)
{
	//return struct1.fingerprint < struct2.fingerprint;
	return struct1->fingerprint < struct2->fingerprint;
}

Mat SearchForSimilarAreasOptimization(Mat currentPatch, Mat texturePic)
{
	Mat mat_null;

	return mat_null;
}