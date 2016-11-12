#ffmpeg音频函数详解

##目录：
###SDL_AudioSpec
###AVSampleFormat
###


##结构详情
###SDL_AudioSpec 
```
typedef struct SDL_AudioSpec
{
    int freq;                   //每秒的样本数 		/**< DSP frequency -- samples per second */
    SDL_AudioFormat format;     //音频数据的格式	/**< Audio data format */
    Uint8 channels;             //通道数，1-单声道，2-立体	/**< Number of channels: 1 mono, 2 stereo */
    Uint8 silence;              //默认值？？？不懂！/**< Audio buffer silence value (calculated) */
    Uint16 samples;             //样本缓冲区大小	/**< Audio buffer size in samples (power of 2) */
    Uint16 padding;             //一些必要的编译环境？	/**< Necessary for some compile environments */
    Uint32 size;                //音频缓冲区字节数	/**< Audio buffer size in bytes (calculated) */
    SDL_AudioCallback callback; //回调函数
    void *userdata;				//用户数据
} SDL_AudioSpec;

```
* SDL_AudioFormat 是 Uint16的别名。
* SDL_AudioCallback 是一个函数指针。
* Uint8 是 uint8_t 的别名。
* Sint8 是 int8_t 的别名。

###
```AVSampleFormat
enum AVSampleFormat {
    AV_SAMPLE_FMT_NONE = -1,
    AV_SAMPLE_FMT_U8,          ///< unsigned 8 bits
    AV_SAMPLE_FMT_S16,         ///< signed 16 bits
    AV_SAMPLE_FMT_S32,         ///< signed 32 bits
    AV_SAMPLE_FMT_FLT,         ///< float
    AV_SAMPLE_FMT_DBL,         ///< double

    AV_SAMPLE_FMT_U8P,         ///< unsigned 8 bits, planar
    AV_SAMPLE_FMT_S16P,        ///< signed 16 bits, planar
    AV_SAMPLE_FMT_S32P,        ///< signed 32 bits, planar
    AV_SAMPLE_FMT_FLTP,        ///< float, planar
    AV_SAMPLE_FMT_DBLP,        ///< double, planar

    AV_SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
};
```