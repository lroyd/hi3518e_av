/*************************************************************************
	> File Name: common.h
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 ************************************************************************/
#ifndef __COMM_TYPE_H__
#define __COMM_TYPE_H__

#include <stdarg.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <sys/types.h>
#include <assert.h>
#include <pthread.h>

//#include "config_site.h"

/************************************************************************
* 			Definition DATA TYPE
* **********************************************************************/
#define TRUE	(1) 
#define FALSE	(0) 

typedef signed char				INT8;
typedef unsigned char			UINT8;
typedef signed short			INT16;
typedef unsigned short			UINT16;
typedef signed int				INT32;
typedef unsigned int			UINT32;

typedef unsigned long long		UINT64;
typedef long long				INT64;

typedef signed char				BOOLEN;
typedef signed char				BOOLEAN;

typedef unsigned int  			TaskId;
typedef unsigned short  		EventId;

/************************************************************************
* 			Definition
* **********************************************************************/







/************************************************************************
* 			Macro Definition
************************************************************************/
enum 
{
	LOG_FATAL = 0,
	LOG_ERROR,
	LOG_WARNING,	
	LOG_DEBUG,
	LOG_INFO,
	LOG_TRACE,
	LOG_MAX,
};


#define GH_DEF(type)		    type

#if defined(__cplusplus)
	#define GH_DECL(type)	    type
#else
	#define GH_DECL(type)	    extern type
#endif

#define _ARRAY_SIZE(a)    (sizeof(a)/sizeof(a[0]))	//Utility macro to compute the number of elements in static array

#define	_ASSERT(expr)	assert(expr)

#define _ERROR_RETURN(expr,retval)    \
	do { \
	if (!(expr)) { return retval; } \
	} while (0)
/************************************************************************
* 			PTHREAD Variable Definition
* **********************************************************************/
#define TPId 			    	pthread_t
#define TPLock 			    	pthread_mutex_t
#define InitTPLock(lck)			pthread_mutex_init(&(lck), NULL)
#define LockTPLock(lck)			pthread_mutex_lock(&(lck))
#define TryLockTPLock(lck)		pthread_mutex_trylock(&(lck))
#define UnLockTPLock(lck)	    pthread_mutex_unlock(&(lck))

#define TPCond 					pthread_cond_t
#define InitTPCond(cnd)			pthread_cond_init(&(cnd), NULL)
#define WaitTPCond(cnd, lck)	pthread_cond_wait(&(cnd), &(lck))
#define SignalTPCond(cnd)		pthread_cond_signal(&(cnd))






/************************************************************************
* 			Built-In Function Definition
* **********************************************************************/
static void XSleep(int nSec, int nUSec)
{
	struct timeval tv;
	tv.tv_sec = nSec;
	tv.tv_usec = nUSec;
	select(0, NULL, NULL, NULL, &tv);
}

static unsigned long long CTimerSec(void) 
{
    unsigned long long x=0;
	struct timeval t;
	gettimeofday(&t, 0);
    x = t.tv_sec;
	return x;
}



#endif


