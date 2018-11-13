/*************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 ************************************************************************/
#ifndef __HI3518E_H__
#define __HI3518E_H__


typedef int (*HI_AUDIO_CBK)(char *, int);

typedef int (*HI_VIDEO_CBK)(int, char *, int);


int HI3515E_DeviceInit(void);



int HI3518E_AudioEnable(unsigned char _ucFlags);
int HI3518E_AudioDisable(void);
int HI3515E_AudioSStart(HI_AUDIO_CBK);
int HI3515E_AudioSStop(void);
int HI3515E_AudioPlayer(char *p_data, char *file_name);


int HI3518E_VideoEnable(void);
int HI3518E_VideoDisable(void);
int HI3515E_VideoSStart(HI_VIDEO_CBK _pVideo);
int HI3515E_VideoSStop(void);




#endif


