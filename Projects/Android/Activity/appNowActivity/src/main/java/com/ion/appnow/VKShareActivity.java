
package com.ion.appnow;

import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.List;

import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;

import android.app.Activity;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;

import com.ion.engine.IonLib;

import com.ion.engine.IonVKLoginActivity;

public class VKShareActivity extends Activity
{	
	int REQUEST_LOGIN = 578;
	public static final String TAG = "ION";		
	
	String share_message = "";
	String share_link = "";
	String share_vk = "";		
	
	@Override
	protected void onCreate(Bundle icicle)
	{
		super.onCreate(icicle);
		
		// Get intent, action and MIME type
	    Intent intent = getIntent();
	    String action = intent.getAction();
	    String type = intent.getType();

	    if (Intent.ACTION_SEND.equals(action) && type != null)
	    {
	        if ("text/plain".equals(type))
	        {	        
	        	share_message = AppNowActivity.share_text;
	        	share_link = AppNowActivity.share_link;
	        	share_vk = AppNowActivity.share_vk;	        		        	
	        }
	        else
	        if (type.startsWith("image/"))
	        {
	            //handleSendImage(intent); // Handle single image being sent
	        }
	    }
	    else
	    if (Intent.ACTION_SEND_MULTIPLE.equals(action) && type != null)
	    {
	        if (type.startsWith("image/"))
	        {
	            //handleSendMultipleImages(intent); // Handle multiple images being sent
	        }
	    }
	    else
	    {
	        // Handle other intents, such as being started from the home screen
	    }
	    		    
   	 	if (!IonVKLoginActivity.token.isEmpty())   	 			    
	    {	    	
	    	MakeVKShare();
	    }
	    else
	    {
	    	OpenVKSession();
	    }
	}
	 
	public void OpenVKSession()
	{ 
		Log.e("ION", "VK LogIn");
			
		if (!IonVKLoginActivity.API_ID.equals(""))
		{	
			Intent intent = new Intent();
			intent.setClass(this, IonVKLoginActivity.class);
			startActivityForResult(intent, REQUEST_LOGIN);
		}
		else
		{		
			Log.e(TAG, "VK app id not setted");
			finish();
		}
	}
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data)
	{  		
		if (requestCode == REQUEST_LOGIN)
    	{
			if (resultCode == RESULT_OK)
	        {	             	       
				Log.e(TAG, "REQUEST_LOGIN ok");
	            MakeVKShare();
	        }
			else
			{
				Log.e(TAG, "REQUEST_LOGIN fail");
				finish();
			}
    	}
		else
		{
			super.onActivityResult(requestCode, resultCode, data);
		}
	}	
		
	void MakeVKShare() 
	{	
		Log.e(TAG, "MakeVKShare");
		
		String request = "";		
		String post_params = "";
		
		if (share_vk.isEmpty())
		{								
	        request = "https://api.vk.com/method/wall.post?access_token=" + IonVKLoginActivity.token;
	        post_params = "message=" + share_message;	        
		}
		else
		{			
			request = "https://api.vk.com/method/wall.repost?access_token=" + IonVKLoginActivity.token + "&object=wall" + share_vk;			
		}				
		
		IonLib.StartRequest("", request, post_params, "vk_share");
			
		finish();
	}
}
