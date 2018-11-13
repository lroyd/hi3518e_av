/*************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: 
	> Created Time: 
 ************************************************************************/
#include "hi3518e_audio.h"
#include "hi3518e_debug.h"

#define DEV_AUDIO     "/dev/acodec"




//默认使用内部HI_ACODEC_TYPE_INNER
int HI_AUDIO_ConfigAcodec(AIO_ATTR_S *pstAioAttr)
{
    int iRet = 0, fd;
	int iAcodecInputVol = 0;
	ACODEC_FS_E i2s_fs_sel = 0;
	ACODEC_MIXER_E input_mode = 0;
    fd = open(DEV_AUDIO, O_RDWR);
    if (fd < 0)
    {
        HIAV_LOG(LOG_ERROR, "%s: can't open Acodec,%s.", __FUNCTION__, DEV_AUDIO);
        return -1;
    }
	
    if(ioctl(fd, ACODEC_SOFT_RESET_CTRL))
    {
    	HIAV_LOG(LOG_ERROR, "Reset audio codec error.");
    }

	switch (pstAioAttr->enSamplerate)
	{
		case AUDIO_SAMPLE_RATE_8000:
			i2s_fs_sel = ACODEC_FS_8000;
			break;

		case AUDIO_SAMPLE_RATE_16000:
			i2s_fs_sel = ACODEC_FS_16000;
			break;

		case AUDIO_SAMPLE_RATE_32000:
			i2s_fs_sel = ACODEC_FS_32000;
			break;

		case AUDIO_SAMPLE_RATE_11025:
			i2s_fs_sel = ACODEC_FS_11025;
			break;

		case AUDIO_SAMPLE_RATE_22050:
			i2s_fs_sel = ACODEC_FS_22050;
			break;

		case AUDIO_SAMPLE_RATE_44100:
			i2s_fs_sel = ACODEC_FS_44100;
			break;

		case AUDIO_SAMPLE_RATE_12000:
			i2s_fs_sel = ACODEC_FS_12000;
			break;

		case AUDIO_SAMPLE_RATE_24000:
			i2s_fs_sel = ACODEC_FS_24000;
			break;

		case AUDIO_SAMPLE_RATE_48000:
			i2s_fs_sel = ACODEC_FS_48000;
			break;

		case AUDIO_SAMPLE_RATE_64000:
			i2s_fs_sel = ACODEC_FS_64000;
			break;

		case AUDIO_SAMPLE_RATE_96000:
			i2s_fs_sel = ACODEC_FS_96000;
			break;
	
		default:
			HIAV_LOG(LOG_ERROR, "%s: not support enSample:%d.", __FUNCTION__, pstAioAttr->enSamplerate);
        	iRet = -1;
			break;
	}    

    if (ioctl(fd, ACODEC_SET_I2S1_FS, &i2s_fs_sel)) 
    {
        HIAV_LOG(LOG_ERROR, "%s: set acodec sample rate failed.", __FUNCTION__);
        iRet = -1;
    }

    //select IN or IN_Difference
    input_mode = ACODEC_MIXER_IN;
    if (ioctl(fd, ACODEC_SET_MIXER_MIC, &input_mode)) 
    {
        HIAV_LOG(LOG_ERROR, "%s: select acodec input_mode failed.", __FUNCTION__);
        iRet = -1;
    }
    
    if (0) /* should be 1 when micin */
    {
        iAcodecInputVol = 30;
        if (ioctl(fd, ACODEC_SET_INPUT_VOL, &iAcodecInputVol))
        {
            HIAV_LOG(LOG_ERROR, "%s: set acodec micin volume failed.", __FUNCTION__);
            return -1;
        }
    }
    
    close(fd);
    return iRet;
}

//
int HI_AUDIO_StartAi(AIO_ATTR_S* pstAioAttr, HI_VOID* pstAiVqeAttr)
{
    int iRet;
	//只有一个音频设备，默认音频号：0
    iRet = HI_MPI_AI_SetPubAttr(0, pstAioAttr);
    if (iRet)
    {
        HIAV_LOG(LOG_ERROR, "%s: HI_MPI_AI_SetPubAttr(%d) failed with %#x.", __FUNCTION__, 0, iRet);
        return iRet;
    }
	
    iRet = HI_MPI_AI_Enable(0);
	if (iRet)
    {
        HIAV_LOG(LOG_ERROR, "%s: HI_MPI_AI_Enable(%d) failed with %#x.", __FUNCTION__, 0, iRet);
        return iRet;
    }   
	

	iRet = HI_MPI_AI_EnableChn(0, 0);
	if (iRet)
	{
		HIAV_LOG(LOG_ERROR, "%s: HI_MPI_AI_EnableChn(%d,%d) failed with %#x.", __FUNCTION__, 0, 0, iRet);
		return iRet;
	}
#if 0	//重采样是否支持，默认关闭
	if (HI_TRUE == bResampleEn)
	{
		iRet = HI_MPI_AI_EnableReSmp(0, 0, enOutSampleRate);
		if (iRet)
		{
			HIAV_LOG(LOG_ERROR, "%s: HI_MPI_AI_EnableReSmp(%d,%d) failed with %#x.", __FUNCTION__, 0, 0, iRet);
			return iRet;
		}
	}
#endif
	if (NULL != pstAiVqeAttr)
	{
		HI_BOOL bAiVqe = HI_TRUE;

		iRet = HI_MPI_AI_SetVqeAttr(0, 0, 0, 0, (AI_VQE_CONFIG_S *)pstAiVqeAttr);

		if (iRet)
		{
			HIAV_LOG(LOG_ERROR, "%s: SetAiVqe%d(%d,%d) failed with %#x.", __FUNCTION__, 1, 0, 0, iRet);
			return iRet;
		}

		iRet = HI_MPI_AI_EnableVqe(0, 0);
		if (iRet)
		{
			HIAV_LOG(LOG_ERROR, "%s: HI_MPI_AI_EnableVqe(%d,%d) failed with %#x.", __FUNCTION__, 0, 0, iRet);
			return iRet;
		}

    }
    
    return 0;
}

int HI_AUDIO_StartAo(AIO_ATTR_S* pstAioAttr , HI_VOID* pstAoVqeAttr)
{
	//只有一个音频设备，默认音频号：0
    int iRet;

    iRet = HI_MPI_AO_SetPubAttr(0, pstAioAttr);
    if(0 != iRet)
    {
        HIAV_LOG(LOG_ERROR, "%s: HI_MPI_AO_SetPubAttr(%d) failed with %#x!", __FUNCTION__, \
               0,iRet);
        return -1;
    }
    
    iRet = HI_MPI_AO_Enable(0);
    if(0 != iRet)
    {
        HIAV_LOG(LOG_ERROR, "%s: HI_MPI_AO_Enable(%d) failed with %#x!", __FUNCTION__, 0, iRet);
        return -1;
    }

	iRet = HI_MPI_AO_EnableChn(0, 0);
	if (0 != iRet)
	{
		HIAV_LOG(LOG_ERROR, "%s: HI_MPI_AO_EnableChn(%d) failed with %#x!", __FUNCTION__, 0, iRet);
		return -1;
	}
#if 0	//重采样是否支持，默认关闭
	iRet = HI_MPI_AO_DisableReSmp(0, 0);
	iRet |= HI_MPI_AO_EnableReSmp(0, 0, enInSampleRate);
	if (0 != iRet)
	{
		HIAV_LOG(LOG_ERROR, "%s: HI_MPI_AO_EnableReSmp(%d,%d) failed with %#x!", __FUNCTION__, 0, 0, iRet);
		return -1;
	}
#endif	

	if (NULL != pstAoVqeAttr)
	{
		iRet = HI_MPI_AO_SetVqeAttr(0, 0, (AO_VQE_CONFIG_S *)pstAoVqeAttr);

		if (iRet)
		{
			HIAV_LOG(LOG_ERROR, "%s: SetAoVqe%d(%d,%d) failed with %#x.", __FUNCTION__, 1, 0, 0, iRet);
			return iRet;
		}

		iRet = HI_MPI_AO_EnableVqe(0, 0);
		if (iRet)
		{
			HIAV_LOG(LOG_ERROR, "%s: HI_MPI_AI_EnableVqe(%d,%d) failed with %#x.", __FUNCTION__, 0, 0, iRet);
			return iRet;
		}

	}

    return 0;	
}

//音量范围 (-12, 6)
int HI_AUDIO_SetVolume(int volume)
{
	int iRet;
	iRet = HI_MPI_AO_SetVolume(0, volume);
	if(0 != iRet)
	{
		HIAV_LOG(LOG_ERROR, "%s: HI_MPI_AO_SetVolume(%d), failed with %#x!", __FUNCTION__, 0, iRet);
	}
	
	return iRet;
}

int HI_AUDIO_StopAi(HI_VOID* pstAiVqeAttr)
{
    int iRet;
    
#if 0
	iRet = HI_MPI_AI_DisableReSmp(0, 0);
	if(0 != iRet)
	{
		HIAV_LOG(LOG_ERROR, "[Func]:%s [Line]:%d [Info]:%s.", __FUNCTION__, __LINE__, "failed");
		return iRet;
	}
#endif
	if (pstAiVqeAttr)
	{
		iRet = HI_MPI_AI_DisableVqe(0, 0);
		if(0 != iRet)
		{
			HIAV_LOG(LOG_ERROR, "[Func]:%s [Line]:%d [Info]:%s.", __FUNCTION__, __LINE__, "failed");
			return iRet;
		}
	}
        
	iRet = HI_MPI_AI_DisableChn(0, 0);
	if(0 != iRet)
	{
		HIAV_LOG(LOG_ERROR, "[Func]:%s [Line]:%d [Info]:%s.", __FUNCTION__, __LINE__, "failed");
		return iRet;
	}

    
    iRet = HI_MPI_AI_Disable(0);
    if(0 != iRet)
    {
        HIAV_LOG(LOG_ERROR, "[Func]:%s [Line]:%d [Info]:%s.", __FUNCTION__, __LINE__, "failed");
        return iRet;
    }
    
    return 0;
}

int HI_AUDIO_StopAo(HI_VOID* pstAoVqeAttr)
{

    int iRet;

#if 0
	iRet = HI_MPI_AO_DisableReSmp(0, 0);
	if (0 != iRet)
	{
		HIAV_LOG(LOG_ERROR, "%s: HI_MPI_AO_DisableReSmp failed with %#x!", __FUNCTION__, iRet);
		return iRet;
	}
#endif	

	if (pstAoVqeAttr)
	{
		iRet = HI_MPI_AO_DisableVqe(0, 0);
		if (0 != iRet)
		{
			HIAV_LOG(LOG_ERROR, "[Func]:%s [Line]:%d [Info]:%s.", __FUNCTION__, __LINE__, "failed");
			return iRet;
		}
	}
	iRet = HI_MPI_AO_DisableChn(0, 0);
	if (0 != iRet)
	{
		HIAV_LOG(LOG_ERROR, "%s: HI_MPI_AO_DisableChn failed with %#x!", __FUNCTION__, iRet);
		return iRet;
	}

	
    iRet = HI_MPI_AO_Disable(0);
    if (0 != iRet)
    {
        HIAV_LOG(LOG_ERROR, "%s: HI_MPI_AO_Disable failed with %#x!", __FUNCTION__, iRet);
        return iRet;
    }

    return 0;
}

//编码大小 默认320
//支持g711a g711u g725 
int HI_AUDIO_StartVenc(PAYLOAD_TYPE_E enType)
{
    int iRet;
    AENC_CHN_ATTR_S stAencAttr;
    AENC_ATTR_ADPCM_S stAdpcmAenc;
    AENC_ATTR_G711_S stAencG711;
    AENC_ATTR_G726_S stAencG726;
    AENC_ATTR_LPCM_S stAencLpcm;
    
    /* set AENC chn attr */
    
    stAencAttr.enType = enType;
    stAencAttr.u32BufSize = 30;
    stAencAttr.u32PtNumPerFrm = 320;
		
    if (PT_ADPCMA == stAencAttr.enType)
    {
        stAencAttr.pValue       = &stAdpcmAenc;
        stAdpcmAenc.enADPCMType = ADPCM_TYPE_DVI4;
    }
    else if (PT_G711A == stAencAttr.enType || PT_G711U == stAencAttr.enType)
    {
        stAencAttr.pValue       = &stAencG711;
    }
    else if (PT_G726 == stAencAttr.enType)
    {
        stAencAttr.pValue       = &stAencG726;
        stAencG726.enG726bps    = MEDIA_G726_40K;
    }
    else if (PT_LPCM == stAencAttr.enType)
    {
        stAencAttr.pValue = &stAencLpcm;
    }
    else
    {
        HIAV_LOG(LOG_ERROR, "%s: invalid aenc payload type:%d.", __FUNCTION__, stAencAttr.enType);
        return -1;
    }    
	/* create aenc chn*/
	iRet = HI_MPI_AENC_CreateChn(0, &stAencAttr);
	if (0 != iRet)
	{
		HIAV_LOG(LOG_ERROR, "%s: HI_MPI_AENC_CreateChn(%d) failed with %#x!", __FUNCTION__, 0, iRet);
		return iRet;
	}        

    return 0;
}

int HI_AUDIO_StopVenc(void)
{
    return HI_MPI_AENC_DestroyChn(0);
}

int HI_AUDIO_ViBindVo(void)
{
    MPP_CHN_S stSrcChn,stDestChn;

    stSrcChn.enModId = HI_ID_AI;
    stSrcChn.s32ChnId = 0;
    stSrcChn.s32DevId = 0;
    stDestChn.enModId = HI_ID_AO;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = 0;
    return HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);	
}

int HI_AUDIO_ViUnBindVo(void)
{
    MPP_CHN_S stSrcChn,stDestChn;

    stSrcChn.enModId = HI_ID_AI;
    stSrcChn.s32ChnId = 0;
    stSrcChn.s32DevId = 0;
    stDestChn.enModId = HI_ID_AO;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = 0;
    return HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn); 
}









