/*************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 ************************************************************************/
#ifndef __HI3518E_AUDIO_H__
#define __HI3518E_AUDIO_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/prctl.h>
#include <sys/un.h>
#include <sys/epoll.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

#include "sample_comm.h"
#include "acodec.h"


#define AUDIO_PT_NUM_SIZE	(160)

int HI_AudConfigAcodec(AIO_ATTR_S *pstAioAttr);
int HI_AudStartAi(AIO_ATTR_S* pstAioAttr, HI_VOID* pstAiVqeAttr);
int HI_AudStartAo(AIO_ATTR_S* pstAioAttr , HI_VOID* pstAoVqeAttr);
int HI_AudSetVolume(int volume);
int HI_AudStopAi(HI_VOID* pstAiVqeAttr);
int HI_AudStopAo(HI_VOID* pstAoVqeAttr);

int HI_AudStartVenc(PAYLOAD_TYPE_E enType);
int HI_AudStopVenc(void);
int HI_AudViBindVo(void);
int HI_AudViUnbindVo(void);



#endif


