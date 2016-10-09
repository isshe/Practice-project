#ffmpeg音频相关函数详解

##目录
###swr_alloc_set_opts



##内容

###av_samples_get_buffer_size
所在文件：samplefmt.h
原型：
```
/**
 * Get the required buffer size for the given audio parameters.
 *

 */
int av_samples_get_buffer_size(int *linesize, int nb_channels, int nb_samples,
                               enum AVSampleFormat sample_fmt, int align);

```
功能：获取给定的音频参数的缓冲区大小。
参数：
* @param[out] linesize calculated linesize, may be NULL
* @param nb_channels   the number of channels
* @param nb_samples    the number of samples in a single channel
* @param sample_fmt    the sample format
* @param align         buffer size alignment (0 = default, 1 = no alignment)
* @return              required buffer size, or negative error code on failure
返回值：
* fail:
* succ: 
相关:
更多：


###swr_convert
* 所在文件：swresample.h
* 原型：
```
/** 
 *
 * in and in_count can be set to 0 to flush the last few samples out at the
 * end.
 *
 * If more input is provided than output space, then the input will be buffered.
 * You can avoid this buffering by using swr_get_out_samples() to retrieve an
 * upper bound on the required number of output samples for the given number of
 * input samples. Conversion will run directly without copying whenever possible.
 *
 * @param s         allocated Swr context, with parameters set
 * @param out       output buffers, only the first one need be set in case of packed audio
 * @param out_count amount of space available for output in samples per channel
 * @param in        input buffers, only the first one need to be set in case of packed audio
 * @param in_count  number of input samples available in one channel
 *
 * @return number of samples output per channel, negative value on error
 */
int swr_convert(struct SwrContext *s, uint8_t **out, int out_count,
                                const uint8_t **in , int in_count);
```
* 功能：
	* 转换音频(Convert audio)
* 参数：
* 返回值：
	* fail:
	* succ: 
* 相关:
* 更多：

###
* 所在文件：
* 原型：
* 功能：
* 参数：
* 返回值：
	* fail:
	* succ: 
* 相关:
* 更多：


