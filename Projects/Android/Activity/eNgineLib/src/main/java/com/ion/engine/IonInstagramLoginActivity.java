
package com.ion.engine;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import org.json.JSONException;
import org.json.JSONObject;
import org.json.JSONTokener;

import android.app.Activity;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.webkit.CookieManager;
import android.webkit.CookieSyncManager;
import android.webkit.WebView;
import android.webkit.WebViewClient;

public class IonInstagramLoginActivity extends Activity
{
    private static final String TAG = "IonInstagramLoginActivity";

    WebView webview;
        
    public static String INSTAGRAM_CALLBACK_BASE = "http://www.oblakarestoran.ru/";
    public static String INSTAGRAM_CLIENT_SECRET = "3579b8f40bb84582b0484718e17f6999";
    public static String INSTAGRAM_CLIENT_ID  = "a5e4c1e00e554ded8b8f784acb69c311";

    public static String token = "";
    
    public static String cur_url = "";  
    
    public static String extractPattern(String string, String pattern)
    {
        Pattern p = Pattern.compile(pattern);
        Matcher m = p.matcher(string);
        if (!m.find())
            return null;
        return m.toMatchResult().group(1);
    }

    public static String[] parseRedirectUrl(String url) throws Exception
    {
        //url is something like http://api.vkontakte.ru/blank.html#access_token=66e8f7a266af0dd477fcd3916366b17436e66af77ac352aeb270be99df7deeb&expires_in=0&user_id=7657164
        String access_token = extractPattern(url, "access_token=(.*?)&");
        Log.i(TAG, "access_token=" + access_token);
        String user_id = extractPattern(url, "user_id=(\\d*)");
        Log.i(TAG, "user_id=" + user_id);
        
        if(user_id==null || user_id.length()==0 || access_token==null || access_token.length()==0)
            throw new Exception("Failed to parse redirect url "+url);
        return new String[]{access_token, user_id};
    }

    
    @Override
    public void onCreate(Bundle savedInstanceState)
    {    	
        super.onCreate(savedInstanceState);
        setContentView(R.layout.login);
        
        webview = (WebView) findViewById(R.id.vkontakteview);
        webview.getSettings().setJavaScriptEnabled(true);
        //webview.clearCache(true);
        
        webview.getSettings().setSupportZoom(true);
        webview.getSettings().setBuiltInZoomControls(true);
        webview.getSettings().setDisplayZoomControls(false);
        
        //Чтобы получать уведомления об окончании загрузки страницы
        webview.setWebViewClient(new VkontakteWebViewClient());
                
        //otherwise CookieManager will fall with java.lang.IllegalStateException: CookieSyncManager::createInstance() needs to be called before CookieSyncManager::getInstance()
        CookieSyncManager.createInstance(this);
        
        CookieManager cookieManager = CookieManager.getInstance();
        cookieManager.removeAllCookie();
        
        String url = "https://api.instagram.com/oauth/authorize/?client_id=" + INSTAGRAM_CLIENT_ID +
        		     "&display=touch&redirect_uri=" + INSTAGRAM_CALLBACK_BASE + 
        		     "&response_type=code";
                       
        webview.loadUrl(url);
    }
    
    TokenTask tokrn_task;
    
    class VkontakteWebViewClient extends WebViewClient
    {
    	public boolean shouldOverrideUrlLoading(WebView view, String url)
    	{
    		Log.i(TAG, "url="+url);
        	
    		cur_url = url;
    		
            String redirect_url = INSTAGRAM_CALLBACK_BASE + "?code=";
                        
            if (url.startsWith(redirect_url))
            {
            	tokrn_task = new TokenTask();
            	tokrn_task.execute();
                
            	return true;
            }
            
            return false;
        }
    }
    
    class TokenTask extends AsyncTask<Void, Void, Void>
    {
        @Override
        protected void onPreExecute()
        {
        	super.onPreExecute();        
        }

        @Override
        protected Void doInBackground(Void... params)
        {
        	Log.i(TAG, "start");
        	
        	HttpClient httpClient = new DefaultHttpClient();
        	
        	String redirect_url = INSTAGRAM_CALLBACK_BASE + "?code=";
        	
        	String authToken = cur_url.substring(redirect_url.length());
            
        	Log.i(TAG, "authToken " + authToken);
        	
            HttpPost httpPost = new HttpPost("https://api.instagram.com/oauth/access_token");            
            
            List<NameValuePair> nameValuePair = new ArrayList<NameValuePair>(5);
            nameValuePair.add(new BasicNameValuePair("code", authToken));
            nameValuePair.add(new BasicNameValuePair("redirect_uri", INSTAGRAM_CALLBACK_BASE));
            nameValuePair.add(new BasicNameValuePair("grant_type", "authorization_code"));
            nameValuePair.add(new BasicNameValuePair("client_id", INSTAGRAM_CLIENT_ID));
            nameValuePair.add(new BasicNameValuePair("client_secret", INSTAGRAM_CLIENT_SECRET));                
            
            try
            {
                httpPost.setEntity(new UrlEncodedFormEntity(nameValuePair));               
            }
            catch (UnsupportedEncodingException e) 
            {
               e.printStackTrace();
            }
                                    
            try
            {
                HttpResponse response = httpClient.execute(httpPost);
                // write response to log
                
                Reader in = new BufferedReader( new InputStreamReader(response.getEntity().getContent(), "UTF-8"));
                	
                StringBuilder builder= new StringBuilder();
                	
                char[] buf = new char[1000];
                
                int l = 0;
                
                while (l >= 0)
                {
                    builder.append(buf, 0, l);
                    l = in.read(buf);
                }
                	
                JSONTokener tokener = new JSONTokener( builder.toString() );                                       
                
                Log.i(TAG, "finished " + builder.toString());
                
                try
                {
                	JSONObject object = (JSONObject)tokener.nextValue();                    
                
                	token = object.getString("access_token");
                	Log.i("Http Post Response:", token);
                }
                catch (JSONException e)
                {
                	
                }
                
            }
            catch (ClientProtocolException e)
            {
                // Log exception
                e.printStackTrace();
            }
            catch (IOException e)
            {
                // Log exception
                e.printStackTrace();
            };
            
        	return null;
        }

        @Override
        protected void onPostExecute(Void result)
        {
        	super.onPostExecute(result);
        	
        	FinishLogin();
        }
      } 
    
    void FinishLogin()
    {
    	Intent intent = new Intent();                    
        intent.putExtra("token", token);        
        setResult(Activity.RESULT_OK, intent);
        
        finish();
    }
}