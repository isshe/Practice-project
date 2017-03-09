#SDL音频相关数据结构详解
##目录
###struct 
 * SDL_AudioSpec

###enum


##介绍
###SDL_AudioSpec
* 所在文件：SDL_audio.h
* 结构内容：
```
typedef struct SDL_AudioSpec
{
    int freq;                   /**< DSP frequency -- samples per second */
    SDL_AudioFormat format;     /**< Audio data format */
    Uint8 channels;             /**< Number of channels: 1 mono, 2 stereo */
    Uint8 silence;              /**< Audio buffer silence value (calculated) */
    Uint16 samples;             /**< Audio buffer size in samples (power of 2) */
    Uint16 padding;             /**< Necessary for some compile environments */
    Uint32 size;                /**< Audio buffer size in bytes (calculated) */
    SDL_AudioCallback callback; /**< Callback that feeds the audio device (NULL to use SDL_QueueAudio()). */
    void *userdata;             /**< Userdata passed to callback (ignored for NULL callbacks). */
} SDL_AudioSpec; 
```
* 结构成员：
    * freq 每秒钟发送给音频设备的sample frame的个数，通常是11025，220502，44100和48000。（sample frame = 样本精度 * 通道数）
    * fromat 每个样本占用的空间大小及格式，例如 AUDIO_S16SYS，样本是有符号的16位整数，字节顺序（大端还是小端）和系统一样。更多的格式可参考SDL_AudioFormat。
    * channels 通道数，在SDL2.0中支持1（mono），2（stereo），4（quad）和6（5.1）
    * samples 缓冲区的大小（ sample frame为单位）。（需要是2的次方？）
    * silence 音频数据中表示静音的值是多少
    * size 缓冲区的大小（字节为单位）
    * callback 音频设备缓冲区的回调函数，用来取音频数据给音频设备。原型为：
        * void (SDLCALL * SDL_AudioCallback) (void *userdata, Uint8 * stream,int len)
        * userdata和结构中的data关联。
        * stream需要填充的缓冲区。
        * 缓冲区大小。
        * 注意：回调函数中，使用stream时，需要先初始化缓冲区(SDL_memset).
    * userdata 在回调函数中使用的数据指针


###
* 所在文件：
* 结构功能：
* 结构内容：
* 结构成员：
* 更多

###
* 所在文件：
* 结构功能：
* 结构内容：
* 结构成员：
* 更多

###
* 所在文件：
* 结构功能：
* 结构内容：
* 结构成员：
* 更多

###
* 所在文件：
* 结构功能：
* 结构内容：
* 结构成员：
* 更多

###
* 所在文件：
* 结构功能：
* 结构内容：
* 结构成员：
* 更多


