#include "../include/trans.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include "../include/get_voice.h"


int mode = 101; //mode=0：停止播放；mode=1~4，播放不同语音文件；初值设为101防止与停止指令冲突
bool flag_first = true;  //是否为第一次进入回调，第一次进入回调读取音频文件，之后不再读取
char *Buff;  //保存待发送的音频数据
int fream = 0;  //记录发送了多少次数据，当达到音频文件大小时注销登录，并等待下一次传输指令
int size;  //音频文件的大小


void CALLBACK fdwVoiceDataCallBack(LONG lVoiceComHandle, char *pRecvDataBuffer, DWORD dwBufSize, BYTE byAudioFlag, DWORD pUser)
{

}

void CALLBACK fVoiceDataCallBack(LONG lVoiceComHandle, char *pRecvDataBuffer, DWORD dwBufSize, BYTE byAudioFlag, void* pUser)
{        
    trans_hk::SendVoice(pRecvDataBuffer);  //传输音频
    //trans_hk::SendVoice(pRecvDataBuffer);
    // {
        // printf("%d\n",mode);
    // }

}


void trans_hk::SendVoice(char *pRecvDataBuffer)
{
    if(mode > 0 && mode != 101) //接收到传输指令
    {
        char buff[160];  //保存单次发送的音频数据，单次只能发送160个字节
        if(flag_first)   //第一次进入该函数
        {
            Buff = Get_Voice(mode,size);   //获取音频数据及其大小
            
            flag_first = false; 
        }
        else if(160*fream < size)    //小于音频文件大小，继续发送
        {           
            for(int i=0;i<160;i++)   //获取剩下的音频数据
            {
                buff[i] = Buff[i+160*fream];
                
            }
            
            fream += 1;
            {
                BOOL ret = NET_DVR_VoiceComSendData(lVoiceHanle, buff, 160);   //发送         
                if (!ret)
                {
                    printf("NET_DVR_VoiceComSendData error, %d!\n", NET_DVR_GetLastError());
                }
            }   
        }
        else if(160*fream > size || 160*fream == size)   //已发送完成
        {
            mode = 0;
            printf("xxx\n");
        }
    }

}



int trans_hk::Log_In()   //登录
{
    //log in   
    if (lUserID < 0)
    {
        printf("Login failed, error code: %d\n", NET_DVR_GetLastError());
        NET_DVR_Cleanup();
        return 0;
    }
    
    if (Demo_VoiceTrans() == HPR_ERROR)
    {
        NET_DVR_Cleanup();
        return HPR_ERROR;
    }

    printf("pyd---Test Voice successfully!\n");
    NET_DVR_Cleanup();
    return HPR_OK;

}

int trans_hk::Demo_VoiceTrans()   //初始化配置
{      
    //check audio param
    BOOL ret = NET_DVR_GetCurrentAudioCompress(lUserID, &lpCompressAudio);
    if (!ret)
    {
        printf("NET_DVR_GetCurrentAudioCompress error, %d!\n", NET_DVR_GetLastError());
    }

    //start_voice
    lVoiceHanle = NET_DVR_StartVoiceCom_MR_V30(lUserID, 1, fVoiceDataCallBack, NULL);
    if (lVoiceHanle < 0)
    {
        printf("NET_DVR_StartVoiceCom_V30 error, %d!\n", NET_DVR_GetLastError());
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return 0;
    }

    // sleep(0.002);  //millisecond

    while(ros::ok())
    {
        if(mode==0)  //停止播放指令
        {
            //参数重新初始化
            mode = 101;
            flag_first = true;
            fream = 0;
            Buff[0] = '\0';
            printf("Logout\n");

            //关闭传输
            if (!NET_DVR_StopVoiceCom(lVoiceHanle))
            {
                printf("NET_DVR_StopVoiceCom error, %d!\n", NET_DVR_GetLastError());
                NET_DVR_Logout(lUserID);
                NET_DVR_Cleanup();
            }
        
            //注销用户
            NET_DVR_Logout(lUserID);
            //释放SDK资源
            NET_DVR_Cleanup();
        }
        ros::spinOnce();
    }
}

trans_hk::trans_hk()
{
    
}


void trans_hk::Init()  //注册回调函数
{
    sub_voice = nh.subscribe("/voice",2, &trans_hk::voiceCallback,this);
}


void trans_hk::voiceCallback(const std_msgs::UInt8::ConstPtr& msg)   
{   
    mode = msg->data;
    // printf("%d\n",mode);
    
    if (mode!=101 && mode!=0)   //配置初始化
    {
        NET_DVR_Init();
        printf("Login\n");
        struLoginInfo.bUseAsynLogin = 0;
        struLoginInfo.wPort = 8000;
        memcpy(struLoginInfo.sDeviceAddress, "192.168.4.164", NET_DVR_DEV_ADDRESS_MAX_LEN);
        memcpy(struLoginInfo.sUserName, "admin", NAME_LEN);
        memcpy(struLoginInfo.sPassword, "tmzx2022", NAME_LEN);
        lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);
        
        Log_In();   //登录摄像头
    }
    
}

int main(int argc, char** argv)
{  
    ros::init(argc,argv,"hk_sdk");
	
    trans_hk trans_hk_;

    trans_hk_.Init();

    ros::spin();

	return 0;
}