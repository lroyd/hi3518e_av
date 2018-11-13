/*************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: 
	> Created Time: 
 ************************************************************************/
#include <stdio.h>
#include "hi3518e.h"


FILE *file = NULL;

int audio_data(char *p_data, int len)
{
	char audio_pt[320] = {0};

	memcpy(audio_pt, p_data, 320);
	HI3515E_AudioPlayer(audio_pt, NULL);
	
	
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
	//保存视频文件
	//file = fopen("test.h264", "wb");
	
	iRet = HI3515E_DeviceInit();
	printf("device init %d\r\n", iRet);
	HI3518E_VideoEnable();
	HI3518E_AudioEnable(0);	//不bind，可以使用bind模式来测试声音
	
	HI3515E_VideoSStart(video_data);
	HI3515E_AudioSStart(audio_data);
	
	while(1)
	{
		sleep(1);
	}
	return 0;
}


