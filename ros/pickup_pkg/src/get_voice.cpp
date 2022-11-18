#include "../include/get_voice.h"




int scanKeyboard()
{
    int in;
    struct termios new_settings;
    struct termios stored_settings;
    tcgetattr(0,&stored_settings);
    new_settings = stored_settings;
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_cc[VTIME] = 0;
    tcgetattr(0,&stored_settings);
    new_settings.c_cc[VMIN] = 1;
    tcsetattr(0,TCSANOW,&new_settings);
    
    in = getchar();
    
    tcsetattr(0,TCSANOW,&stored_settings);
    return in;
}

char* Get_Voice(unsigned int mode, int &size)
{
    FILE *in_file;
    char *inputfilename;
    char *ar;
    if(mode==1)
        inputfilename  = "/home/zxd/catkin_ws/src/pickup_pkg/voice/test.g711"; 
    else if(mode==2)
        inputfilename  = "/home/zxd/catkin_ws/src/pickup_pkg/voice/test2.g711";

    if ((in_file = fopen(inputfilename,"rb"))==0)
    {
        printf("error opening output file %s\n",inputfilename);
        return 0; 
    }       
    
    fseek(in_file, 0, SEEK_END);  
    size = ftell(in_file);  
    rewind(in_file);  
  
    //申请一块能装下整个文件的空间  
    ar = (char*)malloc(sizeof(char)*size);  
    printf("size %d\n",size);
    //读文件  
    int num_read = fread(ar,1,size,in_file);//每次读一个，共读size次  

    if (num_read==0)
    {
        fclose(in_file);
    }

    return ar;
}

void Record_Voice(vector<unsigned char> vec_in)
{
    FILE *in_file;
    int size = 0; 
    char *ar ;
    char *inputfilename;
    inputfilename  = "voice_record.pcm"; 
    if ((in_file = fopen(inputfilename,"wb"))==0)
    {
        printf("error opening output file %s\n",inputfilename);
    }       

    unsigned char *inbuff;

    inbuff   = (unsigned char *)malloc(vec_in.size());

    memset(inbuff, 0, vec_in.size() * sizeof(unsigned char));

    for(int i=0;i<vec_in.size();i++)
    {
        // cout<<vec.size()<<endl;
        inbuff[i] = vec_in[i];
    }
    
    fwrite(inbuff,sizeof(char),vec_in.size(),in_file);

}