
#include "TsPlayer.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include "../IPTVPlayer/PubAndroid.h"	 
#define DPrint(x)

#define M_LIVE	1
#define M_TVOD	2
#define M_VOD	3

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


typedef enum {
	VIDEO_NORMAL = 0,
	VIDEO_FULLSTRETCH,
	VIDEO_4X3,
	VIDEO_16X9,
	
}VideoScreenMode;

VideoScreenMode Current_screen_mode = VIDEO_NORMAL;

int AddOsdPath(void){
	
    FILE * fp;
    int ret = -1;
    const char *OsdPath = "add osdpath osd amvideo";
	
    fp = fopen("/sys/class/vfm/map", "w");
    
    if(fp != NULL) {
        ret = fprintf(fp, "%s", OsdPath);
    } else {
        LOGE("=VDIN CPP=> open /sys/class/vfm/map ERROR(%s)!!\n", strerror(errno));
        return -1;
    }

    if(ret<0)
	LOGE("=VDIN CPP=> add OsdPath ERROR(%s)!!!!!!!!!!!!!!!\n", strerror(errno));
	
    fclose(fp);
    return ret;
}

int RemoveOsdPath(void){
	
		FILE * fp;
    int ret = -1;
    const char *OsdPath = "rm osdpath";
	
    fp = fopen("/sys/class/vfm/map", "w");
    
    if(fp != NULL) {
        ret = fprintf(fp, "%s", OsdPath);
    } else {
        LOGE("=VDIN CPP=> open /sys/class/vfm/map ERROR(%s)!!\n", strerror(errno));
        return -1;
    }

    if(ret<0)
			LOGE("=VDIN CPP=> add OsdPath ERROR(%s)!!!!!!!!!!!!!!!\n", strerror(errno));
	
    fclose(fp);
    return ret;
}

void WriteDaxis(int mode){
	int fd_daxis = -1;
	char daxis_str[32];
	memset(daxis_str,0,32);

	if((fd_daxis = open("/sys/class/display/axis", O_RDWR)) < 0) {
		LOGD("open /sys/class/display/axis fail.");
		close(fd_daxis);
	}
	if(mode == 0){
		strcpy(daxis_str, "150 100 639 529");
		write(fd_daxis, daxis_str, strlen(daxis_str));
	}
	else if(mode == 1){
		strcpy(daxis_str, "0 0 639 529");
		write(fd_daxis, daxis_str, strlen(daxis_str));
	}
	else if(mode == 2){
		strcpy(daxis_str, "0 0 1279 719");
		write(fd_daxis, daxis_str, strlen(daxis_str));
	}
	
	close(fd_daxis);
}

int RmDefPath2(void)
{

    int fd, ret;
    char str[]="rm all";
    //char str[]="rm default";
   // char str[] = "iptvpath";
    fd = open("/sys/class/vfm/map", O_RDWR);

    if(fd < 0) {
        LOGE("=VDIN CPP=> open /sys/class/vfm/map ERROR(%s)!!\n",strerror(errno));
        return 0;
    } else {
        ret = write(fd, str, sizeof(str));
    }
    close(fd);
    return ret;
}

int EnableFreeScale(int osdwidth, int osdheight)
{
	
	int fd0 = -1, fd1 = -1;
		int fd_daxis = -1, fd_vaxis = -1;
		int fd_fb = -1; 
		int fd_freescale = -1,fd_freescale_rect = -1 ,fd_freescale_disp = -1;
		int fd_video = -1;
		int osd_width = 0, osd_height = 0;	
		int ret = -1;
		int x = 0, y = 0, w = 0, h = 0;
		int find_flag = 0;	
			char vaxis_str[80], daxis_str[32];
		 
		    RmDefPath2();
			AddOsdPath();

	
		if((fd0 = open("/dev/graphics/fb0", O_RDWR)) < 0) {
			LOGD("open /dev/graphics/fb0 fail.");
			goto exit;
		}
		if((fd1 = open("/dev/graphics/fb1", O_RDWR)) < 0) {
			LOGD("open /dev/graphics/fb1 fail.");
			goto exit;	
		}
		if((fd_vaxis = open("/sys/class/video/axis", O_RDWR)) < 0) {
			LOGD("open /sys/class/video/axis fail.");
			goto exit;		
		}
			
		if((fd_daxis = open("/sys/class/display/axis", O_RDWR)) < 0) {
			LOGD("open /sys/class/display/axis fail.");
			goto exit;
		}		
		
		if((fd_fb = open("/dev/graphics/fb0", O_RDWR)) < 0) {
			LOGD("open /dev/graphics/fb0 fail.");
			goto exit;
		}
	
		if((fd_video = open("/sys/class/video/disable_video", O_RDWR)) < 0) {
			LOGD("open /sys/class/video/disable_video fail.");
		}
		else
			LOGD("open /sys/class/video/disable_video sucessssss.");
	
		if((fd_freescale = open("/sys/class/freescale/ppscaler", O_RDWR)) < 0) {
			LOGD("open /sys/class/freescale/ppscaler fail.");	
		}
			
		if((fd_freescale_rect = open("/sys/class/freescale/ppscaler_rect", O_RDWR)) < 0) {
			LOGD("open /sys/class/freescale/ppscaler_rect fail.");	
		}
		
		if((fd_freescale_disp = open("/sys/class/freescale/disp", O_RDWR)) < 0) {
			LOGD("open /sys/class/freescale/disp fail.");	
		}
	
		memset(vaxis_str,0,80); 
		if(fd_vaxis>=0){
			int ret_len = read(fd_vaxis, vaxis_str, sizeof(vaxis_str));
			if(ret_len>0){
				if(sscanf(vaxis_str,"%d %d %d %d",&x,&y,&w,&h)>0){
					w = w -x + 1;
					h = h -y + 1;
					find_flag = 1;	
					LOGD("enable mode: vaxis: x:%d, y:%d, w:%d, h:%d.",x,y,w,h);
				}
			}
		}
		
		memset(daxis_str,0,32); 
		
	//	if(ioctl(fd_fb, FBIOGET_VSCREENINFO, &vinfo) == 0) {
	//		osd_width = vinfo.xres;
	//		osd_height = vinfo.yres;
	//		sprintf(daxis_str, "0 0 %d %d 0 0 18 18", vinfo.xres, vinfo.yres);
	//	} else {
	//		LOGI("get FBIOGET_VSCREENINFO fail.");
	//		goto exit;
	//	}
	
		osd_width = osdwidth;
		osd_height = osdheight;
	
		if (fd_freescale >= 0) {
			int len=write(fd_freescale, "0", 1); 
			LOGD("write fd_freescale 0 len is %d.",len);
			//usleep(200000);
		}
		if (fd_video >= 0)	{
			int len = write(fd_video, "1", 1);
			LOGD("write fd_video len is %d.",len);
			//usleep(200000);
		}
		else
			LOGD("write fd_video open /sys/class/video/disable_video fail.");
	
		if (fd_freescale >= 0)	{
			int len = write(fd_freescale, "1", strlen("1"));
			LOGD("write fd_freescale 1 len is %d.",len);
			//usleep(200000);
		}
		if(fd_freescale_rect >= 0)
			write(fd_freescale_rect, "0 0 1920 1080 0", strlen("0 0 1920 1080 0"));
	    if(fd_vaxis >= 0)
			write(fd_vaxis, "0 0 1920 1080", strlen("0 0 1920 1080"));
	
		if(fd_freescale_disp >= 0){
			char temp[32];
			//strcpy(temp, "%d %d",osdwidth,osdheight);
			sprintf(temp, "%d %d",osdwidth,osdheight);
			write(fd_freescale_disp, temp, strlen(temp)); 
           //write(fd_freescale_disp, "1280 720", strlen("1280 720")); //here 640 and 530 is same as osd width and osd height
			//usleep(200000);
		}
		strcpy(daxis_str, "0 0 1920 1080 0 0 64 64");
		write(fd_daxis, daxis_str, strlen(daxis_str));
		
		ioctl(fd0,FBIOPUT_OSD_FREE_SCALE_ENABLE,0);
		ioctl(fd1,FBIOPUT_OSD_FREE_SCALE_ENABLE,0);
		ioctl(fd0,FBIOPUT_OSD_FREE_SCALE_WIDTH,osd_width);
		ioctl(fd0,FBIOPUT_OSD_FREE_SCALE_HEIGHT,osd_height); 
		ioctl(fd1,FBIOPUT_OSD_FREE_SCALE_WIDTH,osd_width);
		ioctl(fd1,FBIOPUT_OSD_FREE_SCALE_HEIGHT,osd_height);	
		ioctl(fd0,FBIOPUT_OSD_FREE_SCALE_ENABLE,1);
		ioctl(fd1,FBIOPUT_OSD_FREE_SCALE_ENABLE,1); 
		
		if((fd_freescale >= 0)&&(find_flag)){
			write(fd_vaxis, vaxis_str, strlen(vaxis_str));
		}
		if ((fd_video >= 0)&&(fd_freescale >= 0))	{
			int len = write(fd_video, "2", strlen("2"));	
			LOGD("write fd_video 2 len is %d.",len);
		}		
		ret = 0;
	
	exit:	
		close(fd0);
		close(fd1);
		close(fd_vaxis);
		close(fd_daxis);	
		close(fd_fb);	
		close(fd_freescale);
		close(fd_video);
		close(fd_freescale_rect);
		close(fd_freescale_disp);

	   RemoveOsdPath();
		return ret;
	

}


int DisableFreeScale(int mode)
{
	int fd0 = -1, fd1 = -1;
	int fd_freescale = -1;
	int ret = -1;
	
	if(mode == 0) return 0;		
		
	if((fd0 = open("/dev/graphics/fb0", O_RDWR)) < 0) {
		LOGD("open /dev/graphics/fb0 fail.");
		goto exit;
	}
	if((fd1 = open("/dev/graphics/fb1", O_RDWR)) < 0) {
		LOGD("open /dev/graphics/fb1 fail.");
		goto exit;	
	}
	if((fd_freescale = open("/sys/class/freescale/ppscaler", O_RDWR)) < 0) {
		LOGD("open /sys/class/freescale/ppscaler fail.");	
	}	
	ioctl(fd0,FBIOPUT_OSD_FREE_SCALE_ENABLE,0);
	ioctl(fd1,FBIOPUT_OSD_FREE_SCALE_ENABLE,0);	
	if (fd_freescale >= 0) 	write(fd_freescale, "0", strlen("0")); 	
	ret =0;
	
exit:	
	close(fd0);
	close(fd1);
	close(fd_freescale);
	return ret;
	
}

int ControlFreeScale(int mode)
{
	int fd_freescale = -1;
	int ret = -1;	

	if((fd_freescale = open("/sys/class/freescale/ppscaler", O_RDWR)) < 0) {
		LOGD("open /sys/class/freescale/ppscaler fail.");	
	}	

	if (fd_freescale >= 0) 	
	{
		if(mode == 0) 
		write(fd_freescale, "0", strlen("0")); 	
		else if(mode == 1) 
		write(fd_freescale, "1", strlen("1")); 
	}
	ret =0;
	
	close(fd_freescale);
	return ret;
}

/*this function only works for the case while video is stopping */
 void enable_osd_free_scale(int width,int height){
	AddOsdPath();
	/****************************************************/
	int fd0 = -1, fd1 = -1;
	int osd_width = 0, osd_height = 0;	
	if((fd0 = open("/dev/graphics/fb0", O_RDWR)) < 0) {
		LOGD("open /dev/graphics/fb0 fail.");
	}
	if((fd1 = open("/dev/graphics/fb1", O_RDWR)) < 0) {
		LOGD("open /dev/graphics/fb1 fail.");
	}
	
  osd_width = width;     //640
	osd_height = height;   //530
	if(fd0>=0 && fd1>=0)
	{
	ioctl(fd0,FBIOPUT_OSD_FREE_SCALE_ENABLE,0);
	ioctl(fd1,FBIOPUT_OSD_FREE_SCALE_ENABLE,0);
	ioctl(fd0,FBIOPUT_OSD_FREE_SCALE_WIDTH,osd_width);
	ioctl(fd0,FBIOPUT_OSD_FREE_SCALE_HEIGHT,osd_height); 
	ioctl(fd1,FBIOPUT_OSD_FREE_SCALE_WIDTH,osd_width);
	ioctl(fd1,FBIOPUT_OSD_FREE_SCALE_HEIGHT,osd_height);	
	ioctl(fd0,FBIOPUT_OSD_FREE_SCALE_ENABLE,1);
	ioctl(fd1,FBIOPUT_OSD_FREE_SCALE_ENABLE,1);
	
/*here we disable osd freescale to ensure we can remove osd path*/		
	ioctl(fd0,FBIOPUT_OSD_FREE_SCALE_ENABLE,0);
	ioctl(fd1,FBIOPUT_OSD_FREE_SCALE_ENABLE,0);	
	}
	close(fd0);
	close(fd1);
	/**************************************************************/
	
	RemoveOsdPath();
}






///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int RmDefPath(void)
{

    int fd, ret;
    char str[]="rm all";
    //char str[]="rm default";
   // char str[] = "iptvpath";
    fd = open("/sys/class/vfm/map", O_RDWR);

    if(fd < 0) {
        LOGE("=VDIN CPP=> open /sys/class/vfm/map ERROR(%s)!!\n",strerror(errno));
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
    
	//const char *IptvPath = "add default decoder deinterlace amvideo";
    fp = fopen("/sys/class/vfm/map", "w");
    
    if(fp != NULL) {
        ret = fprintf(fp, "%s", IptvPath);
    } else {
        LOGE("=VDIN CPP=> open /sys/class/vfm/map ERROR(%s)!!\n", strerror(errno));
        return -1;
    }

    if(ret<0)
	LOGE("=VDIN CPP=> add IptvPath ERROR(%s)!!!!!!!!!!!!!!!\n", strerror(errno));
	
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
        LOGE("=VDIN CPP=> open /sys/class/vfm/map ERROR(%s)!!\n", strerror(errno));
        return -1;
    }

    if(ret<0)
	LOGE("=VDIN CPP=> add IptvPath ERROR(%s)!!!!!!!!!!!!!!!\n", strerror(errno));
	
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
        LOGE("=VDIN CPP=> open /sys/class/vfm/map ERROR(%s)!!\n", strerror(errno));
        return -1;
    }

    if(ret<0)
	LOGE("=VDIN CPP=> add IptvPath ERROR(%s)!!!!!!!!!!!!!!!\n", strerror(errno));
	
    fclose(fp);
    return ret;
}	




int SYS_set_video_preview_win(int x,int y,int w,int h)
{

	
	LOGI("function flag : SYS_set_video_preview_win %d,%d,%d,%d ",x,y,w,h);

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
     LOGI("function flag : set_sys_str %s,%s ",path,val);
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

     LOGI("function flag : set_sys_int %s,%d ",path,val);
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


int enable_gl_2xscale(const char *val)
{
    int fd;
    int bytes;

	fd = open("/sys/class/graphics/fb0/request2XScale",O_RDWR);
	if( fd>= 0)

    {
       bytes = write(fd, val, strlen(val));
		LOGI("gjaoun: enable_gl_2xscale %s\n",val);
        close(fd);
        return 0;
    } else {

	 LOGI("gjaoun:fb:%d failed to open /sys/class/graphics/fb0/request2XScale\n",fd);
	 return -1;
    }
    
}

int Active_osd_viewport(int web_w, int web_h)
	
{
	LOGI("gjaoun :web %d,%d",web_w,web_h);


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
		 LOGI("gjaoun : %s",buf);
		 
		 read(fd_disp,buf,sizeof(buf));
		 if(!strncmp(buf,"720p",4)){

		  modes_heigh = 720 ;
		  modes_width = 1280 ;

		 }
		 else if(!strncmp(buf,"1080p",5)){		

		 modes_heigh = 1080;
		 modes_width = 1920;

		 }
		 else if(!strncmp(buf,"lvds1080p",9)){	
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
		 modes_width = 1024;  

		 }
		 else if(!strncmp(buf,"480p",4))
		 {
		  modes_heigh = 480;
		  modes_width = 854;

		 }
		 else if(!strncmp(buf,"480i",4))
		 {
			 modes_heigh = 480;
			 modes_width = 854;

		 }
		 close(fd_disp);     
	 }   
	 

	 LOGI("gjaoun : mode %d , %d",modes_width,modes_heigh);

	 int w = (modes_width*modes_width)/web_w ;
     int h = (modes_heigh*modes_heigh)/web_h ;
	
	
	sprintf(bcmd,"%d %d %d",16,w,h);
	LOGI("gjaoun scale: %s",bcmd);
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



/////////////////////////////////////////////////////////////g2ed ,ppscaler  freescale

void LunchIptv()
{

 
	   RmDefPath();
	   AddIptvPath();
	   set_sys_int("/sys/class/video/blackout_policy",0);
	   set_sys_str("/sys/class/deinterlace/di0/config","disable");
	   set_sys_int("/sys/module/di/parameters/buf_mgr_mode",0);
	   set_sys_str("/sys/class/display/rd_reg","m 0x1a2b");
	   set_sys_str("/sys/class/display/wr_reg","m 0x1a2b 0x1dc20c81");
	   set_sys_str("/sys/class/graphics/fb0/video_hole","0 0 1920 1080 0");    
	   Active_video_viewport(0,0,1920,1080);
	   EnableFreeScale(1280,720);
	  // Active_osd_viewport(1280, 720);

   
   
}

/*status : 0 - video stop status   1 - video play status*/
void SwitchResolution(int mode , int status) 
{
	

	
	if(mode == 1){
		RmDefPath();
		RemoveIptvPath();
		EnableFreeScale(640,530);		
		Active_video_viewport(0,0,1920,1080);
		AddIptvPath();		
    }
    else if(mode == 2){
		RmDefPath();
		RemoveIptvPath();
		EnableFreeScale(1280,720);		
		Active_video_viewport(0,0,1920,1080);
		AddIptvPath();	
    }
    
}


void QuitIptv()
{

      RmDefPath();
    RestoreDefPath();
    set_sys_str("/sys/class/graphics/fb0/video_hole","0 0 0 0 0");
	set_sys_int("/sys/class/video/blackout_policy",1);
   // enable_gl_2xscale("2");
	//enable_gl_2xscale("");
     DisableFreeScale(1);
}



//////////////////////////////////////////

int SYS_set_global_alpha(int alpha){


	LOGI("function flag :SYS_set_global_alpha  %d",alpha);
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


	LOGI("function flag :SYS_disable_colorkey  ");

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

   

	memset(&aPara,0,sizeof(aPara));
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
	//SYS_set_global_alpha(100);
	//SYS_disable_colorkey();
	m_nOsdBpp = 16;//SYS_get_osdbpp();
	m_nAudioBalance = 3;

	m_nVolume = 100;
	m_bFast = false;
	m_bSetEPGSize = false;

	m_nMode = M_LIVE;
	VideoHide();
	//if (!IsSoftFit())
		LunchIptv();
	VideoShow();
#ifdef WF
	m_fp = NULL;
#endif
}

CTsPlayer::~CTsPlayer()
{




		QuitIptv();
}

//取得播放模式,保留，暂不用
int  CTsPlayer::GetPlayMode()
{


	return 1;
}
int CTsPlayer::SetVideoWindow(int x,int y,int width,int height)
{

/*

    int fd;
    const char *path = "/sys/class/video/axis" ;
    char  bcmd[32];
	//x /= 1.5;
	//y /= 1.5;
	//width /= 1.5;
	//height /= 1.5;
    fd = open(path, O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (fd >= 0) {
            sprintf(bcmd, "%d %d %d %d", x, y, x+width, y+height);
            write(fd, bcmd, strlen(bcmd));
        close(fd);
        return 0;
    }
	return -1;*/

return 0 ;
}


int CTsPlayer::SetColorKey(int enable,int key565)
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
}

int CTsPlayer::VideoShow(void)
{



	return set_sys_int("/sys/class/video/disable_video",0);
}
int CTsPlayer::VideoHide(void)
{


	if (m_nMode == M_LIVE)
		return 0;
	return 0;
	return set_sys_int("/sys/class/video/disable_video",1);
}


void CTsPlayer::InitVideo(PVIDEO_PARA_T pVideoPara)
{



	vPara=*pVideoPara;
	return ;
}

void CTsPlayer::InitAudio(PAUDIO_PARA_T pAudioPara)
{


	aPara=*pAudioPara;
	return ;
}

bool CTsPlayer::StartPlay()
{





	int ret;


	memset(pcodec,0,sizeof(*pcodec));
	pcodec->stream_type=STREAM_TYPE_TS;
	pcodec->video_type = vPara.vFmt;
	pcodec->has_video=1;
	pcodec->audio_type= aPara.aFmt;
	if (!m_bFast)
		pcodec->has_audio=1;
	pcodec->video_pid=(int)vPara.pid;
	pcodec->audio_pid=(int)aPara.pid;
	//pcodec->noblock = 1;
	 if (pcodec->video_type == VFORMAT_H264) {
        pcodec->am_sysinfo.format = VIDEO_DEC_FORMAT_H264;
        pcodec->am_sysinfo.param = (void *)(0);
    }
	printf("set %d,%d,%d,%d\n",vPara.vFmt,aPara.aFmt,vPara.pid,aPara.pid);
	pcodec->noblock = 0;
	/*other setting*/
	ret = codec_init(pcodec);
	__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "StartPlay codec_init After:%d\n", ret);
	if (ret == 0)
	{
		if (m_nMode == M_LIVE)
			set_sys_int("/sys/class/video/blackout_policy",1);
		m_bIsPlay = true;
#ifdef WF
		m_fp = fopen("/mnt/sda/sda1/Live.ts", "wb+");
		//m_fp = fopen("/data/Live.ts", "wb+");	
#endif
	}
	return !ret;
}
int CTsPlayer::WriteData(unsigned char* pBuffer, unsigned int nSize)
{



	if (!m_bIsPlay)
		return -1;
	if (codec_write(pcodec,pBuffer,nSize) > 0){
#ifdef WF
	if (m_fp != NULL){
		fwrite(pBuffer, 1, nSize, m_fp);
	}
#endif
		
	}
	return nSize;
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
	m_bFast = true;	
	Stop();
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
	ret = set_sys_int("/sys/class/video/blackout_policy",1);
	if (ret)
		return false;

	return !ret;
}
bool CTsPlayer::Stop()
{




	if (m_nMode == M_LIVE)
		set_sys_int("/sys/class/video/blackout_policy",0);
	if (m_bIsPlay){
#ifdef WF
		if (m_fp != NULL){
			fclose(m_fp);
			m_fp = NULL;
		}
#endif
		__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "m_bIsPlay is true");
		//codec_set_cntl_mode(pcodec, TRICKMODE_NONE);
		int ret = codec_close(pcodec);
		//codec_set_cntl_mode(pcodec, TRICKMODE_NONE);
		__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "Stop  codec_close After:%d\n", ret);
	}

	else
	{
		__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "m_bIsPlay is false");
	}
	m_bIsPlay = false;
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



	Stop();
	return StartPlay();
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
	if (nAudioBalance == 1){
		codec_left_mono(pcodec);
	}else if(nAudioBalance == 2){
		codec_right_mono(pcodec);
	}else if(nAudioBalance == 3){
		codec_stereo(pcodec);
	}
	return true;
}

void CTsPlayer::GetVideoPixels(int& width, int& height)
{



	int fd = open("/sys/class/display/mode", O_RDONLY); 
	if (fd >= 0)
	{
		char buffer[12] = {0};
		int nLen = read(fd, buffer, sizeof(buffer));
		close(fd);
		__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "read succeed");
		__android_log_print(ANDROID_LOG_INFO, "TsPlayer", "%d:%d", nLen, buffer[0]);
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
		else{
			width = 1920;
			height = 1080;
		}
	}
}

bool CTsPlayer::SetRatio(int nRatio)
{
	return false;
}
ITsPlayer* GetTsPlayer()
{
	return new CTsPlayer();
}
bool CTsPlayer::IsSoftFit()
{
	return false;
}
void CTsPlayer::SetEPGSize(int w, int h)
{

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

