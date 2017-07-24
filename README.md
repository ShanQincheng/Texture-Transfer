### Run command
### .exe -i inputFileName -o outputFileName -p patchSize -r overlapping region size -t texture picture

### 如果 Github 显示效果不佳，请移步 http://codingstory.net/shi-yong-opencv-shi-xian-wen-li-zhuan-yi-texture-transfer/


***

# 前言
> A big THANKS to my "Analysis of Algorithms" course teacher, 張元翔

這是“演算法分析”課程第二次機測，具體要求如下
![](http://www.codingstory.net/content/images/2017/07/QQ--20170723163350.png)
![](http://www.codingstory.net/content/images/2017/07/QQ--20170723163413.png)
![](http://www.codingstory.net/content/images/2017/07/QQ--20170723163436.png)

***

**三個參考鏈接**
##### [使用 OpenCV 操作圖像](http://codingstory.net/opencv-shi-xian-tu-xiang-de-du-qu-xian-shi-cun-chu-jian-dan-pin-jie/)
##### [使用 OpenCV 實現紋理合成(Texture Synthesis)](http://codingstory.net/shi-yong-opencv-shi-xian-wen-li-he-cheng-texture-synthesis/)
##### [源程式碼已上傳 Github](https://github.com/ShanQincheng/Texture-Transfer)

***
# 環境
```
OS                                    win10, x64
IDE                                   Visual Studio 2017
```

***

# 開始
##### 1、思路
* ( 1 ) 切割圖像
* ( 2 ) 在 texture picture 中尋找紋理最相似(*灰階差值最小*)的 patch
* ( 3 ) 水平拼貼
* ( 4 ) 垂直拼貼
* ( 5 ) 重複 3 4 步驟得到最終圖像

##### 2、如何在 texture picture 中尋找紋理最相似的 patch
* ( 1 ) 想像在 texture picture 中切割出 N 塊**尺寸相同**的 patch。
* ( 2 ) 在 texture picture 將 N 塊 patch 遍歷一遍, 計算每一個 patch 與原圖像 patch 的灰階差值。
* ( 3 ) 在 texture picture 中找出紋理最相似(*灰階差值最小*)的 patch 用於拼貼

##### 3、重點
* ( 1 ) patch size 和 overlapping region size 需要適時調整
* ( 2 ) 加速尋找最相似 patch


***

### 思路
#### 1、切割圖像
以規則的網狀結構切割圖像,將每一個網孔看作一個 patch
![](http://www.codingstory.net/content/images/2017/07/Texture_Transfer.png)

#### 2、找紋理最接近的 patch
以圖像左上角的 patch 為例，在 texture picture 中找到與圖像左上角 patch 紋理最接近的 patch ( 注意, 尋找的 patch 長寬都與圖像左上角 patch 相同, 即網孔的長寬。)
![](http://www.codingstory.net/content/images/2017/07/QQ--20170723180300.png)
**把找到的第一塊紋理最相似的 patch, 看作最終生成圖的基礎圖像**

#### 3、水平拼貼
同理，再從 texture picture 中找出與圖像左上角第二個 patch 紋理最接近的 patch
![](http://www.codingstory.net/content/images/2017/07/QQ--20170723180.png)
把從 texture picture 中找到的第二塊紋理最接近的 patch 與找到的第一塊紋理最接近的 patch **(基礎圖像)**,使用水平拼貼貼合,貼合後成為新的，**一塊更大的基礎圖像**(==*馬賽克部分表示還未生成的區域*== )
![](http://www.codingstory.net/content/images/2017/07/QQ--20170724132355.png)
以此類推，找到 N 個紋理最接近的 patch, 水平拼貼出第一條圖像**作為基礎圖像**
![](http://www.codingstory.net/content/images/2017/07/QQ--20170724143011.png)

#### 4、垂直拼貼
換行，同理找到 N 個紋理最接近的 patch, 水平拼貼出第二條圖像。
![](http://www.codingstory.net/content/images/2017/07/QQ--20170724151800.png)
將這兩行 patch, 使用垂直拼貼，得到更大的基礎圖像
![](http://www.codingstory.net/content/images/2017/07/QQ--20170724152119.png)

#### 5、重複 3 4 步驟得到最終圖像
水平拼貼 patch 得到一條紋理最接近的圖像, 再用垂直拼貼得到一塊最接近的圖像。反复操作，得到最終的圖像。
![](http://www.codingstory.net/content/images/2017/07/QQ--20170724152717.png)

***

### 如何在 texture picture 中尋找最相似的 patch
###### 以原圖像左上角第一個 patch 為例
![](http://www.codingstory.net/content/images/2017/07/QQ--20170723180300.png)
###### ( 1 ) 在 texture picture 的左上角，切割出**尺寸相同**的一個 patch。
![](http://www.codingstory.net/content/images/2017/07/QQ--20170724170242.png)

###### ( 2 ) 將兩個 patch 轉為灰階圖像
```language-python line-numbers
//The conversion from a BGR image to gray is done with:

cvtColor(src, bwsrc, CV_BGR2GRAY);
```

###### ( 3 )計算灰階差值
按照像素位置一一對應的關係，一對一對地相減，結果取絕對值，**此為一對像素點的灰階差值**。然後遍歷所有像素點，將所有像素點的灰階差值加總(*此為兩個 patch 的灰階差值*)

###### 在 texture picture 中計算所有可能的 patch, 找出灰階差值最小的那一塊用于拼貼
```language-python line-numbers
int minPixelRowPositon = 0;
	int minPixelColPosition = 0;
	int minimumDifferenceSum = INT_MAX;
	int speedupValue = 3;
	MinimumDifferenceLocation* minimumDifferenceLocationArray = (MinimumDifferenceLocation*)calloc(cols_need_be_compared, sizeof(MinimumDifferenceLocation));
	for (int i = 0; i < rows_need_be_compared; i += speedupValue)
	{
		for (int j = 0; j < cols_need_be_compared; j ++)
		{
			for (int k = 0; k < patchRows; k += speedupValue)  // calculate the sum of the gray scale difference for each possible similar patch
			{
				for (int l = 0; l < patchCols; l += speedupValue)
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
```

***


### 重點
###### patch size 和 overlapping region size 需要適時調整
1、patch size, 和 overlapping region size( *兩張圖片拼接在一起時重疊區域寬度* ) 是人為設定的。patch size 越小,圖片越精細。overlapping region size (*< patch size*)越大, 圖片越精細。**生成的圖片越精細，所花費的計算時間長很多很多**

2、因為圖像的行數和列數不一定剛好能夠整除用戶設定的 patch size。當在圖片最右方和最下方取樣時，不可避免的需要按照剩下的圖像寬度進行調整, 以便最終生成的圖像列數和行數與原圖像相同。

###### 做法有兩種。

( 1 ) 改變 patch size 之後再去 texture picture 搜尋紋理最接近的 patch。
 
( 2 ) 改變 overlapping region size, 拼貼時多重疊一些。

![](http://www.codingstory.net/content/images/2017/07/QQ--20170724155721.png)

###### 加速尋找最相似 patch
( 1 ) 減少取樣的個數，即是在 Texture 影像中任意座標取固定 Patch 張數 (例如: 1000張)，取其中最佳匹配者；

( 2 ) 減少比對的總像素，例如：每隔 2 列 2 行像素才計算 RGB 差異，換言之，每 4 x 4 小格僅計算其中1個像素的 RGB 差異。

***
# 以上
2017 年 7 月 24 日
