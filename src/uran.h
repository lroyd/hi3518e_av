/*******************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 *******************************************************************************/
#ifndef __RECT_URAN_H__
#define __RECT_URAN_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "uran_time.h"


typedef enum
{
	MANGER_ROLE_MASTER = 0x00,	//具有fd监听和定时器功能
	MANGER_ROLE_SLAVE,   		//只具有fd监听功能	
}MANGER_TYPE;			//注意每一个管理者有自己独立的线程

typedef enum
{
	EVENT_TIMER = 0x00, //创建定时器，只有master可以使用
	EVENT_INPUT,      	//事件输入监听
	EVENT_INLT,
	EVENT_OUTPUT,		//not use
	EVENT_OOB,			//not use
}EVENT_TYPE;

typedef struct _tagEvent
{    
	amx_rbtree_node_t 	timer;    
	volatile INT16 		m_iTimerSet;
	EVENT_TYPE			m_emType;				//用户使用
	INT32				m_iEventFD;  			//用户使用
	INT32				(*m_Handle)(void *);	//用户使用
	void 				*m_pManger;
}T_EventInfo, *PT_EventInfo;

typedef struct _tagEventManger
{   
	INT32 				m_iEp;
	pthread_t			m_pthreadID;
	struct epoll_event	*m_pEvent;
	INT32				m_iEventNum;
	T_EventInfo			in_tMainTimer;		//守护定时器
	T_TreeInfo			in_tTree;
	INT32				m_iType;
	
	void				(*pCleanup)(void *);
	void				*pArg;
}T_EvtMangerInfo, *PT_EvtMangerInfo;


/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
INT32 UranMangerCreate(MANGER_TYPE _emRole, INT32 _iTrigNum, INT32 (*_pUserHandle)(void *), INT32 _iUserMSec, void **_pThis);
/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
INT32 UranMangerDestroy(void *_pThis);
/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
INT32 UranMangerSetCleanup(void *_pThis, void (*_pCleanup)(void *), void *_pArg);
/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
INT32 UranRegister(void *_pThis, void *_pManger);
/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
INT32 UranCancel(void *_pThis);


#ifdef __cplusplus
}
#endif

#endif


