
package com.ion.engine;

import android.app.Activity;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.LinearInterpolator;
import android.view.animation.RotateAnimation;
import android.webkit.CookieManager;
import android.webkit.CookieSyncManager;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

public class IonWebViewActivity extends Activity
{
    private static final String TAG = "IonWebViewActivity";

    static String url = "";
    static String stop_url = "";
    
    WebView webview = null;   
    
    //View back_btn_on = null;
    //View back_btn_off = null;
    
    //View frvd_btn_on = null;
    //View frvd_btn_off = null;
    
    //ImageButton refresh_btn = null;    
    RotateAnimation refresh_anim = null;
    LinearLayout open_layout = null;    
    LinearLayout open_layout_back = null;
    TextView url_txt = null;
    TextView url_txt2 = null;
    TextView loading_txt = null;
    ImageView refresh_img = null;
       
    
    public class Listiner implements View.OnClickListener
    {
    	@Override
        public void onClick(View v)
    	{
    		onButtonsClick(v);
    	}        
    }
    
    Listiner listiner;    
    
    
    public void onButtonsClick(View v)
    {
    	if (v.getId() == R.id.closeWeb)
    	{    		
    		finish();
    	}
    	else
    	/*if (v.getId() == R.id.nextPage)
    	{
    		webview.goForward();    		
    	}
    	else
   		if (v.getId() == R.id.prevPage)
       	{
   			webview.goBack();   			
       	}    
   		else*/
   	   	if (v.getId() == R.id.refreshWeb)
   	    {
   	   		webview.reload();   	   		
   	    }  
   	   	else
    	if (v.getId() == R.id.openBrowserPanel)
    	{    		
    		open_layout.setVisibility(0);
    		
    		AlphaAnimation fade_in = new AlphaAnimation(0.0f, 1.0f);
    		fade_in.setDuration(300);
    	        		        		    		
    		//open_layout.startAnimation(fade_in);
    		
    		open_layout_back.setVisibility(0);
    		
    		fade_in = new AlphaAnimation(0.0f, 0.5f);
    		fade_in.setDuration(300);
    	        		        		    		
    		//open_layout_back.startAnimation(fade_in);
    		
    		//open_layout.setAlpha(1);
    		//open_layout_back.setAlpha(0.5f);
    	}
    	else
        if (v.getId() == R.id.closeBrowserPanel)
        {        	
        	open_layout.setVisibility(8);
        	
        	AlphaAnimation fade_out = new AlphaAnimation(1.0f, 0.0f);
        	fade_out.setDuration(300);
    	        		        		    		
    		//open_layout.startAnimation(fade_out);
    		
    		open_layout_back.setVisibility(8);
    		
    		fade_out = new AlphaAnimation(0.5f, 0.0f);
        	fade_out.setDuration(300);
    	        		        		    		
        	//open_layout_back.startAnimation(fade_out);
        	
        	//open_layout.setAlpha(0);
    		//open_layout_back.setAlpha(0);
        }    	
        else
        if (v.getId() == R.id.openInBrowser)
        {
        	Intent intent = new Intent(Intent.ACTION_VIEW); 
    		intent.setData(Uri.parse(url)); 
    		startActivity(intent);
        }
        else
        if (v.getId() == R.id.copyLink)
        {
        	ClipboardManager ClipMan = (ClipboardManager) getSystemService(Context.CLIPBOARD_SERVICE);
            ClipMan.setText(url);
        }
    }
    
    public String trimToLength(TextView painter, String initialText)
	{
    	painter.setText(url);    	    	
    	
	    String message = initialText;
	    String output = initialText;

	    float currentWidth = painter.getPaint().measureText(output);
	    while (currentWidth > painter.getWidth())
	    {
	    	if (message.length()-1 == 0)
	    	{
	    		break;
	    	}
	    	
	        message = message.substring(0, message.length()-1);

	        output = message + "...";

	        currentWidth = painter.getPaint().measureText(output);
	    }

	    return output;
	}
    
    void SetTexttURL(String set_url)
    {
    	url_txt.setText(url);//trimToLength(url_txt, url));
    	url_txt2.setText(url);
    }
    
    class IonWebViewClient extends WebViewClient
    {
    	
        @Override
        public void onPageStarted(WebView view, String set_url, Bitmap favicon)
        {
        	url = set_url;

            Log.e(TAG, "onPageStarted " + url);
            Log.e(TAG, "onPageStarted " + stop_url);

            if (!stop_url.isEmpty() && url.contains(stop_url))
            {
                Log.e(TAG, "onPageStarted triggered");

                finish();
            }

        	SetTexttURL(url);
        	
        	UpdateNavButtons();
        	
            super.onPageStarted(view, url, favicon);
            StartRefreshAnim(1);   
            
            refresh_img.setVisibility(0);
            loading_txt.setVisibility(0);
        }
        
        @Override
        public void onPageFinished(WebView view, String url)
        {
            super.onPageFinished(view, url);
            StartRefreshAnim(0);
            
            refresh_img.setVisibility(View.INVISIBLE);
            loading_txt.setVisibility(View.INVISIBLE);                        
            
            Log.e(TAG, "onPageFinished " + url);       
            
            UpdateNavButtons();
        }
    }
    
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.webview);
        
        webview = (WebView) findViewById(R.id.webview);
        webview.getSettings().setJavaScriptEnabled(true);
        
        String agent_name = webview.getSettings().getUserAgentString() + " ION ENgine";
        webview.getSettings().setUserAgentString(agent_name);
        
        webview.setWebViewClient(new IonWebViewClient());
        //webview.clearCache(true);
        
        webview.getSettings().setSupportZoom(true);
        webview.getSettings().setBuiltInZoomControls(true);
        webview.getSettings().setDisplayZoomControls(false);               
        
        open_layout = (LinearLayout) findViewById(R.id.BrowserPanel);        
        open_layout.setVisibility(8);
        open_layout.setAlpha(1.0f);
        
                
        open_layout_back = (LinearLayout) findViewById(R.id.BrowserPanelBack);        
        open_layout_back.setVisibility(8);
        open_layout_back.setAlpha(0.5f);
                                               
                        
        url_txt = (TextView) findViewById(R.id.BrowserPanelURL);        
        url_txt2 = (TextView) findViewById(R.id.BrowserPanelURL2);
        
        SetTexttURL(url);
        
        loading_txt = (TextView) findViewById(R.id.LoadingURL);
        
        CookieSyncManager.createInstance(this);
        
        CookieManager cookieManager = CookieManager.getInstance();
        cookieManager.removeAllCookie();
                
        webview.loadUrl(url);
        
        listiner = new Listiner();
        
        View button = findViewById(R.id.closeWeb);
        button.setOnClickListener(listiner);                           

        button = findViewById(R.id.openBrowserPanel);
        button.setOnClickListener(listiner);
        
        button = findViewById(R.id.closeBrowserPanel);
        button.setOnClickListener(listiner);
        
        button = findViewById(R.id.copyLink);
        button.setOnClickListener(listiner);
        
        button = findViewById(R.id.openInBrowser);
        button.setOnClickListener(listiner);
        
        //refresh_btn = (ImageButton) findViewById(R.id.refreshWeb);      
        //refresh_btn.setOnClickListener(listiner);

        refresh_img = (ImageView) findViewById(R.id.refreshWeb);     
        
        refresh_img.setVisibility(View.INVISIBLE);
        loading_txt.setVisibility(View.INVISIBLE);
               
        
        /*back_btn_on = findViewById(R.id.prevPage);
        back_btn_on.setOnClickListener(listiner);
        
        back_btn_off = findViewById(R.id.prevPage2);
        
        frvd_btn_on = findViewById(R.id.nextPage);
        frvd_btn_on.setOnClickListener(listiner);
        
        frvd_btn_off = findViewById(R.id.nextPage2);*/ 
    }
    
    void StartRefreshAnim(int anim)
    {
    	if (anim == 1)
    	{
    		refresh_anim = new RotateAnimation(0f, 360f, Animation.RELATIVE_TO_SELF, 0.5f, Animation.RELATIVE_TO_SELF, 0.5f);
            refresh_anim.setInterpolator(new LinearInterpolator());
            refresh_anim.setRepeatCount(Animation.INFINITE);
            refresh_anim.setDuration(700);
                        
            refresh_img.startAnimation(refresh_anim);
    	}
    	else
    	{    		            
    		refresh_img.clearAnimation();
    	}
    }
    
    void UpdateNavButtons()
    {    	
    	int vis_back = 8;
    	int vis_frvd = 8;
    	
    	if (webview.canGoBack())
    	{
    		vis_back = 0;
    		
    	}
    	
    	if (webview.canGoForward())
    	{
    		vis_frvd = 0;    	
    	}    	
    	
    	/*back_btn_on.setVisibility(vis_back);
    	back_btn_off.setVisibility(8 - vis_back);
    	
    	frvd_btn_on.setVisibility(vis_frvd);
    	frvd_btn_off.setVisibility(8 - vis_frvd);*/
    }    
}