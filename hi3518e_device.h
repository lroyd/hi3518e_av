/*************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 ************************************************************************/
#ifndef __HI3518E_DEV_H__
#define __HI3518E_DEV_H__

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



int HI_DEVICE_SystemInit(VB_CONF_S *pstVbConf);
void HI_DEVICE_SystemExit(void);

int HI_DEVICE_GetPicSize(VIDEO_NORM_E enNorm, PIC_SIZE_E enPicSize, SIZE_S *pstSize);
int HI_DEVICE_CalcPicVbBlkSize(VIDEO_NORM_E enNorm, PIC_SIZE_E enPicSize, PIXEL_FORMAT_E enPixFmt, int u32AlignWidth);

void HI_DEVICE_ISPStop(void);

int HI_DEVICE_VpssMemConfig(void);
int HI_DEVICE_VpssStartGroup(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S *pstVpssGrpAttr);
int HI_DEVICE_VpssEnableChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, 
                                                  VPSS_CHN_ATTR_S *pstVpssChnAttr,
                                                  VPSS_CHN_MODE_S *pstVpssChnMode,
                                                  VPSS_EXT_CHN_ATTR_S *pstVpssExtChnAttr);
int HI_DEVICE_VpssStopGroup(VPSS_GRP VpssGrp);
int HI_DEVICE_VpssStart(int s32GrpCnt, SIZE_S *pstSize, int s32ChnCnt,VPSS_GRP_ATTR_S *pstVpssGrpAttr);
int HI_DEVICE_VpssStop(int s32GrpCnt, int s32ChnCnt);
												  


#endif


