#include "stdafx.h"
#include "ValleyRTCModule.h"

#include "api/include/ValleyRtcExtInterface.h" 
#include "xRTCEngineInterface.h" 

#include <list>
#include <mutex>  


#define TRANSLATE_AUDIO_ERR(x) x
#define TRANSLATE_VIDEO_ERR(x) x
#define MAX_STRVAL 10
 
typedef std::list<objBaseEvent*> evtlist_t; 
typedef std::mutex   				        lock_t;
typedef std::recursive_mutex   				recursive_lock_t;
typedef std::unique_lock<lock_t>		    sc_lock_t;

using namespace Valley; 

class tMemory
{ 
public: 
	const char* put(const char* val){
		if (nPos == MAX_STRVAL || nullptr == val)
			return nullptr;

		values[nPos] = val;
		return values[nPos].c_str();
	}

	const char* put(uint64_t val){
		if (nPos == MAX_STRVAL)
			return nullptr;

		char buf[24];
		sprintf(buf, "%I64u", val);
		values[nPos] = buf;

		return values[nPos].c_str();
	}

private:
	std::string values[10];
	int nPos = 0;
}; 

class RtcHaneleImpl;
class RtcSinkImpl; 
struct ValleyChannelState
{
	xRTCEngineInterface* engine = nullptr;
	Valley::IRtcChannel* impl = nullptr;
	Valley::IRtcMsger*   rtc_msgr = nullptr;
	Valley::IRtcUsers*   rtc_users = nullptr;
	RtcSinkImpl*         sink = nullptr;
	RtcHaneleImpl*       handler = nullptr;
	void*                userdata = nullptr;
	evtlist_t            evtlist;
	lock_t               lock;
};

class RtcSinkImpl : public ::Valley::IRtcSink
{
public:
	ValleyChannelState * inst;
	RtcSinkImpl(ValleyChannelState* state)
	{
		inst = state;
	}
	virtual void Respond(int type, int ec, ::Valley::object_base* ob, void* userdata)
	{ 
		tMemory* m = nullptr;
		objBaseEvent* e = nullptr;

		switch (type)
		{
		case IRtcChannel::RespondLogin:
		{ 
			objRespLogin* r = new objRespLogin;
			r->ec = TRANSLATE_AUDIO_ERR(ec);
			r->evttype = RTC_RESP_LOGINED;

			object_login* lp = (object_login*)ob;
			if (0 == ec)
			{
				m = new tMemory();
				r->userid = m->put(lp->getUserID()); 
			}  
			e = r; 
		}
		break;
		case IRtcChannel::RespondChannelAttr:
		{
			object_channel_attr* lp = (object_channel_attr*)ob; 
		}
		break;
		case IRtcUsers::RespondKickOff:
		{
			object_userid* lp = (object_userid*)ob; 
		}
		break;
		case IRtcUsers::RespondUserAttr:
		{
			object_user_attr* lp = (object_user_attr*)ob; 
		}
		break;
		case IRtcAudio::RespondBlockUser:
		{
			object_block_speak* lp = (object_block_speak*)ob; 
		}
		break;
		case IRtcAudio::RespondDisableUserSpeak:
		{
			object_disable_speak* lp = (object_disable_speak*)ob; 
		}
		break;

		case  IRtcMsger::RespondSendMessage:
		{
			object_msg* lp = (object_msg*)ob;

			objRespMsg* r = new objRespMsg;
			r->evttype = RTC_RESP_SEND_MSG;

			m = new tMemory(); 
			r->touserid = m->put(lp->getToUserID());
			r->data     = m->put(lp->getData());
			r->token    = m->put(lp->getToken());

			r->datalen  = lp->getLength();
			r->msgtime  = lp->getTime() / 1000;
			r->msgtype  = lp->getMsgType();

			e = r;
		}
		break;
		default:
			return;
			break;
		}

		if (nullptr != e)
		{
			e->memory = m;
			inst->evtlist.push_back(e);
		} 
	}
	virtual void Notify(int type, ::Valley::object_base* ob, void* userdata)
	{
		tMemory* m = nullptr;
		objBaseEvent* e = nullptr;

		switch (type)
		{
		case IRtcChannel::NotifyChannelAttr:
		{
			object_channel_attr* lp = (object_channel_attr*)ob; 
		}
		break;
		case IRtcChannel::NotifyChannelClose:
		{
			object_error* lp = (object_error*)ob; 
		}
		break;
		case IRtcChannel::NotifyConnectionLost:
		{
			objNtfNetLost* r = new objNtfNetLost;
			r->evttype = RTC_NTF_NET_LOST;  
			e = r;
		}
		break;
		case IRtcChannel::NotifyReConnected:
		{
			objNtfNetResume* r = new objNtfNetResume;
			r->evttype = RTC_NTF_NET_RESUME;
			e = r;
		}
		break;
		case IRtcUsers::NotifyKickOff:
		{
			object_userid* lp = (object_userid*)ob; 
		}
		break;
		case IRtcChannel::NotifyDuplicateLogined:
		{
			objNtfDupLogined* r = new objNtfDupLogined;
			r->evttype = RTC_NTF_DUP_LOGINED;
			e = r;
		}
		break;
		case IRtcUsers::NotifyUserAttr:
		{
			object_user_attr* lp = (object_user_attr*)ob; 
		}
		break;
		case IRtcUsers::NotifyUserEnterChannel:
		{ 
			object_user* lp = (object_user*)ob;

			objNtfUserEnter* r = new objNtfUserEnter;
			r->evttype = RTC_NTF_USER_ENTER;
			 
			m = new tMemory();
			r->userid   = m->put(lp->getUserID()); 
			 
			e = r;
		}
		break;
		case IRtcUsers::NotifyUserLeaveChannel:
		{
			object_userid* lp = (object_userid*)ob; 

			objNtfUserLeave* r = new objNtfUserLeave;
			r->evttype = RTC_NTF_USER_LEAVE;

			m = new tMemory();
			r->userid = m->put(lp->getUserID()); 

			e = r;
		}
		break;
		case IRtcAudio::NotifyDisableUserSpeak:
		{
			object_disable_speak* lp = (object_disable_speak*)ob; 
		}
		break;
		case IRtcAudio::NotifyUserSpeaking:
		{
			object_user_speaking* lp = (object_user_speaking*)ob; 
		}
		break;
		case IRtcMsger::NotifyRecvedMessage:
		{
			object_msg* lp = (object_msg*)ob;

			objNtfMsg* r = new objNtfMsg;
			r->evttype = RTC_NTF_RECV_MSG;

			m = new tMemory();
			r->fromuserid = m->put(lp->getFromUserID());
			r->touserid   = m->put(lp->getToUserID());
			r->data       = m->put(lp->getData());
			r->token      = m->put(lp->getToken());

			r->datalen    = lp->getLength();
			r->msgtime    = lp->getTime()/1000;
			r->msgtype    = lp->getMsgType();

			e = r;
		}
		break;
		default:
			return;
			break;
		}

		if (nullptr != e)
		{
			e->memory = m;
			inst->evtlist.push_back(e);
		}
	}
};
  

class RtcHaneleImpl : public IxRTCHandler
{
public:
	RtcHaneleImpl(ValleyChannelState* state)
	{
		inst = state;
	}
public:
	void __stdcall OnCreateChannel(uint64_t uChannelID, uint32_t uToken)
	{
		
	}

	 
	void __stdcall OnEnterChannel(const char * pszChannelName, uint64_t uUserID)
	{ 
		sc_lock_t sc(inst->lock);

		loginstatus = LOGIN_STATUS_LOGINED;

		objRespLogin* r = new objRespLogin;
		r->ec = 0;
		r->evttype = RTC_RESP_LOGINED;

		tMemory* m = new tMemory();
		r->userid = m->put(uUserID);

		r->memory = m;
		inst->evtlist.push_back(r);
	}



	void __stdcall OnLeaveChannel()
	{
		sc_lock_t sc(inst->lock);
		loginstatus = LOGIN_STATUS_NONE;

		objNtfNetLost* r = new objNtfNetLost;
		r->evttype = RTC_NTF_NET_LOST;

		r->memory = nullptr;
		inst->evtlist.push_back(r); 
	}

	
	void __stdcall OnUserEnter(uint64_t uUserID)
	{   
		sc_lock_t sc(inst->lock);

		objNtfUserEnter* r = new objNtfUserEnter;
		r->evttype = RTC_NTF_USER_ENTER;

		tMemory* m = new tMemory();
		r->userid = m->put(uUserID);

		r->memory = m;
		inst->evtlist.push_back(r);
	}


	void __stdcall OnUserLeave(uint64_t uUserID)
	{
		sc_lock_t sc(inst->lock);

		objNtfUserLeave* r = new objNtfUserLeave;
		r->evttype = RTC_NTF_USER_LEAVE;


		tMemory* m = new tMemory();

		r->userid = m->put(uUserID);

		r->memory = m;
		inst->evtlist.push_back(r);
	}

	void __stdcall OnUserFirstVideo(uint64_t uUserID, uint32_t uWidth, uint32_t uHeight)
	{

	}

	void __stdcall OnUserFirstAudio(uint64_t uUserID)
	{

	}

	void __stdcall OnUserVideoChange(uint64_t uUserID, bool bEnable)
	{

	}

	void __stdcall OnUserAudioChange(uint64_t uUserID, bool bEnable)
	{

	}


	void __stdcall OnUserOffMessage(IIMUserMessage ** ppIMessage, uint32_t uCount, uint32_t uEnd)
	{

	}

	void __stdcall OnUserMessage(IIMUserMessage * pIMessage)
	{

	}

	void __stdcall OnUserSendMessage(IIMUserMessage * pIMessage, int32_t nErrorCode)
	{

	}

	void __stdcall OnChannelMessage(IIMUserMessage * pIMessage)
	{

	}

	void __stdcall OnError(uint32_t nError, const char * pszErrorMsg)
	{

	}

	void __stdcall OnNetStatus(int nStatus)
	{
		if (USER_STATUS_OFFLINE == nStatus || USER_STATUS_UNKOWN == nStatus)
		{
			if (LOGIN_STATUS_LOGINED == loginstatus)
			{
				sc_lock_t sc(inst->lock);
				loginstatus = LOGIN_STATUS_NONE;

				objNtfNetLost* r = new objNtfNetLost;
				r->evttype = RTC_NTF_NET_LOST;

				r->memory = nullptr;
				inst->evtlist.push_back(r); 
			} 
		}
		else
		{
			if (LOGIN_STATUS_NONE == loginstatus)
			{
				sc_lock_t sc(inst->lock);
				loginstatus = LOGIN_STATUS_LOGINED;

				objNtfNetResume* r = new objNtfNetResume;
				r->evttype = RTC_NTF_NET_RESUME;

				r->memory = nullptr;
				inst->evtlist.push_back(r); 
			} 
		} 
	}

	int GetLoginStatus()
	{
		return loginstatus;
	}
	 
public:
	ValleyChannelState * inst = nullptr;
	int                  loginstatus = LOGIN_STATUS_NONE;
	bool                 disableAudio = false;
	bool                 disableVideo = false;
};



__VALLEYAPI bool Valley_InitSDK(const char* workfolder, const char* localconfig)
{
	::InitSDK(workfolder, localconfig);
	return true;
}


__VALLEYAPI void Valley_CleanSDK()
{
	::CleanSDK();  
}

__VALLEYAPI const char*	Valley_GetSDKVersion()
{
	return "2.1";
}

__VALLEYAPI const char*	 Valley_GetErrDesc(int ec)
{
	if (ERR_NOT_INIT == ec) 
		ec = ERR_NOT_INITIALIZE; 
	else if (ERR_INVOKE_ORDER == ec)
		ec = ERR_INVOKE;

	return GetErrDesc(ec);
}

__VALLEYAPI handle_t Valley_CreateChannel(bool withVideo)
{
	ValleyChannelState* s = new ValleyChannelState();
	if (withVideo)
	{
		xRTCEngineInterface* pxRTCEngine = xRTCEngineCreate();

		s->handler = new RtcHaneleImpl(s);

		pxRTCEngine->setHandler(s->handler, 0);
		pxRTCEngine->setVideoProfile(VIDEO_CAPTURE_TYPE_4X3_640, false);
	//	pxRTCEngine->enableAudio();
	//	pxRTCEngine->enableVideo();

		s->engine = pxRTCEngine;
	} 
	else
	{
		IRtcChannel* pxChannel = CreateChannel();
		s->sink = new RtcSinkImpl(s);
		pxChannel->RegisterRtcSink(s->sink, true); 
		s->impl = pxChannel;
	}
		

	return s;
}
 
__VALLEYAPI void Valley_Release(handle_t s)
{
	ValleyChannelState* st = (ValleyChannelState*)s;
	if (nullptr == st)
		return;

	if (st->engine)
	{
		delete st->handler;
		st->engine->release();
	}
	else if (st->impl)
	{
		delete st->sink;
		st->impl->Release(false);
	}
		

	{
		sc_lock_t sc(st->lock);
		while (!st->evtlist.empty())
		{
			free(st->evtlist.front());
			st->evtlist.pop_front();
		}
	}
	 
	delete st;
}


__VALLEYAPI int Valley_EnableInterface(handle_t s, int iids)
{
	ValleyChannelState* st = (ValleyChannelState*)s;
	if (nullptr == st || nullptr == st->impl)
		return ERR_NOT_INIT;

	return TRANSLATE_AUDIO_ERR(st->impl->EnableInterface(iids));
}

__VALLEYAPI objBaseEvent*  Valley_GetEvent(handle_t s)
{
	ValleyChannelState* st = (ValleyChannelState*)s;
	if (nullptr == st)
		return nullptr;

	if (st->impl)
	{
		st->impl->Poll();
		if (!st->evtlist.empty())
		{
			objBaseEvent* e = st->evtlist.front();
			st->evtlist.pop_front();
			return e;
		}
		else
		{
			return nullptr;
		}
	}
	else  
	{
		sc_lock_t sc(st->lock);
		if (!st->evtlist.empty())
		{
			objBaseEvent* e = st->evtlist.front();
			st->evtlist.pop_front();
			return e;
		}
		else
		{
			return nullptr;
		}
	}
}

__VALLEYAPI void Valley_ReleaseEvent(objBaseEvent* e)
{
	if (nullptr == e)
		return; 

	if (nullptr != e->memory)
	{
		tMemory* pM = (tMemory*)e->memory;
		delete pM;
	}

	delete e;
}

__VALLEYAPI int Valley_Login(handle_t s, const char* channelid, const char* userid)
{
	ValleyChannelState* st = (ValleyChannelState*)s;
	if (nullptr == st)
		return ERR_NOT_INIT;

	if (st->impl)
	{
		return TRANSLATE_AUDIO_ERR(st->impl->Login(channelid, userid, nullptr));
	}
	else
	{  
		xRTCEngineInterface* pxRTCEngine = st->engine; 
		pxRTCEngine->setUserInfo("xRTCTest", 0, _atoi64(userid), ""); 
		st->handler->loginstatus = LOGIN_STATUS_LOGINING;
		if (!st->handler->disableAudio)
			st->engine->enableAudio();

		if (!st->handler->disableVideo)
			st->engine->enableVideo(); 

		int ec = st->engine->joinChannel(nullptr, _atoi64(channelid), "");
		if (ERR_SUCCEED == ec)
		{
			return ec;
		}
		else
		{
			st->handler->loginstatus = LOGIN_STATUS_NONE;
			return TRANSLATE_VIDEO_ERR(ec);
		} 
	}
}


__VALLEYAPI void Valley_Logout(handle_t s)
{
	ValleyChannelState* st = (ValleyChannelState*)s;
	if (nullptr == st)
		return;

	if (st->impl)
	{
		return st->impl->Logout();
	}
	else
	{
		xRTCEngineInterface* pxRTCEngine = st->engine;
		pxRTCEngine->leaveChannel(); 
	}
}

__VALLEYAPI int Valley_GetLoginStatus(handle_t s)
{ 
	ValleyChannelState* st = (ValleyChannelState*)s;
	if (nullptr == st)
		return LOGIN_STATUS_NONE;

	if (st->impl)
	{
		return st->impl->GetLoginStatus();
	}
	else
	{
		return st->handler->GetLoginStatus();
	}
}

__VALLEYAPI int Valley_SetChannelAttr(handle_t s, const char* name, const char* value)
{ 
	ValleyChannelState* st = (ValleyChannelState*)s;
	if (nullptr == st || nullptr == st->impl)
		return ERR_NOT_INIT;
	 
	return TRANSLATE_AUDIO_ERR(st->impl->SetChannelAttr(name, value));
}


__VALLEYAPI int Valley_SendMsgr(handle_t s, int type, const char* data, int len, const char* token, const char* toUserID)
{ 
	ValleyChannelState* st = (ValleyChannelState*)s;
	if (nullptr == st || nullptr == st->rtc_msgr)
		return ERR_NOT_INIT;

	return TRANSLATE_AUDIO_ERR(st->rtc_msgr->SendMsgr((msgtype)type, data, len, token, toUserID));
}

__VALLEYAPI int Valley_SetVideoProfile(handle_t s, int profile)
{
	ValleyChannelState* st = (ValleyChannelState*)s;
	if (nullptr == st || nullptr == st->engine)
		return ERR_NOT_INIT;
	 
	return TRANSLATE_VIDEO_ERR(st->engine->setVideoProfile(profile, false)); 
}


__VALLEYAPI int Valley_SetLocalVideo(handle_t s, handle_t hVideo)
{  
	ValleyChannelState* st = (ValleyChannelState*)s;
	if (nullptr == st || nullptr == st->engine)
		return ERR_NOT_INIT; 

	xRTCCanvas canvas;
	canvas.m_UserID = 0;
	canvas.m_CanvasType = 0;
	canvas.m_RenderType = 0;
	canvas.m_pWndObj = hVideo;  

	return TRANSLATE_VIDEO_ERR(st->engine->setupLocalVideo(&canvas));
}

__VALLEYAPI int Valley_RemoveLocalVideo(handle_t s)
{ 
	ValleyChannelState* st = (ValleyChannelState*)s;
	if (nullptr == st || nullptr == st->engine)
		return ERR_NOT_INIT;

	return TRANSLATE_VIDEO_ERR(st->engine->removeLocalVideo());
}


__VALLEYAPI int Valley_SetUserVideo(handle_t s, const char* userid, handle_t hVideo)
{ 
	ValleyChannelState* st = (ValleyChannelState*)s;
	if (nullptr == st || nullptr == st->engine)
		return ERR_NOT_INIT; 
 
	xRTCCanvas canvas;
	canvas.m_UserID = _atoi64(userid);
	canvas.m_CanvasType = 0;
	canvas.m_RenderType = 0;
	canvas.m_pWndObj = hVideo;

	return TRANSLATE_VIDEO_ERR(st->engine->setupRemoteVideo(&canvas));
}

__VALLEYAPI int Valley_RemoveUserVideo(handle_t s, const char* userid)
{ 
	ValleyChannelState* st = (ValleyChannelState*)s;
	if (nullptr == st || nullptr == st->engine)
		return ERR_NOT_INIT;
	 
	return TRANSLATE_VIDEO_ERR(st->engine->removeRemoteVideo(_atoi64(userid)));
}

__VALLEYAPI int Valley_EnableLocalAudio(handle_t s, bool bEnable)
{ 
	ValleyChannelState* st = (ValleyChannelState*)s;
	if (nullptr == st || nullptr == st->engine)
		return ERR_NOT_INIT;

	bool bMute = !bEnable; 
	return TRANSLATE_VIDEO_ERR(st->engine->muteLocalAudioSteam(bMute));
}


__VALLEYAPI int Valley_EnableLocalVideo(handle_t s, bool bEnable)
{ 
	ValleyChannelState* st = (ValleyChannelState*)s;
	if (nullptr == st || nullptr == st->engine)
		return ERR_NOT_INIT;

	bool bMute = !bEnable;
	return TRANSLATE_VIDEO_ERR(st->engine->muteLocalAudioSteam(bMute));
}

__VALLEYAPI int Valley_EnableRemoteAudio(handle_t s, const char* userid, bool bEnable)
{ 
	ValleyChannelState* st = (ValleyChannelState*)s;
	if (nullptr == st || nullptr == st->engine)
		return ERR_NOT_INIT;

	bool bMute = !bEnable;
	return TRANSLATE_VIDEO_ERR(st->engine->muteRemoteAudioSteam(_atoi64(userid), bMute));
}

__VALLEYAPI int Valley_EnableRemoteVideo(handle_t s, const char* userid, bool bEnable)
{ 
	ValleyChannelState* st = (ValleyChannelState*)s;
	if (nullptr == st || nullptr == st->engine)
		return ERR_NOT_INIT;

	bool bMute = !bEnable;
	return TRANSLATE_VIDEO_ERR(st->engine->muteRemoteVideoStream(_atoi64(userid), bMute));
}

__VALLEYAPI int Valley_DisableAudio(handle_t s)
{ 
	ValleyChannelState* st = (ValleyChannelState*)s;
	if (nullptr == st || nullptr == st->engine)
		return ERR_NOT_INIT;

	if (STATUS_NONE != st->handler->GetLoginStatus())
		return ERR_INVOKE_ORDER;
	  
	st->handler->disableAudio = true;
	return ERR_SUCCEED;
}


__VALLEYAPI int Valley_DisableVideo(handle_t s)
{ 
	ValleyChannelState* st = (ValleyChannelState*)s;
	if (nullptr == st || nullptr == st->engine)
		return ERR_NOT_INIT;

	if (STATUS_NONE != st->handler->GetLoginStatus())
		return ERR_INVOKE_ORDER;

	st->handler->disableVideo = true;
	return ERR_SUCCEED;
}