#SDL音视频公共数据结构详解
##目录
* 


##正文
###AVPacketList
* 所在文件：
* 结构功能：AVPcket的单链表结构
* 结构内容：
```
AVPacket pkt
AVPacketList * next
```
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

### SDL_Event
* 所在文件：
* 结构功能：SDL事件相关
* 结构内容：

|Uint32				| type  			|	event type, shared with all events|
|-------------------------------------|:-----------------------------|:-----|
|SDL_CommonEvent 	|common		|common event data|
|SDL_WindowEvent|window| window event data
SDL_KeyboardEvent|key|keyboard event data
SDL_TextEditingEvent|edit|text editing event data
SDL_TextInputEvent|text|text input event data
SDL_MouseMotionEvent|motion|mouse motion event data
SDL_MouseButtonEvent |button|mouse button event data
SDL_MouseWheelEvent|wheel|mouse wheel event data
SDL_JoyAxisEvent|jaxis|joystick axis event data
SDL_JoyBallEvent|jball|joystick ball event data
SDL_JoyHatEvent|jhat|joystick hat event data
SDL_JoyButtonEvent|jbutton|joystick button event data
SDL_JoyDeviceEvent|jdevice|joystick device event data
SDL_ControllerAxisEvent|caxis|game controller axis event data
SDL_ControllerButtonEvent|cbutton|game controller button event data
SDL_ControllerDeviceEvent|cdevice|game controller device event data
SDL_AudioDeviceEvent|adevice|audio device event data (>= SDL 2.0.4)
SDL_QuitEvent|quit|quit request event data
SDL_UserEvent|user|custom event data
SDL_SysWMEvent|syswm|system dependent window event data
SDL_TouchFingerEvent|tfinger|touch finger event data
SDL_MultiGestureEvent|mgesture|multi finger gesture data
SDL_DollarGestureEvent|dgesture|multi finger gesture data
SDL_DropEvent|drop|drag and drop event data|


* 结构成员介绍：
* 更多
