#pragma once
 
#define __VALLEYAPI extern "C"

#define ERR_SUCCEED   0
#define ERR_NOT_INIT  -10001   // δ��ʼ��
#define ERR_INVOKE_ORDER -10002  // ����˳�����

#define IID_RTCMSGR 0x10 
#define RESERVE_CHANNEL_ATTR_CONFIG "##INNER##CHANNEL_SATTR_CONFIG"  //���÷����ڲ����ԣ��������������ڲ�������

//��¼״̬
enum 
{
	LOGIN_STATUS_NONE = 0,        // δ��¼
	LOGIN_STATUS_LOGINING = 1,    // ��¼��
	LOGIN_STATUS_LOGINED = 2,     // �Ѿ���¼
};

enum MSGTYPE
{
	msgtypeText = 1,		 // �ı���Ϣ, SDK������Ϣԭʼ���ݣ�ֻ�ǽ������ƽ̨ͳһ����utf8��ʽ���Ա������SDK��˫����ͳһ�γ�����
	msgtypeAudio = 2,		 // ������Ϣ
	msgtypeBinary = 3,       // ��������Ϣ
	msgtypeCmd = 10,         // ������Ϣ�����ڷ������ϴ洢������ת��
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

// �¼�����
struct objBaseEvent{   
	const void* memory;
	int evttype; 
};


// ֪ͨ�¼�����
struct objNtfEvent :objBaseEvent{     
	#define RTC_EVT_NTF_FROM   1000
};

// ��Ӧ�¼����࣬��Ӧ�¼����������룬ec=0��ʶ�ɹ���ec!=0��ʶʧ�ܣ�����ʧ�ܼ���������
struct objRespEvent :objBaseEvent{    
	#define RTC_EVT_RESP_FROM  0 
	int ec;
};

//��Ӧ�¼� �յ��û���¼�ɹ���ʧ�ܵ�Ӧ��
struct objRespLogin: objRespEvent{     
#define RTC_RESP_LOGINED (RTC_EVT_RESP_FROM+1)
	const char* userid;
};

//֪ͨ�¼� �յ��û����뷿��
struct objNtfUserEnter : objNtfEvent{  
#define RTC_NTF_USER_ENTER (RTC_EVT_NTF_FROM+1)
	const char* userid; 
};

//֪ͨ�¼� �յ��û��뿪����
struct objNtfUserLeave : objNtfEvent{ 
#define RTC_NTF_USER_LEAVE (RTC_EVT_NTF_FROM+2)
	const char* userid; 
};

//��Ӧ�¼� �յ�����Ϣ�ɹ���ʧ�ܵ�Ӧ��
struct objRespMsg : objRespEvent{
#define RTC_RESP_SEND_MSG (RTC_EVT_RESP_FROM+50) 
	const char* touserid;
	const char* data;
	int         datalen;
	const char* token;
	int         msgtype;
	seond_t     msgtime; 
};

//֪ͨ�¼� �յ��û���������Ϣ
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

//֪ͨ�¼� �յ�����Ͽ�����Ϣ
struct objNtfNetLost : objNtfEvent{
#define RTC_NTF_NET_LOST (RTC_EVT_NTF_FROM+80) 
};

//֪ͨ�¼� �յ������������ϵ���Ϣ
struct objNtfNetResume : objNtfEvent{
#define RTC_NTF_NET_RESUME (RTC_EVT_NTF_FROM+81) 
};

//֪ͨ�¼� �յ��˺���ص�¼����Ϣ
struct objNtfDupLogined : objNtfEvent{
#define RTC_NTF_DUP_LOGINED (RTC_EVT_NTF_FROM+82) 
};



__VALLEYAPI bool         Valley_InitSDK(const char* workfolder, const char* localconfig); //��ʼ��SDK,һ������ֻ��Ҫ��ʼ��һ��
__VALLEYAPI void         Valley_CleanSDK();       //��ʼ��SDK,һ������ֻ��Ҫ��ʼ��һ��
__VALLEYAPI const char*	 Valley_GetSDKVersion();  //��ȡSDK�汾
__VALLEYAPI const char*	 Valley_GetErrDesc(int ec);     //��ȡ�������Ӧ������

__VALLEYAPI handle_t     Valley_CreateChannel(bool withVideo);  //����һ�����䣬�Ƿ���Ҫ֧����Ƶ
__VALLEYAPI void         Valley_Release(handle_t s);  //����һ������

__VALLEYAPI int          Valley_EnableInterface(handle_t s, int iids);  //���������Ч

__VALLEYAPI objBaseEvent* Valley_GetEvent(handle_t s);           //��ȡ�¼�����֧�֣�����������û���¼����ؿ�֧�֣����¼������¼��ṹ֧�֣���Ҫ����Valley_ReleaseEvent�ͷ��¼�����ָ��
__VALLEYAPI void          Valley_ReleaseEvent(objBaseEvent* e);  //�ͷ��¼�����ָ��

__VALLEYAPI int          Valley_Login(handle_t s, const char* channelid, const char* userid); //��¼����
__VALLEYAPI void         Valley_Logout(handle_t s);        //�˳�����
__VALLEYAPI int          Valley_GetLoginStatus(handle_t s);  //��ȡ��¼״̬

__VALLEYAPI int          Valley_SetChannelAttr(handle_t s, const char* name, const char* value);  // ���÷�������
__VALLEYAPI int          Valley_SendMsgr(handle_t s, int msgtype, const char* data, int len, const char* token, const char* toUserID);  // ������Ϣ

__VALLEYAPI int          Valley_SetVideoProfile(handle_t s, int profile);   // ���ñ�����Ƶ����

__VALLEYAPI int          Valley_SetLocalVideo(handle_t s, handle_t hVideo);   // ���ñ�����Ƶ��ʾ����
__VALLEYAPI int          Valley_RemoveLocalVideo(handle_t s);   // �رձ�����Ƶ��ʾ

__VALLEYAPI int          Valley_SetUserVideo(handle_t s, const char* userid, handle_t hVideo);  // �����û���Ƶ��ʾ����
__VALLEYAPI int          Valley_RemoveUserVideo(handle_t s, const char* userid);  // �ر��û���Ƶ��ʾ

__VALLEYAPI int          Valley_EnableLocalAudio(handle_t s, bool bEnable);  // �رջ�� ��������
__VALLEYAPI int          Valley_EnableLocalVideo(handle_t s, bool bEnable);  // �رջ�� ������Ƶ

__VALLEYAPI int          Valley_EnableRemoteAudio(handle_t s, const char* userid, bool bEnable); // �رջ���û�����
__VALLEYAPI int          Valley_EnableRemoteVideo(handle_t s, const char* userid, bool bEnable); // �رջ���û���Ƶ

__VALLEYAPI int          Valley_DisableAudio(handle_t s); // ����֧��������Ĭ��֧��, ֻ���ڵ�¼ǰ����һ�Σ�������Ч
__VALLEYAPI int          Valley_DisableVideo(handle_t s); // ����֧����Ƶ��Ĭ��֧��, ֻ���ڵ�¼ǰ����һ�Σ�������Ч
 

//�����Ǽ򵥵��õ����ӣ���¼���������䣬һ������IM���ܣ�������������Ϣ�����Է��û�����ʲô�ģ�һ��������������Ƶͨ��
 
/*
  1) ��ʼ��SDK
  const char* localconfig = "<?xml version=\"1.0\" encoding=\"utf-8\"?><root><localversion>1</localversion></root>";
  localconfig���ñ�ʾ֧�ֱ���ͨ��
  Valley_InitSDK("d:\\workfolder\", localconfig);

  handle_t channelim    = Valley_CreateChannel(false);   // ����һ�����䣬��֧����Ƶ������imͨ��
  handle_t channelvideo = Valley_CreateChannel(true);    // ����һ�����䣬֧����Ƶ����������Ƶͨ�� 


  Valley_EnableInterface(channelim, IID_RTCMSGR);  //IM���䣬ֻ��Ҫ֧����Ϣ����


  2����¼IM���� 
  const char* localimserver = "<?xml version=\"1.0\" encoding=\"utf-8\"?><root><mcu><channel>s-10.80.64.56:8002</channel></mcu></root>";
  Valley_SetChannelAttr(RESERVE_CHANNEL_ATTR_CONFIG, localimserver);
  Valley_Login(channelim, ��98��, "123"); 

  3����¼��Ƶ����  
  Valley_SetLocalVideo(channelvideo, m_hMyWnd); //���ñ�����Ƶ����
  Valley_SetVideoProfile(channelvideo, VIDEO_CAPTURE_TYPE_4X3_640); //���ñ�����Ƶ��С
  Valley_Login(channelvideo, ��225��, "123");


  4�� ��ʱȡ������������¼�������
  void ontimer()
  {
      // ����IM������Ϣ
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
	  
	  // ��������Ƶ������Ϣ
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


  5�����䲻���ˣ��˳�����
  Valley_Logout(channelim);
  Valley_Logout(channelvideo);

  6�� ���ٷ������
  Valley_Release(channelvideo);  
  channelvideo = NULL;


  Valley_Release(channelim);
  channelim = NULL;

  7) �˳����̣��ͷ�SDK
  Valley_CleanSDK();
  */