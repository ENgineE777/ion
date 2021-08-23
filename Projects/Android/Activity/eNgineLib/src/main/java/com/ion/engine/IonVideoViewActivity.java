
package com.ion.engine;

import android.R.integer;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;
import android.media.MediaPlayer.OnInfoListener;
import android.media.MediaPlayer.OnPreparedListener;
import android.media.MediaPlayer.OnSeekCompleteListener;
import android.net.Uri;
import android.opengl.Visibility;
import android.os.Bundle;
import android.text.Html;
import android.text.TextWatcher;
import android.text.format.Formatter;
import android.util.Log;
import android.webkit.CookieManager;
import android.webkit.CookieSyncManager;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.MediaController;
import android.widget.ProgressBar;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;
import android.widget.VideoView;
import android.view.Display;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.view.Window;
import android.view.WindowManager;
import android.view.animation.RotateAnimation;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.LinearInterpolator;

import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.json.JSONArray;
import org.json.JSONObject;

import android.content.ClipboardManager;
import android.content.Context;
import android.content.res.Configuration;
import android.widget.ZoomButtonsController;


class Pair
{
	 public String qual = "";
	 public String url = "";
	 Pair(String in_qual, String in_url)
	 {
		 qual = in_qual;
		 url = in_url;
	 }
}





public class IonVideoViewActivity extends Activity
implements 
OnClickListener,
SeekBar.OnSeekBarChangeListener,
MediaPlayer.OnPreparedListener,
MediaPlayer.OnCompletionListener,
MediaPlayer.OnBufferingUpdateListener, OnSeekCompleteListener, OnInfoListener
{
	 private static final String TAG = "IonVidewView";
	public static String url ="rtsp://v3.cache7.c.youtube.com/CiILENy73wIaGQlOCTh0GvUeYRMYDSANFEgGUgZ2aWRlb3MM/0/0/0/video.3gp";
	public static String showName ="";
	public static List<Pair> quallity_list = new ArrayList<Pair>();
	public static int curQuallity = 0;
	
	
	public static int lastProgress = 0;
	public String rtsp = "";
     //private VideoView videoView = null;
     private ProgressBar prog = null;
     private Context ctx = null;
     private MediaController mediaController = null;
     private SeekBar seekBar = null;
     private View closeBtn = null;
     private TextView videoName = null;
     private View playPauseBtn = null;
     private TextView currentProgress = null;
     private TextView maxDuration = null;
     private MediaPlayer mediaPlayer = null;
     private SurfaceView surfaceView = null;
     private SurfaceHolder surfaceHolder = null;
     private ActivityWorkTimerTask workTask  = null;
     private GetRtspTimerTask rtspTask = null;
     private Timer timer = null;
     private boolean prepared = false;
     private int setProgress = 0;
     private boolean  needSeek = false;
     private int bufprogress = -1;
     private int maxprogress = 0;
     private boolean infoReady = false;
     private long timeLastClick = 0;
     private long prevTime = 0;
     private boolean was_MEDIA_INFO_BUFFERING_START = false;
     private boolean was_MEDIA_INFO_BUFFERING_END = false;
     private boolean show = true;
     
     


     class GetRtspTimerTask extends TimerTask
  	{
  		  public void run()
  		  {
  	    	 if (rtsp == "")
  	    	 {
  	    		 rtsp = getRstpLinks("r6AcYr9b3Vk");

  	    	 }
  		  }
  	} 
     
     class ActivityWorkTimerTask extends TimerTask
 	{
 		  public void run()
 		  {
 	         runOnUiThread(new Runnable() {

 	             @Override
 	             public void run() {
 	            	ActivityWork();
 	                 //title.clearComposingText();//not useful

 	             }
 	         });
 		  }
 	} 


        
    synchronized public void ActivityWork()
 	{				

    	 if (prepared == false)
    		 return;

    	 if ( mediaPlayer == null )
    		 return;
    	 
    	 if ( !infoReady )
    	 {
    		timeLastClick = 0;
			int videoWidth = mediaPlayer.getVideoWidth();
            int videoHeight = mediaPlayer.getVideoHeight();
            
            if (videoWidth > 0 && videoHeight > 0)
            {  
            	Log.v("VideoView", "videoWidth="+videoWidth + " videoHeight="+videoHeight);
         
            	int screenWidth = getWindowManager().getDefaultDisplay().getWidth();                   
            	android.view.ViewGroup.LayoutParams lp = surfaceView.getLayoutParams();                       
            	lp.width = screenWidth;                      
            	lp.height = (int) (((float)videoHeight / (float)videoWidth) * (float)screenWidth);

	            //Commit the layout parameters
	            surfaceView.setLayoutParams(lp);
            
	            maxprogress = mediaPlayer.getDuration();
	            seekBar.setMax(maxprogress);
     
	            
	            infoReady = true;
            }
    	 }
    	 
    	 if ( !infoReady )
    		 return;
    	

      	if (needSeek)
      	{
      		needSeek = false;
    		Log.v("VideoView", "seekTo " + lastProgress);
   		 	mediaPlayer.seekTo(lastProgress);
      	}

      	int curprogress = mediaPlayer.getCurrentPosition();
      	 
    	 lastProgress = curprogress;
    	 seekBar.setProgress(curprogress);
    	 currentProgress.setText(stringForTime(curprogress));
    	 maxDuration.setText(stringForTime(maxprogress));
    	 

     	
    	 long curTime = System.currentTimeMillis();
    	 long dt = curTime - prevTime;
    	 prevTime = curTime;
    	 timeLastClick += dt;
    	 if (timeLastClick > 4000 && show == true && mediaPlayer.isPlaying())
    	 {
    		 
    		 WindowManager mWindowManager =  (WindowManager) getSystemService(WINDOW_SERVICE);
    		    Display mDisplay = mWindowManager.getDefaultDisplay();

    		 //if ( mDisplay.getOrientation()  == 0)
    			  ShowControls(false);
    	 }
    		 
    	// seekBar.setSecondaryProgress(seekBar.getMax()/2);
    	//Log.v("VideoView", "setSecondaryProgress " + (seekBar.getMax()/2));
    	 
    	 
    	 
    	 if (bufprogress  < 0)
    	 {
    		 //prog.setVisibility(View.INVISIBLE);
    		 
    		 if (was_MEDIA_INFO_BUFFERING_START)
    		 {
    			 prog.setVisibility(View.VISIBLE);
    			 was_MEDIA_INFO_BUFFERING_START = false;
    		 }
    		 else if (was_MEDIA_INFO_BUFFERING_END)
    		 {
    			 prog.setVisibility(View.INVISIBLE);
    			 was_MEDIA_INFO_BUFFERING_END = false;
    		 }
    	 }
    	 else
    	 {
    		 int buffer =   (int)( ((float)bufprogress)/100.0 * (float)maxprogress );
    		 
    		 //Log.v("VideoView", "buffer=" + buffer + " curprogress="+curprogress);
    		 
    		 if (buffer < curprogress)
    			 buffer = curprogress;
    		 if (buffer > maxprogress)
    			 buffer = maxprogress;
    	 
    		 //not work
    		 //seekBar.setSecondaryProgress(maxprogress/2);   // for buffer progress
    		

    		 
    		 int dif = buffer - curprogress;
    		 if ( dif < 1000 )
    			 prog.setVisibility(View.VISIBLE);
    		 else
    			 prog.setVisibility(View.INVISIBLE);
    	 }	
    		 
    		 
    		 
 	}
     
    
    @Override
    public void onPause()
    {
    	super.onPause();
    	Log.v("VideoView", "onPause");
    	 
    	try
    	{
    		//prepared = false;
    		//mediaPlayer.stop();
    	}
    	catch(Exception e)
    	{
    		Log.v("VideoView", ""+e.toString());
    	}
    }
    
    @Override
    public void onRestart() 
    {
    	 Log.v("VideoView", "onRestart");
    	 super.onRestart();
    
    }
    
    @Override
    public void onResume()
    {
    	super.onResume();
    	
    	 Log.v("VideoView", "onResume");
    	 
    }
    
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
    	Log.v("VideoView", "onCreate");
               super.onCreate(savedInstanceState);
               requestWindowFeature(Window.FEATURE_NO_TITLE);
               getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
               WindowManager.LayoutParams.FLAG_FULLSCREEN);
               getWindow().setFormat(PixelFormat.TRANSLUCENT);
               setContentView(R.layout.videoview);
               
               
               
               ctx = this;
               prog = (ProgressBar) findViewById(R.id.prog);
               prog.setVisibility(View.INVISIBLE);
              // videoView = (VideoView) findViewById(R.id.video);
               
               currentProgress = (TextView) findViewById(R.id.currentProgress);
               maxDuration = (TextView) findViewById(R.id.maxDuration);;
               
               
               

               mediaPlayer = new MediaPlayer();
               //mediaController = new MediaController(this);
               //mediaController.setAnchorView(videoView);
               //videoView.setMediaController(mediaController);
               mediaPlayer.setOnCompletionListener(this);
               mediaPlayer.setOnPreparedListener(this);
               mediaPlayer.setOnSeekCompleteListener(this);
               mediaPlayer.setOnBufferingUpdateListener(this);
               mediaPlayer.setOnInfoListener(this);

               
              
               


               mediaPlayer.setOnErrorListener(new OnErrorListener()
               {
                     @Override
                     public boolean onError(MediaPlayer mp, int what, int extra)
                     {
                    	 // TODO Auto-generated method stub
                    	 Log.e("VideoView", "error what="+what + " extra="+extra);
                    	 if (what == MediaPlayer.MEDIA_ERROR_UNKNOWN)
                    	 {
                    		 Toast.makeText(ctx, "Error occured, continue try playing....", 1000).show();
                    		 return true;
                    	 }
                    	 
                    	 Toast.makeText(ctx, "Error occured, exiting video player....", 1500).show();
                    	 return false;
                     }
               });


               
               View button = null;
                
               button = findViewById(R.id.playPauseButton);
               playPauseBtn = button;
               button.setOnClickListener(this);
               ToggleButton tb = (ToggleButton)button;
               tb.setChecked(true);
               button.setBackgroundResource(R.drawable.my_pause);
               button = findViewById(R.id.CloseBtn);
               closeBtn = button;
               button.setOnClickListener(this);
               videoName = (TextView)findViewById(R.id.videoName);
               
               FrameLayout fl = (FrameLayout)findViewById(R.id.frameLayout);
               fl.setOnClickListener(this);
               
               seekBar = (SeekBar) findViewById(R.id.seekBar);
               seekBar.setOnSeekBarChangeListener(this);
               seekBar.setIndeterminate(false);
               

              workTask = new ActivityWorkTimerTask();
              timer = new Timer();
             
              boolean useRtsp = false;
              
              if ( useRtsp )
              {
            	  rtspTask = new GetRtspTimerTask();
            	  timer.schedule(rtspTask, 1000, 500);
            	  
            	  while(rtsp == "")
                  {
                 
                  }
                 
            	  if (rtsp == null)
                  {
                	  Toast.makeText(ctx, "Error occured", 500).show();
                	  finish();
                	  return;
                  }
                  
            	  Log.w("ION", "rtsp = "+rtsp);
              }
              
              //rtsp = "rtsp://v4.cache3.c.youtube.com/CjYLENy73wIaLQlW_ji2apr6AxMYDSANFEIJbXYtZ29vZ2xlSARSBXdhdGNoYOr_86Xm06e5UAw=/0/0/0/video.3gp";
              //Uri video = Uri.parse(rtsp);
              //url =  "/mnt/sdcard/DCIM/100MEDIA/VIDEO0001.mp4";
             // url = rtsp;
              //video = Uri.parse(url);
              
          	Button Quallity0Btn = (Button) findViewById(R.id.Quallity0Btn);
        	Button Quallity1Btn = (Button) findViewById(R.id.Quallity1Btn);
        	Button Quallity2Btn = (Button) findViewById(R.id.Quallity2Btn);
        	Button Quallity3Btn = (Button) findViewById(R.id.Quallity3Btn);
        	Button Quallity4Btn = (Button) findViewById(R.id.Quallity4Btn);           
             
        	if (quallity_list.size() >= 1)
        	{
        		Button QuallityBtn = Quallity0Btn;
        		QuallityBtn.setBackgroundResource(R.drawable.quallity_back);
        		QuallityBtn.setOnClickListener(this);
        		QuallityBtn.setText(quallity_list.get(0).qual);
        	}
        	else
        	{
        		Button QuallityBtn = Quallity0Btn;
        		QuallityBtn.setVisibility(View.INVISIBLE);
        	}
        	
        	if (quallity_list.size() >= 2)
        	{
        		Button QuallityBtn = Quallity1Btn;
        		QuallityBtn.setBackgroundResource(R.drawable.quallity_back);
        		QuallityBtn.setOnClickListener(this);
        		QuallityBtn.setText(quallity_list.get(1).qual);
        	}
        	else
        	{
        		Button QuallityBtn = Quallity1Btn;
        		QuallityBtn.setVisibility(View.INVISIBLE);
        	}
        	
        	if (quallity_list.size() >= 3)
        	{
        		Button QuallityBtn = Quallity2Btn;
        		QuallityBtn.setBackgroundResource(R.drawable.quallity_back);
        		QuallityBtn.setOnClickListener(this);
        		QuallityBtn.setText(quallity_list.get(2).qual);
        	}
        	else
        	{
        		Button QuallityBtn = Quallity2Btn;
        		QuallityBtn.setVisibility(View.INVISIBLE);
        	}

        	
        	if (quallity_list.size() >= 4)
        	{
        		Button QuallityBtn = Quallity3Btn;
        		QuallityBtn.setBackgroundResource(R.drawable.quallity_back);
        		QuallityBtn.setOnClickListener(this);
        		QuallityBtn.setText(quallity_list.get(3).qual);
        	}
        	else
        	{
        		Button QuallityBtn = Quallity3Btn;
        		QuallityBtn.setVisibility(View.INVISIBLE);
        	}
        	
        	if (quallity_list.size() >= 5)
        	{
        		Button QuallityBtn = Quallity4Btn;
        		QuallityBtn.setBackgroundResource(R.drawable.quallity_back);
        		QuallityBtn.setOnClickListener(this);
        		QuallityBtn.setText(quallity_list.get(4).qual);
        	}
        	else
        	{
        		Button QuallityBtn = Quallity4Btn;
        		QuallityBtn.setVisibility(View.INVISIBLE);
        	}
        	
        	UpdateQuallityButtonsTextures();
        	
              int index = url.lastIndexOf("/");
              if (index < 0)
            	  index = url.lastIndexOf("\\");

              String name = url;
              if (index >= 0 && index < url.length()-1)
            	  name = url.substring(index+1);
              String text = "";
               
              if ( showName == "" )
            	  text = name;
              else
            	  text = showName;
              
              Log.v("VideoView", "showName="+showName);
              
              String content = "";
              try {
            	  	content = new String(text.getBytes(), "UTF-8");
              	}
              catch (UnsupportedEncodingException e1)
              	{
            	  // TODO Auto-generated catch block
            	  e1.printStackTrace();
              	}
              
              
              videoName.setText( Html.fromHtml(content).toString() );
              videoName.setText( showName );
              //videoName.setText( "привет" );
              Log.v("VideoView", "content="+Html.fromHtml(content).toString());
              timer.schedule(workTask, 1000, 500);
              
              surfaceView = (SurfaceView)findViewById(R.id.surfaceview);
              surfaceView.setOnClickListener(this);
              
              surfaceHolder = surfaceView.getHolder();
              surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
             
              surfaceHolder.addCallback(new SurfaceHolder.Callback() 
              {
                  @Override
                  public void surfaceCreated(SurfaceHolder holder) 
                  {
               	   Log.v("VideoView", "surfaceCreated");
               	   surfaceHolder=holder;
                      mediaPlayer.setDisplay(surfaceHolder);
                      surfaceHolder.setKeepScreenOn(true);
                      mediaPlayer.setScreenOnWhilePlaying(true);
                      //Get the dimensions of the video
                      infoReady = false;
                      PrepareMediaPlayer();
                      

                  }
                  @Override
                  public void surfaceDestroyed(SurfaceHolder holder) 
                  { 
               	   
                	  Log.v("VideoView", "surfaceDestroyed");
              		prepared = false;
              		if (mediaPlayer != null)
              		{
              			mediaPlayer.stop();
              			mediaPlayer.setDisplay(null);
              			mediaPlayer.reset();
              		}
                  }
                  @Override
                  public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
                  {
               	   
                  }
              });
           
               
    }

    void PrepareMediaPlayer()
    {
    	prog.setVisibility(View.VISIBLE);
        try
        {
        mediaPlayer.setDataSource(url);
        }
        catch(IOException e)
        {
      	  Log.e("VideoView", "set data source failed");
        }
        
        try
        {
      	  mediaPlayer.prepareAsync();
        }
        catch(Exception  e)
        {
     	   Log.e("VideoView", "error set surface"+ e.toString());
        }
    }
    void ShowControls(boolean show)
    {
    	this.show = show;
    	int value = 0;
    	if (show)
    		value = View.VISIBLE;
    	else
    		value = View.INVISIBLE;
    	
    	videoName.setVisibility(value);
    	closeBtn.setVisibility(value);
    	playPauseBtn.setVisibility(value);
    	currentProgress.setVisibility(value);
    	maxDuration.setVisibility(value);
    	seekBar.setVisibility(value);
    	
    	ImageView  upperPanelBack = (ImageView) findViewById(R.id.upperPanelBack);
       	
    	upperPanelBack.setVisibility(value);
    	
    	LinearLayout  linearLayoutQuallity = (LinearLayout) findViewById(R.id.linearLayoutQuallity);
    	linearLayoutQuallity.setVisibility(value);
    	
    	ImageView  bottomPanelBackWithQuallity = (ImageView) findViewById(R.id.bottomPanelBackWithQuallity);
    	ImageView  bottomPanelBackWithoutQuallity = (ImageView) findViewById(R.id.bottomPanelBackWithoutQuallity);
    	
    	if (quallity_list.size() > 0 )
    	{
    		bottomPanelBackWithQuallity.setVisibility(value);
    		bottomPanelBackWithoutQuallity.setVisibility(View.INVISIBLE);
    	}
    	else
    	{
    		bottomPanelBackWithQuallity.setVisibility(View.INVISIBLE);
    		bottomPanelBackWithoutQuallity.setVisibility(value);
    	}
    	
    }
    
   
    

    
    @Override
    public void onCompletion(MediaPlayer mp) 
    {
    	Log.v("VideoView", "onCompletion");
    	finish();
    }
             
    @Override
    public void onClick(View v) 
    {
    	Log.v("VideoView", "onClick "+ v.toString());

    	if (v.getId() == R.id.CloseBtn)
    	{    		
    		finish();
    	}
    	
    	if (v.getId() == R.id.playPauseButton)
    	{    		
    		ToggleButton playPauseButton = (ToggleButton) findViewById(R.id.playPauseButton);
        	if (!mediaPlayer.isPlaying()) 
        	{ // Checked - Pause icon visible
        		mediaPlayer.start();
                playPauseButton.setBackgroundResource(R.drawable.my_pause);
            } else { // Unchecked - Play icon visible
            	mediaPlayer.pause();
                playPauseButton.setBackgroundResource(R.drawable.my_play);
            }
    	}
    	
    	Button Quallity0Btn = (Button) findViewById(R.id.Quallity0Btn);
    	Button Quallity1Btn = (Button) findViewById(R.id.Quallity1Btn);
    	Button Quallity2Btn = (Button) findViewById(R.id.Quallity2Btn);
    	Button Quallity3Btn = (Button) findViewById(R.id.Quallity3Btn);
    	Button Quallity4Btn = (Button) findViewById(R.id.Quallity4Btn);
  	
   	
    	if (v.getId() == R.id.Quallity0Btn)
    	{    		
    		Button QuallityBtn = Quallity0Btn;
    		if (curQuallity != 0)
    		{
    			url = quallity_list.get(0).url;
    			mediaPlayer.reset();
    			PrepareMediaPlayer();
    		}
    		curQuallity = 0;
             
    	}
    	
    	if (v.getId() == R.id.Quallity1Btn)
    	{   Button QuallityBtn = Quallity1Btn;
			if (curQuallity != 1)
			{
				url = quallity_list.get(1).url;
    			mediaPlayer.reset();
    			PrepareMediaPlayer();
			}
    		curQuallity = 1;
    	}
    	
    	if (v.getId() == R.id.Quallity2Btn)
    	{    		
    		Button QuallityBtn = Quallity2Btn;
    		if (curQuallity != 2)
    		{
    			url = quallity_list.get(2).url;
    			mediaPlayer.reset();
    			PrepareMediaPlayer();
    		}
    		curQuallity = 2;
    	}
    	
    	if (v.getId() == R.id.Quallity3Btn)
    	{    		
    		Button QuallityBtn = Quallity3Btn;
    		if (curQuallity != 3)
    		{
    			url = quallity_list.get(3).url;
    			mediaPlayer.reset();
    			PrepareMediaPlayer();
    		}
    		curQuallity = 3;
    	}
    	
    	if (v.getId() == R.id.Quallity4Btn)
    	{    		
    		Button QuallityBtn = Quallity4Btn;
    		if (curQuallity != 4)
    		{
    			url = quallity_list.get(4).url;
    			mediaPlayer.reset();
    			PrepareMediaPlayer();
    		}
    		curQuallity = 4;
    	}
    	
    	UpdateQuallityButtonsTextures();


    	timeLastClick = 0;
    	ShowControls(true);
    	
        return;
    }
    
    void UpdateQuallityButtonsTextures()
    {
    	
    	Button Quallity0Btn = (Button) findViewById(R.id.Quallity0Btn);
    	Button Quallity1Btn = (Button) findViewById(R.id.Quallity1Btn);
    	Button Quallity2Btn = (Button) findViewById(R.id.Quallity2Btn);
    	Button Quallity3Btn = (Button) findViewById(R.id.Quallity3Btn);
    	Button Quallity4Btn = (Button) findViewById(R.id.Quallity4Btn);
    	
    	Quallity0Btn.setBackgroundResource(R.drawable.quallity_back);
    	Quallity1Btn.setBackgroundResource(R.drawable.quallity_back);
    	Quallity2Btn.setBackgroundResource(R.drawable.quallity_back);
    	Quallity3Btn.setBackgroundResource(R.drawable.quallity_back);
    	Quallity4Btn.setBackgroundResource(R.drawable.quallity_back);
    	
    	if (curQuallity == 0)
    		Quallity0Btn.setBackgroundResource(R.drawable.quallity_back_sel);
    	if (curQuallity == 1)
    		Quallity1Btn.setBackgroundResource(R.drawable.quallity_back_sel);
    	if (curQuallity == 2)
    		Quallity2Btn.setBackgroundResource(R.drawable.quallity_back_sel);
    	if (curQuallity == 3)
    		Quallity3Btn.setBackgroundResource(R.drawable.quallity_back_sel);
    	if (curQuallity == 4)
    		Quallity4Btn.setBackgroundResource(R.drawable.quallity_back_sel);
    	
    }
    
    @Override
    public void onPrepared(MediaPlayer mp) 
    {  
    	
    	Log.v("VideoView", "onPrepared");
    	runOnUiThread(new Runnable() {
    		 
    		
             @Override
             public void run() 
             {
            	 

            	 try 
            	 {
         			int videoWidth = mediaPlayer.getVideoWidth();
                    int videoHeight = mediaPlayer.getVideoHeight();
                    int screenWidth = getWindowManager().getDefaultDisplay().getWidth();                   
            		 android.view.ViewGroup.LayoutParams lp = surfaceView.getLayoutParams();                       
                 	         
                 	if (videoWidth > 0 && videoHeight > 0)
                 	{
                 		lp.width = screenWidth;  
                 		lp.height = (int) (((float)videoHeight / (float)videoWidth) * (float)screenWidth);
                 	}
                 	else
                 		lp.width = lp.height = 0;

     	            //Commit the layout parameters
     	            surfaceView.setLayoutParams(lp);
     	            
     	           prog.setVisibility(View.INVISIBLE);
            		mediaPlayer.start();
            		
            		ToggleButton playPauseButton = (ToggleButton) findViewById(R.id.playPauseButton);
            		playPauseButton.setBackgroundResource(R.drawable.my_pause);
            		
            		needSeek = true;

            	 }
            	 catch( IllegalStateException e)
            	 {
            		 Log.e("VideoView", "erro mediaPlayer.start() " + e.toString());
            	 }

            	
            	
            	currentProgress.setText( stringForTime(0) );
           	 	maxDuration.setText(stringForTime(0));
           	 	timeLastClick = 0;
           	 	prevTime = 0;
           	 	ShowControls(true);
           	 	prepared = true;
             }
         });
    	
        
   	 	

        
        
    	//mp.start();
       
      
        
        
        
        
    }
    
    @Override
    synchronized protected void onDestroy() 
    {
    	Log.v("VideoView", "onDestroy");
          try {
        	  timer.cancel();
        	  mediaPlayer.stop();
        	  mediaPlayer.release();
        	  mediaPlayer = null;
        	  
          } catch (Exception e) {
                  //
          }
          super.onDestroy();
    }
    
    public class Listiner implements View.OnClickListener
    {
    	@Override
        public void onClick(View v)
    	{
    		//onButtonsClick(v);
    	}        
    }
    
    
    
    Listiner listiner;

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) 
    {
    	
        if (fromUser) 
        {
        	setProgress = progress;
        }
        	
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
        // TODO Auto-generated method stub

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar)
    {
    	runOnUiThread(new Runnable()
    	{

            @Override
            public void run() 
            {
            	Log.v( "VideoView", "seekTo "+setProgress);
            	mediaPlayer.seekTo(setProgress);
            	
            	

            }        
    	});

    }

	@Override
	public void onBufferingUpdate(MediaPlayer mp, int percent) {
		// TODO Auto-generated method stub
		Log.v("ION", "onBufferingUpdate percent"+percent);
		bufprogress = percent;
	} 
	
	private String getRstpLinks(String code)
	{
		String[] urls = new String[3];
	    String link = "http://gdata.youtube.com/feeds/api/videos/" + code + "?alt=json";
	    String json = getJsonString(link); // here you request from the server
	    
	    if (json == "")
	    	return null;
	    try {
	        JSONObject obj = new JSONObject(json);
	        String entry = obj.getString("entry");
	        JSONObject enObj = new JSONObject(entry);
	        String group = enObj.getString("media$group");
	        JSONObject grObj = new JSONObject(group);
	        String content = grObj.getString("media$content");
	        JSONObject cntObj = new JSONObject(group);
	        JSONArray array = grObj.getJSONArray("media$content");
	        Log.v(TAG, "---------------------- read json response ---------------------");
	        for(int j=0; j< array.length(); j++)
	        {
	            JSONObject thumbs = array.getJSONObject(j);
	            Log.v(TAG, "thumbs = "+thumbs.toString()); 
	            String url = thumbs.getString("url");
	            urls[j] = url;
	            Log.v(TAG, "url = "+url);

	            //data.setThumbUrl(thumbUrl);
	        }


	        Log.v(TAG, content);
	    } catch (Exception e) {
	        Log.e(TAG, e.toString());
	        urls[0] = urls[1] = urls[2] = null;
	    }
	    return urls[2];
	    
	   // if (urls[0].contains("rtsp"))
	    	//return urls[0];
	    //if (urls[1].contains("rtsp"))
	    	//return urls[1];
	    

	}
	
	public static String getJsonString(String url)
	{
	    Log.e("Request URL", url);
	    StringBuilder buffer = new StringBuilder();
	    HttpClient client = new DefaultHttpClient();
	    HttpGet     request = new HttpGet( url );
	    HttpEntity entity = null;
	    try {
	        HttpResponse response = client.execute(request);

	        if( response.getStatusLine().getStatusCode() == 200 ){
	            entity = response.getEntity();
	            InputStream is = entity.getContent();
	            BufferedReader br = new BufferedReader(new InputStreamReader(is));
	            String line = null;
	            while( (line = br.readLine() )!= null ){
	                buffer.append(line);
	            }
	            br.close();

	        }
	    } catch (ClientProtocolException e) {
	        // TODO Auto-generated catch block
	        e.printStackTrace();
	    } catch (IOException e) {
	        // TODO Auto-generated catch block
	        e.printStackTrace();}
	    catch (Exception e) 
	    {
		        // TODO Auto-generated catch block
	    	Log.e("LOG", ""+e.toString());
		       // e.printStackTrace();
	    }
	    finally{
	        try {
	        	if (entity != null)
	        		entity.consumeContent();
	        } catch (Exception e) {
	            Log.e(TAG, "Exception = " + e.toString() );
	        }
	    }
	    
	    return buffer.toString();
	}

	@Override
	public void onSeekComplete(MediaPlayer arg0)
	{
		// TODO Auto-generated method stub
		Log.v("VideoView", "onSeekComplete");
		
	}

	@Override
	public boolean onInfo(MediaPlayer arg0, int arg1, int arg2)
	{
		boolean ret = true;
		switch (arg1)
		{
		case MediaPlayer.MEDIA_INFO_UNKNOWN:
			
			Log.v("VideoView", "MEDIA_INFO_UNKNOWN=" + arg1);
			break;
		case MediaPlayer.MEDIA_INFO_VIDEO_TRACK_LAGGING:
			Log.v("VideoView", "MEDIA_INFO_VIDEO_TRACK_LAGGING="+arg1);
			break;
		case MediaPlayer.MEDIA_INFO_VIDEO_RENDERING_START:
			Log.v("VideoView", "MEDIA_INFO_VIDEO_RENDERING_START="+arg1);
			break;
		case MediaPlayer.MEDIA_INFO_BUFFERING_START:
			Log.v("VideoView", "MEDIA_INFO_BUFFERING_START="+arg1);
			was_MEDIA_INFO_BUFFERING_START = true;
			break;
		case MediaPlayer.MEDIA_INFO_BUFFERING_END:
			Log.v("VideoView", "MEDIA_INFO_BUFFERING_END="+arg1);
			was_MEDIA_INFO_BUFFERING_END = true;
			break; 
		case MediaPlayer.MEDIA_INFO_BAD_INTERLEAVING: 
			Log.v("VideoView", "MEDIA_INFO_BAD_INTERLEAVING="+arg1);
			break;
		case MediaPlayer.MEDIA_INFO_NOT_SEEKABLE :
			Log.v("VideoView", "MEDIA_INFO_NOT_SEEKABLE="+arg1);
			break;
		case MediaPlayer.MEDIA_INFO_METADATA_UPDATE :
			Log.v("VideoView", "MEDIA_INFO_METADATA_UPDATE="+arg1);
			break;
		//case MediaPlayer.MEDIA_INFO_UNSUPPORTED_SUBTITLE:
			//Log.v("VideoView", "arg1="+arg1);
			//break;
		//case MediaPlayer.MEDIA_INFO_SUBTITLE_TIMED_OUT: 
		//	Log.v("VideoView", "arg1="+arg1);
		//	break;
			default:
			{
				//Log.v("VideoView", "arg1="+arg1);
			}
		}

		return ret;
	}
		
		private String stringForTime(int timeMs) {
			       int totalSeconds = timeMs / 1000;
			
			        int seconds = totalSeconds % 60;
			        int minutes = (totalSeconds / 60) % 60;
			        int hours   = totalSeconds / 3600;
			

					
			        if (hours > 0) {
			            return String.format("%d:%02d:%02d", hours, minutes, seconds).toString();
			        } else {
			            return String.format("%02d:%02d", minutes, seconds).toString();
			        }
			    }
    
}
