#ffmpeg视音频公共数据结构详解
##目录


##内容

###AVFormatContext
* 所在文件：avformat.h
* 结构功能：封装格式上下文结构体,也是统领全局的结构体,保存了视频文件封装格式相关信息.
* 结构内容：
```
typedef struct AVFormatContext {  
    const AVClass *av_class;  
    struct AVInputFormat *iformat;  
    struct AVOutputFormat *oformat;  
    void *priv_data;  
    AVIOContext *pb;  
 
    int ctx_flags; /**< Format-specific flags, see AVFMTCTX_xx */  
    unsigned int nb_streams;  
    AVStream **streams;  
  
    char filename[1024]; /**< input or output filename */   
    int64_t start_time;  
    int64_t duration;  
    int bit_rate;  
    unsigned int packet_size;  
    int max_delay;  
    ......
     
```
* 结构成员介绍：(主要，常用）
    * iformat：输入数据的封装格式
    * pb：输入数据的缓存
    * nb_streams：视音频流的个数
    * streams：视音频流 (注意是一个二重指针）
    * filename[1024]：文件名
    * duration：时长（单位：微秒us，转换为秒需要除以1000000）
    * bit_rate：比特率（单位bps，转换为kbps需要除以1000）
    * metadata：元数据
* 更多:

###AVInputFormat
* 所在文件：
* 结构功能：每种封装格式(例如FLV, MKV, MP4, AVI)对应一个该结构体。
* 结构内容：
```
typedef struct AVInputFormat {
    const char *name;
    const char *long_name;
    int flags;
    const char *extensions;
    const struct AVCodecTag * const *codec_tag;
    const AVClass *priv_class; ///< AVClass for the private context
    const char *mime_type;
    struct AVInputFormat *next;
    int raw_codec_id;
    int priv_data_size;
    ...还有好多...

```
* 结构成员介绍：(主要）
    * name：格式名称，例如：flac
    * long_name: 也是格式名称， 例如：raw FLAC
    ...
* 更多:

###AVStream
* 所在文件：
* 结构功能：视频文件中每个视频(音频)流对应一个该结构体。
* 结构内容：
* 结构成员介绍：
* 更多

###AVCodecContext
* 所在文件：
* 结构功能：编码器上下文结构体,保存了视频(音频)编解码相关信息。
* 结构内容：
* 结构成员介绍：
* 更多

###AVCodec
* 所在文件：
* 结构功能：每种视频(音频)编解码器(例如H.264解码器)对应一个该结构体。
* 结构内容：
* 结构成员介绍：
* 更多

###AVPacket
* 所在文件：
* 结构功能：存储一帧压缩编码数据
* 结构内容：
* 结构成员介绍：
* 更多

###AVFrame
* 所在文件：
* 结构功能：存储一帧解码后像素(采样)数据
* 结构内容：
* 结构成员介绍：
* 更多

###
* 所在文件：
* 结构功能：
* 结构内容：
* 结构成员介绍：
* 更多

###
* 所在文件：
* 结构功能：
* 结构内容：
* 结构成员介绍：
* 更多


