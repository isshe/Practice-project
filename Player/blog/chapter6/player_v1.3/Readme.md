重写!

遇到的问题：
1. 视频播放断断续续。

2. //如果先入队，再使用这个函数处理，显示会不对，不明白为什么
     	  sws_scale(ps->psws_ctx, (uint8_t const *const *)pframe->data,
             	pframe->linesize, 0, ps->pixel_h,
             	ps->out_frame.data, ps->out_frame.linesize);   

