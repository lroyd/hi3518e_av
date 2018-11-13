/*************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: 
	> Created Time: 
 ************************************************************************/
#include "hi3518e_device.h"
#include "hi3518e_debug.h"




int HI_DEVICE_SystemInit(VB_CONF_S *pstVbConf)
{
	int ret = -1;
    HI_MPI_SYS_Exit();
    HI_MPI_VB_Exit();

    if (NULL == pstVbConf)
    {
        HIAV_LOG(LOG_ERROR, "input parameter is null, it is invaild!");
        return -1;
    }

    ret = HI_MPI_VB_SetConf(pstVbConf);
    if (0 != ret)
    {
        HIAV_LOG(LOG_ERROR, "HI_MPI_VB_SetConf failed!");
        return -1;
    }

    ret = HI_MPI_VB_Init();
    if (0 != ret)
    {
        HIAV_LOG(LOG_ERROR, "HI_MPI_VB_Init failed!");
        return -1;
    }
	MPP_SYS_CONF_S stSysConf = {0};
    stSysConf.u32AlignWidth = SAMPLE_SYS_ALIGN_WIDTH;
    ret = HI_MPI_SYS_SetConf(&stSysConf);
    if (0 != ret)
    {
        HIAV_LOG(LOG_ERROR, "HI_MPI_SYS_SetConf failed");
        return -1;
    }

    ret = HI_MPI_SYS_Init();
    if (0 != ret)
    {
        HIAV_LOG(LOG_ERROR, "HI_MPI_SYS_Init failed!");
        return -1;
    }

    return 0;	
}


void HI_DEVICE_SystemExit(void)
{
    HI_MPI_SYS_Exit();
    HI_MPI_VB_Exit();
    return;
}

int HI_DEVICE_GetPicSize(VIDEO_NORM_E enNorm, PIC_SIZE_E enPicSize, SIZE_S *pstSize)
{
    switch (enPicSize)
    {
        case PIC_QCIF:
            pstSize->u32Width  = 176;
            pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?144:120;
            break;
        case PIC_CIF:
            pstSize->u32Width  = 352;
            pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?288:240;
            break;
        case PIC_D1:
            pstSize->u32Width  = 720;
            pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
            break;
        case PIC_960H:
            pstSize->u32Width  = 960;
            pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
            break;			
        case PIC_2CIF:
            pstSize->u32Width  = 360;
            pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
            break;
        case PIC_QVGA:    /* 320 * 240 */
            pstSize->u32Width  = 320;
            pstSize->u32Height = 240;
            break;
        case PIC_VGA:     /* 640 * 480 */
            pstSize->u32Width  = 640;
            pstSize->u32Height = 480;
            break;
        case PIC_XGA:     /* 1024 * 768 */
            pstSize->u32Width  = 1024;
            pstSize->u32Height = 768;
            break;
        case PIC_SXGA:    /* 1400 * 1050 */
            pstSize->u32Width  = 1400;
            pstSize->u32Height = 1050;
            break;
        case PIC_UXGA:    /* 1600 * 1200 */
            pstSize->u32Width  = 1600;
            pstSize->u32Height = 1200;
            break;
        case PIC_QXGA:    /* 2048 * 1536 */
            pstSize->u32Width  = 2048;
            pstSize->u32Height = 1536;
            break;
        case PIC_WVGA:    /* 854 * 480 */
            pstSize->u32Width  = 854;
            pstSize->u32Height = 480;
            break;
        case PIC_WSXGA:   /* 1680 * 1050 */
            pstSize->u32Width = 1680;
            pstSize->u32Height = 1050;
            break;
        case PIC_WUXGA:   /* 1920 * 1200 */
            pstSize->u32Width  = 1920;
            pstSize->u32Height = 1200;
            break;
        case PIC_WQXGA:   /* 2560 * 1600 */
            pstSize->u32Width  = 2560;
            pstSize->u32Height = 1600;
            break;
        case PIC_HD720:   /* 1280 * 720 */
            pstSize->u32Width  = 1280;
            pstSize->u32Height = 720;
            break;
        case PIC_HD1080:  /* 1920 * 1080 */
            pstSize->u32Width  = 1920;
            pstSize->u32Height = 1080;
            break;
        case PIC_5M:      /* 2592 * 1944 */
            pstSize->u32Width  = 2592;
            pstSize->u32Height = 1944;
            break;
            
        default:
            return -1;
    }
    return HI_SUCCESS;
}



int HI_DEVICE_CalcPicVbBlkSize(VIDEO_NORM_E enNorm, PIC_SIZE_E enPicSize, PIXEL_FORMAT_E enPixFmt, int u32AlignWidth)
{
    int s32Ret = -1;
    SIZE_S stSize;
    int u32VbSize;
    int u32HeaderSize;

    s32Ret = HI_DEVICE_GetPicSize(enNorm, enPicSize, &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        HIAV_LOG(LOG_ERROR, "get picture size[%d] failed!", enPicSize);
		return -1;
    }

    if (PIXEL_FORMAT_YUV_SEMIPLANAR_422 != enPixFmt && PIXEL_FORMAT_YUV_SEMIPLANAR_420 != enPixFmt)
    {
        HIAV_LOG(LOG_ERROR, "pixel format[%d] input failed!\n", enPixFmt);
		return -1;
    }

    if (16!=u32AlignWidth && 32!=u32AlignWidth && 64!=u32AlignWidth)
    {
        HIAV_LOG(LOG_ERROR, "system align width[%d] input failed!", u32AlignWidth);
        return -1;
    }
    //printf("w:%d, u32AlignWidth:%d\n", CEILING_2_POWER(stSize.u32Width,u32AlignWidth), u32AlignWidth);
    u32VbSize = (CEILING_2_POWER(stSize.u32Width, u32AlignWidth) * \
            CEILING_2_POWER(stSize.u32Height,u32AlignWidth) * \
           ((PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixFmt)?2:1.5));

    VB_PIC_HEADER_SIZE(stSize.u32Width, stSize.u32Height, enPixFmt, u32HeaderSize);
    u32VbSize += u32HeaderSize;

    return u32VbSize;
}

/////////////////////////////////////////////////////////////////
//ISP部分
void HI_DEVICE_ISPStop(void)
{

    return;

}


/////////////////////////////////////////////////////////////////
//VPSS部分
int HI_DEVICE_VpssMemConfig(void)
{
    HI_CHAR * pcMmzName;
    MPP_CHN_S stMppChnVpss;
    int s32Ret, i;

    /*vpss group max is 64, not need config vpss chn.*/
    for(i=0;i<64;i++)
    {
        stMppChnVpss.enModId  = HI_ID_VPSS;
        stMppChnVpss.s32DevId = i;
        stMppChnVpss.s32ChnId = 0;

        if(0 == (i%2))
        {
            pcMmzName = NULL;  
        }
        else
        {
            pcMmzName = "ddr1";
        }

        /*vpss*/
        s32Ret = HI_MPI_SYS_SetMemConf(&stMppChnVpss, pcMmzName);
        if (HI_SUCCESS != s32Ret)
        {
            HIAV_LOG(LOG_ERROR, "Vpss HI_MPI_SYS_SetMemConf ERR !");
            return -1;
        }
    }
    return HI_SUCCESS;
}


int HI_DEVICE_VpssStartGroup(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S *pstVpssGrpAttr)
{
    int s32Ret;
    VPSS_NR_PARAM_U unNrParam = {{0}};
    
    if (VpssGrp < 0 || VpssGrp > VPSS_MAX_GRP_NUM)
    {
        HIAV_LOG(LOG_ERROR, "VpssGrp%d is out of rang.", VpssGrp);
        return -1;
    }

    if (HI_NULL == pstVpssGrpAttr)
    {
        HIAV_LOG(LOG_ERROR, "null ptr,line%d.", __LINE__);
        return -1;
    }

    s32Ret = HI_MPI_VPSS_CreateGrp(VpssGrp, pstVpssGrpAttr);
    if (s32Ret != HI_SUCCESS)
    {
        HIAV_LOG(LOG_ERROR, "HI_MPI_VPSS_CreateGrp failed with %#x!", s32Ret);
        return -1;
    }

    /*** set vpss 3DNR param ***/
    s32Ret = HI_MPI_VPSS_GetNRParam(VpssGrp, &unNrParam);
    if (s32Ret != HI_SUCCESS)
    {
        HIAV_LOG(LOG_ERROR, "failed with %#x!", s32Ret);
        return -1;
    }
    
    
    s32Ret = HI_MPI_VPSS_SetNRParam(VpssGrp, &unNrParam);
    if (s32Ret != HI_SUCCESS)
    {
        HIAV_LOG(LOG_ERROR, "failed with %#x!", s32Ret);
        return -1;
    }

    s32Ret = HI_MPI_VPSS_StartGrp(VpssGrp);
    if (s32Ret != HI_SUCCESS)
    {
        HIAV_LOG(LOG_ERROR, "HI_MPI_VPSS_StartGrp failed with %#x", s32Ret);
        return -1;
    }

    return HI_SUCCESS;
}

int HI_DEVICE_VpssEnableChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, 
                                                  VPSS_CHN_ATTR_S *pstVpssChnAttr,
                                                  VPSS_CHN_MODE_S *pstVpssChnMode,
                                                  VPSS_EXT_CHN_ATTR_S *pstVpssExtChnAttr)
{
    int s32Ret;

    if (VpssGrp < 0 || VpssGrp > VPSS_MAX_GRP_NUM)
    {
        HIAV_LOG(LOG_ERROR, "VpssGrp%d is out of rang[0,%d].", VpssGrp, VPSS_MAX_GRP_NUM);
        return -1;
    }

    if (VpssChn < 0 || VpssChn > VPSS_MAX_CHN_NUM)
    {
        HIAV_LOG(LOG_ERROR, "VpssChn%d is out of rang[0,%d].", VpssChn, VPSS_MAX_CHN_NUM);
        return -1;
    }

    if (HI_NULL == pstVpssChnAttr && HI_NULL == pstVpssExtChnAttr)
    {
        HIAV_LOG(LOG_ERROR, "null ptr,line%d.", __LINE__);
        return -1;
    }

    if (VpssChn < VPSS_MAX_PHY_CHN_NUM)
    {
        s32Ret = HI_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, pstVpssChnAttr);
        if (s32Ret != HI_SUCCESS)
        {
            HIAV_LOG(LOG_ERROR, "HI_MPI_VPSS_SetChnAttr failed with %#x", s32Ret);
            return -1;
        }
    }
    else
    {
        s32Ret = HI_MPI_VPSS_SetExtChnAttr(VpssGrp, VpssChn, pstVpssExtChnAttr);
        if (s32Ret != HI_SUCCESS)
        {
            HIAV_LOG(LOG_ERROR, "%s failed with %#x", __FUNCTION__, s32Ret);
            return -1;
        }
    }
    
    if (VpssChn < VPSS_MAX_PHY_CHN_NUM && HI_NULL != pstVpssChnMode)
    {
        s32Ret = HI_MPI_VPSS_SetChnMode(VpssGrp, VpssChn, pstVpssChnMode);
        if (s32Ret != HI_SUCCESS)
        {
            HIAV_LOG(LOG_ERROR, "%s failed with %#x", __FUNCTION__, s32Ret);
            return -1;
        }     
    }
    
    s32Ret = HI_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
    if (s32Ret != HI_SUCCESS)
    {
        HIAV_LOG(LOG_ERROR, "HI_MPI_VPSS_EnableChn failed with %#x", s32Ret);
        return -1;
    }

    return HI_SUCCESS;
}

int HI_DEVICE_VpssStopGroup(VPSS_GRP VpssGrp)
{
    int s32Ret;

    if (VpssGrp < 0 || VpssGrp > VPSS_MAX_GRP_NUM)
    {
        HIAV_LOG(LOG_ERROR, "VpssGrp%d is out of rang[0,%d].", VpssGrp, VPSS_MAX_GRP_NUM);
        return -1;
    }

    s32Ret = HI_MPI_VPSS_StopGrp(VpssGrp);
    if (s32Ret != HI_SUCCESS)
    {
        HIAV_LOG(LOG_ERROR, "%s failed with %#x", __FUNCTION__, s32Ret);
        return -1;
    }

    s32Ret = HI_MPI_VPSS_DestroyGrp(VpssGrp);
    if (s32Ret != HI_SUCCESS)
    {
        HIAV_LOG(LOG_ERROR, "%s failed with %#x", __FUNCTION__, s32Ret);
        return -1;
    }

    return HI_SUCCESS;
}

int HI_DEVICE_VpssDisableChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn)
{
    int s32Ret;

    if (VpssGrp < 0 || VpssGrp > VPSS_MAX_GRP_NUM)
    {
        HIAV_LOG(LOG_ERROR, "VpssGrp%d is out of rang[0,%d].", VpssGrp, VPSS_MAX_GRP_NUM);
        return -1;
    }

    if (VpssChn < 0 || VpssChn > VPSS_MAX_CHN_NUM)
    {
        HIAV_LOG(LOG_ERROR, "VpssChn%d is out of rang[0,%d].", VpssChn, VPSS_MAX_CHN_NUM);
        return -1;
    }
    
    s32Ret = HI_MPI_VPSS_DisableChn(VpssGrp, VpssChn);
    if (s32Ret != HI_SUCCESS)
    {
        HIAV_LOG(LOG_ERROR, "%s failed with %#x", __FUNCTION__, s32Ret);
        return -1;
    }

    return HI_SUCCESS;
}

int HI_DEVICE_VpssStart(int s32GrpCnt, SIZE_S *pstSize, int s32ChnCnt,VPSS_GRP_ATTR_S *pstVpssGrpAttr)
{
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VPSS_GRP_ATTR_S stGrpAttr = {0};
    VPSS_CHN_ATTR_S stChnAttr = {0};
    VPSS_NR_PARAM_U unNrParam = {{0}};
    int s32Ret;
    int i, j;

    /*** Set Vpss Grp Attr ***/

    if(NULL == pstVpssGrpAttr)
    {
        stGrpAttr.u32MaxW = pstSize->u32Width;
        stGrpAttr.u32MaxH = pstSize->u32Height;
        stGrpAttr.bIeEn = HI_FALSE;
        stGrpAttr.bNrEn = HI_TRUE;
        stGrpAttr.bHistEn = HI_FALSE;
        stGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
        stGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;
    }
    else
    {
        memcpy(&stGrpAttr,pstVpssGrpAttr,sizeof(VPSS_GRP_ATTR_S));
    }
    

    for(i=0; i<s32GrpCnt; i++)
    {
        VpssGrp = i;
        /*** create vpss group ***/
        s32Ret = HI_MPI_VPSS_CreateGrp(VpssGrp, &stGrpAttr);
        if (s32Ret != HI_SUCCESS)
        {
            HIAV_LOG(LOG_ERROR, "HI_MPI_VPSS_CreateGrp failed with %#x!", s32Ret);
            return -1;
        }

        /*** set vpss param ***/
        s32Ret = HI_MPI_VPSS_GetNRParam(VpssGrp, &unNrParam);
        if (s32Ret != HI_SUCCESS)
        {
            HIAV_LOG(LOG_ERROR, "failed with %#x!", s32Ret);
            return -1;
        }        
        
        s32Ret = HI_MPI_VPSS_SetNRParam(VpssGrp, &unNrParam);
        if (s32Ret != HI_SUCCESS)
        {
            HIAV_LOG(LOG_ERROR, "failed with %#x!", s32Ret);
            return -1;
        }

        /*** enable vpss chn, with frame ***/
        for(j=0; j<s32ChnCnt; j++)
        {
            VpssChn = j;
            /* Set Vpss Chn attr */
            stChnAttr.bSpEn = HI_FALSE;
            stChnAttr.bBorderEn = HI_TRUE;
            stChnAttr.stBorder.u32Color = 0xff00;
            stChnAttr.stBorder.u32LeftWidth = 2;
            stChnAttr.stBorder.u32RightWidth = 2;
            stChnAttr.stBorder.u32TopWidth = 2;
            stChnAttr.stBorder.u32BottomWidth = 2;
            
            s32Ret = HI_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stChnAttr);
            if (s32Ret != HI_SUCCESS)
            {
                HIAV_LOG(LOG_ERROR, "HI_MPI_VPSS_SetChnAttr failed with %#x", s32Ret);
                return -1;
            }
    
            s32Ret = HI_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
            if (s32Ret != HI_SUCCESS)
            {
                HIAV_LOG(LOG_ERROR, "HI_MPI_VPSS_EnableChn failed with %#x", s32Ret);
                return -1;
            }
        }

        /*** start vpss group ***/
        s32Ret = HI_MPI_VPSS_StartGrp(VpssGrp);
        if (s32Ret != HI_SUCCESS)
        {
            HIAV_LOG(LOG_ERROR, "HI_MPI_VPSS_StartGrp failed with %#x", s32Ret);
            return -1;
        }

    }
    return HI_SUCCESS;
}

int HI_DEVICE_VpssStop(int s32GrpCnt, int s32ChnCnt)
{
    int i, j;
    int s32Ret = HI_SUCCESS;
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;

    for(i=0; i<s32GrpCnt; i++)
    {
        VpssGrp = i;
        s32Ret = HI_MPI_VPSS_StopGrp(VpssGrp);
        if (s32Ret != HI_SUCCESS)
        {
            HIAV_LOG(LOG_ERROR, "failed with %#x!", s32Ret);
            return -1;
        }
        for(j=0; j<s32ChnCnt; j++)
        {
            VpssChn = j;
            s32Ret = HI_MPI_VPSS_DisableChn(VpssGrp, VpssChn);
            if (s32Ret != HI_SUCCESS)
            {
                HIAV_LOG(LOG_ERROR, "failed with %#x!", s32Ret);
                return -1;
            }
        }
    
        s32Ret = HI_MPI_VPSS_DestroyGrp(VpssGrp);
        if (s32Ret != HI_SUCCESS)
        {
            HIAV_LOG(LOG_ERROR, "failed with %#x!", s32Ret);
            return -1;
        }
    }

    return HI_SUCCESS;
}







