/*************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: 
	> Created Time: 
 ************************************************************************/
#include "hi3518e.h"
#include "hi3518e_audio.h"
#include "hi3518e_device.h"


#define EPOLL_EVENT_NUM_MAX		(16) 

///////////////////////////////////////////////////////////////////
VB_CONF_S stVbConf;


///////////////////////////////////////////////////////////////////
static int audio_bing_flags = 0;	//bind标识
pthread_t	thread_id;

static int efd = -1;	//全局epoll,fd
static int audio_fd = -1;

hi_audio_cb audio_cb = NULL;

///////////////////////////////////////////////////////////////////
typedef struct _tagVideoInfo
{
	PIC_SIZE_E		emSize;			//注意要和in_tSize大小对应
	SAMPLE_RC_E		emRate;
	PAYLOAD_TYPE_E	emPayload;
	VIDEO_NORM_E	emNorm;
	int				m_iProfile;		//0：baseline	2：hp
	SAMPLE_VI_CONFIG_S in_tViConfig;//释放的时候用到
	
	int				m_iVfd;			//video fd
	unsigned char	m_ucRunning;	
	
	HI_VIDEO_CBK	pVideoCbk;
	
}T_VideoInfo;

T_VideoInfo g_tVideoInfo;




///////////////////////////////////////////////////////////////////
int uepoll_create(int *_efd)
{
	*_efd = -1;
	*_efd = epoll_create(EPOLL_EVENT_NUM_MAX); 
	if(*_efd < 0)
	{
		return -1;		
	}	

	return 0;
}

int uepoll_add(int _efd, int _fd)
{
	
	struct epoll_event ev;
	ev.data.fd = _fd;
	ev.events = (EPOLLIN|EPOLLET);
	
	if(epoll_ctl(_efd, EPOLL_CTL_ADD, _fd, &ev) == -1)
	{
		return -1;
	}
	
	return 0;
}

int uepoll_del(int _efd, int _fd) 
{
	
	if(epoll_ctl(_efd, EPOLL_CTL_DEL, _fd, NULL) == -1)
	{
		return -1;
	}
	
	return 0;
}


/*********************************************************************************/
//默认参数：8k，16bit，不编码,	flags:1绑定模式
int hi3518_audio_init(int flags)
{
	int iRet = 0;
	AIO_ATTR_S stAioAttr;
	
    stAioAttr.enSamplerate   = AUDIO_SAMPLE_RATE_8000;
    stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_MONO;
    stAioAttr.u32EXFlag      = 0;
    stAioAttr.u32FrmNum      = 30;
    stAioAttr.u32PtNumPerFrm = AUDIO_PT_NUM_SIZE;		//每帧采样160个点，每个点时16bit，最终绝对字节数160*2 = 320 
    stAioAttr.u32ChnCnt      = 1;
    stAioAttr.u32ClkSel      = 0;

	//1.初始化音频配置
	iRet |= HI_AudConfigAcodec(&stAioAttr);
	
	//2.使能ai,默认不使用音质增强
	iRet |= HI_AudStartAi(&stAioAttr, NULL);
	
	//3.使能ao,默认不使用音质增强
	iRet |= HI_AudStartAo(&stAioAttr, NULL);
	
	//4.设置音量
	HI_AudSetVolume(6);
	
	audio_bing_flags = flags;
	if (flags)
	{
		//绑定模式
		iRet |= HI_AudViBindVo();
	}
	
	return iRet;
}

int hi3518_audio_deinit(void)
{
	int iRet = 0;
	if (audio_bing_flags)
	{
		//解除绑定
		iRet |= HI_AudViUnbindVo();
	}
	
	iRet |= HI_AudStopAi(NULL);
	iRet |= HI_AudStopAo(NULL);

	return iRet;
}

//音频采集开始，后台线程
int HI3515E_AudioSStart(hi_audio_cb p_cb)
{
	int iRet = 0;
    AI_CHN_PARAM_S stAiChnPara;

    iRet = HI_MPI_AI_GetChnParam(0, 0, &stAiChnPara);
    if (0 != iRet)
    {
        printf("%s: Get ai chn param failed\n", __FUNCTION__);
        return -1;
    }

    stAiChnPara.u32UsrFrmDepth = 30;
    
    iRet = HI_MPI_AI_SetChnParam(0, 0, &stAiChnPara);
    if (0 != iRet)
    {
        printf("%s: set ai chn param failed\n", __FUNCTION__);
        return -1;
    }

    audio_fd = HI_MPI_AI_GetFd(0, 0);	
	//将fd加入epoll
	
	if (efd != -1)
	{
		uepoll_add(efd, audio_fd);
	}
	

	audio_cb = p_cb;

	
	
	return iRet;
}
int HI3515E_AudioSStop(void)
{
	int iRet = 0;
	
	
	if (efd != -1)
	{
		uepoll_del(efd, audio_fd);
	}	
	
	//关闭相应的音频属性
	return iRet;
}


static int audioInputProcess()
{
	int iRet = 0;
	
    AUDIO_FRAME_S stFrame = {0};
	AEC_FRAME_S   stAecFrm = {0};
	char data[320] = {0};
	iRet = HI_MPI_AI_GetFrame(0, 0, &stFrame, &stAecFrm, HI_FALSE);	
	if (0 != iRet )
	{
		//直接退出
		return iRet;
	}	
	//回调用户
	if (audio_cb)
	{
		audio_cb(stFrame.pVirAddr[0], stFrame.u32Len);
	}

	//printf("------ Len = %d, TimeStamp = %d, Seq = %d, Bitwidth = %d, Soundmode = %d\r\n", \
				stFrame.u32Len, stFrame.u64TimeStamp, stFrame.u32Seq, stFrame.enBitwidth, stFrame.enSoundmode);

	
	//HI_MPI_AO_SendFrame(0, 0, &stFrame, 1000);
	
	iRet = HI_MPI_AI_ReleaseFrame(0, 0, &stFrame, &stAecFrm);
	if (0 != iRet )
	{
		printf("%s: HI_MPI_AI_ReleaseFrame(%d, %d), failed with %#x!\n", __FUNCTION__, 0, 0, iRet);
		HI3515E_AudioSStop();
	}	
	return iRet;
}


//音频播放，单帧播放直接调用,pcm格式
int hi3518_audio_output(char *p_data, char *file_name)
{
	int iRet = 0;

	AUDIO_FRAME_S stFrame; 

	stFrame.pVirAddr[0] = p_data;	//要保证是320个字节

	stFrame.u32Len = AUDIO_PT_NUM_SIZE * 2; //160 * 2 = 320
	//stFrame.enBitwidth	= 1;
	//stFrame.enSoundmode	= 0;
	iRet = HI_MPI_AO_SendFrame(0, 0, &stFrame, 1000);
	if (0 != iRet )
	{
		printf("%s: HI_MPI_AO_SendFrame(%d, %d), failed with %#x!\n",__FUNCTION__, 0, 0, iRet);
	}	
	
	return iRet;
}

/*********************************************************************************/



int HI3515E_VideoInit(void)
{
	T_VideoInfo *tVideo = &g_tVideoInfo;
	int iRet = 0;
	/**************************************/
	tVideo->emSize			= PIC_D1;
	tVideo->emRate			= SAMPLE_RC_VBR;
	tVideo->emPayload		= PT_H264;
	tVideo->emNorm			= VIDEO_ENCODING_MODE_PAL;
	tVideo->m_iProfile		= 2;
	tVideo->m_iVfd			= -1;
	tVideo->m_ucRunning		= 0;
	tVideo->pVideoCbk		= NULL;
	
	tVideo->in_tViConfig.enViMode   = SAMPLE_VI_MODE_BT1120_720P;		//摄像头类型，写死了
	tVideo->in_tViConfig.enRotate   = ROTATE_NONE;
	tVideo->in_tViConfig.enNorm     = VIDEO_ENCODING_MODE_AUTO;
	tVideo->in_tViConfig.enViChnSet = VI_CHN_SET_NORMAL;
	tVideo->in_tViConfig.enWDRMode  = WDR_MODE_NONE;
	/**************************************/
	SIZE_S stSize;
	
    iRet = HI_VidStartVi(&tVideo->in_tViConfig);	
    if (0 != iRet)
    {
        printf("start vi failed!\n");
        goto END_VENC_CLASSIC_1;
    }	
	
	iRet = HI_DevSystemGetPicSize(tVideo->emNorm, tVideo->emSize, &stSize);
    if (0 != iRet)
    {
        printf("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        goto END_VENC_CLASSIC_1;
    }	
	
	VPSS_GRP_ATTR_S stVpssGrpAttr;
	VPSS_CHN_ATTR_S stVpssChnAttr;
	VPSS_CHN_MODE_S stVpssChnMode;		
	
	stVpssGrpAttr.u32MaxW = stSize.u32Width;
	stVpssGrpAttr.u32MaxH = stSize.u32Height;
	stVpssGrpAttr.bIeEn = HI_FALSE;
	stVpssGrpAttr.bNrEn = HI_TRUE;
	stVpssGrpAttr.bHistEn = HI_FALSE;
	stVpssGrpAttr.bDciEn = HI_FALSE;
	stVpssGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
	stVpssGrpAttr.enPixFmt = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
	
	iRet = HI_DevVpssStartGroup(0, &stVpssGrpAttr);
	if (0 != iRet)
	{
		printf("Start Vpss failed!\n");
		goto END_VENC_CLASSIC_2;
	}	
	
	iRet = HI_VidViBindVpss(tVideo->in_tViConfig.enViMode);
	if (0 != iRet)
	{
		printf("Vi bind Vpss failed!\n");
		goto END_VENC_CLASSIC_3;
	}	
	
	stVpssChnMode.enChnMode      = VPSS_CHN_MODE_USER;
	stVpssChnMode.bDouble        = HI_FALSE;
	stVpssChnMode.enPixelFormat  = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
	stVpssChnMode.u32Width       = stSize.u32Width;
	stVpssChnMode.u32Height      = stSize.u32Height;
	stVpssChnMode.enCompressMode = COMPRESS_MODE_SEG;
	memset(&stVpssChnAttr, 0, sizeof(stVpssChnAttr));
	stVpssChnAttr.s32SrcFrameRate = -1;
	stVpssChnAttr.s32DstFrameRate = -1;
	iRet = HI_DevVpssEnableChn(0, 0, &stVpssChnAttr, &stVpssChnMode, HI_NULL);	
	if (0 != iRet)
	{
		printf("Enable vpss chn failed!\n");
		goto END_VENC_CLASSIC_4;
	}	
	iRet = HI_VidVencStart(0, tVideo->emPayload, tVideo->emNorm, tVideo->emSize, tVideo->emRate, tVideo->m_iProfile);	
	if (0 != iRet)
	{
		printf("Start Venc failed!\n");
		goto END_VENC_CLASSIC_5;
	}
	iRet = HI_VidVencBindVpss(0, 0, 0);
	if (0 != iRet)
	{
		printf("Start Venc failed!\n");
		goto END_VENC_CLASSIC_5;
	}	
	
	return 0;	
	
END_VENC_CLASSIC_5:
	HI_VidVencUnBindVpss(0, 0, 0);
	HI_VidVencStop(0);
    HI_VidViUnBindVpss(tVideo->in_tViConfig.enViMode);
END_VENC_CLASSIC_4:	
	HI_DevVpssDisableChn(0, 0);
END_VENC_CLASSIC_3:    
    HI_VidViUnBindVpss(tVideo->in_tViConfig.enViMode);
END_VENC_CLASSIC_2:    
    HI_DevVpssStopGroup(0);
END_VENC_CLASSIC_1:	
    HI_VidStopVi(&tVideo->in_tViConfig);
	
	return iRet;	
}

int HI3515E_VideoDeinit(void)
{
	T_VideoInfo *tVideo = &g_tVideoInfo;
	
	HI_VidVencUnBindVpss(0, 0, 0);
	HI_VidVencStop(0);
	HI_DevVpssDisableChn(0, 0);
    HI_VidViUnBindVpss(tVideo->in_tViConfig.enViMode);
    HI_DevVpssStopGroup(0);
    HI_VidStopVi(&tVideo->in_tViConfig);
	
	return 0;	
}

int HI3515E_VideoSStart(HI_VIDEO_CBK _pVideo)
{
	T_VideoInfo *tVideo = &g_tVideoInfo;
	int iRet = 0;
	
	if (tVideo->m_ucRunning)
	{
		return iRet;
	}
	else
	{
		tVideo->m_ucRunning = 1;
	}	
	
	tVideo->pVideoCbk		= _pVideo;
	
	tVideo->m_iVfd = HI_MPI_VENC_GetFd(0);

	if (efd != -1 && tVideo->m_iVfd != -1)
	{
		printf("++++ video_fd = %d\r\n", tVideo->m_iVfd);
		uepoll_add(efd, tVideo->m_iVfd);
	}

	return iRet;
}

int HI3515E_VideoSStop(void)
{
	T_VideoInfo *tVideo = &g_tVideoInfo;
	int iRet = 0;
	
	if (tVideo->m_ucRunning)
	{
		tVideo->m_ucRunning = 0;
	}
	else
	{
		return iRet;
	}	
	
	if (efd != -1)
	{
		uepoll_del(efd, tVideo->m_iVfd);
	}	

	return iRet;
}

FILE *file = NULL;

static int videoInputProcess(void)
{
	int iRet = 0, i;
	T_VideoInfo *tVideo = &g_tVideoInfo;
    VENC_CHN_STAT_S stStat;
    VENC_STREAM_S stStream;	
	
	memset(&stStream, 0, sizeof(stStream));
	iRet = HI_MPI_VENC_Query(0, &stStat);
	if (0 != iRet)
	{
		printf("HI_MPI_VENC_Query chn[%d] failed with %#x!\n", 0, iRet);
		return iRet;
	}	
	
	if(0 == stStat.u32CurPacks)
	{
	  printf("NOTE: Current  frame is NULL!\n");
	  return iRet;
	}	
	stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
	stStream.u32PackCount = stStat.u32CurPacks;
	iRet = HI_MPI_VENC_GetStream(0, &stStream, HI_TRUE);	
	if (0 != iRet)
	{
		free(stStream.pstPack);
		stStream.pstPack = NULL;
		printf("HI_MPI_VENC_GetStream failed with %#x!\n", iRet);
		goto EXIT;
	}

	//注意编码出来的是带海思头的
	//printf("xxxx u32PackCount = %d\r\n", stStream.u32PackCount);
	//HI_VidVencSaveStream(PT_H264, file, &stStream);
	for (i=0; i<stStream.u32PackCount; i++)
	{
		if (stStream.pstPack[i].DataType.enH264EType == H264E_NALU_SEI)
			continue;
		
		if (tVideo->pVideoCbk)
		{
			tVideo->pVideoCbk(stStream.pstPack[i].DataType.enH264EType,\
								stStream.pstPack[i].pu8Addr+stStream.pstPack[i].u32Offset,\
								stStream.pstPack[i].u32Len-stStream.pstPack[i].u32Offset);
		}	
	}

	iRet = HI_MPI_VENC_ReleaseStream(0, &stStream);
EXIT:
	free(stStream.pstPack);
	stStream.pstPack = NULL;	
	return iRet;
}






/*********************************************************************************/
static void *device_read_thread(void *arg)
{	
	T_VideoInfo *tVideo = &g_tVideoInfo;
	struct epoll_event	events[EPOLL_EVENT_NUM_MAX];  	
	int i,nfds, iRet;

	//file = fopen("test.h264", "wb");
	int fd = -1;
	while(1)
	{
		nfds = epoll_wait(efd, events, EPOLL_EVENT_NUM_MAX, 0);  //1000ms
		if (nfds <= 0)  
		{
			//超时
			//printf("userial epoll_wait..\r\n");
		}
		else
		{
			for (i = 0; i < nfds; i++)  
			{
				fd = events[i].data.fd;
				if (fd == audio_fd)
				{
					//音频采集处理
					audioInputProcess();
				}
				else if (fd == tVideo->m_iVfd)
				{
					//视频采集处理
					videoInputProcess();
					
				}
			}
		}
	}

	printf("device read thread has exit~");
	
    return NULL;    
}



//初始化系统
int HI3515E_DeviceInit(void)
{
	int iRet = 0;
	//1.初始化系统 mpi
#if 1	
	memset(&stVbConf, 0, sizeof(VB_CONF_S));
	//初始化vb写死的,根据图像大小
	HI_U32 u32HeaderSize = 0;
	SIZE_S stSize;
	stSize.u32Width = 720;//1280;	//tVideo->emSize 对应上
	stSize.u32Height= 576;//720;	
	HI_U32 u32AlignWidth = 64;
	PIXEL_FORMAT_E enPixFmt = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
	HI_U32 u32BlkSize = CEILING_2_POWER(stSize.u32Width, u32AlignWidth)*CEILING_2_POWER(stSize.u32Height,u32AlignWidth)*((PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixFmt)?2:1.5);
	//VB_PIC_HEADER_SIZE(stSize.u32Width, stSize.u32Height, enPixFmt, u32HeaderSize);
	stVbConf.u32MaxPoolCnt = 128;
	stVbConf.astCommPool[0].u32BlkSize = u32BlkSize + u32HeaderSize;
	stVbConf.astCommPool[0].u32BlkCnt = 4;
	stVbConf.astCommPool[1].u32BlkSize = 640;
	stVbConf.astCommPool[1].u32BlkCnt = 4;	
	printf("Vb size = %d, Vb cnt = %d\r\n", stVbConf.astCommPool[0].u32BlkSize, 4);
#endif		
    iRet |= HI_DevSystemInit(&stVbConf);	
	

	
	//2.创建主epoll
	if (uepoll_create(&efd))
	{
		return -1;
	}

	//创建线程
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	
	iRet = pthread_attr_setstacksize(&attr, 20*1024); //20K, 音频够用，视频估计不够
	if(0 != iRet) 
	{
		iRet = pthread_attr_destroy(&attr);
		printf("pthread_attr_setstacksize error");
		return -1;
	}

	iRet = pthread_create(&thread_id, &attr, device_read_thread, NULL);
	if(0 != iRet) 
	{
		iRet = pthread_attr_destroy(&attr);
		printf("pthread_create failed!");
		return -1;
	}
	
	pthread_attr_destroy(&attr);
	pthread_detach(thread_id);	

	return iRet;
}


/*****************************************************************/

/*****************************************************************/

int audio_data(char *p_data, int len)
{
	char audio_pt[320] = {0};

	memcpy(audio_pt, p_data, 320);
	hi3518_audio_output(audio_pt, NULL);
	
	
	return 0;
}



int video_data(int naltype, char *p_data, int len)
{

	printf("nal type %d, len %d\r\n", naltype, len);
	//fwrite(p_data, len, 1, file);
	//fflush(file);	
	
	return 0;
}



int main()
{
	int iRet = 0;
	iRet = HI3515E_DeviceInit();
	printf("device init %d\r\n", iRet);
	HI3515E_VideoInit();
	//iRet = hi3518_audio_init(0);	//不bind，可以使用bind模式来测试声音
	//printf("audio init %d\r\n", iRet);
	//HI3515E_AudioSStart(audio_data);
	
	
	HI3515E_VideoSStart(video_data);
	
	while(1)
	{
		sleep(1);
	}
	return 0;
}


