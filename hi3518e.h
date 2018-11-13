/*************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 ************************************************************************/
#ifndef __HI3518E_H__
#define __HI3518E_H__


typedef int (*hi_audio_cb)(char *, int);

typedef int (*HI_VIDEO_CBK)(int, char *, int);


int HI3515E_DeviceInit(void);


int HI3515E_AudioSStart(hi_audio_cb);
int HI3515E_AudioSStop(void);
//int hi3518_audio_output(char *p_data, char *file_name);




#endif


