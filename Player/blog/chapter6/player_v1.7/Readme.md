
目标：
1. 实现快进、快退。


###一些知识：
* ffmpeg中定义：
``` 
#define AV_TIME_BASE 1000000
#define AV_TIME_BASE_Q (AVRational){1, AV_TIME_BASE}
#define AVSEEK_FLAG_BACKWARD 1 	//向后查找标志。
#define AVSEEK_FLAG_BYTE   2
#define AVSEEK_FLAG_ANY   4
#define AVSEEK_FLAG_FRAME   8
```

	
*函数av_rescale_q 
```
int64_t av_rescale_q(int64_t a, AVRational bq, AVRational cq)

Returns a * bq / cq.
```

* 函数av_seek_frame
```
int av_seek_frame(AVFormatContext *s, int stream_index, int64_t timestamp, int flags)

Seeks to the key frame at timestamp.
stream_index: If stream_index is -1, a default stream is selected, and timestamp is automatically converted from AV_TIME_BASE units to the stream specific time_base.

timestamp is measured in AVStream.time_base units or if there is no stream specified then in AV_TIME_BASE units.

flags: Set options regarding direction and seeking mode.
AVSEEK_FLAG_ANY: Seek to any frame, not just keyframes
AVSEEK_FLAG_BACKWARD: Seek backward
AVSEEK_FLAG_BYTE: Seeking based on position in bytes
```


