
###sws_getContext
* 所在文件：swscale.h
* 原型：
```
struct SwsContext *sws_getContext(int srcW, int srcH, enum AVPixelFormat srcFormat,
                                  int dstW, int dstH, enum AVPixelFormat dstFormat,
                                  int flags, SwsFilter *srcFilter,
                                  SwsFilter *dstFilter, const double *param);
```
* 功能：
	* 分配并返回一个SwsContext. 
	* You need it to perform scaling/conversion operations using sws_scale().
* 参数：
	* srcW: 原图像的宽。
	* srcH：原图像的高。
	* srcFormat: 原图像的格式。
	* dstW：目的图像的宽。
	* dstH：目的图像的高。
	* dstFormat: 目的图像的格式。
	* flags：指定选项（specify which algorithm and options to use for rescaling）
	* param: 额外的数据。
* 返回值：指向SwsContext的指针或者NULL.
* 相关:
* 更多：


###sws_scale
* 所在文件：swscale.h
* 原型：
```
int sws_scale(struct SwsContext *c, const uint8_t *const srcSlice[],
              const int srcStride[], int srcSliceY, int srcSliceH,
              uint8_t *const dst[], const int dstStride[]);
```
* 功能：
	* 把srcslice里的图像片段按比例处理后，[存到]dstSlice中。不大理解这个函数。和sws_getContext一样不够理解。
	* 片段表示一副图片中一部分连续的行。
* 参数：如名。
* 返回值：输入的片段的高。
* 相关:
* 更多：
/**
 * Scale the image slice in srcSlice and put the resulting scaled
 * slice in the image in dst. A slice is a sequence of consecutive
 * rows in an image.
 *
 * Slices have to be provided in sequential order, either in
 * top-bottom or bottom-top order. If slices are provided in
 * non-sequential order the behavior of the function is undefined.
 *
 * @param c         the scaling context previously created with
 *                  sws_getContext()
 * @param srcSlice  the array containing the pointers to the planes of
 *                  the source slice
 * @param srcStride the array containing the strides for each plane of
 *                  the source image
 * @param srcSliceY the position in the source image of the slice to
 *                  process, that is the number (counted starting from
 *                  zero) in the image of the first row of the slice
 * @param srcSliceH the height of the source slice, that is the number
 *                  of rows in the slice
 * @param dst       the array containing the pointers to the planes of
 *                  the destination image
 * @param dstStride the array containing the strides for each plane of
 *                  the destination image
 * @return          the height of the output slice
 */

###
* 所在文件：
* 原型：
* 功能：
* 参数：
* 返回值：
* 相关:
* 更多：
