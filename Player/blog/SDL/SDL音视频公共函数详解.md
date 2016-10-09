
###SDL_AddTimer
* 所在文件：timer.h
* 原型：
```
SDL_TimerID SDL_AddTimer(Uint32            interval,
                         SDL_TimerCallback callback,
                         void*             param)
```
* 功能：在interval毫秒以后, 开一个独立新线程运行callback。（1s = 1000ms)
* 参数：
	* interval: 时间间隔。
	* callback：uint32_t callback(uint32_t interval, void *param)
		* 当callback**返回0停止timer；返回 > 0, 继续以返回值为时间间隔调用callback.**
		* 两个参数对应SDL_AddTimer的两个参数。
* 返回值：
	* 0: 错误.
	* >0 : timer_ID
* 相关:
* 更多：



###SDL_CreateThread
* 所在文件：SDL_thread.h
* 原型：
```
SDL_Thread* SDL_CreateThread(SDL_ThreadFunction fn,
                             const char*        name,
                             void*              data)
```
* 功能：新建一个线程.
* 参数：
	* fn: 	开线程运行的函数的函数名
	* name: 给线程起个名字。
	* data: 数据。（fn的参数）
* 返回值：NULL或者一个指向线程的指针。
* 相关:SDL_WaitThread().
* 更多：

###
* 所在文件：
* 原型：
* 功能：
* 参数：
* 返回值：
* 相关:
* 更多：

