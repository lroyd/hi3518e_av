/*************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 ************************************************************************/
#ifndef __HI3518E_VIDEO_H__
#define __HI3518E_VIDEO_H__


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

#include "hi_mipi.h"
#include "hi_common.h"

#include "sample_comm.h"


int HI_VidStartVi(SAMPLE_VI_CONFIG_S* pstViConfig);
int HI_VidStopVi(SAMPLE_VI_CONFIG_S* pstViConfig);
int HI_VidViBindVpss(SAMPLE_VI_MODE_E enViMode);
int HI_VidViUnBindVpss(SAMPLE_VI_MODE_E enViMode);

int HI_VidVencStart(VENC_CHN VencChn, PAYLOAD_TYPE_E enType, VIDEO_NORM_E enNorm, PIC_SIZE_E enSize, SAMPLE_RC_E enRcMode,HI_U32  u32Profile);
int HI_VidVencStop(VENC_CHN VencChn);

int HI_VidVencBindVpss(VENC_CHN VeChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn);
int HI_VidVencUnBindVpss(VENC_CHN VeChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn);

int HI_VidVencSaveStream(PAYLOAD_TYPE_E enType, FILE *pFd, VENC_STREAM_S *pstStream);



#endif


