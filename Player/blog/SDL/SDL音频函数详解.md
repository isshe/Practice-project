#SDL音频相关函数详解

##目录
* SDL_OpenAudioDevice



##介绍

###SDL_GetAudioDeviceName
所在文件：SDL_audio.h
原型：const char *SDL_GetAudioDriver(int index)
功能：
参数：
更多：

###SDL_OpenAudioDevice
所在文件：SDL_audio.h
原型：SDL_AudioDeviceID SDL_OpenAudioDevice(const char *device, int iscapture, const SDL_AudioSpec * desired, SDL_AudioSpec * obtained, int allowed_changes)
功能：打开音频设备。打开音频设备后，会为callback函数单独开启一个线程，不断将音频发送给音频设备播放。
参数：
    * iscapture: 设为0，非0的值在当前SDL2版本还不支持。
    * desired: 期望得到的音频输出格式。
    * obtained: 实际的输出格式。
    * allowed_changes: 当期望和实际的不一样时，能不能够对某一些输出参数进行修改。
        * 设为0，则不能修改。设为如下的值，则可对相应的参数修改：
        * SDL_AUDIO_ALLOW_FREQUENCY_CHANGE
        * SDL_AUDIO_ALLOW_FORMAT_CHANGE
        * SDL_AUDIO_ALLOW_CHANNELS_CHANGE
        * SDL_AUDIO_ALLOW_ANY_CHANGE
更多：

###SDL_OpenAudio
所在文件：SDL_audio.h
原型：int SDL_OpenAudio(SDL_AudioSpec * desired, SDL_AudioSpec * obtained)
功能：打开音频设备。
参数：
更多：

###SDL_MixAudio
所在文件：
原型：
```
void SDL_MixAudio(Uint8*       dst,
                  const Uint8* src,
                  Uint32       len,
                  int          volume)
```
功能：混合音频
参数：
    * dst: 目的音频。
    * src: 原音频。
    * len：音频缓冲区的字节数(Bytes)
    * volume：范围是0-128， 一般设置SDL_MIX_MAXVOLUME
更多：

###
所在文件：
原型：
功能：
参数：
更多：

###
所在文件：
原型：
功能：
参数：
更多：

###
所在文件：
原型：
功能：
参数：
更多：

###
所在文件：
原型：
功能：
参数：
更多：
