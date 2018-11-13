/*************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: 
	> Created Time: 
 ************************************************************************/
#include "hi3518e_audio.h"

#define DEV_AUDIO     "/dev/acodec"




//默认使用内部HI_ACODEC_TYPE_INNER
int HI_AudConfigAcodec(AIO_ATTR_S *pstAioAttr)
{
    int ret = 0, fd;
	int iAcodecInputVol = 0;
	ACODEC_FS_E i2s_fs_sel = 0;
	ACODEC_MIXER_E input_mode = 0;
    fd = open(DEV_AUDIO, O_RDWR);
    if (fd < 0)
    {
        printf("%s: can't open Acodec,%s\n", __FUNCTION__, DEV_AUDIO);
        return -1;
    }
	
    if(ioctl(fd, ACODEC_SOFT_RESET_CTRL))
    {
    	printf("Reset audio codec error\n");
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
			printf("%s: not support enSample:%d\n", __FUNCTION__, pstAioAttr->enSamplerate);
        	ret = -1;
			break;
	}    

    if (ioctl(fd, ACODEC_SET_I2S1_FS, &i2s_fs_sel)) 
    {
        printf("%s: set acodec sample rate failed\n", __FUNCTION__);
        ret = -1;
    }

    //select IN or IN_Difference
    input_mode = ACODEC_MIXER_IN;
    if (ioctl(fd, ACODEC_SET_MIXER_MIC, &input_mode)) 
    {
        printf("%s: select acodec input_mode failed\n", __FUNCTION__);
        ret = -1;
    }
    
    if (0) /* should be 1 when micin */
    {
        iAcodecInputVol = 30;
        if (ioctl(fd, ACODEC_SET_INPUT_VOL, &iAcodecInputVol))
        {
            printf("%s: set acodec micin volume failed\n", __FUNCTION__);
            return -1;
        }
    }
    
    close(fd);
    return ret;
}

//
int HI_AudStartAi(AIO_ATTR_S* pstAioAttr, HI_VOID* pstAiVqeAttr)
{
    int ret;
	//只有一个音频设备，默认音频号：0
    ret = HI_MPI_AI_SetPubAttr(0, pstAioAttr);
    if (ret)
    {
        printf("%s: HI_MPI_AI_SetPubAttr(%d) failed with %#x\n", __FUNCTION__, 0, ret);
        return ret;
    }
	
    ret = HI_MPI_AI_Enable(0);
	if (ret)
    {
        printf("%s: HI_MPI_AI_Enable(%d) failed with %#x\n", __FUNCTION__, 0, ret);
        return ret;
    }   
	

	ret = HI_MPI_AI_EnableChn(0, 0);
	if (ret)
	{
		printf("%s: HI_MPI_AI_EnableChn(%d,%d) failed with %#x\n", __FUNCTION__, 0, 0, ret);
		return ret;
	}
#if 0	//重采样是否支持，默认关闭
	if (HI_TRUE == bResampleEn)
	{
		ret = HI_MPI_AI_EnableReSmp(0, 0, enOutSampleRate);
		if (ret)
		{
			printf("%s: HI_MPI_AI_EnableReSmp(%d,%d) failed with %#x\n", __FUNCTION__, 0, 0, ret);
			return ret;
		}
	}
#endif
	if (NULL != pstAiVqeAttr)
	{
		HI_BOOL bAiVqe = HI_TRUE;

		ret = HI_MPI_AI_SetVqeAttr(0, 0, 0, 0, (AI_VQE_CONFIG_S *)pstAiVqeAttr);

		if (ret)
		{
			printf("%s: SetAiVqe%d(%d,%d) failed with %#x\n", __FUNCTION__, 1, 0, 0, ret);
			return ret;
		}

		ret = HI_MPI_AI_EnableVqe(0, 0);
		if (ret)
		{
			printf("%s: HI_MPI_AI_EnableVqe(%d,%d) failed with %#x\n", __FUNCTION__, 0, 0, ret);
			return ret;
		}

    }
    
    return 0;
}

int HI_AudStartAo(AIO_ATTR_S* pstAioAttr , HI_VOID* pstAoVqeAttr)
{
	//只有一个音频设备，默认音频号：0
    int ret;

    ret = HI_MPI_AO_SetPubAttr(0, pstAioAttr);
    if(0 != ret)
    {
        printf("%s: HI_MPI_AO_SetPubAttr(%d) failed with %#x!\n", __FUNCTION__, \
               0,ret);
        return -1;
    }
    
    ret = HI_MPI_AO_Enable(0);
    if(0 != ret)
    {
        printf("%s: HI_MPI_AO_Enable(%d) failed with %#x!\n", __FUNCTION__, 0, ret);
        return -1;
    }

	ret = HI_MPI_AO_EnableChn(0, 0);
	if (0 != ret)
	{
		printf("%s: HI_MPI_AO_EnableChn(%d) failed with %#x!\n", __FUNCTION__, 0, ret);
		return -1;
	}
#if 0	//重采样是否支持，默认关闭
	ret = HI_MPI_AO_DisableReSmp(0, 0);
	ret |= HI_MPI_AO_EnableReSmp(0, 0, enInSampleRate);
	if (0 != ret)
	{
		printf("%s: HI_MPI_AO_EnableReSmp(%d,%d) failed with %#x!\n", __FUNCTION__, 0, 0, ret);
		return -1;
	}
#endif	

	if (NULL != pstAoVqeAttr)
	{
		ret = HI_MPI_AO_SetVqeAttr(0, 0, (AO_VQE_CONFIG_S *)pstAoVqeAttr);

		if (ret)
		{
			printf("%s: SetAoVqe%d(%d,%d) failed with %#x\n", __FUNCTION__, 1, 0, 0, ret);
			return ret;
		}

		ret = HI_MPI_AO_EnableVqe(0, 0);
		if (ret)
		{
			printf("%s: HI_MPI_AI_EnableVqe(%d,%d) failed with %#x\n", __FUNCTION__, 0, 0, ret);
			return ret;
		}

	}

    return 0;	
}

//音量范围 (-12, 6)
int HI_AudSetVolume(int volume)
{
	int ret;
	ret = HI_MPI_AO_SetVolume(0, volume);
	if(0 != ret)
	{
		printf("%s: HI_MPI_AO_SetVolume(%d), failed with %#x!\n", __FUNCTION__, 0, ret);
	}
	
	return ret;
}

int HI_AudStopAi(HI_VOID* pstAiVqeAttr)
{
    int ret;
    
#if 0
	ret = HI_MPI_AI_DisableReSmp(0, 0);
	if(0 != ret)
	{
		printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
		return ret;
	}
#endif
	if (pstAiVqeAttr)
	{
		ret = HI_MPI_AI_DisableVqe(0, 0);
		if(0 != ret)
		{
			printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
			return ret;
		}
	}
        
	ret = HI_MPI_AI_DisableChn(0, 0);
	if(0 != ret)
	{
		printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
		return ret;
	}

    
    ret = HI_MPI_AI_Disable(0);
    if(0 != ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
        return ret;
    }
    
    return 0;
}

int HI_AudStopAo(HI_VOID* pstAoVqeAttr)
{

    int ret;

#if 0
	ret = HI_MPI_AO_DisableReSmp(0, 0);
	if (0 != ret)
	{
		printf("%s: HI_MPI_AO_DisableReSmp failed with %#x!\n", __FUNCTION__, ret);
		return ret;
	}
#endif	

	if (pstAoVqeAttr)
	{
		ret = HI_MPI_AO_DisableVqe(0, 0);
		if (0 != ret)
		{
			printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
			return ret;
		}
	}
	ret = HI_MPI_AO_DisableChn(0, 0);
	if (0 != ret)
	{
		printf("%s: HI_MPI_AO_DisableChn failed with %#x!\n", __FUNCTION__, ret);
		return ret;
	}

	
    ret = HI_MPI_AO_Disable(0);
    if (0 != ret)
    {
        printf("%s: HI_MPI_AO_Disable failed with %#x!\n", __FUNCTION__, ret);
        return ret;
    }

    return 0;
}

//编码大小 默认320
//支持g711a g711u g725 
int HI_AudStartVenc(PAYLOAD_TYPE_E enType)
{
    int ret;
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
        printf("%s: invalid aenc payload type:%d\n", __FUNCTION__, stAencAttr.enType);
        return -1;
    }    
	/* create aenc chn*/
	ret = HI_MPI_AENC_CreateChn(0, &stAencAttr);
	if (0 != ret)
	{
		printf("%s: HI_MPI_AENC_CreateChn(%d) failed with %#x!\n", __FUNCTION__, 0, ret);
		return ret;
	}        

    return 0;
}

int HI_AudStopVenc(void)
{
    return HI_MPI_AENC_DestroyChn(0);
}

int HI_AudViBindVo(void)
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

int HI_AudViUnbindVo(void)
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









