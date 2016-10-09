#目标达成
实现了视音频同步。

任务：
1. 使声音和视频可以同时播放。
2. 音视频同步


遇到问题：
1. video packet 解码出来的frame本来是存到一个队列中的，但是，却在put队列的时候出现覆盖的现象。一直找不到原因（并不知道覆盖了）。才知道原来ffmpeg在分内存的时候，有什么共享内存之类的情况。

2. audio_clock 老是获取不对。

博客：

需要解决的问题：
1. 如何获取音频的当前时间作为基准。
2. 如何获取视频的frame的pts，对照基准，判断快或慢？


巧妙的地方：
1. 因为一个音频的packet可能含有多个frame，
	故而把packet.pts+这段缓冲区数据能播放的时候（用图示）。
	再获取的时候，就减。
	
	
	
	
	
1. 获取音频当前时间：
```
     if (packet.pts != AV_NOPTS_VALUE)
     {
          ps->audio_clock = packet.pts * av_q2d(ps->paudio_stream->time_base);
     }
```
