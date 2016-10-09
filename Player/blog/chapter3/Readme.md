* 一些主要的函数（重要）：
	* av_register_all(): 注册所有组件。
	* avformat_open_input(): 打开输入文件。
	* avformat_find_stream_info(): 获取视频文件信息。
	* avcodec_find_decoder(): 查找解码器。
	* avcodec_open2(): 打开解码器。
	* av_read_frame(): 从输入文件读取一帧**压缩数据**。
	* avcodec_decode_video2(): 解码一帧压缩数据。
	* avcodec_close(): 关闭解码器。
	* avformat_close_input(): 关闭输入视频文件。
