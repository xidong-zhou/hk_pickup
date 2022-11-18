/*
 *      Author: zxd
 * Create Date: 2022-11-15
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include "../include/get_voice.h"
#include "sdk/HCNetSDK.h"
#include <ros/ros.h>
#include <std_msgs/UInt8.h> 
// #include <std_msgs/Uint8.h>   

#ifndef _TRANS_H
#define _TRANS_H_


#define  HPR_OK 0
#define  HPR_ERROR -1

using namespace std;

LONG lVoiceHanle;
long lUserID;

extern char* Get_Voice(unsigned int mode, int &size);
extern int scanKeyboard();
extern void Record_Voice(vector<unsigned char> vec_in);

class trans_hk  
{
 public:
    trans_hk() ;
    
    void Init();


    NET_DVR_USER_LOGIN_INFO struLoginInfo = {0};
    NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = {0};
    
    NET_DVR_COMPRESSION_AUDIO lpCompressAudio = {0};
    
    ros::NodeHandle nh;  
    ros::Subscriber sub_voice;
    void voiceCallback(const std_msgs::UInt8::ConstPtr& msg);
    static void SendVoice(char *pRecvDataBuffer);

 private: 
    int Demo_VoiceTrans();
    int Log_In();
    
};

#endif


