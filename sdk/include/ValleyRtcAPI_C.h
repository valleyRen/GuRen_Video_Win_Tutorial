#pragma once
 
#define __VALLEYAPI extern "C"

#define ERR_SUCCEED   0
#define ERR_NOT_INIT  -10001   // 未初始化
#define ERR_INVOKE_ORDER -10002  // 调用顺序错误

#define IID_RTCMSGR 0x10 
#define RESERVE_CHANNEL_ATTR_CONFIG "##INNER##CHANNEL_SATTR_CONFIG"  //设置房间内部属性，保留用来设置内部服务器

//登录状态
enum 
{
	LOGIN_STATUS_NONE = 0,        // 未登录
	LOGIN_STATUS_LOGINING = 1,    // 登录中
	LOGIN_STATUS_LOGINED = 2,     // 已经登录
};

enum MSGTYPE
{
	msgtypeText = 1,		 // 文本消息, SDK不改消息原始内容，只是建议各个平台统一采用utf8格式，以避免调用SDK的双方不统一形成乱码
	msgtypeAudio = 2,		 // 语音消息
	msgtypeBinary = 3,       // 二进制消息
	msgtypeCmd = 10,         // 命令消息，不在服务器上存储，仅仅转发
};


#ifndef VIDEO_CAPTURE_TYPE_16X9  
#define VIDEO_CAPTURE_TYPE_16X9         ( 0x000 ) 
// Image resolution
#define VIDEO_SIZE_160                  ( 1 )
#define VIDEO_SIZE_320                  ( 2 )
#define VIDEO_SIZE_480                  ( 3 )
#define VIDEO_SIZE_640                  ( 4 )
#define VIDEO_SIZE_800                  ( 5 )
#define VIDEO_SIZE_960                  ( 6 )
#define VIDEO_SIZE_1280                 ( 8 )
#define VIDEO_SIZE_1920                 ( 0xf ) 

#define VIDEO_CAPTURE_TYPE_16X9_160     ( VIDEO_CAPTURE_TYPE_16X9+VIDEO_SIZE_160 )   // 160*120
#define VIDEO_CAPTURE_TYPE_16X9_320     ( VIDEO_CAPTURE_TYPE_16X9+VIDEO_SIZE_320 )
#define VIDEO_CAPTURE_TYPE_16X9_480     ( VIDEO_CAPTURE_TYPE_16X9+VIDEO_SIZE_480 )
#define VIDEO_CAPTURE_TYPE_16X9_640     ( VIDEO_CAPTURE_TYPE_16X9+VIDEO_SIZE_640 )
#define VIDEO_CAPTURE_TYPE_16X9_800     ( VIDEO_CAPTURE_TYPE_16X9+VIDEO_SIZE_800 )
#define VIDEO_CAPTURE_TYPE_16X9_960     ( VIDEO_CAPTURE_TYPE_16X9+VIDEO_SIZE_960 )
#define VIDEO_CAPTURE_TYPE_16X9_1280    ( VIDEO_CAPTURE_TYPE_16X9+VIDEO_SIZE_1280 )   // 1280*720
#define VIDEO_CAPTURE_TYPE_16X9_1920    ( VIDEO_CAPTURE_TYPE_16X9+VIDEO_SIZE_1920 )
 
#define VIDEO_CAPTURE_TYPE_4X3          ( 0x100 )
#define VIDEO_CAPTURE_TYPE_4X3_160      ( VIDEO_CAPTURE_TYPE_4X3+VIDEO_SIZE_160 )   // 160*120
#define VIDEO_CAPTURE_TYPE_4X3_320      ( VIDEO_CAPTURE_TYPE_4X3+VIDEO_SIZE_320 )
#define VIDEO_CAPTURE_TYPE_4X3_480      ( VIDEO_CAPTURE_TYPE_4X3+VIDEO_SIZE_480 )
#define VIDEO_CAPTURE_TYPE_4X3_640      ( VIDEO_CAPTURE_TYPE_4X3+VIDEO_SIZE_640 )
#define VIDEO_CAPTURE_TYPE_4X3_800      ( VIDEO_CAPTURE_TYPE_4X3+VIDEO_SIZE_800 )
#define VIDEO_CAPTURE_TYPE_4X3_960      ( VIDEO_CAPTURE_TYPE_4X3+VIDEO_SIZE_960 )
#define VIDEO_CAPTURE_TYPE_4X3_1280     ( VIDEO_CAPTURE_TYPE_4X3+VIDEO_SIZE_1280 )   // 1280*720
#define VIDEO_CAPTURE_TYPE_4X3_1920     ( VIDEO_CAPTURE_TYPE_4X3+VIDEO_SIZE_1920 )   // 1280*720

#define VIDEO_CAPTURE_TYPE_16X16        ( 0x200 )  
#define NO_ROTATE_TYPE_90               ( 1 )

#endif //VIDEO_CAPTURE_TYPE_16X9
 

typedef void* handle_t;
typedef long long seond_t;

// 事件基类
struct objBaseEvent{   
	const void* memory;
	int evttype; 
};


// 通知事件基类
struct objNtfEvent :objBaseEvent{     
	#define RTC_EVT_NTF_FROM   1000
};

// 响应事件基类，响应事件都带错误码，ec=0标识成功，ec!=0标识失败，具体失败见错误描述
struct objRespEvent :objBaseEvent{    
	#define RTC_EVT_RESP_FROM  0 
	int ec;
};

//响应事件 收到用户登录成功或失败的应答
struct objRespLogin: objRespEvent{     
#define RTC_RESP_LOGINED (RTC_EVT_RESP_FROM+1)
	const char* userid;
};

//通知事件 收到用户进入房间
struct objNtfUserEnter : objNtfEvent{  
#define RTC_NTF_USER_ENTER (RTC_EVT_NTF_FROM+1)
	const char* userid; 
};

//通知事件 收到用户离开房间
struct objNtfUserLeave : objNtfEvent{ 
#define RTC_NTF_USER_LEAVE (RTC_EVT_NTF_FROM+2)
	const char* userid; 
};

//响应事件 收到发消息成功或失败的应答
struct objRespMsg : objRespEvent{
#define RTC_RESP_SEND_MSG (RTC_EVT_RESP_FROM+50) 
	const char* touserid;
	const char* data;
	int         datalen;
	const char* token;
	int         msgtype;
	seond_t     msgtime; 
};

//通知事件 收到用户发来的消息
struct objNtfMsg : objNtfEvent{
#define RTC_NTF_RECV_MSG (RTC_EVT_NTF_FROM+50)
	const char* fromuserid;
	const char* touserid;
	const char* data;
	int         datalen;
	const char* token;
	int         msgtype;
	seond_t     msgtime;
};

//通知事件 收到网络断开的消息
struct objNtfNetLost : objNtfEvent{
#define RTC_NTF_NET_LOST (RTC_EVT_NTF_FROM+80) 
};

//通知事件 收到网络重新连上的消息
struct objNtfNetResume : objNtfEvent{
#define RTC_NTF_NET_RESUME (RTC_EVT_NTF_FROM+81) 
};

//通知事件 收到账号异地登录的消息
struct objNtfDupLogined : objNtfEvent{
#define RTC_NTF_DUP_LOGINED (RTC_EVT_NTF_FROM+82) 
};



__VALLEYAPI bool         Valley_InitSDK(const char* workfolder, const char* localconfig); //初始化SDK,一个进程只需要初始化一次
__VALLEYAPI void         Valley_CleanSDK();       //初始化SDK,一个进程只需要初始化一次
__VALLEYAPI const char*	 Valley_GetSDKVersion();  //获取SDK版本
__VALLEYAPI const char*	 Valley_GetErrDesc(int ec);     //获取错误码对应的描述

__VALLEYAPI handle_t     Valley_CreateChannel(bool withVideo);  //创建一个房间，是否需要支持视频
__VALLEYAPI void         Valley_Release(handle_t s);  //销毁一个房间

__VALLEYAPI int          Valley_EnableInterface(handle_t s, int iids);  //房间对象有效

__VALLEYAPI objBaseEvent* Valley_GetEvent(handle_t s);           //获取事件对象支持，不会阻塞，没有事件返回空支持，有事件返回事件结构支持，需要调用Valley_ReleaseEvent释放事件对象指针
__VALLEYAPI void          Valley_ReleaseEvent(objBaseEvent* e);  //释放事件对象指针

__VALLEYAPI int          Valley_Login(handle_t s, const char* channelid, const char* userid); //登录房间
__VALLEYAPI void         Valley_Logout(handle_t s);        //退出房间
__VALLEYAPI int          Valley_GetLoginStatus(handle_t s);  //获取登录状态

__VALLEYAPI int          Valley_SetChannelAttr(handle_t s, const char* name, const char* value);  // 设置房间属性
__VALLEYAPI int          Valley_SendMsgr(handle_t s, int msgtype, const char* data, int len, const char* token, const char* toUserID);  // 发送消息

__VALLEYAPI int          Valley_SetVideoProfile(handle_t s, int profile);   // 设置本地视频属性

__VALLEYAPI int          Valley_SetLocalVideo(handle_t s, handle_t hVideo);   // 设置本地视频显示窗口
__VALLEYAPI int          Valley_RemoveLocalVideo(handle_t s);   // 关闭本地视频显示

__VALLEYAPI int          Valley_SetUserVideo(handle_t s, const char* userid, handle_t hVideo);  // 设置用户视频显示窗口
__VALLEYAPI int          Valley_RemoveUserVideo(handle_t s, const char* userid);  // 关闭用户视频显示

__VALLEYAPI int          Valley_EnableLocalAudio(handle_t s, bool bEnable);  // 关闭或打开 本地语音
__VALLEYAPI int          Valley_EnableLocalVideo(handle_t s, bool bEnable);  // 关闭或打开 本地视频

__VALLEYAPI int          Valley_EnableRemoteAudio(handle_t s, const char* userid, bool bEnable); // 关闭或打开用户语音
__VALLEYAPI int          Valley_EnableRemoteVideo(handle_t s, const char* userid, bool bEnable); // 关闭或打开用户视频

__VALLEYAPI int          Valley_DisableAudio(handle_t s); // 房间支持语音，默认支持, 只能在登录前调用一次，否则无效
__VALLEYAPI int          Valley_DisableVideo(handle_t s); // 房间支持视频，默认支持, 只能在登录前调用一次，否则无效
 

//下面是简单调用的例子，登录了两个房间，一个类似IM功能，仅仅用来发消息，可以发用户邀请什么的，一个房间用来做视频通话
 
/*
  1) 初始化SDK
  const char* localconfig = "<?xml version=\"1.0\" encoding=\"utf-8\"?><root><localversion>1</localversion></root>";
  localconfig配置表示支持本地通信
  Valley_InitSDK("d:\\workfolder\", localconfig);

  handle_t channelim    = Valley_CreateChannel(false);   // 创建一个房间，不支持视频，用于im通信
  handle_t channelvideo = Valley_CreateChannel(true);    // 创建一个房间，支持视频，用于视音频通话 


  Valley_EnableInterface(channelim, IID_RTCMSGR);  //IM房间，只需要支持消息传递


  2）登录IM房间 
  const char* localimserver = "<?xml version=\"1.0\" encoding=\"utf-8\"?><root><mcu><channel>s-10.80.64.56:8002</channel></mcu></root>";
  Valley_SetChannelAttr(RESERVE_CHANNEL_ATTR_CONFIG, localimserver);
  Valley_Login(channelim, “98”, "123"); 

  3）登录视频房间  
  Valley_SetLocalVideo(channelvideo, m_hMyWnd); //设置本地视频窗口
  Valley_SetVideoProfile(channelvideo, VIDEO_CAPTURE_TYPE_4X3_640); //设置本地视频大小
  Valley_Login(channelvideo, “225”, "123");


  4） 定时取出房间里面的事件来处理
  void ontimer()
  {
      // 处理IM房间消息
      while(true)
	  {
         objBaseEvent* e = Valley_GetEvent(channelim);
		 if(NULL == e)
		    break;

		  if(d->evttype > RTC_EVT_NTF_FROM) 
		     process_im_notify_event(e); 
		  else 
		      process_im_resp_event(e);

		  Valley_ReleaseEvent(e);
	  }
	  
	  // 处理视音频房间消息
	  while(true)
	  {
		  objBaseEvent* e = Valley_GetEvent(channelvideo);
		  if(NULL == e)
			break;

		  if(d->evttype > RTC_EVT_NTF_FROM) 
			 process_av_notify_event(e); 
		  else 
			 process_av_resp_event(e); 

		  Valley_ReleaseEvent(e);
	  }
  }


  5）房间不用了，退出房间
  Valley_Logout(channelim);
  Valley_Logout(channelvideo);

  6） 销毁房间对象
  Valley_Release(channelvideo);  
  channelvideo = NULL;


  Valley_Release(channelim);
  channelim = NULL;

  7) 退出进程，释放SDK
  Valley_CleanSDK();
  */