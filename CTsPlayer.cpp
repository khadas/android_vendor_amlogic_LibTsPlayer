#include "CTsPlayer.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/system_properties.h>
#include <cutils/properties.h>
#include <fcntl.h>
//#include "player.h"
#include "player_set_sys.h"

#include <sys/times.h>



//#include "player_set_sys.c"
//#include "../LibPlayer/amplayer/player/include/player.h"

//#include "../IPTVPlayer/PubAndroid.h"	 
using namespace android;

#define DPrint(x)

#define M_LIVE	1
#define M_TVOD	2
#define M_VOD	3
#define RES_VIDEO_SIZE 256
#define RES_AUDIO_SIZE 64
#define UNIT_FREQ   96000
#define MAX_WRITE_COUNT 20


#ifndef FBIOPUT_OSD_SRCCOLORKEY
#define  FBIOPUT_OSD_SRCCOLORKEY    0x46fb
#endif

#ifndef FBIOPUT_OSD_SRCKEY_ENABLE
#define  FBIOPUT_OSD_SRCKEY_ENABLE  0x46fa
#endif


#ifndef FBIOPUT_OSD_SET_GBL_ALPHA
#define  FBIOPUT_OSD_SET_GBL_ALPHA  0x4500
#endif

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "TsPlayer", __VA_ARGS__) 
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG , "TsPlayer", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  , "TsPlayer", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN  , "TsPlayer", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , "TsPlayer", __VA_ARGS__)




#define  FBIOPUT_OSD_FREE_SCALE_ENABLE	0x4504
#define  FBIOPUT_OSD_FREE_SCALE_WIDTH	0x4505
#define  FBIOPUT_OSD_FREE_SCALE_HEIGHT	0x4506


//log switch
char prop_shouldshowlog;


typedef enum {
	VIDEO_NORMAL = 0,
	VIDEO_FULLSTRETCH,
	VIDEO_4X3,
	VIDEO_16X9,
	
}VideoScreenMode;

VideoScreenMode Current_screen_mode = VIDEO_NORMAL;



int RmDefPath(void)
{
    int fd, ret;
    //char str[]="rm all";
    char str[]="rm default";
   // char str[] = "iptvpath";
    fd = open("/sys/class/vfm/map", O_RDWR);

    if(fd < 0) {
        if (prop_shouldshowlog == '1') {
            LOGE("=VDIN CPP=> open /sys/class/vfm/map ERROR(%s)!!\n",strerror(errno));
        }
        //LOGE("=VDIN CPP=> open /sys/class/vfm/map ERROR(%s)!!\n",strerror(errno));
        return 0;
    } else {
        ret = write(fd, str, sizeof(str));
    }
    close(fd);
    return ret;
}

int AddIptvPath(void){
	
    FILE * fp;
    int ret = -1;
    const char *IptvPath = "add iptvpath decoder deinterlace amvideo";
	 //const char *IptvPath = "add iptvpath decoder ppmgr amvideo";

    fp = fopen("/sys/class/vfm/map", "w");
    
    if(fp != NULL) {
        ret = fprintf(fp, "%s", IptvPath);
    } else {
        if (prop_shouldshowlog == '1') {
            LOGE("=VDIN CPP=> open /sys/class/vfm/map ERROR(%s)!!\n", strerror(errno));
        }
        //LOGE("=VDIN CPP=> open /sys/class/vfm/map ERROR(%s)!!\n", strerror(errno));
        return -1;
    }

    if(ret<0)
        if (prop_shouldshowlog == '1') {
            LOGE("=VDIN CPP=> add IptvPath ERROR(%s)!!!!!!!!!!!!!!!\n", strerror(errno));
        }
        //LOGE("=VDIN CPP=> add IptvPath ERROR(%s)!!!!!!!!!!!!!!!\n", strerror(errno));
	
    fclose(fp);
    return ret; 
}


int RemoveIptvPath(void){

    FILE * fp;
    int ret = -1;
   const char *IptvPath = "rm iptvpath";
   //const char *IptvPath = "rm default";
	
    fp = fopen("/sys/class/vfm/map", "w");
    
    if(fp != NULL) {
        ret = fprintf(fp, "%s", IptvPath);
    } else {
        if (prop_shouldshowlog == '1') {
            LOGE("=VDIN CPP=> open /sys/class/vfm/map ERROR(%s)!!\n", strerror(errno));
        }
        //LOGE("=VDIN CPP=> open /sys/class/vfm/map ERROR(%s)!!\n", strerror(errno));
        return -1;
    }

    if(ret<0)
        if (prop_shouldshowlog == '1') {
            LOGE("=VDIN CPP=> add IptvPath ERROR(%s)!!!!!!!!!!!!!!!\n", strerror(errno));
        }
        //LOGE("=VDIN CPP=> add IptvPath ERROR(%s)!!!!!!!!!!!!!!!\n", strerror(errno));
	
    fclose(fp);
    return ret;
}

int RestoreDefPath(void){

    FILE * fp; 
    int ret = -1;
    const char *DefaultPath = "add default decoder ppmgr amvideo";
	
    fp = fopen("/sys/class/vfm/map", "w");
    
    if(fp != NULL) {
        ret = fprintf(fp, "%s", DefaultPath);
    } else {
        if (prop_shouldshowlog == '1') {
            LOGE("=VDIN CPP=> open /sys/class/vfm/map ERROR(%s)!!\n", strerror(errno));
		}
        //LOGE("=VDIN CPP=> open /sys/class/vfm/map ERROR(%s)!!\n", strerror(errno));
        return -1;
    }

    if(ret<0)
        if (prop_shouldshowlog == '1') {
            LOGE("=VDIN CPP=> add IptvPath ERROR(%s)!!!!!!!!!!!!!!!\n", strerror(errno));
        }
        //LOGE("=VDIN CPP=> add IptvPath ERROR(%s)!!!!!!!!!!!!!!!\n", strerror(errno));
	
    fclose(fp);
    return ret;
}	




int SYS_set_video_preview_win(int x,int y,int w,int h)
{

    if (prop_shouldshowlog == '1') {
        LOGI("function flag : SYS_set_video_preview_win %d,%d,%d,%d ",x,y,w,h);
    }
    //LOGI("function flag : SYS_set_video_preview_win %d,%d,%d,%d ",x,y,w,h);

    int fd;
    const  char *path = "/sys/class/video/axis";    
    char  bcmd[32];

    memset(bcmd,0,sizeof(bcmd));
    
    fd=open(path, O_CREAT|O_RDWR | O_TRUNC, 0644);
    if(fd>=0)
    {
        
        sprintf(bcmd,"%d %d %d %d",x,y,w+x,h+y);
        write(fd,bcmd,strlen(bcmd));
        close(fd);
        return 0;
    }
    return -1;    
}

#define VIDEO_SCREEN_W 1280
#define VIDEO_SCREEN_H 720



  int set_sys_str(const char *path, const char *val)
  {
      if (prop_shouldshowlog == '1') {
          LOGI("function flag : set_sys_str %s,%s ",path,val);
      }
      //LOGI("function flag : set_sys_str %s,%s ",path,val);
	  int fd;
	  int bytes;
	  fd = open(path, O_CREAT | O_RDWR | O_TRUNC, 0644);
	  if (fd >= 0) {
		  bytes = write(fd, val, strlen(val));
		  close(fd);
		  return 0;
	  } else {
	  }
	  return -1;
  }
  int set_sys_int(const char *path,int val)
  {
      if (prop_shouldshowlog == '1') {
          LOGI("function flag : set_sys_int %s,%d ",path,val);
      }
      //LOGI("function flag : set_sys_int %s,%d ",path,val);
	  int fd;
	  char	bcmd[16];
	  fd=open(path, O_CREAT|O_RDWR | O_TRUNC, 0644);
	  if(fd>=0)
	  {
		  sprintf(bcmd,"%d",val);
		  write(fd,bcmd,strlen(bcmd));
		  printf("set fs%s=%d ok\n",path,val);
		  close(fd);
		  return 0;
	  }
	  printf("set fs %s=%d failed\n",path,val);
	  return -1;
  }

void get_display_mode(char *mode)
{
    int fd;
    char *path = "/sys/class/display/mode";
    if (!mode) {
       // log_error("[get_display_mode]Invalide parameter!");
        return;
    }
    fd = open(path, O_RDONLY);
    if (fd >= 0) {
        memset(mode, 0, 16); // clean buffer and read 15 byte to avoid strlen > 15	
        read(fd, mode, 15);
        //log_print("[get_display_mode]mode=%s strlen=%d\n", mode, strlen(mode));
        mode[strlen(mode)] = '\0';
        close(fd);
    } else {
        sprintf(mode, "%s", "fail");
    };
    //log_print("[get_display_mode]display_mode=%s\n", mode);
    return ;
}


int GL_2X_iptv_scale720(int mSwitch)
{
    char mode[16];
	char writedata[40] = {0};
	char m1080scale[8];
	char vaxis_newx_str[10] = {0};
	char vaxis_newy_str[10] = {0};
	char vaxis_width_str[10] = {0};
	char vaxis_height_str[10] = {0};
	
	int request2XScaleFile = -1, scaleOsd1File = -1, scaleaxisOsd1File = -1, Fb0Blank = -1, Fb1Blank = -1;
	char raxis_str[32],saxis_str[32];
	int vaxis_newx= -1,vaxis_newy = -1,vaxis_width= -1,vaxis_height= -1;
  
	get_display_mode(mode);

  
	if((request2XScaleFile = open("/sys/class/graphics/fb0/request2XScale", O_RDWR)) < 0) {
		//log_print("open /sys/class/graphics/fb0/scale fail.");
	}
	if((scaleOsd1File = open("/sys/class/graphics/fb1/scale", O_RDWR)) < 0) {
		///log_print("open /sys/class/graphics/fb1/scale fail.");
	}
	if((scaleaxisOsd1File = open("/sys/class/graphics/fb1/scale_axis", O_RDWR)) < 0) {
		//log_print("open /sys/class/graphics/fb1/scale_axis fail.");
	}
	if((Fb0Blank = open("/sys/class/graphics/fb0/blank", O_RDWR)) < 0) {
		//log_print("open /sys/class/graphics/fb0/blank fail.");
	}
	if((Fb1Blank = open("/sys/class/graphics/fb1/blank", O_RDWR)) < 0) {
		//log_print("open /sys/class/graphics/fb1/blank fail.");
	}
	if(mSwitch == 0)
	{		if(!strncmp(mode, "1080i", 5) || !strncmp(mode, "1080p", 5))
		    {
			    set_sys_str("/sys/class/graphics/fb0/scale","0");
                set_sys_str("/sys/class/graphics/fb0/scale_axis","0 0 959 1079");            
			}
	        set_sys_str("/sys/class/display/axis","0 0 1280 720 0 0 18 18");
			write(request2XScaleFile, "2", strlen("2"));
			write(scaleOsd1File, "0", strlen("0"));
	}
	else if(mSwitch == 1)
	{
		
		//write(Fb0Blank, "1", strlen("1"));
		//write(Fb1Blank, "1", strlen("1"));
		if(!strncmp(mode, "480i", 4) || !strncmp(mode, "480p", 4))
		{
		    if(!strncmp(mode, "480i", 4))
		    {
               	property_get("ubootenv.var.480ioutputx",vaxis_newx_str,"0");			  
			    property_get("ubootenv.var.480ioutputy",vaxis_newy_str,"0");			  
			    property_get("ubootenv.var.480ioutputwidth",vaxis_width_str,"720");			  
			    property_get("ubootenv.var.480ioutputheight",vaxis_height_str,"480");

			}
			else
		    {            
               	property_get("ubootenv.var.480poutputx",vaxis_newx_str,"0");			  
			    property_get("ubootenv.var.480poutputy",vaxis_newy_str,"0");			  
			    property_get("ubootenv.var.480poutputwidth",vaxis_width_str,"720");			  
			    property_get("ubootenv.var.480poutputheight",vaxis_height_str,"480");        
			}

			vaxis_newx = atoi(vaxis_newx_str);
			vaxis_newy = atoi(vaxis_newy_str);
			vaxis_width = atoi(vaxis_width_str);
			vaxis_height = atoi(vaxis_height_str);
			
            if (prop_shouldshowlog == '1') {
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
				                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);
            }

            //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
            //	                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);
			
			sprintf(writedata,"%d %d 1280 720 0 0 18 18",
				             vaxis_newx,
				             vaxis_newy);
		    set_sys_str("/sys/class/display/axis",writedata);

			memset(writedata,0,strlen(writedata));

			sprintf(writedata,"16 %d %d",
				           vaxis_width,
				           vaxis_height);
			write(request2XScaleFile, writedata, strlen(writedata));
			
            memset(writedata,0,strlen(writedata));
			
            sprintf(writedata,"1280 720 %d %d",
							vaxis_width,
							vaxis_height);
			write(scaleaxisOsd1File, writedata, strlen(writedata));
			write(scaleOsd1File, "0x10001", strlen("0x10001"));

		}
		else if(!strncmp(mode, "576i", 4) || !strncmp(mode, "576p", 4))
		{
		    if(!strncmp(mode, "576i", 4))
		    {
               	property_get("ubootenv.var.576ioutputx",vaxis_newx_str,"0");			  
			    property_get("ubootenv.var.576ioutputy",vaxis_newy_str,"0");			  
			    property_get("ubootenv.var.576ioutputwidth",vaxis_width_str,"720");			  
			    property_get("ubootenv.var.576ioutputheight",vaxis_height_str,"576");

			}
			else
		    {            
               	property_get("ubootenv.var.576poutputx",vaxis_newx_str,"0");			  
			    property_get("ubootenv.var.576poutputy",vaxis_newy_str,"0");			  
			    property_get("ubootenv.var.576poutputwidth",vaxis_width_str,"720");			  
			    property_get("ubootenv.var.576poutputheight",vaxis_height_str,"576");        
			}


			vaxis_newx = atoi(vaxis_newx_str);
			vaxis_newy = atoi(vaxis_newy_str);
			vaxis_width = atoi(vaxis_width_str);
			vaxis_height = atoi(vaxis_height_str);

            if (prop_shouldshowlog == '1') {
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
				                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);
            }
            //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
            //	                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);
			
			sprintf(writedata,"%d %d 1280 720 0 0 18 18",
										 vaxis_newx,
										 vaxis_newy);
			set_sys_str("/sys/class/display/axis",writedata);
						
			memset(writedata,0,strlen(writedata));
			sprintf(writedata,"16 %d %d",
							vaxis_width,
							vaxis_height);
			write(request2XScaleFile, writedata, strlen(writedata));

            memset(writedata,0,strlen(writedata));
			
            sprintf(writedata,"1280 720 %d %d",
							vaxis_width,
							vaxis_height);
			write(scaleaxisOsd1File, writedata, strlen(writedata));
			write(scaleOsd1File, "0x10001", strlen("0x10001"));
		}
		else if(!strncmp(mode, "720p", 4))
		{
		    property_get("ubootenv.var.720poutputx",vaxis_newx_str,"0");			  
			property_get("ubootenv.var.720poutputy",vaxis_newy_str,"0");			  
			property_get("ubootenv.var.720poutputwidth",vaxis_width_str,"1280"); 		  
			property_get("ubootenv.var.720poutputheight",vaxis_height_str,"720");
			
            if (prop_shouldshowlog == '1') {
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_height_str:%s\n",
		                            vaxis_height_str);
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newy_str:%s\n",
                                    vaxis_newy_str);
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_width_str:%s\n",
                                    vaxis_width_str);
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_height_str:%s\n",
                                    vaxis_height_str);
			}

		    //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_height_str:%s\n",
		    // 						vaxis_height_str);
		    //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newy_str:%s\n",
			//						vaxis_newy_str);
		    //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_width_str:%s\n",
			//						vaxis_width_str);
		    //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_height_str:%s\n",
			//						vaxis_height_str);

			vaxis_newx = atoi(vaxis_newx_str);
			vaxis_newy = atoi(vaxis_newy_str);
			vaxis_width = atoi(vaxis_width_str);
			vaxis_height = atoi(vaxis_height_str);
			
            if (prop_shouldshowlog == '1') {
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
            	                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);
            }

            //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
            //	                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);

			sprintf(writedata,"%d %d 1280 720 0 0 18 18",
										 vaxis_newx,
										 vaxis_newy);
			set_sys_str("/sys/class/display/axis",writedata);

			memset(writedata,0,strlen(writedata));
			sprintf(writedata,"16 %d %d",
							vaxis_width,
							vaxis_height);
			write(request2XScaleFile, writedata, strlen(writedata));

			memset(writedata,0,strlen(writedata));
			
            sprintf(writedata,"1280 720 %d %d",
							vaxis_width,
							vaxis_height);
			write(scaleaxisOsd1File, writedata, strlen(writedata));
			write(scaleOsd1File, "0x10001", strlen("0x10001"));
			
		}
		else if(!strncmp(mode, "1080i", 5) || !strncmp(mode, "1080p", 5))
		{
			/*write(request2XScaleFile, "8 0 0", strlen("8 0 0"));
			write(scaleaxisOsd1File, "1280 720 1920 1080", strlen("1280 720 1920 1080"));
			write(scaleOsd1File, "0x10001", strlen("0x10001"));*/
			if(!strncmp(mode, "1080i", 5))
		    {
               	property_get("ubootenv.var.1080ioutputx",vaxis_newx_str,"0");			  
			    property_get("ubootenv.var.1080ioutputy",vaxis_newy_str,"0");			  
			    property_get("ubootenv.var.1080ioutputwidth",vaxis_width_str,"1920");			  
			    property_get("ubootenv.var.1080ioutputheight",vaxis_height_str,"1080");

			}
			else
		    {            
               	property_get("ubootenv.var.1080poutputx",vaxis_newx_str,"0");			  
			    property_get("ubootenv.var.1080poutputy",vaxis_newy_str,"0");			  
			    property_get("ubootenv.var.1080poutputwidth",vaxis_width_str,"1920");			  
			    property_get("ubootenv.var.1080poutputheight",vaxis_height_str,"1080");        
			}
			
            if (prop_shouldshowlog == '1') {
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "GL_2X_iptv_scale530 : 1080i");
            }
            //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "GL_2X_iptv_scale530 : 1080i");

			vaxis_newx = atoi(vaxis_newx_str);
			vaxis_newy = atoi(vaxis_newy_str);
			vaxis_width = atoi(vaxis_width_str);
			vaxis_height = atoi(vaxis_height_str);
			
            if (prop_shouldshowlog == '1') {
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
                                              vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);
            }

            //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
            //	                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);

			sprintf(writedata,"%d %d 1280 720 0 0 18 18",
								(int(vaxis_newx/2))*2,
								(int(vaxis_newy/2))*2);						 										 
			set_sys_str("/sys/class/display/axis",writedata);

			memset(writedata,0,strlen(writedata));

			sprintf(writedata,"0 0 %d %d",
								960-int(vaxis_newx/2)-1,
								1080-int(vaxis_newy/2)-1);
			
			set_sys_str("/sys/class/graphics/fb0/scale_axis",writedata);


			memset(writedata,0,strlen(writedata));

			sprintf(writedata,"7 %d %d",
							  int(vaxis_width/2),
							  vaxis_height);
			write(request2XScaleFile, writedata, strlen(writedata));

			memset(writedata,0,strlen(writedata));
			
            sprintf(writedata,"1280 720 %d %d",
							vaxis_width,
							vaxis_height);
			write(scaleaxisOsd1File, writedata, strlen(writedata));
			write(scaleOsd1File, "0x10001", strlen("0x10001"));

		}
	}

	if(request2XScaleFile >= 0)
		close(request2XScaleFile);
	if(scaleOsd1File >= 0)
		close(scaleOsd1File);
	if(scaleaxisOsd1File >= 0)
		close(scaleaxisOsd1File);
	if(Fb0Blank >= 0)
		close(Fb0Blank);
	if(Fb1Blank >= 0)
		close(Fb1Blank);
	return 0;
}


int GL_2X_iptv_scale530(int mSwitch)
{
    char mode[16]= {0};
	char writedata[40] = {0};
	char m1080scale[8];
	char vaxis_newx_str[10] = {0};
	char vaxis_newy_str[10] = {0};
	char vaxis_width_str[10] = {0};
	char vaxis_height_str[10] = {0};
	
	int request2XScaleFile = -1, scaleOsd1File = -1,scaleOsd0File = -1, scaleaxisOsd1File = -1, Fb0Blank = -1, Fb1Blank = -1;
	char raxis_str[32],saxis_str[32];
    int vaxis_newx= -1,vaxis_newy = -1,vaxis_width= -1,vaxis_height= -1;
	
    if (prop_shouldshowlog == '1') {
        __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "GL_2X_iptv_scale530");
    }
    //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "GL_2X_iptv_scale530");
	

	
	get_display_mode(mode);
  
	if((request2XScaleFile = open("/sys/class/graphics/fb0/request2XScale", O_RDWR)) < 0) {
		//log_print("open /sys/class/graphics/fb0/scale fail.");
	}
	if((scaleOsd1File = open("/sys/class/graphics/fb1/scale", O_RDWR)) < 0) {
		//log_print("open /sys/class/graphics/fb1/scale fail.");
	}
	if((scaleOsd0File = open("/sys/class/graphics/fb0/scale", O_RDWR)) < 0) {
        if (prop_shouldshowlog == '1') {
            __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "open /sys/class/graphics/fb0/scale fail.");
        }
        //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "open /sys/class/graphics/fb0/scale fail.");
	}
	if((scaleaxisOsd1File = open("/sys/class/graphics/fb1/scale_axis", O_RDWR)) < 0) {
		//log_print("open /sys/class/graphics/fb1/scale_axis fail.");
	}
	
	if((Fb0Blank = open("/sys/class/graphics/fb0/blank", O_RDWR)) < 0) {
		//log_print("open /sys/class/graphics/fb0/blank fail.");
	}
	if((Fb1Blank = open("/sys/class/graphics/fb1/blank", O_RDWR)) < 0) {
		//log_print("open /sys/class/graphics/fb1/blank fail.");
	}
	if(mSwitch == 0)
	{
			if(!strncmp(mode, "1080i", 5) || !strncmp(mode, "1080p", 5))
		    {
			    set_sys_str("/sys/class/graphics/fb0/scale","0");
                set_sys_str("/sys/class/graphics/fb0/scale_axis","0 0 959 1079");            
			}
	    set_sys_str("/sys/class/display/axis","0 0 1280 720 0 0 18 18");
			write(request2XScaleFile, "2", strlen("2"));
			write(scaleOsd1File, "0", strlen("0"));
			write(scaleOsd0File, "0", strlen("0"));
	}
	else if(mSwitch == 1)
	{
	   //640, 530   
	 

		
		if(!strncmp(mode, "480i", 4) || !strncmp(mode, "480p", 4))
		{
		    if(!strncmp(mode, "480i", 4))
		    {
               	property_get("ubootenv.var.480ioutputx",vaxis_newx_str,"0");			  
			    property_get("ubootenv.var.480ioutputy",vaxis_newy_str,"0");			  
			    property_get("ubootenv.var.480ioutputwidth",vaxis_width_str,"720");			  
			    property_get("ubootenv.var.480ioutputheight",vaxis_height_str,"480");

			}
			else
		    {            
               	property_get("ubootenv.var.480poutputx",vaxis_newx_str,"0");			  
			    property_get("ubootenv.var.480poutputy",vaxis_newy_str,"0");			  
			    property_get("ubootenv.var.480poutputwidth",vaxis_width_str,"720");			  
			    property_get("ubootenv.var.480poutputheight",vaxis_height_str,"480");        
			}

			vaxis_newx = atoi(vaxis_newx_str);
			vaxis_newy = atoi(vaxis_newy_str);
			vaxis_width = atoi(vaxis_width_str);
			vaxis_height = atoi(vaxis_height_str);
			
            if (prop_shouldshowlog == '1') {
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
                                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);
            }

            //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
            //	                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);
			
			sprintf(writedata,"%d %d 1280 720 0 0 18 18",
				             vaxis_newx,
				             vaxis_newy);
		    set_sys_str("/sys/class/display/axis",writedata);

			memset(writedata,0,strlen(writedata));

			sprintf(writedata,"16 %d %d",
				           int(vaxis_width*1280/640),
				           int(vaxis_height*720/530));
			write(request2XScaleFile, writedata, strlen(writedata));
			
            memset(writedata,0,strlen(writedata));
			
            sprintf(writedata,"1280 720 %d %d",
							int(vaxis_width*1280/640),
							int(vaxis_height*720/530));
			write(scaleaxisOsd1File, writedata, strlen(writedata));
			write(scaleOsd1File, "0x10001", strlen("0x10001"));

		}
		else if(!strncmp(mode, "576i", 4) || !strncmp(mode, "576p", 4))
		{
		    
		    if(!strncmp(mode, "576i", 4))
		    {
               	property_get("ubootenv.var.576ioutputx",vaxis_newx_str,"0");			  
			    property_get("ubootenv.var.576ioutputy",vaxis_newy_str,"0");			  
			    property_get("ubootenv.var.576ioutputwidth",vaxis_width_str,"720");			  
			    property_get("ubootenv.var.576ioutputheight",vaxis_height_str,"576");

			}
			else
		    {            
               	property_get("ubootenv.var.576poutputx",vaxis_newx_str,"0");			  
			    property_get("ubootenv.var.576poutputy",vaxis_newy_str,"0");			  
			    property_get("ubootenv.var.576poutputwidth",vaxis_width_str,"720");			  
			    property_get("ubootenv.var.576poutputheight",vaxis_height_str,"576");        
			}


			vaxis_newx = atoi(vaxis_newx_str);
			vaxis_newy = atoi(vaxis_newy_str);
			vaxis_width = atoi(vaxis_width_str);
			vaxis_height = atoi(vaxis_height_str);

            if (prop_shouldshowlog == '1') {
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
                                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);
            }
            //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
            //	                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);
			
			sprintf(writedata,"%d %d 1280 720 0 0 18 18",
										 vaxis_newx,
										 vaxis_newy);
			set_sys_str("/sys/class/display/axis",writedata);
						
			memset(writedata,0,strlen(writedata));
			sprintf(writedata,"16 %d %d",
							int(vaxis_width*1280/640),
							int(vaxis_height*720/530));
			write(request2XScaleFile, writedata, strlen(writedata));

            memset(writedata,0,strlen(writedata));
			
            sprintf(writedata,"1280 720 %d %d",
							int(vaxis_width*1280/640),
							int(vaxis_height*720/530));
			write(scaleaxisOsd1File, writedata, strlen(writedata));
			write(scaleOsd1File, "0x10001", strlen("0x10001"));

		}
		else if(!strncmp(mode, "720p", 4))
		{
		    
			property_get("ubootenv.var.720poutputx",vaxis_newx_str,"0");			  
			property_get("ubootenv.var.720poutputy",vaxis_newy_str,"0");			  
			property_get("ubootenv.var.720poutputwidth",vaxis_width_str,"1280"); 		  
			property_get("ubootenv.var.720poutputheight",vaxis_height_str,"720");
			
            if (prop_shouldshowlog == '1') {
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_height_str:%s\n",
		                            vaxis_height_str);
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newy_str:%s\n",
                                    vaxis_newy_str);
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_width_str:%s\n",
                                    vaxis_width_str);
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_height_str:%s\n",
                                    vaxis_height_str);
			}

            //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_height_str:%s\n",
            //						vaxis_height_str);
            //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newy_str:%s\n",
            //						vaxis_newy_str);
            //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_width_str:%s\n",
            //						vaxis_width_str);
            //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_height_str:%s\n",
            //						vaxis_height_str);

			vaxis_newx = atoi(vaxis_newx_str);
			vaxis_newy = atoi(vaxis_newy_str);
			vaxis_width = atoi(vaxis_width_str);
			vaxis_height = atoi(vaxis_height_str);
			
            if (prop_shouldshowlog == '1') {
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
                                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);
            }

            //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
            //	                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);

			sprintf(writedata,"%d %d 1280 720 0 0 18 18",
										 vaxis_newx,
										 vaxis_newy);
			set_sys_str("/sys/class/display/axis",writedata);

			memset(writedata,0,strlen(writedata));
			sprintf(writedata,"16 %d %d",
							int(vaxis_width*1280/640),
							int(vaxis_height*720/530));
			write(request2XScaleFile, writedata, strlen(writedata));
			memset(writedata,0,strlen(writedata));
			
            sprintf(writedata,"1280 720 %d %d",
							int(vaxis_width*1280/640),
							int(vaxis_height*720/530));
			write(scaleaxisOsd1File, writedata, strlen(writedata));
			write(scaleOsd1File, "0x10001", strlen("0x10001"));		    
		}
		else if(!strncmp(mode, "1080i", 5) || !strncmp(mode, "1080p", 5))
		{
		    
		    if(!strncmp(mode, "1080i", 5))
		    {
               	property_get("ubootenv.var.1080ioutputx",vaxis_newx_str,"0");			  
			    property_get("ubootenv.var.1080ioutputy",vaxis_newy_str,"0");			  
			    property_get("ubootenv.var.1080ioutputwidth",vaxis_width_str,"1920");			  
			    property_get("ubootenv.var.1080ioutputheight",vaxis_height_str,"1080");

			}
			else
		    {            
               	property_get("ubootenv.var.1080poutputx",vaxis_newx_str,"0");			  
			    property_get("ubootenv.var.1080poutputy",vaxis_newy_str,"0");			  
			    property_get("ubootenv.var.1080poutputwidth",vaxis_width_str,"1920");			  
			    property_get("ubootenv.var.1080poutputheight",vaxis_height_str,"1080");        
			}

            if (prop_shouldshowlog == '1') {
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "GL_2X_iptv_scale530 : 1080i");
            }
		   //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "GL_2X_iptv_scale530 : 1080i");

			vaxis_newx = atoi(vaxis_newx_str);
			vaxis_newy = atoi(vaxis_newy_str);
			vaxis_width = atoi(vaxis_width_str);
			vaxis_height = atoi(vaxis_height_str);

			if (prop_shouldshowlog == '1') {
				__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
				                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);
			}
			//__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
			//	                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);

			sprintf(writedata,"%d %d 1280 720 0 0 18 18",
								(int(vaxis_newx/2))*2,
								(int(vaxis_newy/2))*2);						 										 
			set_sys_str("/sys/class/display/axis",writedata);

			memset(writedata,0,strlen(writedata));

			sprintf(writedata,"0 0 %d %d",
								960-int(vaxis_newx/2)-1,
								1080-int(vaxis_newy/2)-1);
			
			set_sys_str("/sys/class/graphics/fb0/scale_axis",writedata);


			memset(writedata,0,strlen(writedata));

			sprintf(writedata,"7 %d %d",
							  int(vaxis_width),
							  int(vaxis_height*720/530));
			write(request2XScaleFile, writedata, strlen(writedata));
			memset(writedata,0,strlen(writedata));
			
            sprintf(writedata,"1280 720 %d %d",
							int(vaxis_width*2),
							int(vaxis_height*720/530));
			write(scaleaxisOsd1File, writedata, strlen(writedata));
			write(scaleOsd1File, "0x10001", strlen("0x10001"));
		}
	}

	if(request2XScaleFile >= 0)
		close(request2XScaleFile);
	if(scaleOsd1File >= 0)
		close(scaleOsd1File);
	if(scaleaxisOsd1File >= 0)
		close(scaleaxisOsd1File);
	if(scaleOsd0File >= 0)
		close(scaleOsd0File);
	/*if(scaleaxisOsd0File >= 0)
		close(scaleaxisOsd0File);*/
	if(Fb0Blank >= 0)
		close(Fb0Blank);
	if(Fb1Blank >= 0)
		close(Fb1Blank);
	return 0;
}

int enable_gl_2xscale(const char *val)
{

	return 0;

    int fd;
    int bytes;

	fd = open("/sys/class/graphics/fb0/request2XScale",O_RDWR);
	if( fd>= 0)

    {
        bytes = write(fd, val, strlen(val));
        if (prop_shouldshowlog == '1') {
            LOGI("gjaoun: enable_gl_2xscale %s\n",val);
        }
        //LOGI("gjaoun: enable_gl_2xscale %s\n",val);
        close(fd);
        return 0;
    } else {
        if (prop_shouldshowlog == '1') {
            LOGI("gjaoun:fb:%d failed to open /sys/class/graphics/fb0/request2XScale\n",fd);
        }
        //LOGI("gjaoun:fb:%d failed to open /sys/class/graphics/fb0/request2XScale\n",fd);
	 return -1;
    }
    
}

int Active_osd_viewport(int web_w, int web_h)
	
{
    if (prop_shouldshowlog == '1') {
        LOGI("gjaoun :web %d,%d",web_w,web_h);
    }
    //LOGI("gjaoun :web %d,%d",web_w,web_h);

//1080p50hz  1080i50hz  720p50hz  576i  480i
    if(web_w == 0 || web_h == 0) 
    	{
		 web_w = 1280 ;
		 web_h = 720;

	}
	 char buf[16];	 
	 char  bcmd[32];
	 int modes_width = 1920;
	 int modes_heigh = 1280;


	 int fd_disp = open("/sys/class/display/mode", O_RDONLY); 
	 if (fd_disp >= 0) {
		 memset(buf,0,16);
		 memset(bcmd,0,sizeof(bcmd));
		 read(fd_disp,buf,sizeof(buf));
        if (prop_shouldshowlog == '1') {
            LOGI("gjaoun : %s",buf);
        }
        // LOGI("gjaoun : %s",buf);
		 
		 read(fd_disp,buf,sizeof(buf));
		 if(!strncmp(buf,"720p50hz",8)){

		  modes_heigh = 720 ;
		  modes_width = 1280 ;

		 }
		 else if(!strncmp(buf,"1080p50hz",9)){		

		 modes_heigh = 1080;
		 modes_width = 1920;

		 }
		 else if(!strncmp(buf,"1080i50hz",9)){	
		 modes_heigh = 1080;
		 modes_width = 1920;

		 }
		 else if(!strncmp(buf,"576p",4))
		 {

		 modes_heigh = 576;
		 modes_width = 1024;

		 }
		 else if(!strncmp(buf,"576i",4))
		 {
		 modes_heigh = 576;
		 modes_width = 720;  

		 }
		 else if(!strncmp(buf,"480p",4))
		 {
		  modes_heigh = 480;
		  modes_width = 720;

		 }
		 else if(!strncmp(buf,"480i",4))
		 {
			 modes_heigh = 480;
			 modes_width = 720;

		 }
		 close(fd_disp);     
	 }   
	 
    if (prop_shouldshowlog == '1') {
        LOGI("gjaoun : mode %d , %d",modes_width,modes_heigh);
    }
    //LOGI("gjaoun : mode %d , %d",modes_width,modes_heigh);
     sprintf(bcmd,"%d %d %d %d %d",0,0,modes_width,modes_heigh,0);
     set_sys_str("/sys/class/graphics/fb0/video_hole",bcmd);	
    //Active_video_viewport(0,0,modes_width,modes_heigh);
	 //int w = (modes_width*modes_width)/web_w ;
     //int h = (modes_heigh*modes_heigh)/web_h ;
	
	memset(bcmd,0,sizeof(bcmd));
	sprintf(bcmd,"%d %d %d",16,modes_width,modes_heigh);
    if (prop_shouldshowlog == '1') {
        LOGI("gjaoun scale: %s",bcmd);
    }
    //LOGI("gjaoun scale: %s",bcmd);
	enable_gl_2xscale(bcmd); 



    return 0 ;

}

int Active_video_viewport(int x,int y,int width,int height)
{



    int fd;
    const char *path = "/sys/class/video/axis" ;
    char  bcmd[32];

    fd = open(path, O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (fd >= 0) {
            sprintf(bcmd, "%d %d %d %d", x, y, x+width, y+height);
            write(fd, bcmd, strlen(bcmd));
        close(fd);
        return 0;
    }
	return -1;
}


void LunchIptv()
{
	
		int ret;
        if(prop_shouldshowlog == '1') {
            __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "LunchIptv\n");
        }
        //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "LunchIptv\n");

		ret = disable_freescale_MBX();
        if (prop_shouldshowlog == '1') {
            LOGI("disable freeacale:%d\n", ret);
        }
        //  LOGI("disable freeacale:%d\n", ret);
	  
//    ret = GL_2X_iptv_scale720(1);
//   LOGI("GL2XScale:%d\n", ret);
    
    
    set_sys_int("/sys/class/graphics/fb0/free_scale",0);
    RmDefPath();
    AddIptvPath();
    
    
    set_sys_str("/sys/class/deinterlace/di0/config","disable");
    set_sys_int("/sys/module/di/parameters/buf_mgr_mode",0);
    set_sys_str("/sys/class/display/rd_reg","m 0x1a2b");
    set_sys_str("/sys/class/display/wr_reg","m 0x1a2b 0x1dc20c81");
//    set_sys_str("/sys/class/graphics/fb0/video_hole","0 0 1920 1080 0 8");	
    //Active_video_viewport(0,0,1280,720);
    //set_sys_int("/sys/class/video/blackout_policy",0);
    //Active_osd_viewport(1280, 720);
	//set_sys_int("/sys/module/amvdec_h264/parameters/error_recovery_mode",0);
	



}

/*status : 0 - video stop status   1 - video play status*/

void SwitchResolution(int mode , int status) 
{
    if (prop_shouldshowlog == '1') {
        LOGE("SwitchResolution:  mode=%d, status=%d.\n", mode, status);
    }
    //LOGE("SwitchResolution:  mode=%d, status=%d.\n", mode, status);

	
	if(mode == 1){
		if(status == 0){	
          
			//RemoveIptvPath();
           //Active_osd_viewport(640, 530);
             GL_2X_iptv_scale530(1);
		  //  AddIptvPath();
           }
		// Active_video_viewport(0,0,1920,1080);	
		
    }

	else if(mode == 2){
		if(status == 0){
         
			//RemoveIptvPath();
          // Active_osd_viewport(1280, 720);
          
           GL_2X_iptv_scale720(1);
		   
		  // AddIptvPath();
		
		}
	 // Active_video_viewport(0,0,1920,1080);			
      }
	


}


void QuitIptv()
{


  set_sys_str("/sys/class/graphics/fb0/video_hole","0 0 0 0 0 0");
	set_sys_int("/sys/class/video/blackout_policy",1);  
	set_sys_int("/sys/class/video/disable_video",1);	
	//enable_gl_2xscale("2");
    //enable_gl_2xscale("");
		int ret;
		    ret = GL_2X_iptv_scale720(0);
			GL_2X_iptv_scale530(0);

    ret = enable_freescale_MBX();

    if (prop_shouldshowlog == '1') {
        LOGI("enableFreescaleMBX:%d\n", ret);
    }
    // LOGI("enableFreescaleMBX:%d\n", ret);
	
    //RmDefPath();
    RemoveIptvPath();
    RestoreDefPath();
	enable_gl_2xscale("2 0 0");

 	//set_sys_int("/sys/module/amvdec_h264/parameters/error_recovery_mode",0);
    set_sys_int("/sys/class/graphics/fb0/free_scale",1);
	
	/*enable freescale for ui and disable opengl scale*/
	
    if (prop_shouldshowlog == '1') {
        __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "QuitIptv\n");
    }

    //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "QuitIptv\n");
	      
}



int SYS_set_global_alpha(int alpha){


    if (prop_shouldshowlog == '1') {
        LOGI("function flag :SYS_set_global_alpha  %d",alpha);
    }
    //LOGI("function flag :SYS_set_global_alpha  %d",alpha);
    int ret = -1;   
    int fd_fb0 = open("/dev/graphics/fb0", O_RDWR); 
    if (fd_fb0 >= 0) {   
        uint32_t myAlpha = alpha;  
        ret = ioctl(fd_fb0, FBIOPUT_OSD_SET_GBL_ALPHA, &myAlpha);    
        close(fd_fb0);   

    }   
    return ret;
}
int SYS_disable_colorkey(void)
{

    if (prop_shouldshowlog == '1') {
        LOGI("function flag :SYS_disable_colorkey  ");
    }
    //LOGI("function flag :SYS_disable_colorkey  ");

    int ret = -1;
    int fd_fb0 = open("/dev/graphics/fb0", O_RDWR);
    if (fd_fb0 >= 0) {
        uint32_t myKeyColor_en = 0;
        ret = ioctl(fd_fb0, FBIOPUT_OSD_SRCKEY_ENABLE, &myKeyColor_en);
        close(fd_fb0);
    }
    return ret;

}

CTsPlayer::CTsPlayer()
{
    property_get("iptv.shouldshowlog",&prop_shouldshowlog,"0");//initial the log switch
	set_sys_int("/sys/class/graphics/fb0/blank",1);
	set_sys_int("/sys/class/video/blackout_policy",1);
	set_sys_int("/sys/class/video/disable_video",2);	
	memset(a_aPara,0,sizeof(AUDIO_PARA_T)*MAX_AUDIO_PARAM_SIZE);
	memset(&vPara,0,sizeof(vPara));
	memset(&codec,0,sizeof(codec));
	player_pid=-1;
	pcodec=&codec;
	codec_audio_basic_init();
	//0:normal，1:full stretch，2:4-3，3:16-9
	set_sys_int("/sys/class/video/screen_mode", 1);
	set_sys_int("/sys/class/tsync/enable", 1);
	//set_sys_int("/sys/class/graphics/fb0/blank", 1);
	//set_sys_int("/sys/class/graphics/fb1/blank", 1);
	//SetColorKey(1, 0);
	m_bIsPlay = false;
	pfunc_player_evt = NULL;
	//SYS_set_global_alpha(100);
	//SYS_disable_colorkey();
	m_nOsdBpp = 16;//SYS_get_osdbpp();
	m_nAudioBalance = 3;

	m_nVolume = 100;
	m_bFast = false;
	m_bSetEPGSize = false;
	m_bWrFirstPkg = false;

	m_nMode = M_LIVE;
	//VideoHide();
	//if (!IsSoftFit())
		LunchIptv();
	//VideoShow();
#ifdef WF
	m_fp = NULL;
#endif
}

CTsPlayer::~CTsPlayer()
{
	  set_sys_int("/sys/class/graphics/fb0/blank",1);
		QuitIptv();
}

//取得播放模式,保留，暂不用
int  CTsPlayer::GetPlayMode()
{	
	return 1;
}
int CTsPlayer::SetVideoWindow(int x,int y,int width,int height)
{
// int     m_nEPGWidth;
 // int     m_nEPGHeight;
   int epg_centre_x = 0;
   int epg_centre_y = 0;
   int old_videowindow_certre_x = 0;
   int old_videowindow_certre_y = 0;
   int new_videowindow_certre_x = 0;
   int new_videowindow_certre_y = 0;
   int new_videowindow_width = 0;
   int new_videowindow_height = 0;
   char vaxis_newx_str[10] = {0};
	 char vaxis_newy_str[10] = {0};
	 char vaxis_width_str[10] = {0};
	 char vaxis_height_str[10] = {0};
	 char mode[16]= {0};
   int vaxis_newx= -1,vaxis_newy = -1,vaxis_width= -1,vaxis_height= -1;
	int fd_axis,fd_mode;
	int x2 = 0,y2 = 0,width2 = 0,height2 = 0;

	 const char *path_mode = "/sys/class/video/screen_mode" ;
	 const char *path_axis = "/sys/class/video/axis" ;

	char  bcmd[32];
      
	char buffer[15];
	int mode_w = 0,mode_h = 0;
	
    if (prop_shouldshowlog == '1') {
        __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "CTsPlayer::SetVideoWindow:  %d, %d ,%d ,%d\n",x ,y ,width,height);
    }

    //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "CTsPlayer::SetVideoWindow:  %d, %d ,%d ,%d\n",x ,y ,width,height);
  
   /*adjust axis as rate recurrence*/
   get_display_mode(mode);
   GetVideoPixels(mode_w, mode_h);
   
   x2=x*mode_w/m_nEPGWidth;
	 width2=width*mode_w/m_nEPGWidth;
	 y2=y*mode_h/m_nEPGHeight;
   height2=height*mode_h/m_nEPGHeight;
   
   old_videowindow_certre_x = x2+int(width2/2);
   old_videowindow_certre_y = y2+int(height2/2);
   
   if(!strncmp(mode, "480i", 4) || !strncmp(mode, "480p", 4))
	 {
		  if(!strncmp(mode, "480i", 4))
		  {
          property_get("ubootenv.var.480ioutputx",vaxis_newx_str,"0");			  
			    property_get("ubootenv.var.480ioutputy",vaxis_newy_str,"0");			  
			    property_get("ubootenv.var.480ioutputwidth",vaxis_width_str,"720");			  
			    property_get("ubootenv.var.480ioutputheight",vaxis_height_str,"480");

			}
			else
		  {            
          property_get("ubootenv.var.480poutputx",vaxis_newx_str,"0");			  
			    property_get("ubootenv.var.480poutputy",vaxis_newy_str,"0");			  
			    property_get("ubootenv.var.480poutputwidth",vaxis_width_str,"720");			  
			    property_get("ubootenv.var.480poutputheight",vaxis_height_str,"480");        
			}

			vaxis_newx = atoi(vaxis_newx_str);
			vaxis_newy = atoi(vaxis_newy_str);
			vaxis_width = atoi(vaxis_width_str);
			vaxis_height = atoi(vaxis_height_str);
			
            if (prop_shouldshowlog == '1') {
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
                                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);
            }

            //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
            //	                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);
			epg_centre_x = vaxis_newx+int(vaxis_width/2);
			epg_centre_y = vaxis_newy+int(vaxis_height/2);
			
			new_videowindow_certre_x = epg_centre_x + int((old_videowindow_certre_x-mode_w/2)*vaxis_width/mode_w);
      new_videowindow_certre_y = epg_centre_y + int((old_videowindow_certre_y-mode_h/2)*vaxis_height/mode_h);
			new_videowindow_width = int(width2*vaxis_width/mode_w);
      new_videowindow_height = int(height2*vaxis_height/mode_h);
			
			
			
		}
		else if(!strncmp(mode, "576i", 4) || !strncmp(mode, "576p", 4))
		{
		    
		    if(!strncmp(mode, "576i", 4))
		    {
          property_get("ubootenv.var.576ioutputx",vaxis_newx_str,"0");			  
			    property_get("ubootenv.var.576ioutputy",vaxis_newy_str,"0");			  
			    property_get("ubootenv.var.576ioutputwidth",vaxis_width_str,"720");			  
			    property_get("ubootenv.var.576ioutputheight",vaxis_height_str,"576");

			}
			else
		    {            
          property_get("ubootenv.var.576poutputx",vaxis_newx_str,"0");			  
			    property_get("ubootenv.var.576poutputy",vaxis_newy_str,"0");			  
			    property_get("ubootenv.var.576poutputwidth",vaxis_width_str,"720");			  
			    property_get("ubootenv.var.576poutputheight",vaxis_height_str,"576");        
			}


			vaxis_newx = atoi(vaxis_newx_str);
			vaxis_newy = atoi(vaxis_newy_str);
			vaxis_width = atoi(vaxis_width_str);
			vaxis_height = atoi(vaxis_height_str);

            if (prop_shouldshowlog == '1') {
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
                                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);
            }
            //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
            //	                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);
			epg_centre_x = vaxis_newx+int(vaxis_width/2);
			epg_centre_y = vaxis_newy+int(vaxis_height/2);
			
			new_videowindow_certre_x = epg_centre_x + int((old_videowindow_certre_x-mode_w/2)*vaxis_width/mode_w);
      new_videowindow_certre_y = epg_centre_y + int((old_videowindow_certre_y-mode_h/2)*vaxis_height/mode_h);
			new_videowindow_width = int(width2*vaxis_width/mode_w);
      new_videowindow_height = int(height2*vaxis_height/mode_h);
		}
		else if(!strncmp(mode, "720p", 4))
		{
		    
			property_get("ubootenv.var.720poutputx",vaxis_newx_str,"0");			  
			property_get("ubootenv.var.720poutputy",vaxis_newy_str,"0");			  
			property_get("ubootenv.var.720poutputwidth",vaxis_width_str,"1280"); 		  
			property_get("ubootenv.var.720poutputheight",vaxis_height_str,"720");
			
            if (prop_shouldshowlog == '1') {
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_height_str:%s\n",
                                    vaxis_height_str);
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newy_str:%s\n",
                                    vaxis_newy_str);
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_width_str:%s\n",
                                    vaxis_width_str);
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_height_str:%s\n",
                                    vaxis_height_str);
            }

            //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_height_str:%s\n",
            //						vaxis_height_str);
            //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newy_str:%s\n",
            //						vaxis_newy_str);
            //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_width_str:%s\n",
            //						vaxis_width_str);
            //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_height_str:%s\n",
            //						vaxis_height_str);

			vaxis_newx = atoi(vaxis_newx_str);
			vaxis_newy = atoi(vaxis_newy_str);
			vaxis_width = atoi(vaxis_width_str);
			vaxis_height = atoi(vaxis_height_str);
			
            if (prop_shouldshowlog == '1') {
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
                                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);
            }

            //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
            //	                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);
      epg_centre_x = vaxis_newx+int(vaxis_width/2);
			epg_centre_y = vaxis_newy+int(vaxis_height/2);
			
			new_videowindow_certre_x = epg_centre_x + int((old_videowindow_certre_x-mode_w/2)*vaxis_width/mode_w);
      new_videowindow_certre_y = epg_centre_y + int((old_videowindow_certre_y-mode_h/2)*vaxis_height/mode_h);
			new_videowindow_width = int(width2*vaxis_width/mode_w);
      new_videowindow_height = int(height2*vaxis_height/mode_h);
					    
		}
		else if(!strncmp(mode, "1080i", 5) || !strncmp(mode, "1080p", 5))
		{
		    
		  if(!strncmp(mode, "1080i", 5))
		  {
          property_get("ubootenv.var.1080ioutputx",vaxis_newx_str,"0");			  
			    property_get("ubootenv.var.1080ioutputy",vaxis_newy_str,"0");			  
			    property_get("ubootenv.var.1080ioutputwidth",vaxis_width_str,"1920");			  
			    property_get("ubootenv.var.1080ioutputheight",vaxis_height_str,"1080");

			}
			else
		    {            
                property_get("ubootenv.var.1080poutputx",vaxis_newx_str,"0");			  
			    property_get("ubootenv.var.1080poutputy",vaxis_newy_str,"0");			  
			    property_get("ubootenv.var.1080poutputwidth",vaxis_width_str,"1920");			  
			    property_get("ubootenv.var.1080poutputheight",vaxis_height_str,"1080");        
			}

			if(prop_shouldshowlog == '1') {
				__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "GL_2X_iptv_scale530 : 1080i");
			}
		    //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "GL_2X_iptv_scale530 : 1080i");

			vaxis_newx = atoi(vaxis_newx_str);
			vaxis_newy = atoi(vaxis_newy_str);
			vaxis_width = atoi(vaxis_width_str);
			vaxis_height = atoi(vaxis_height_str);

            if (prop_shouldshowlog == '1') {
                __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
                                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);
            }
            //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "vaxis_newx:%d vaxis_newy:%d vaxis_width:%d vaxis_height:%d\n",
            //	                                  vaxis_newx,vaxis_newy,vaxis_width,vaxis_height);
            epg_centre_x = vaxis_newx+int(vaxis_width/2);
			epg_centre_y = vaxis_newy+int(vaxis_height/2);
			
			new_videowindow_certre_x = epg_centre_x + int((old_videowindow_certre_x-mode_w/2)*vaxis_width/mode_w);
            new_videowindow_certre_y = epg_centre_y + int((old_videowindow_certre_y-mode_h/2)*vaxis_height/mode_h);
			new_videowindow_width = int(width2*vaxis_width/mode_w);
            new_videowindow_height = int(height2*vaxis_height/mode_h);
	}
   
  
    if (prop_shouldshowlog == '1') {
        __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "CTsPlayer::mode_w = %d , mode_h = %d , mw = %d, mh = %d \n",mode_w,mode_h,m_nEPGWidth,m_nEPGHeight);
    }
    //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "CTsPlayer::mode_w = %d , mode_h = %d , mw = %d, mh = %d \n",mode_w,mode_h,m_nEPGWidth,m_nEPGHeight);

   if(m_nEPGWidth !=0 && m_nEPGHeight !=0)
	 {
			  fd_mode = open(path_mode, O_CREAT | O_RDWR | O_TRUNC, 0644);
			  if (fd_mode >= 0)
			  {
			  	 write(fd_mode, "1", 1);
			  	 close(fd_mode);
			  }
	 }
	fd_axis = open(path_axis, O_CREAT | O_RDWR | O_TRUNC, 0644);
  if (fd_axis >= 0) 
  {

		sprintf(bcmd, "%d %d %d %d", new_videowindow_certre_x-int(new_videowindow_width/2)-1,
		                             new_videowindow_certre_y-int(new_videowindow_height/2)-1,
		                             new_videowindow_certre_x+int(new_videowindow_width/2)+1,
		                             new_videowindow_certre_y+int(new_videowindow_height/2)+1);            
            
		write(fd_axis, bcmd, strlen(bcmd));
        if (prop_shouldshowlog == '1') {
            __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "setvideoaxis: %s\n", bcmd);
        }
        //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "setvideoaxis: %s\n", bcmd);
        close(fd_axis);
        //set_sys_str("/sys/class/graphics/fb0/video_hole","0 0 1280 720 0 8");
        return 0;
  }
  return 0 ;
}

int CTsPlayer::VideoShow(void)
{
	set_sys_str("/sys/class/graphics/fb0/video_hole","0 0 1280 720 0 8");
	return 0;
}

/*int CTsPlayer::SetColorKey(int enable,int key565)
{



	//if (m_nOsdBpp != 16)
	//	return 0;
	int ret = -1;
    int fd_fb0 = open("/dev/graphics/fb0", O_RDWR);
    if (fd_fb0 >= 0) {
        uint32_t myKeyColor = key565;
        uint32_t myKeyColor_en = !!enable;
        printf("enablecolorkey color=%#x\n", myKeyColor);
		myKeyColor=0xff;
		ret = ioctl(fd_fb0, FBIOPUT_OSD_SRCCOLORKEY, &myKeyColor);
		myKeyColor = key565;
        ret = ioctl(fd_fb0, FBIOPUT_OSD_SRCCOLORKEY, &myKeyColor);
        ret += ioctl(fd_fb0, FBIOPUT_OSD_SRCKEY_ENABLE, &myKeyColor_en);
        close(fd_fb0);
    }
    return ret;
}*/


int CTsPlayer::VideoHide(void)
{
	set_sys_str("/sys/class/graphics/fb0/video_hole","0 0 0 0 0 0");
	return 0;
}


void CTsPlayer::InitVideo(PVIDEO_PARA_T pVideoPara)
{
	vPara=*pVideoPara;
    if (prop_shouldshowlog == '1') {
        __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "InitAudio vPara->pid:%d vPara->vFmt%d\n",vPara.pid,vPara.vFmt);
    }
    //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "InitAudio vPara->pid:%d vPara->vFmt%d\n",vPara.pid,vPara.vFmt);
	return ;
}

void CTsPlayer::InitAudio(PAUDIO_PARA_T pAudioPara)
{
    PAUDIO_PARA_T pAP = pAudioPara;
	int count = 0;
    if(prop_shouldshowlog == '1') {
        __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "InitAudio");
    }
    //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "InitAudio");
    memset(a_aPara,0,sizeof(AUDIO_PARA_T)*MAX_AUDIO_PARAM_SIZE);
	
	while((pAP->pid != 0)
		   &&(count<MAX_AUDIO_PARAM_SIZE))
	{
    a_aPara[count]= *pAP;
    if (prop_shouldshowlog == '1') {
        __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "InitAudio pAP->pid:%d pAP->aFmt:%d\n",pAP->pid,pAP->aFmt);
    }
    //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "InitAudio pAP->pid:%d pAP->aFmt:%d\n",pAP->pid,pAP->aFmt);
		pAP++;
		count++;		
	}
	return ;
}

bool CTsPlayer::StartPlay()
{
	int ret;
	memset(pcodec,0,sizeof(*pcodec));
	pcodec->stream_type=STREAM_TYPE_TS;
	pcodec->video_type = vPara.vFmt;
	pcodec->has_video=1;
	pcodec->audio_type=a_aPara[0].aFmt;
	
	if ( pcodec->audio_type == 19)
	{
	   	pcodec->audio_type = AFORMAT_EAC3;
	}
	
	if(IS_AUIDO_NEED_EXT_INFO(pcodec->audio_type))
	{
        pcodec->audio_info.valid = 1;
        if (prop_shouldshowlog == '1') {
            LOGI("set audio_info.valid to 1");
        }
        //LOGI("set audio_info.valid to 1");
  }

	if (!m_bFast)
	{
		if((int)a_aPara[0].pid != 0)
		{
		    pcodec->has_audio=1;
	      pcodec->audio_pid=(int)a_aPara[0].pid;
	    }
	    //pcodec->audio_samplerate=a_aPara[0].nSampleRate;	
	    //pcodec->audio_channels=a_aPara[0].nChannels;
        if (prop_shouldshowlog == '1') {
            __android_log_print(ANDROID_LOG_INFO,"TsPlayer","pcodec->audio_samplerate:%d pcodec->audio_channels:%d\n",pcodec->audio_samplerate,pcodec->audio_channels);
        }
        //__android_log_print(ANDROID_LOG_INFO,"TsPlayer","pcodec->audio_samplerate:%d pcodec->audio_channels:%d\n",pcodec->audio_samplerate,pcodec->audio_channels);
  }
  
	pcodec->video_pid=(int)vPara.pid;
	//pcodec->noblock = 1;
	 if (pcodec->video_type == VFORMAT_H264) {
        pcodec->am_sysinfo.format = VIDEO_DEC_FORMAT_H264;
        pcodec->am_sysinfo.param = (void *)(0);
    }
	
    if (prop_shouldshowlog == '1') {
        __android_log_print(ANDROID_LOG_INFO,"TsPlayer","set %d,%d,%d,%d\n",vPara.vFmt,a_aPara[0].aFmt,vPara.pid,a_aPara[0].pid);
    }
    //__android_log_print(ANDROID_LOG_INFO,"TsPlayer","set %d,%d,%d,%d\n",vPara.vFmt,a_aPara[0].aFmt,vPara.pid,a_aPara[0].pid);
    pcodec->noblock = 0;
	
    /*other setting*/
    ret = codec_init(pcodec);
    if (prop_shouldshowlog == '1') {
        __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "StartPlay codec_init After:%d\n", ret);
    }
    //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "StartPlay codec_init After:%d\n", ret);
    if (ret == 0)
    {
        if (m_nMode == M_LIVE)
        set_sys_int("/sys/class/video/blackout_policy",0);
		m_bIsPlay = true;
#ifdef WF
		m_fp = fopen("/mnt/sda/sda1/Live.ts", "wb+");
		//m_fp = fopen("/data/Live.ts", "wb+");	
#endif
	}
    m_bWrFirstPkg = true;
    set_sys_str("/sys/class/graphics/fb0/video_hole","0 0 1280 720 0 8");
    writecount = 0;
	return !ret;
}

int CTsPlayer::WriteData(unsigned char* pBuffer, unsigned int nSize)
{	
	int ret = -1;
    buf_status audio_buf;
	buf_status video_buf;

	if (!m_bIsPlay)
		return -1;
    if(m_bWrFirstPkg == false)
    {
		
	   codec_get_vbuf_state(pcodec,&audio_buf);
	   codec_get_vbuf_state(pcodec,&video_buf);

       if(pcodec->has_video)
       {
		   if(pcodec->video_type == VFORMAT_MJPEG)
		   {
               if(video_buf.data_len < (RES_VIDEO_SIZE >> 2))
               {
                  if(pfunc_player_evt != NULL)
                  {
                     pfunc_player_evt(IPTV_PLAYER_EVT_ABEND,player_evt_hander);
				  }
                  //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "WriteData video low level\n");
			   }
		   }
		   else
		   {
               if(video_buf.data_len< RES_VIDEO_SIZE)
               {

			      if(pfunc_player_evt != NULL)
                  {
                     pfunc_player_evt(IPTV_PLAYER_EVT_ABEND,player_evt_hander);
				  }
                  //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "WriteData video low level\n");
			   }
		   }
	   }

	   if(pcodec->has_audio)
       {
           if(audio_buf.data_len < RES_AUDIO_SIZE)
           {
               if(pfunc_player_evt != NULL)
               {
                    pfunc_player_evt(IPTV_PLAYER_EVT_ABEND,player_evt_hander);
			   }
               //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "WriteData audio low level\n");
		   }
	   }
    }
	
	ret = codec_write(pcodec,pBuffer,nSize);
	
	if (ret > 0)
	{
    #ifdef WF
		if (m_fp != NULL)
		{
			fwrite(pBuffer, 1, nSize, m_fp);
		}
    #endif
	  if(writecount >= MAX_WRITE_COUNT)
	  {
	      m_bWrFirstPkg = false;
          writecount = 0;
	  }

      if(m_bWrFirstPkg == true)
      {
          writecount++;
	  }
	  
	  //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "codec_write return > 0\n");
	}
	else
	{
		//__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "codec_write return < 0\n");
		return -1;
	}
	return ret;
}

bool CTsPlayer::Pause()
{



	codec_pause(pcodec);
	return true;
}

bool CTsPlayer::Resume()
{



	codec_resume(pcodec);
	return true;
}

bool CTsPlayer::Fast()
{
	int ret;
	


		ret = set_sys_int("/sys/class/video/blackout_policy",0);
	if (ret)
		return false;
	Stop();
	m_bFast = true;	
	ret = StartPlay();
	if (!ret)
		return false;


	printf("Fast: codec_set_cntl_mode %d\n",TRICKMODE_I);
	ret = codec_set_cntl_mode(pcodec, TRICKMODE_I);
	return !ret;
}
bool CTsPlayer::StopFast()
{




	int ret;
	m_bFast = false;
	ret = codec_set_cntl_mode(pcodec, TRICKMODE_NONE);
	Stop();
	ret = StartPlay();
	if (!ret)
		return false;
	//ret = set_sys_int("/sys/class/video/blackout_policy",1);//for debug
	if (ret)
		return false;

	return !ret;
}



bool CTsPlayer::Stop()
{


    
  	int ret;
  	
  	set_sys_int("/sys/class/video/disable_video",0);	
	//if (m_nMode == M_LIVE)
		//set_sys_int("/sys/class/video/blackout_policy",1);
    //set_sys_str("/sys/class/graphics/fb0/video_hole","0 0 0 0 0 0");
    
	if (m_bIsPlay){
#ifdef WF
		if (m_fp != NULL){
			fclose(m_fp);
			m_fp = NULL;
		}
#endif

        m_bFast = false;
        m_bIsPlay = false;
        ret = codec_set_cntl_mode(pcodec, TRICKMODE_NONE);
        if (prop_shouldshowlog == '1') {
            __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "m_bIsPlay is true");
        }
        //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "m_bIsPlay is true");
        //codec_set_cntl_mode(pcodec, TRICKMODE_NONE);
        ret = codec_close(pcodec);
        pcodec->handle = -1;
        //codec_set_cntl_mode(pcodec, TRICKMODE_NONE);
        if (prop_shouldshowlog == '1') {
            __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "Stop  codec_close After:%d\n", ret);
        }
        //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "Stop  codec_close After:%d\n", ret);
    }

	else
	{
        if (prop_shouldshowlog == '1') {
            __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "m_bIsPlay is false");
        }
        //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "m_bIsPlay is false");
    }
	//VideoHide(); 
	if (m_bSetEPGSize){
		if (m_nEPGWidth == 1280 && m_nEPGHeight == 720)
			SwitchResolution(2, 0);
		else
			SwitchResolution(1, 0);	
		m_bSetEPGSize = false;
	}

	return true;
}
bool CTsPlayer::Seek()
{	


  // set_sys_int("/sys/class/video/blackout_policy",1);
	Stop();
	usleep(500*1000);
	StartPlay();
	return true;
	//return true;
}
int CTsPlayer::GetVolume()
{



	float volume = 1.0f;
	int ret;

	ret = codec_get_volume(pcodec, &volume);
	if (ret < 0)
	{
		return m_nVolume;
	}
	int nVolume = volume * 100;
	if (nVolume <= 0)
		return m_nVolume;
    
	return (int)(volume*100);
}
bool CTsPlayer::SetVolume(int volume)
{

    if (prop_shouldshowlog == '1') {
        __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "SetVolume");
    }
    //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "SetVolume");
    int ret = codec_set_volume(pcodec, (float)volume/100.0);
    m_nVolume = volume;
    return true;//!ret;
}
//获取当前声道,1:左声道，2:右声道，3:双声道
int CTsPlayer::GetAudioBalance()
{
	return m_nAudioBalance;
}
//设置声道
//nAudioBlance:,1:左声道，2:右声道，3:双声道
bool CTsPlayer::SetAudioBalance(int nAudioBalance)
{
	if((nAudioBalance < 1) && (nAudioBalance > 3))
		return false;
	m_nAudioBalance = nAudioBalance;
	if (nAudioBalance == 1){
        if (prop_shouldshowlog == '1') {
            __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "SetAudioBalance 1\n");
        }
        //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "SetAudioBalance 1\n");
        codec_left_mono(pcodec);
    }else if(nAudioBalance == 2){
        if (prop_shouldshowlog == '1') {
            __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "SetAudioBalance 2\n");
        }
        //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "SetAudioBalance 2\n");
        codec_right_mono(pcodec);
    }else if(nAudioBalance == 3){
        if (prop_shouldshowlog == '1') {
            __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "SetAudioBalance 3\n");
        }
        //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "SetAudioBalance 3\n");
        codec_stereo(pcodec);
	}
	return true;
}

void CTsPlayer::GetVideoPixels(int& width, int& height)
{

    if (prop_shouldshowlog == '1') {
        __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "GetVideoPixels");
	}
    //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "GetVideoPixels");


	int fd = open("/sys/class/display/mode", O_RDONLY); 
	if (fd >= 0)
	{
		char buffer[12] = {0};
		int nLen = read(fd, buffer, sizeof(buffer));
		close(fd);
        if (prop_shouldshowlog == '1') {
            __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "read succeed");
            __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "%d:%d", nLen, buffer[0]);
        }
        //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "read succeed");
        //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "%d:%d", nLen, buffer[0]);
		for(int i=0;i<(int)sizeof(buffer);i++)
		{
			if ((buffer[i] == 'p') || (buffer[i] == 'P') || (buffer[i] == 'i') || (buffer[i] == 'I'))
			{
				buffer[i] = 0;
				break;
			}
		}
		height = atoi(buffer);
		if (height == 1080)
			width = 1920;
		else if(height == 720)
			width = 1280;
		else if(height == 480)
			width = 720;
		else if(height == 576)
			width = 720;
		else{
			width = 1920;
			height = 1080;
		}
	}
}

bool CTsPlayer::SetRatio(int nRatio)
{
	char mode[16]={0};
	char writedata[40] = {0};
	int width = 0;
	int height = 0;
	int new_x = 0;
	int new_y = 0;
	int new_width = 0;
	int new_height = 0;
	int mode_width = 0;
	int mode_height = 0;
	vdec_status vdec;
	codec_get_vdec_state(pcodec,&vdec);
	width = vdec.width;
	height = vdec.height;
    if (prop_shouldshowlog == '1') {
        __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "SetRatio width:%d height:%d\n",width,height);
    }
    //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "SetRatio width:%d height:%d\n",width,height);
  	
	get_display_mode(mode);
	if(!strncmp(mode, "480i", 4) || !strncmp(mode, "480p", 4))
	{
     mode_width = 720;
     mode_height = 480;
	}
	else if(!strncmp(mode, "576i", 4) || !strncmp(mode, "576p", 4))
	{
     mode_width = 720;
     mode_height = 576;
	}
	else if(!strncmp(mode, "720p", 4))
	{
     mode_width = 1280;
     mode_height = 720;
	}
	else if(!strncmp(mode, "1080i", 5) || !strncmp(mode, "1080p", 5))
	{
	   mode_width = 1920;
     mode_height = 1080;
	}
	set_sys_int("/sys/class/video/disable_video",2);
	if(nRatio == 1)
	{
		set_sys_int("/sys/class/video/screen_mode",1);
		return true;
	}
	else if(nRatio == 2)
	{
		set_sys_int("/sys/class/video/screen_mode",1);
		new_width=mode_width;
		new_height=int(mode_width*height/width);
		new_x = 0;
		new_y = int((mode_height-new_height)/2);
        if (prop_shouldshowlog == '1') {
            __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "SetRatio new_x:%d new_y:%d new_width:%d new_height%d\n"
                                             ,new_x,new_y,new_width,new_height);
        }
        //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "SetRatio new_x:%d new_y:%d new_width:%d new_height%d\n"
        //                                    ,new_x,new_y,new_width,new_height);
        sprintf(writedata,"%d %d %d %d",
                         new_x,new_y,new_x+new_width-1,new_y+new_height-1);
		set_sys_str("/sys/class/video/axis",writedata);
		return true;
	}
	else if(nRatio == 3)
	{
		set_sys_int("/sys/class/video/screen_mode",1);
		new_width = int(mode_height*width/height);
		new_height = mode_height;
		new_x = int((mode_width - new_width)/2);
		new_y = 0;
        if (prop_shouldshowlog == '1') {
            __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "SetRatio new_x:%d new_y:%d new_width:%d new_height%d\n"
                                             ,new_x,new_y,new_width,new_height);
        }
		//__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "SetRatio new_x:%d new_y:%d new_width:%d new_height%d\n"
		 //                                    ,new_x,new_y,new_width,new_height);
    sprintf(writedata,"%d %d %d %d",
				         new_x,new_y,new_x+new_width-1,new_y+new_height-1);
		set_sys_str("/sys/class/video/axis",writedata);
		return true;
	}
	else if(nRatio == 255)
	{
		set_sys_int("/sys/class/video/disable_video",1);
		return true;
	}
	return false;
}
/*
ITsPlayer* GetTsPlayer()
{
	return new CTsPlayer();
}
*/
bool CTsPlayer::IsSoftFit()
{
	return false;
}

void CTsPlayer::SetEPGSize(int w, int h)
{
    if (prop_shouldshowlog == '1') {
        LOGE("SetEPGSize:  w=%d, h=%d,  m_bIsPlay=%d,  m_bSetEPGSize=%d.\n", w, h, m_bIsPlay, m_bSetEPGSize);
    }
    //LOGE("SetEPGSize:  w=%d, h=%d,  m_bIsPlay=%d,  m_bSetEPGSize=%d.\n", w, h, m_bIsPlay, m_bSetEPGSize);
    //if (IsSoftFit())
        //return;
    m_nEPGWidth = w;
    m_nEPGHeight = h;
    if (!m_bIsPlay){
        if (w == 1280 && h == 720)
			SwitchResolution(2, 0);
		else
			SwitchResolution(1, 0);	
	}else
		m_bSetEPGSize = true;
}

void CTsPlayer::SwitchAudioTrack(int pid)
{
    int count = 0;
  
	while((a_aPara[count].pid != pid)
		   &&(a_aPara[count].pid != 0)
		   &&(count < MAX_AUDIO_PARAM_SIZE))
  {
       count++;
	}
	
	if (!m_bIsPlay)
		return ;
	
	codec_audio_automute(pcodec->adec_priv, 1);
  codec_close_audio(pcodec);
  pcodec->audio_pid = 0xffff;

  if (codec_set_audio_pid(pcodec)) 
  {
        if (prop_shouldshowlog == '1') {
            __android_log_print(ANDROID_LOG_INFO,"TsPlayer","set invalid audio pid failed\n");
        }
        //__android_log_print(ANDROID_LOG_INFO,"TsPlayer","set invalid audio pid failed\n");
        return;
  }
  
    if(count < MAX_AUDIO_PARAM_SIZE)
    {
  	    pcodec->has_audio=1;
        pcodec->audio_type= a_aPara[count].aFmt;
	    pcodec->audio_pid=(int)a_aPara[count].pid;
	    //pcodec->audio_samplerate=a_aPara[count].nSampleRate;	
	    //pcodec->audio_channels=a_aPara[count].nChannels;
    }
    if (prop_shouldshowlog == '1'){
        __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "SwitchAudioTrack pcodec->audio_samplerate:%d pcodec->audio_channels:%d\n",pcodec->audio_samplerate,pcodec->audio_channels);
        __android_log_print(ANDROID_LOG_INFO, "TsPlayer", "SwitchAudioTrack pcodec->audio_type:%d pcodec->audio_pid:%d\n",pcodec->audio_type,pcodec->audio_pid);
    }
    //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "SwitchAudioTrack pcodec->audio_samplerate:%d pcodec->audio_channels:%d\n",pcodec->audio_samplerate,pcodec->audio_channels);
    //__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "SwitchAudioTrack pcodec->audio_type:%d pcodec->audio_pid:%d\n",pcodec->audio_type,pcodec->audio_pid);
    //codec_set_audio_pid(pcodec);
    if(IS_AUIDO_NEED_EXT_INFO(pcodec->audio_type))
	{
        pcodec->audio_info.valid = 1;
        if (prop_shouldshowlog == '1') {
            LOGI("set audio_info.valid to 1");
        }
        //LOGI("set audio_info.valid to 1");
    }
  
    if (codec_audio_reinit(pcodec)) 
    {
        if (prop_shouldshowlog == '1') {
            __android_log_print(ANDROID_LOG_INFO,"TsPlayer","reset init failed\n");
        }
        // __android_log_print(ANDROID_LOG_INFO,"TsPlayer","reset init failed\n");
        return;
    }
  
    if (codec_reset_audio(pcodec)) 
    {
	    if (prop_shouldshowlog == '1'){
            __android_log_print(ANDROID_LOG_INFO,"TsPlayer","reset audio failed\n");
        }
        // __android_log_print(ANDROID_LOG_INFO,"TsPlayer","reset audio failed\n");
        return;
    }
    codec_resume_audio(pcodec, 1);
    codec_audio_automute(pcodec->adec_priv, 0);
  
    return ;
}


void CTsPlayer::SwitchSubtitle(int pid) 
{
}

void CTsPlayer::SetProperty(int nType, int nSub, int nValue) 
{

}

long CTsPlayer::GetCurrentPlayTime() 
{
    long video_pts = 0;
    video_pts = codec_get_vpts(pcodec);
	return video_pts;
}
void CTsPlayer::leaveChannel()
{
    if (prop_shouldshowlog == '1') {
        __android_log_print(ANDROID_LOG_INFO,"TsPlayer","leaveChannel be call\n");
    }
    //__android_log_print(ANDROID_LOG_INFO,"TsPlayer","leaveChannel be call\n");
    Stop();
}

void CTsPlayer::SetSurface(Surface* pSurface)
{
	  sp<ISurfaceTexture> surfaceTexture;
	  sp<ANativeWindow> 	mNativeWindow;
	  int usage=0;
	  surfaceTexture=pSurface->getSurfaceTexture();
    mNativeWindow=new SurfaceTextureClient(surfaceTexture);
    native_window_set_usage(mNativeWindow.get(),usage | GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_EXTERNAL_DISP | GRALLOC_USAGE_AML_VIDEO_OVERLAY);
}

void CTsPlayer::playerback_register_evt_cb(IPTV_PLAYER_EVT_CB pfunc, void *hander)
{
    pfunc_player_evt = pfunc ;

	player_evt_hander = hander;
}
