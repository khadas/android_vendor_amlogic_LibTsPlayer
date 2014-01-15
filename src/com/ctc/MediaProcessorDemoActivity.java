/**
 * @file 		MediaProcessorDemoActivity.java
 * @author    	zhouyj
 * @date      	2012/9/5
 * @version   	ver 1.0
 * @brief     	楠岃瘉CTC_MediaProcessor绫讳腑鏂规硶鐨刯ni鎺ュ彛鑳藉惁浣跨敤銆� * @attention
*/
package com.ctc;

import java.io.FileOutputStream;
import java.text.SimpleDateFormat;
import java.util.Date;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.PowerManager;
import android.widget.Button;
import android.widget.TextView; 
import android.view.KeyEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.Surface;
import android.view.View;
import android.graphics.Canvas;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.Log; 

import com.subtitleparser.SubID;
import com.subtitleparser.SubtitleUtils;
import com.subtitleview.SubtitleView;
import com.subtitleparser.Subtitle;
import android.os.Handler;
import android.os.Message;
import android.view.Gravity;
import android.graphics.Typeface;


public class MediaProcessorDemoActivity extends Activity {
	private String TAG="MediaProcessorDemoActivity";
	public String result_s = "success";
	public int result_i = 0; 
	public int sub_id = 0x109;//0x106, 0x107, 0x108, 0x109 
	public boolean result_b = false;
	Surface mySurface;
	SurfaceHolder myHolder;
	SurfaceView mySurfaceView;
	String url = "/storage/external_storage/sda1/TV_2008911.ts";   
	
	private Button pause;
	private Button resume; 
	private Button seek;
	private Button videoshow; 
	private Button videohide;
	private Button fast;
	private Button stopfast; 
	private Button stop;
	private Button getVolume;
	private Button setVolume;
	private Button setRatio; 
	private Button getAudioBalance;
	private Button setAudioBalance;
	private Button getVideoPixels;
	private Button isSoftFit;
	private Button getPlayMode;
	private Button setEPGSize;
	private Button switchSubtitle;
	
	private TextView Function;
	private TextView Return_t;
	private TextView Result;
	private TextView resultView;
	
	private int flag = 0;
	
	//for subtitle
	private SubtitleUtils subtitleUtils = null;
	private SubtitleView subTitleView = null;
	private subview_set sub_para = null;
	private Thread SubTitleThread=null;
	private int PLAYER_INIT=0;
	private int PLAYER_PALY=1;
	private int PLAYER_STOP=2;
	private int player_status=0;
	private Handler MainHandler=null;
	private final int MSG_INIT_SUBID=0;
	private final int MSG_SUB_TICK=1;	
		

	class drawSurface implements Runnable{
		public String url;
		public void run()
		{
			nativeWriteData(this.url);
		} 
	}
	
	 public boolean onKeyDown(int keyCode, KeyEvent msg) {
	     Log.d(getClass().getName(), "onKeyDown()"); 
		 if(keyCode == KeyEvent.KEYCODE_POWER){
		 	return (true);
		 }
		 return super.onKeyDown(keyCode, msg);
	   }
	
    @Override
    public void onPause(){
        super.onPause(); 
        // Log.i("CTC apk", "onPause"); 
        
        player_status=PLAYER_STOP;
        Log.d(getClass().getName(), "onPause()"); 
        Log.d(getClass().getName(), "before nativeStop, time is: " + System.currentTimeMillis());

        nativeStop();  
        Log.d(getClass().getName(), "before nativeSetEPGSize, time is: " + System.currentTimeMillis());
        
        nativeSetEPGSize(1280, 720); 
        
        Log.d(getClass().getName(), "after nativeSetEPGSize, time is: " + System.currentTimeMillis());

        Date a = new Date();
        
        SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss");
        Log.i("CTC apk", "release start" + sdf.format(a));  
        if(flag != 1){
        	nativeDelete();
        	flag = 0;
        }
        Date b = new Date();
        Log.i("CTC apk", "release end" + sdf.format(b));
        //if(mWakeLock != null && mWakeLock.isHeld())
        {
                // mWakeLock.release();
        }
    }
    @Override
    public void onStop(){
        super.onStop();
         Log.i("CTC apk", "onStop"); 
    	
    }
    
     // private PowerManager.WakeLock mWakeLock = null;
     private IntentFilter mFilter = null;
    
    @Override
    public void onResume()
    {
             
            Log.d(getClass().getName(), "onResume()");
            
            Log.i("create surface:", "next");  
            if (nativeCreateSurface(mySurface, 1280, 720) == 0) 
            	Log.i("create surface:", "success");  
            if (nativeInit() == 0)
            	Log.i("Init:", "success");
            else 
            	Log.i("Init:", "error"); 

    		nativeSetEPGSize(1280, 720);
            nativeSetVideoWindow(420, 40, 640, 480); 
            
            nativeStartPlay();
            Log.i("play", "success"); 
            
            drawSurface playData = new drawSurface();  
            playData.url = url;
            Thread player = new Thread(playData);
            player.start(); 
            
            subinit();

			MainHandler=new Handler(){
			
				@Override
				public void handleMessage(Message msg) {
					// TODO Auto-generated method stub
					Log.d(TAG, "get msg "+msg.what);
					switch(msg.what){
						case MSG_INIT_SUBID: 
							setSubId();
							break;
						case MSG_SUB_TICK:
							Log.d(TAG,"tick time "+msg.arg1);
							subTitleView.tick(msg.arg1);
							break;
						default : break;
					}
					super.handleMessage(msg);
				}
				
			};
            
			SubTitleThread=new Thread(){
				public void run () {
	              //  openFile(sub_para.sub_id);
					while(player_status==PLAYER_PALY){
					try {
						sleep(500);
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
	
					if(subTitleView!=null&&sub_para.sub_id!=null&&nativeGetCurrentPlayTime()>0){
						Message msg=MainHandler.obtainMessage(MSG_SUB_TICK);
						msg.arg1=nativeGetCurrentPlayTime();
						MainHandler.sendMessage(msg);
					}else
					{
						if(sub_para.sub_id==null){
							Message msg=MainHandler.obtainMessage(MSG_INIT_SUBID);
							MainHandler.sendMessage(msg);
						}
					}
					
				 }
					
	            }
			};	
            player_status=PLAYER_PALY;
            SubTitleThread.start();
            
            super.onResume();
    }

/** Called when the activity is first created. */ 
    @Override 
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main); 
        
        mFilter = new IntentFilter();
        mFilter.addAction(Intent.ACTION_SCREEN_OFF);
        mFilter.addAction(Intent.ACTION_SCREEN_ON);
        mFilter.addAction("com.android.smart.terminal.iptv.power");
        mFilter.setPriority(IntentFilter.SYSTEM_HIGH_PRIORITY);

        Log.i("CTC apk", "onCreate"); 
        mySurfaceView = (SurfaceView)this.findViewById(R.id.SurfaceView01);
        myHolder = mySurfaceView.getHolder();
        myHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        mySurface = myHolder.getSurface(); 
        
        
        
        Function = (TextView)findViewById(R.id.Function);
        Return_t = (TextView)findViewById(R.id.Return_t);
        Result = (TextView)findViewById(R.id.Result);
        
       
        
        //pause
        pause = (Button)findViewById(R.id.pause);
        pause.setOnClickListener(new Button.OnClickListener(){
        	public void onClick(View v)
        	{
        		result_b = nativePause();
        		Function.setText("Pause");
        		if (result_b == true)
        		{
        			Return_t.setText("true");
        			Result.setTextColor(Color.BLUE); 
        			Result.setText("success");
        			nativeWriteFile("function:Pause", "return:true", "result:success");  
        		}
        		else
        		{
        			Return_t.setText("false"); 
        			Result.setTextColor(Color.RED); 
        			Result.setText("error");
        			nativeWriteFile("function:Pause", "return:false", "result:error");
        		}
        		
        	}
        });
        
        //resume
        resume = (Button)findViewById(R.id.resume);
        resume.setOnClickListener(new Button.OnClickListener(){
        	public void onClick(View v)
        	{ 
        		result_b = nativeResume();
        		Function.setText("Resume");
        		if (result_b == true)
        		{
        			Return_t.setText("true");
        			Result.setTextColor(Color.BLUE);
        			Result.setText("success");
        			nativeWriteFile("function:Resume", "return:true", "result:success");  
        		}
        		else
        		{
        			Return_t.setText("false");   
        			Result.setTextColor(Color.RED);
        			Result.setText("error");
        			nativeWriteFile("function:Resume", "return:false", "result:error"); 
        		}
        	}
        });
        
        //seek
        seek = (Button)findViewById(R.id.seek);
        seek.setOnClickListener(new Button.OnClickListener(){ 
        	public void onClick(View v)
        	{
        		result_b = nativeSeek();
        		Function.setText("Seek");  
        		if (result_b == true)
        		{
        			Return_t.setText("true");
        			Result.setTextColor(Color.BLUE);
        			Result.setText("success");
        			nativeWriteFile("function:Seek", "return:true", "result:success"); 
        		}
        		else
        		{
        			Return_t.setText("false");
        			Result.setTextColor(Color.RED);
        			Result.setText("error");
        			nativeWriteFile("function:Seek", "return:false", "result:error"); 
        		}
        	}
        }); 
       
        //fast
        fast = (Button)findViewById(R.id.fast);
        fast.setOnClickListener(new Button.OnClickListener(){
        	public void onClick(View v)
        	{
        		result_b = nativeFast(); 
        		Function.setText("Fast");
        		if (result_b == true)
        		{
        			Return_t.setText("true");
        			Result.setTextColor(Color.BLUE);
        			Result.setText("success");
        			nativeWriteFile("function:Fast", "return:true", "result:success"); 
        		}
        		else
        		{
        			Return_t.setText("false");
        			Result.setTextColor(Color.RED);
        			Result.setText("error");
        			nativeWriteFile("function:Fast", "return:false", "result:error"); 
        		}
        	}
        });  
        
        //stopfast
        stopfast = (Button)findViewById(R.id.stopfast);
        stopfast.setOnClickListener(new Button.OnClickListener(){
        	public void onClick(View v)
        	{
        		result_b = nativeStopFast();
        		Function.setText("StopFast");
        		if (result_b == true)
        		{
        			result_s = String.valueOf(result_b);
        			Return_t.setText(result_s);
        			Result.setTextColor(Color.BLUE);
        			Result.setText("success");
        			nativeWriteFile("function:StopFast", "return:true", "result:success"); 
        		}
        		else
        		{
        			Return_t.setText("false");
        			Result.setTextColor(Color.RED);
        			Result.setText("error");
        			nativeWriteFile("function:StopFast", "return:false", "result:error"); 
        		}
        	}
        }); 
        
        //videoshow
        videoshow = (Button)findViewById(R.id.videoshow);
        videoshow.setOnClickListener(new Button.OnClickListener(){
        	public void onClick(View v)
        	{
        		result_i = nativeVideoShow();
        		Function.setText("VideoShow");
        		if (result_i == 0)
        		{
        			result_s = String.valueOf(result_i);
            		Return_t.setText(result_s);
            		Result.setTextColor(Color.BLUE);
        			Result.setText("success");
        			nativeWriteFile("function:VideoShow", "return:"+result_s, "result:success");
        		}
        		else
        		{
        			result_s = String.valueOf(result_i);
            		Return_t.setText(result_s);
            		Result.setTextColor(Color.RED);
        			Result.setText("error");
        			nativeWriteFile("function:VideoShow", "return:"+result_s, "result:error");
        		}
        	} 
        }); 
         
        //videohide 
        videohide = (Button)findViewById(R.id.videohide);
        videohide.setOnClickListener(new Button.OnClickListener(){
        	public void onClick(View v)
        	{
        		result_i = nativeVideoHide();
        		Function.setText("VideoHide");
        		if (result_i == 0)
        		{
        			result_s = String.valueOf(result_i);
            		Return_t.setText(result_s);
            		Result.setTextColor(Color.BLUE);
        			Result.setText("success");
        			nativeWriteFile("function:VideoHide", "return:"+result_s, "result:success");
        		}
        		else
        		{
        			result_s = String.valueOf(result_i);
            		Return_t.setText(result_s);
            		Result.setTextColor(Color.RED);
        			Result.setText("error");
        			nativeWriteFile("function:VideoHide", "return:"+result_s, "result:error");
        		}
        	}
        }); 
        
        //stop
        stop = (Button)findViewById(R.id.stop);
        stop.setOnClickListener(new Button.OnClickListener(){
        	public void onClick(View v)
        	{
        		result_b = nativeStop();
        		Function.setText("Stop");
        		if (result_b == true)
        		{
        			result_s = String.valueOf(result_b);
        			Return_t.setText(result_s);
        			Result.setTextColor(Color.BLUE);
        			Result.setText("success");
        			nativeWriteFile("function:Stop", "return:true", "result:success");
        		}
        		else
        		{
        			Return_t.setText("false");
        			Result.setTextColor(Color.RED);
        			Result.setText("error");
        			nativeWriteFile("function:Stop", "return:false", "result:error");
        		}
        		
        	}
        }); 
        
        //getVolume
        getVolume = (Button)findViewById(R.id.getVolume);
        getVolume.setOnClickListener(new Button.OnClickListener(){
        	public void onClick(View v)
        	{
        		result_i = nativeGetVolume();
        		Function.setText("GetVolume");
        		result_s = String.valueOf(result_i);
        		Return_t.setText(result_s);
        		Result.setTextColor(Color.BLUE);
    			Result.setText("success");
    			nativeWriteFile("function:GetVolume", "return:"+result_s, "result:success");
        	}
        }); 
        
        //setVolume
        setVolume = (Button)findViewById(R.id.setVolume);
        setVolume.setOnClickListener(new Button.OnClickListener(){ 
        	public void onClick(View v)
        	{
        		result_b = nativeSetVolume(60);
        		Function.setText("SetVolume");
        		if (result_b == true)
        		{
        			result_s = String.valueOf(result_b);
        			Return_t.setText(result_s);
        			Result.setTextColor(Color.BLUE);
        			Result.setText("success");
        			nativeWriteFile("function:SetVolume", "return:true", "result:success");
        		}
        		else
        		{
        			Return_t.setText("false");
        			Result.setTextColor(Color.RED);
        			Result.setText("error");
        			nativeWriteFile("function:SetVolume", "return:false", "result:error");
        		}
        	}
        }); 
        
        //setRatio
        setRatio = (Button)findViewById(R.id.setRatio);
        setRatio.setOnClickListener(new Button.OnClickListener(){ 
        	public void onClick(View v)
        	{
        		result_b = nativeSetRatio(1);
        		Function.setText("SetRatio");
        		if (result_b == true)
        		{
        			result_s = String.valueOf(result_b);
        			Return_t.setText(result_s);
        			Result.setTextColor(Color.BLUE);
        			Result.setText("success");
        			nativeWriteFile("function:SetRatio", "return:true", "result:success");
        		}
        		else
        		{
        			Return_t.setText("false");
        			Result.setTextColor(Color.RED);
        			Result.setText("error");
        			nativeWriteFile("function:SetRatio", "return:false", "result:error");
        		}
        	}
        }); 
        
        //getAudioBalance
        getAudioBalance = (Button)findViewById(R.id.getAudioBalance);
        getAudioBalance.setOnClickListener(new Button.OnClickListener(){ 
        	public void onClick(View v)
        	{
        		result_i = nativeGetAudioBalance();
        		Function.setText("GetAudioBalance");
        		result_s = String.valueOf(result_i);
        		Return_t.setText(result_s);
        		Result.setTextColor(Color.BLUE);
    			Result.setText("success");
    			nativeWriteFile("function:GetAudioBalance", "return:"+result_s, "result:success");
        	}
        }); 
        
        //setAudioBalance
        setAudioBalance = (Button)findViewById(R.id.setAudioBalance);
        setAudioBalance.setOnClickListener(new Button.OnClickListener(){ 
        	public void onClick(View v)
        	{
        		result_i = nativeGetAudioBalance();
        		result_b = nativeSetAudioBalance(result_i>1?(result_i-1):3);
        		Function.setText("SetAudioBalance");
        		if (result_b == true)
        		{
        			result_s = String.valueOf(result_b);
        			Return_t.setText(result_s);
        			Result.setTextColor(Color.BLUE);
        			Result.setText("success");
        			nativeWriteFile("function:SetAudioBalance", "return:"+result_s, "result:success");
        		}
        		else
        		{ 
        			Return_t.setText("false");
        			Result.setTextColor(Color.RED);
        			Result.setText("error");
        			nativeWriteFile("function:GetAudioBalance", "return:false", "result:error");
        		}
        	}
        }); 
        
        //getVideoPixels
        getVideoPixels = (Button)findViewById(R.id.getVideoPixels);
        getVideoPixels.setOnClickListener(new Button.OnClickListener(){ 
        	public void onClick(View v) 
        	{
        		nativeGetVideoPixels();
        		Function.setText("GetVideoPixels");
        		Return_t.setText("void");
        		Result.setTextColor(Color.BLUE);
    			Result.setText("success");
    			nativeWriteFile("function:GetVideoPixels", "return:void", "result:success");
        	}
        }); 
        
        //isSoftFit
        isSoftFit = (Button)findViewById(R.id.isSoftFit);
        isSoftFit.setOnClickListener(new Button.OnClickListener(){  
        	public void onClick(View v)
        	{
        		result_b = nativeIsSoftFit();
        		Function.setText("IsSoftFit");
        		if (result_b == true)
        		{
        			result_s = String.valueOf(result_b);
        			Return_t.setText(result_s);
        			Result.setTextColor(Color.BLUE);
        			Result.setText("soft fit");
        			nativeWriteFile("function:IsSoftFit", "return:true", "result:soft fit");
        		}
        		else
        		{
        			Return_t.setText("false");
        			Result.setTextColor(Color.RED);
        			Result.setText("hardware fit");
        			nativeWriteFile("function:IsSoftFit", "return:false", "result:hardware fit");
        		}
        		
        	}
        }); 
        
        //getPlayMode
        getPlayMode = (Button)findViewById(R.id.getPlayMode);
        getPlayMode.setOnClickListener(new Button.OnClickListener(){ 
        	public void onClick(View v)
        	{
        		result_i = nativeGetPlayMode();
        		Function.setText("GetPlayMode");
        		result_s = String.valueOf(result_i);
        		Return_t.setText(result_s);
        		Result.setTextColor(Color.BLUE);
    			Result.setText("success");
    			nativeWriteFile("function:GetPlayMode", "return:"+result_s, "result:success");
        	}
        }); 
        
        //setEPGSize
        setEPGSize = (Button)findViewById(R.id.setEPGSize);
        setEPGSize.setOnClickListener(new Button.OnClickListener(){ 
        	public void onClick(View v)
        	{
        		nativeSetEPGSize(640, 530);
        		Function.setText("SetEPGSize");
        		Return_t.setText("void");
        		Result.setTextColor(Color.BLUE);
        		Result.setText("success");
        		nativeWriteFile("function:SetEPGSize", "return:void", "result:success"); 
        		
        	}
        });    
        
        //SwitchSubtitle
        switchSubtitle = (Button)findViewById(R.id.switchSubtitle);
        switchSubtitle.setOnClickListener(new Button.OnClickListener(){ 
        	public void onClick(View v)
        	{
        		nativeSwitchSubtitle(sub_id>0x106?(--sub_id):0x109);
        	}
        });   
        
        
    }
    
    
		protected void subinit() {
		    subtitleUtils = new SubtitleUtils("/tmp/test.rmvb");
		    sub_para = new subview_set();
		     
		    sub_para.totalnum = 0;
		    sub_para.curid = 0;
		
		    sub_para.enable =true;  
		    sub_para.color =  android.graphics.Color.RED ;//android.graphics.Color.WHITE;
			sub_para.font= 20;//20;
			sub_para.position_v=0;//0;   	
		    sub_para.sub_id = null;
		}
	
		private void setSubtitleView()
		{
			subTitleView = (SubtitleView) findViewById(R.id.subTitle);
			subTitleView.clear();
			subTitleView.setGravity(Gravity.CENTER);
			subTitleView.setTextColor(sub_para.color);
			subTitleView.setTextSize(sub_para.font);
			subTitleView.setTextStyle(Typeface.BOLD);
			subTitleView.setPadding(
			subTitleView.getPaddingLeft(),
			subTitleView.getPaddingTop(),
			subTitleView.getPaddingRight(),
			//100); //TODO getWindowManager().getDefaultDisplay().getRawHeight()*sub_para.position_v/20+10);
			getWindowManager().getDefaultDisplay().getHeight()*sub_para.position_v/20+10);
		//		FrameLayout.LayoutParams frameParams = (FrameLayout.LayoutParams) subTitleView.getLayoutParams();
		//		frameParams.width = 1280;
		//		frameParams.height=80;	        	
		//   	subTitleView.setLayoutParams(frameParams);
		}
	
		private void setSubId(){
			 sub_para.totalnum =subtitleUtils.getInSubTotal();
			 Log.d(TAG, "setSubId ==totalnum "+sub_para.totalnum);
		     if(sub_para.totalnum >0){
		
		         sub_para.curid = subtitleUtils.getCurrentInSubtitleIndexByJni();
		         Log.d(TAG, "setSubId ==curid "+sub_para.curid+"sub_para.enable "+sub_para.enable);
		
		         if(sub_para.curid == 0xff||sub_para.enable==false)
		             sub_para.curid = sub_para.totalnum;
		         if(sub_para.totalnum>0)
		             sub_para.sub_id =subtitleUtils.getSubID(sub_para.curid);
		         else
		             sub_para.sub_id = null;
		         if(sub_para.sub_id==null){
		        	 Log.d(TAG, "fatal warning  get sud id null");
		         }
		         setSubtitleView();
		         new Thread () {
		             public void run () {
		                 openFile(sub_para.sub_id);
		             }
		         }.start();
		     }else{
		         sub_para.sub_id = null;
		     }
		}
		private void openFile(SubID filepath) {
			
			if(filepath==null)
				return;
			
			try {
				if(subTitleView.setFile(filepath,"GBK")==Subtitle.SUBTYPE.SUB_INVALID)
					return;			
			} 
			catch(Exception e) {
				Log.d(TAG, "open:error");
				subTitleView = null;
				e.printStackTrace();
			}
		
		}    		
    
    static {
    	System.loadLibrary("CTC_MediaProcessorjni");   
    }
    private TextView v;
    
    private static native int nativeCreateSurface(Surface mySurface, int width, int heigth);
    private static native int nativeInit();
    private static native int nativeWriteData(String url);
    private static native int nativeSetVideoWindow(int x, int y, int width, int height);
    private static native boolean nativeStartPlay();
    private static native int nativeGetPlayMode();
    private static native boolean nativePause();
    private static native boolean nativeResume();  
    private static native boolean nativeSeek();
    private static native int nativeVideoShow();
    private static native int nativeVideoHide();
    private static native boolean nativeFast();
    private static native boolean nativeStopFast(); 
    private static native boolean nativeStop();
    private static native int nativeGetVolume();
    private static native boolean nativeSetVolume(int volume);
    private static native boolean nativeSetRatio(int nRatio);
    private static native int nativeGetAudioBalance();
    private static native boolean nativeSetAudioBalance(int nAudioBalance);
    private static native void nativeGetVideoPixels();
    private static native boolean nativeIsSoftFit();
    private static native boolean nativeDelete();
    private static native void nativeSetEPGSize(int w, int h);
    private static native void nativeWriteFile(String functionName, String returnValue, String resultValue);
    
    private static native int nativeGetCurrentPlayTime();
    private static native void nativeInitSubtitle();
    private static native void nativeSwitchSubtitle(int sub_pid);
}

class subview_set{
	public int totalnum; 
	public int curid;
	public int color;
	public int font; 
	public SubID sub_id;
    public SubID sub_id_bac;
	public boolean enable;
	public int position_v;
}