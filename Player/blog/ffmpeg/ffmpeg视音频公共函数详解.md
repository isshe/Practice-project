#ffmpeg视音频公共函数详解

##库的介绍
* avcodec: 编解码（最重要的库）
* avformat：封装格式处理。
* avfilter： 滤镜特效处理。
* avdevice： 各种设备的输入输出。
* avutil： 工具库（大部分库都需要这个库的支持）
* postproc： 后加工。
* swresample： 音频采样数据格式转换。
* swscale： 视频像素数据格式转换。


##avcodec库
###avcodec_open2
所在文件：avcodec.h
原型：int avcodec_open2(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options);  
功能：用于初始化一个视音频编解码器的AVCodecContext.
参数：
* avctx：需要初始化的AVCodecContext。
* codec：输入的AVCodec
* options：一些选项。例如使用libx264编码的时候，“preset”，“tune”等都可以通过该参数设置。
更多：http://blog.csdn.net/leixiaohua1020/article/details/44117891 




