
package com.ion.engine;

import java.net.URLEncoder;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.util.Log;
import android.webkit.CookieManager;
import android.webkit.CookieSyncManager;
import android.webkit.WebView;
import android.webkit.WebViewClient;

public class IonVKLoginActivity extends Activity
{
    private static final String TAG = "IonVKLoginActivity";

    WebView webview;
    
    public static String API_ID="";
    public static String redirect_url="http://oauth.vk.com/blank.html";
    public static String token = "";
    public static String user_id = "";
    
    public static String getSettings()
    {
        int settings = 2 + 4 + 16 + 8192 + 65536 + 134217728;
        
        return Integer.toString(settings);
        //return "friends,photos,audio,video,docs,notes,pages,wall,groups,messages,offline,notifications";
    }

    public static String getUrl(String api_id, String settings)
    {
        String url="http://oauth.vk.com/authorize?client_id="+api_id+"&display=touch&scope="+settings+"&redirect_uri="+URLEncoder.encode(redirect_url)+"&response_type=token";
        return url;
    }

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
        
        String url = getUrl(API_ID, getSettings());
        webview.loadUrl(url);
    }
    
    class VkontakteWebViewClient extends WebViewClient
    {
        @Override
        public void onPageStarted(WebView view, String url, Bitmap favicon)
        {
            super.onPageStarted(view, url, favicon);
            parseUrl(url);
        }
    }
    
    private void parseUrl(String url)
    {
        try
        {
            if (url==null)
            {
            	return;
            }
            
            Log.i(TAG, "url="+url);
            	
            if(url.startsWith(redirect_url))
            {
                if(!url.contains("error="))
                {   
                	String[] auth = parseRedirectUrl(url);
                	
                	token = auth[0];
                    user_id = "" + Long.parseLong(auth[1]);                                        
                	                    
                    SharedPreferences prefs = getSharedPreferences(getPackageName(), MODE_PRIVATE);
                                       
                    SharedPreferences.Editor editor = prefs.edit();;
                    
                    editor.putString("VK_token", token);                    
                    editor.putString("VK_user_id", user_id);
                    editor.commit();
                    
                    Intent intent = new Intent();                    
                    intent.putExtra("token", token);
                    intent.putExtra("user_id", Long.parseLong(auth[1]));
                    setResult(Activity.RESULT_OK, intent);
                }
                finish();
            }
            
            if (url.contains("error=access_denied&error_reason=user_denied"))
            {
            	finish();
            }
        }
        
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }
}