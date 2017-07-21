// TextureTransfer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int main(int argc, char** argv)
{
	string infileName, texturefileName, outfileName;
	int patchSize = -1;
	int overlappingSize = -1;

	for (int i = 0; i < 9; i++)
	{
		if (strcmp("-i", argv[i + 1]) == 0)
		{
			infileName = argv[i + 2];

			i ++;
		}
		else if (strcmp("-o", argv[i + 1]) == 0)
		{
			outfileName = argv[i + 2];
			i++;
		}
		else if (strcmp("-p", argv[i + 1]) == 0)
		{
			patchSize = atoi(argv[i + 2]);
			i++;
		}
		else if (strcmp("-r", argv[i + 1]) == 0)
		{
			overlappingSize = atoi(argv[i + 2]);
			i++;
		}
		else if (strcmp("-t", argv[i + 1]) == 0)
		{
			texturefileName = argv[i + 2];                                                                                                                                                                                                                                                                                                                         
			i++;
		}
		else {
			exit(0);
		}
	}
	
	/*
	infileName = "Texture_Transfer.bmp";
	texturefileName = "Texture_Rice.bmp";
	patchSize = 2;
	overlappingSize = 1;
	*/
	Mat pic = imread(infileName);
	Mat texturePic = imread(texturefileName);
	Mat rowsPatch, rowsTempPatch, transferPic, transferTempPic;
	
	int stride = patchSize - overlappingSize;
	int rowsPos = 0, colsPos = 0;



	
	for(int w = 0; w < pic.rows; w += stride) // makeup picture by vertical texture synthesis 
	{
		int rangeRow = patchSize;
		if (w + patchSize > pic.rows) // handle the bottom small row picture
		{
			rangeRow = pic.rows - w;
		}
		Mat colsPatch, colsTempPatch;
		for (int i = 0; i < pic.cols; i += stride)  // makeup picture by horizontal texture synthesis
		{
			int rangeCol = patchSize;
			Mat patch, similarPatch;

			if (i + patchSize > pic.cols) // handle rightmost small col picture
			{
				rangeCol = pic.cols - i;
			}
			patch.create(rangeRow, rangeCol, CV_8UC3);
			for (int j = 0; j < rangeRow; j++) // create the patch to be found in texture picture
			{
				for (int k = 0; k < rangeCol; k++)
				{
					Vec3b RGB = pic.at<Vec3b>(w + j, i + k);
					patch.at<Vec3b>(j, k) = RGB;
				}
			}
			similarPatch = SearchForSimilarAreas(patch, texturePic).clone(); // find the most similar patch in texture picture
			if (i == 0)
			{
				colsPatch = similarPatch.clone();
			}
			else { // synthesis the similar patch to the big line picture by horizontal texture synthesis
				if (rangeCol > overlappingSize)
					colsTempPatch = Horizontal(colsPatch, similarPatch, overlappingSize).clone();
				else {
					break;
				}
				/*
					from one patch to one line picture according to one by one 
					the most similar patch was found and then texture synthesis
				*/	
				colsPatch = colsTempPatch.clone();  
			}
		}

		if (w == 0) // synthesis one line picture to the big picture
		{
			transferPic = colsPatch.clone();
		}
		else {
			if(rangeRow > overlappingSize)
				transferTempPic = Vertical(transferPic, colsPatch, overlappingSize).clone();
			else {
				break;
			}
				
			transferPic = transferTempPic.clone();
		}
	}
	
	namedWindow("picture window", WINDOW_AUTOSIZE);
	namedWindow("texturePic window", WINDOW_AUTOSIZE);	
	namedWindow("transferPic window", WINDOW_AUTOSIZE);

	imshow("picture window", pic);
	imshow("texturePic window", texturePic);
	imshow("transferPic window", transferPic);

	imwrite(outfileName, transferPic);

	waitKey(0);
    return 0;
}

