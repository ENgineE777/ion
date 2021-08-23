
package com.ion.engine;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.math.BigDecimal;
import java.security.MessageDigest;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.GregorianCalendar;
import java.util.List;
import java.util.TimerTask;

import net.robotmedia.billing.BillingController;
import net.robotmedia.billing.BillingRequest.ResponseCode;
import net.robotmedia.billing.helper.AbstractBillingObserver;
import net.robotmedia.billing.model.Transaction.PurchaseState;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.ActivityNotFoundException;
import android.content.BroadcastReceiver;
import android.content.ContentResolver;
import android.content.ContentUris;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.pm.Signature;
import android.content.res.Resources;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.graphics.Rect;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.os.BatteryManager;
import android.os.Bundle;
import android.os.Environment;
import android.os.Parcelable;
import android.provider.CalendarContract;
import android.provider.ContactsContract;
import android.provider.ContactsContract.CommonDataKinds.Phone;
import android.provider.MediaStore;
import android.telephony.PhoneNumberUtils;
import android.telephony.SmsManager;
import android.telephony.TelephonyManager;
import android.util.Base64;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputMethodManager;
import android.webkit.CookieManager;
import android.webkit.CookieSyncManager;
import android.webkit.WebView;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.facebook.*;
import com.facebook.login.LoginManager;
import com.facebook.login.LoginResult;
import com.facebook.AccessToken;

import com.google.android.gms.maps.UiSettings;
import com.ion.engine.IonDownloader.UIHolder;

public class IonActivity extends Activity
{
	InputConnection input_connection = null;
	
	public native void IonInit(boolean need_activate_pack);
	public native void IonStep();


	public static IonActivity activity;
	
	public static final String TAG = "ION";	
	
	public static String device_id = "";
	
	int REQUEST_LOGIN = 578;
	int SELECT_IMAGE = 579;	
	int SELECT_PHOTO = 577;
	int REQUEST_LOGIN_INST = 580;
	int image_index = 0;
	
	public static String facebook_app_id = "";
    public IonView ion_view;
    public IonInputView mInputView;
    
    //private SupportMapFragment mMap = null;   //
    //MapView mMap = null;    
    private UiSettings mUiSettings;
    
    public IonDownloader downloader;            
    
    private AbstractBillingObserver billingObserver;

	private CallbackManager callbackManager;


	String appflood_ads_key = null;
	String appflood_ads_secret = null;

	public String cur_kb_type = "";
	public String cur_rk_type = "";

	Cursor contact_reader;
	
	FrameLayout frameLayout;
	
    public class IonSensorEventListener implements SensorEventListener
    {
    	 public void onSensorChanged(SensorEvent event)
    	 { 
     	    if (event.sensor.getType() == Sensor.TYPE_GYROSCOPE)
     	    {
     	    	IonLib.SetGyro(event.values[0], event.values[1], event.values[2]);
     	    }
     	 }
     	   
     	 public void onAccuracyChanged(Sensor sensor, int accuracy)
     	 {
     		 
     	 }
    }
    
    IonSensorEventListener ion_sensor;    	   
    Location loc_loc;
    
    private class IonLocationListener implements LocationListener
    {
        @Override
        public void onLocationChanged(Location loc)
        {
        	loc_loc = loc;

        	runOnUiThread(new Runnable()
    		{
    			public void run()
    			{
    				IonLib.SetGPSData(0, (float)loc_loc.getLatitude(), (float)loc_loc.getLongitude());
    			}
    		});
        }

        @Override
        public void onProviderDisabled(String provider) {}

        @Override
        public void onProviderEnabled(String provider) {}

        @Override
        public void onStatusChanged(String provider, int status, Bundle extras) {}
    }
    
    IonLocationListener locationListiner = null;
    
    // ================================
    // Begin. PushWoosh.
    // ================================
    
    /*private BroadcastReceiver mBroadcastReceiver = new RegisterBroadcastReceiver()
    {
        @Override
        public void onRegisterActionReceive(Context context, Intent intent)
        {
            checkMessage(intent, false);
        }
    };
    
    private BroadcastReceiver mReceiver = new BasePushMessageReceiver()
    {
        @Override
        protected void onMessageReceive(Intent intent)
        {
            //JSON_DATA_KEY contains JSON payload of push notification.
            ShowMessage(intent.getExtras().getString(JSON_DATA_KEY), false);
        }
    };
    
    public void registerPushWooshReceivers()
    {
        IntentFilter intentFilter = new IntentFilter(getPackageName() + ".action.PUSH_MESSAGE_RECEIVE");
     
        registerReceiver(mReceiver, intentFilter);
        registerReceiver(mBroadcastReceiver, new IntentFilter(getPackageName() + "." + PushManager.REGISTER_BROAD_CAST_ACTION));
    }
    
    public void unregisterPushWooshReceivers()
    {
        //Unregister receivers on pause
        try
        {
            unregisterReceiver(mReceiver);
        }
        catch (Exception e)
        {
            // pass.
        }
     
        try
        {
            unregisterReceiver(mBroadcastReceiver);
        }
        catch (Exception e)
        {
            //pass through
        }
    }
    
    private void checkMessage(Intent intent, boolean on_start)
    {
        if (null != intent)
        {
            if (intent.hasExtra(PushManager.PUSH_RECEIVE_EVENT))
            {
                ShowMessage(intent.getExtras().getString(PushManager.PUSH_RECEIVE_EVENT), on_start);
            }
            else if (intent.hasExtra(PushManager.REGISTER_EVENT))
            {
            	// Toast.makeText(this, intent.getExtras().getString(PushManager.REGISTER_EVENT), Toast.LENGTH_SHORT).show();
                // showMessage("register");
            }
            else if (intent.hasExtra(PushManager.UNREGISTER_EVENT))
            {
                // showMessage("unregister");
            }
            else if (intent.hasExtra(PushManager.REGISTER_ERROR_EVENT))
            {
                // showMessage("register error");
            }
            else if (intent.hasExtra(PushManager.UNREGISTER_ERROR_EVENT))
            {
                // showMessage("unregister error");
            }
     
            resetIntentValues();
        }
    }
     
    // Will check main Activity intent and if it contains any PushWoosh data, will clear it

    private void resetIntentValues()
    {
        Intent mainAppIntent = getIntent();
     
        if (mainAppIntent.hasExtra(PushManager.PUSH_RECEIVE_EVENT))
        {
            mainAppIntent.removeExtra(PushManager.PUSH_RECEIVE_EVENT);
        }
        else if (mainAppIntent.hasExtra(PushManager.REGISTER_EVENT))
        {
            mainAppIntent.removeExtra(PushManager.REGISTER_EVENT);
        }
        else if (mainAppIntent.hasExtra(PushManager.UNREGISTER_EVENT))
        {
            mainAppIntent.removeExtra(PushManager.UNREGISTER_EVENT);
        }
        else if (mainAppIntent.hasExtra(PushManager.REGISTER_ERROR_EVENT))
        {
            mainAppIntent.removeExtra(PushManager.REGISTER_ERROR_EVENT);
        }
        else if (mainAppIntent.hasExtra(PushManager.UNREGISTER_ERROR_EVENT))
        {
            mainAppIntent.removeExtra(PushManager.UNREGISTER_ERROR_EVENT);
        }
     
        setIntent(mainAppIntent);
    }        
     
    private void ShowMessage(String message, boolean on_start)
    {
    	Log.d("ShowMessage", message);
    	
    	try
    	{
    		JSONObject json = new JSONObject(message);
    		
    		if (json.has("foreground"))
    		{
    			boolean foregound = json.getBoolean("foreground");
    			
    			if (foregound)
    			{
    				// Skip foreground notifications
    				return;
    			}
    		}
    		else
    		{
    			// Skip foreground notifications
    			return;
    		}
    		
    		String userdata = null;
    		
    		if (json.has("userdata"))
    		{
    			userdata = json.getString("userdata");
    		}
    		else
    		{
    			userdata = "";
    		}
    		
    		Log.d("ShowMessage", "UserData: " + userdata);
    		
    		if (on_start)
    		{
    			IonLib.SchedulePushNotificationUserData(userdata);
    		}
    		else
    		{
    			IonLib.ProcessPushNotificationUserData(userdata);
    			
    			CreateInfoNotification(json.getString("title"));
    		}
    	}
    	catch (JSONException e)
    	{
    		Log.e("PushJSON", e.toString());
		}
    }
    
    @Override
    protected void onNewIntent(Intent intent)
    {
        super.onNewIntent(intent);
        checkMessage(intent, false);
    }

    // ================================
    // End. PushWoosh.
    // ================================
    */
    
    /*private int lastId = 1;
    
    public int CreateInfoNotification(String message)
    {
    	NotificationManager notificationManager = (NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE);

    	Context context = getApplicationContext();
    	
    	
    	String notificationTitle = getString(R.string.ion_activity);
    	String notificationText = message;
    	Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(""));
    	
    	PendingIntent pendingIntent = PendingIntent.getActivity(IonActivity.this,
    		       					  0, intent,
    		       					  Intent.FLAG_ACTIVITY_NEW_TASK);


		//Notification notification = new Notification(R.drawable.ic_launcher,
		//											 getString(R.string.ion_activity),
		//											 System.currentTimeMillis());
    	//notification.defaults |= Notification.DEFAULT_SOUND;
    	//notification.flags |= Notification.FLAG_AUTO_CANCEL;
    	//notification.setLatestEventInfo(context, notificationTitle, notificationText, pendingIntent);

		Notification notification = new Notification.Builder(context)
									.setContentTitle(notificationTitle)
									.setContentText(notificationText)
									.setSmallIcon(R.drawable.ic_launcher)
									.build();

		notificationManager.notify(lastId, notification);
        
        return lastId++;
    }  */
    
    private void RestoreTransactions()
    {
		if (!billingObserver.isTransactionsRestored())
		{
			BillingController.restoreTransactions(this);		
		}
	}
    
    public void onBillingChecked(boolean supported)
    {
		if (supported)
		{
			RestoreTransactions();			
		}		
	}
    
    public void onPurchaseStateChanged(String itemId, PurchaseState state)
    {
		Log.i(TAG, "onPurchaseStateChanged() itemId: " + itemId + " st - " + state);
					
		if (state == PurchaseState.PURCHASED)
		{
			IonLib.ResolveRequestedItem(true);
		}
		else
		{
			IonLib.ResolveRequestedItem(false);		
		}
	}

	public void onRequestPurchaseResponse(String itemId, ResponseCode response)
	{
		Log.i(TAG, "onRequestPurchaseResponse() itemId: " + itemId + " rs - " + response);
		IonLib.ResolveRequestedItem(false);
	}
	
	public void onSubscriptionChecked(boolean supported)
	{
		
	}
    
    public void LoadSOLibs()
    {
    	IonLib.Load();
    }
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        return false;
    }

	void SetupShortcut()
	{
		Log.e("Fuckkkk!!!!", "ShortcutIconResource icon");

		Intent shortcutIntent = new Intent(Intent.ACTION_MAIN);
		shortcutIntent.setClassName(this, this.getClass().getName());
		//shortcutIntent.putExtra(EXTRA_KEY, "ApiDemos Provided This Shortcut");

		// Then, set up the container intent (the response to the caller)

		Intent intent = new Intent();
		intent.putExtra(Intent.EXTRA_SHORTCUT_INTENT, shortcutIntent);
		intent.putExtra(Intent.EXTRA_SHORTCUT_NAME, getString(R.string.ion_activity));
		Parcelable iconResource = Intent.ShortcutIconResource.fromContext(this,  R.drawable.ic_launcher);

		intent.putExtra(Intent.EXTRA_SHORTCUT_ICON_RESOURCE, iconResource);

		setResult(RESULT_OK, intent);
	}

	void EnableGPS()
	{
		LocationManager locationManager = (LocationManager)getSystemService(Context.LOCATION_SERVICE);
		locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 3000, 10, locationListiner);
		locationManager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER, 3000, 10, locationListiner);
	}

	void DisableGPS()
	{
		LocationManager locationManager = (LocationManager)getSystemService(Context.LOCATION_SERVICE);
		locationManager.removeUpdates(locationListiner);
	}


	@Override
    protected void onCreate(Bundle icicle)
	{
		final Intent intent = getIntent();
		final String action = intent.getAction();

		if (Intent.ACTION_CREATE_SHORTCUT.equals(action))
		{
			super.onCreate(icicle);

			SetupShortcut();
			finish();
			return;
		}

		LoadSOLibs();

    	ApplicationInfo appInfo = null;
		PackageManager packMgmr = this.getPackageManager();
        
		{
			PackageInfo packageInfo;
			try
			{
				packageInfo = getPackageManager().getPackageInfo(getPackageName(), PackageManager.GET_SIGNATURES);

				IonLib.SetInstallationTime(packageInfo.firstInstallTime);

				for (Signature signature : packageInfo.signatures)
				{
                   MessageDigest md = MessageDigest.getInstance("SHA");
                   md.update(signature.toByteArray());
                   String key = new String(Base64.encode(md.digest(), 0));
                   // String key = new String(Base64.encodeBytes(md.digest()));
                   Log.e("Hash key", key);
				} 
			}
			catch (NameNotFoundException e1)
			{
				Log.e("Name not found", e1.toString());
			} 			
			catch (Exception e)
			{
				Log.e("Exception", e.toString());
			}
		}
        
		String sd_path = "";
		
		File sd_file = Environment.getExternalStorageDirectory();
		
		sd_path = sd_file.getAbsolutePath() + "/Android/data/" + getPackageName() + "/";
			
		File dir = new File(sd_path);
		dir.mkdirs();
			
		Log.i(TAG, sd_path);
			
		IonLib.SetBasePath(sd_path);

		Log.e(TAG, "Package: " + getPackageName());

		String locale = java.util.Locale.getDefault().getLanguage();		
		Log.e(TAG, "SysLang: " + locale);
		
		IonLib.SetLocale(locale);
		
    	activity = this;
    	
    	try
		{
			appInfo = packMgmr.getApplicationInfo(getPackageName(), 0);
			IonLib.SetApkName(appInfo.sourceDir);
		}
		catch (NameNotFoundException e)
		{
			e.printStackTrace();
			throw new RuntimeException("Unable to locate assets, aborting...");
		}
    	
    	IonLib.SetLoadFromApk(ReadFromApk());
    	
    	if (IonLib.GetProjectOrientation() == 0)
    	{    	    	
    		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
    	}
    	else
    	{
    		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);    	
    	}
    	
    	int prj_theme = IonLib.GetProjectTheme();
    	
    	if (prj_theme == 0)
    	{    	
    		setTheme(android.R.style.Theme_Black_NoTitleBar_Fullscreen);
    	}
    	else
    	if (prj_theme == 1)
    	{
    		setTheme(android.R.style.Theme_Black_NoTitleBar);
    	}
    	else
        if (prj_theme == 2)
        {
        	setTheme(android.R.style.Theme_Black);
        }    	    
    	
    	if (((TelephonyManager)getSystemService(Context.TELEPHONY_SERVICE)).getPhoneType() == TelephonyManager.PHONE_TYPE_NONE)
    	{
    		IonLib.SetAllowCall(false);
    	}
    	else
    	{
    		IonLib.SetAllowCall(true);
    	}

    	super.onCreate(icicle);

    	if (UseDownloader())
    	{
    		 InitDownloader();	
    	}
    	else
    	{    		  
    		StartENgine();
    	}

		DisplayMetrics metrics = new DisplayMetrics();
		getWindowManager().getDefaultDisplay().getRealMetrics(metrics);

		Log.i(TAG, "density " + metrics.densityDpi);
		IonLib.SetScreenDPI(metrics.densityDpi);

		billingObserver = new AbstractBillingObserver(this)
			{
				public void onBillingChecked(boolean supported)
				{
					IonActivity.this.onBillingChecked(supported);
				}

				public void onPurchaseStateChanged(String itemId, PurchaseState state)
				{
					IonActivity.this.onPurchaseStateChanged(itemId, state);
				}

				public void onRequestPurchaseResponse(String itemId, ResponseCode response)
				{
					IonActivity.this.onRequestPurchaseResponse(itemId, response);
				}

				public void onSubscriptionChecked(boolean supported)
				{
					IonActivity.this.onSubscriptionChecked(supported);
				}		
		};				
	    
		{
			WebView web = new WebView(this);
			web.destroy();
		}
	    
		BillingController.setDebug(true);
		BillingController.setConfiguration(new BillingController.IConfiguration()
		{
			
			public byte[] getObfuscationSalt()
			{
				return IonActivity.this.GetSALT();
			}

			public String getPublicKey()
			{
				return IonActivity.this.GetPublicKey();
			}
		});
		
		BillingController.registerObserver(billingObserver);
		BillingController.checkBillingSupported(this);
		BillingController.checkSubscriptionSupported(this);
		
		//registerPushWooshReceivers();
		
		try
		{
			PackageInfo packageInfo = getPackageManager().getPackageInfo(getPackageName(), PackageManager.GET_ACTIVITIES | PackageManager.GET_META_DATA);
			String prefix = /*getPackageName()*/ "com.ion.packagename" + ".MetaActivity.";

			for (int i = 0, size = packageInfo.activities.length; i < size; ++i)
			{	
				Bundle metaData = packageInfo.activities[i].metaData;
				
				String name = packageInfo.activities[i].name;

				//Log.d("ION", "packageInfo.activities: " + packageInfo.activities[i].name);

				if (name.startsWith(prefix))
				{
					String module_name = name.substring(prefix.length());
					
					Log.d("ION", "module_name: " + module_name);
					
					if (module_name.equals("pushwoosh"))
					{
						String pw_application = metaData.getString("pw_application");
						if (pw_application == null) break;
						
						pw_application = pw_application.substring("val:".length());
						
						String pw_apis_project_id = metaData.getString("pw_apis_project_id");
						if (pw_apis_project_id == null) break;
						
						pw_apis_project_id = pw_apis_project_id.substring("val:".length());
						
						Log.d("ION", "pw_application: " + pw_application);
						Log.d("ION", "pw_apis_project_id: " + pw_apis_project_id);
						
						//Create and start push manager
					    //PushManager pushManager = new PushManager(this, pw_application/*"14D4F-20D8C"*/ , pw_apis_project_id /*"986201009374"*/ );
					    //pushManager.onStartup(this);
					 
					    //checkMessage(getIntent(), true);
					}
					else
					if (module_name.equals("facebook"))
					{
						String str = metaData.getString("appid");
						if (str == null) continue;
						
						facebook_app_id = str.substring("val:".length());	
						
						Log.d("ION", "facebook_app_id: " + facebook_app_id);
					}
					else
					if (module_name.equals("vk"))
					{
						String vk_id = metaData.getString("appid");
						if (vk_id == null) continue;
						
						IonVKLoginActivity.API_ID = vk_id.substring("val:".length());
						
						Log.d("ION", "vk: " + IonVKLoginActivity.API_ID);
					}					
					else
					if (module_name.equals("ads"))
					{	
						appflood_ads_key = metaData.getString("ads_key");
						if (appflood_ads_key == null) continue;
						appflood_ads_key = appflood_ads_key.substring("val:".length());
							
						String appflood_ads_secret = metaData.getString("ads_secret");
						if (appflood_ads_secret == null) continue;
						appflood_ads_secret = appflood_ads_secret.substring("val:".length());
							
						Log.d("ION", "ads_key: " + appflood_ads_key);
						Log.d("ION", "ads_secret: " + appflood_ads_secret);
					}					
					else
					if (module_name.equals("ads_mmedia"))
					{						
					}
					else
					if (module_name.equals("ads_admob"))
					{
						String ads_admob_appid = packageInfo.activities[i].metaData.getString("ads_key");
						if (ads_admob_appid == null) continue;
						ads_admob_appid = ads_admob_appid.substring("val:".length());
						
						Log.d("ION", "ads_admob_appid: " + ads_admob_appid);
					}
                    else
                    {
                        ReadModules(module_name, metaData);
                    }
				}
			}
		}
		catch (NameNotFoundException e)
		{
			// TODO Auto-generated catch block
			// e.printStackTrace();
		}
		    
		//IonTimerTask ionTask = new IonTimerTask();
        //Timer ionTimer = new Timer();

        //ionTimer.schedule(ionTask, 1000, 1);     
        
        ion_sensor = new IonSensorEventListener();
        
        SensorManager sm = (SensorManager)getSystemService(Context.SENSOR_SERVICE);
        	
        sm.registerListener(ion_sensor,
        					sm.getDefaultSensor(Sensor.TYPE_GYROSCOPE),
        					SensorManager.SENSOR_DELAY_UI);
        
        device_id = ODIN.getODIN1(getApplicationContext());
        
        IonLib.SetDeviceVendor(android.os.Build.MANUFACTURER);
        IonLib.SetDeviceName(android.os.Build.MODEL);
        IonLib.SetDeviceID(device_id);
        
        if (getPackageManager().checkPermission("android.permission.READ_PHONE_STATE", getPackageName()) == 0)
        {
        	TelephonyManager tm = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
        
        	if (tm.getSimOperatorName() != null)
        	{
        		IonLib.SetSimOperatorName( tm.getSimOperatorName() );
        	}               	
        }                
        
        {
        	SharedPreferences prefs = getSharedPreferences(getPackageName(), MODE_PRIVATE);
        			        	
        	String restoredText = prefs.getString("VK_token", null);
        
        	if (restoredText != null) 
        	{
        		IonVKLoginActivity.token = restoredText;
        		IonVKLoginActivity.user_id = prefs.getString("VK_user_id", null);
        		
        		Log.e(TAG, "sfaf " + IonVKLoginActivity.token + " " + IonVKLoginActivity.user_id);
        	}
        }
             
        locationListiner = new IonLocationListener();

		if (!facebook_app_id.equals(""))
		{
			FacebookSdk.sdkInitialize(this.getApplicationContext());
			FacebookSdk.setApplicationId(facebook_app_id);

			callbackManager = CallbackManager.Factory.create();

			LoginManager.getInstance().registerCallback(callbackManager,
					new FacebookCallback<LoginResult>() {
						@Override
						public void onSuccess(LoginResult loginResult) {
							IonLib.SetSignInStatusFB(1, AccessToken.getCurrentAccessToken().getToken());
						}

						@Override
						public void onCancel() {
							Log.e(TAG, "FB cancel");
							IonLib.SetSignInStatusFB(0, "cancel");
						}

						@Override
						public void onError(FacebookException exception) {
							Log.e(TAG, "FB error - " + exception.getMessage());
							IonLib.SetSignInStatusFB(0, "error");
						}
					});
		}
	}

	public void ReadModules(String module_name, Bundle metaData)
    {
    	
    }

	@Override
	public boolean onPrepareOptionsMenu (Menu menu)
	{
		return false;
	}

    @Override
    protected void onDestroy()
    {
		super.onDestroy();

		if (downloader != null) downloader.OnDestroy();
		BillingController.unregisterObserver(billingObserver);
    }
    
    @Override
	protected void onPause()
	{
		super.onPause();
        
        if (downloader != null)
        {
        	if ( downloader.enoughSpace == 0 )
        	{
        		android.os.Process.killProcess(android.os.Process.myPid());
        	}
        }
        
        //registerPushWooshReceivers();                
        
        if (ion_view != null)
        {
        	((InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE)).hideSoftInputFromWindow(ion_view.getWindowToken(), 0);
			ion_view.onPause();
        }               
    }

    @Override protected void onResume()
	{
        super.onResume();
        if (ion_view != null) ion_view.onResume();
        
        if (locationListiner != null)
		{
			LocationManager locationManager = (LocationManager)getSystemService(Context.LOCATION_SERVICE);
			locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 5000, 10, locationListiner);
		}                
    }

	Rect rect_sk = new Rect();

	public void GetSoftKeyboardHeight()
	{
		//runOnUiThread(new Runnable() {
		//	public void run()
			{
				ion_view.getWindowVisibleDisplayFrame(rect_sk);

				IonLib.SetSoftKeyboardHeight(rect_sk.bottom - rect_sk.top);

			}
		//});
	}
	
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event)
	{
		Log.d("ion", "Fired onKeyDown " + keyCode);

	    if ((keyCode == KeyEvent.KEYCODE_BACK))
	    {
	    	IonLib.SetBackBtnStateOn();
	    	
	    	return true;
	    }
	    
	    return super.onKeyDown(keyCode, event);
	}

	@Override
    protected void onStart()
    {		
		if (downloader != null) downloader.OnStart();
		super.onStart();

		IonLib.MobileSetOnPause(0);
    }

	@Override
    protected void onStop()
    {	
		if (downloader != null)
		{
			downloader.OnStop();	
		}
		
		if (ion_view != null)
		{
			((InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE)).hideSoftInputFromWindow(ion_view.getWindowToken(), 0);
		}

		if (locationListiner != null)
		{
			LocationManager locationManager = (LocationManager)getSystemService(Context.LOCATION_SERVICE);
			locationManager.removeUpdates(locationListiner);
		}
		
		IonLib.MobileSetOnPause(1);
		
		super.onStop();			
    }

	public void SetStopURL(String url)
	{
		IonWebViewActivity.stop_url = url;
	}

	public void OpenURLInner(String url)
	{
		IonWebViewActivity.url = url;
		
		Intent intent = new Intent();
		intent.setClass(this, IonWebViewActivity.class);										
		startActivity(intent);		
	}
	
	public void OpenVideo(String url)
	{
		int lastI = 0;
		
		IonVideoViewActivity.quallity_list.clear();
		
		for (int i = 0 ; i < 10; ++i)
		{
			if (url.indexOf("quallity"+ i +"_") >= 0)
			{
				int i1 = url.indexOf("_", lastI);
				int i2 = url.indexOf("=", lastI);
				String qual = url.substring(i1+1, i2);
				i1 = i2;
				i2 = url.indexOf(";", lastI);
				String url_for_qual = url.substring(i1+1, i2);
				IonVideoViewActivity.quallity_list.add(new Pair(qual, url_for_qual));
				lastI = i2+1;
			}
			else
				break;
		}


		if (IonVideoViewActivity.quallity_list.size() == 0)
		{
		int i = url.indexOf(";");
		if (i == 0)
		{
			Log.e("ION", "OpenVideo i == 0 error: ");
			return;
		}
		IonVideoViewActivity.url = url.substring(0, i);
		IonVideoViewActivity.showName = url.substring(i+1, url.length());
		}
		else
		{
			IonVideoViewActivity.url = IonVideoViewActivity.quallity_list.get(0).url;
			IonVideoViewActivity.showName = url.substring(lastI, url.length());
		}
		Log.v("ION", "OpenVideo url= " + IonVideoViewActivity.url + " showName=" + IonVideoViewActivity.showName);

		IonVideoViewActivity.lastProgress = 0; 
		Intent intent = new Intent();
		intent.setClass(this, IonVideoViewActivity.class);			
		try 
		{
			startActivity(intent);
		}
		catch(Exception e)
		{
			Log.e("ION", "OpenVideo startActivity error: "+e.toString());
		}
	}
	
	public void OpenURL(String url)
	{							
		Intent intent = new Intent(Intent.ACTION_VIEW); 
		intent.setData(Uri.parse(url)); 
		startActivity(intent);
	}
	
	public void CallPhone(String tel)
	{		
		Intent intent = new Intent();
		intent.setAction(Intent.ACTION_CALL);
		intent.setData(Uri.parse("tel:" + tel)); // set the Uri
		startActivity(intent);
	}
	
	public void SendMail(String mail_data)
	{		
		String mail_to = "";
		String mail_subject = "";
		String mail_text = "";
		
		int index = 0;
		int last_index = 0;
		
		for (int i=0; i<mail_data.length() - 1; i++)
		{
			if (mail_data.charAt(i)== '|' && mail_data.charAt(i + 1) == '<')
			{			
				if (index == 0)
				{
					mail_to = mail_data.substring(last_index, i);
				}
				else
				if (index == 1)
				{
					mail_subject = mail_data.substring(last_index, i);					
				
					last_index = i+2;
					
					mail_text = mail_data.substring(last_index);
					break;
				}
									
				index++;
				last_index = i+2;				
			}			
		}
		
		String[] recipients = new String[]{mail_to};
		
		
		Intent testIntent = new Intent(android.content.Intent.ACTION_SEND);  
		testIntent.setType("message/rfc822");  
		testIntent.putExtra(android.content.Intent.EXTRA_SUBJECT, mail_subject);  
		testIntent.putExtra(android.content.Intent.EXTRA_TEXT, mail_text);  
		testIntent.putExtra(android.content.Intent.EXTRA_EMAIL, recipients);  
		startActivity(testIntent);  
	}		
	
	public void TrackUserAge(String age)
 	{	
		Log.e(TAG, "Tracking System not implemented");			
    }
	
	public void TrackUserGender(String gender)
 	{	
		Log.e(TAG, "Tracking System not implemented");			
    }
	
	public void AddEventParamName(String name)
 	{	
		Log.e(TAG, "Tracking System not implemented");			
    }
	
	public void AddEventParamValue(String value)
 	{	
		Log.e(TAG, "Tracking System not implemented");			
    }
	
	public void TrackEvent(String event_name)
 	{	
		Log.e(TAG, "Tracking System not implemented");			
    } 
	
	public void SetClipboardText(String text)
	{
		android.text.ClipboardManager clipboard = (android.text.ClipboardManager)getSystemService(CLIPBOARD_SERVICE); 
		clipboard.setText(text);
	}
	
	String ClipboardText_data;
	public void GetClipboardText()
	{
		runOnUiThread(new Runnable()
		{
			public void run()
			{
			    android.text.ClipboardManager clipboard = (android.text.ClipboardManager)getSystemService(CLIPBOARD_SERVICE); 
			    ClipboardText_data = (String) clipboard.getText();
				IonLib.PasteFromClipboard(ClipboardText_data);
			}
		});
	}
	
	public void IsClipboardEmpty()
	{
		runOnUiThread(new Runnable() {
			public void run() {
				android.text.ClipboardManager clipboard = (android.text.ClipboardManager) getSystemService(CLIPBOARD_SERVICE);
				if (clipboard.hasText())
					IonLib.OnRequestIsClipboardEmpty(0);
				else
					IonLib.OnRequestIsClipboardEmpty(1);


			}
		});
	}

	String ShowKeyboardUI_data;
	
	public void ShowKeyboardUI()
	{		
		Log.e(TAG, "ShowKeyboardUI");
		
		String[] strings = ShowKeyboardUI_data.split(";", 2);
		
		if (!cur_kb_type.equals(strings[0]) || !cur_rk_type.equals(strings[1]))
		{			
			RemoveInputView();		
		}
		
		cur_kb_type = strings[0];
		cur_rk_type = strings[1];							
		
		InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
		
		if (mInputView == null)
		{				
			((InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE)).hideSoftInputFromWindow(ion_view.getWindowToken(), 0);
			
			mInputView = new IonInputView(this);		
			addContentView(mInputView, new ViewGroup.LayoutParams(1, 1));
			
			Log.e(TAG, "addContentView");
		}		
		
		mInputView.setFocusableInTouchMode(true);
		mInputView.setFocusable(true);
		mInputView.requestFocus();
	    
		imm.showSoftInput(mInputView, InputMethodManager.SHOW_IMPLICIT);		
	}
	
	public void ShowKeyboard(String data)
	{
		ShowKeyboardUI_data = data;
		
		runOnUiThread(new Runnable() {
			public void run() {
				IonActivity.activity.ShowKeyboardUI();
			}
		});						
	}

	public void HideKeyboard()
	{
		//Log.e(TAG, "HideKeyboard");
				
		((InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE)).hideSoftInputFromWindow(ion_view.getWindowToken(), 0);
	}

	/*@Override
	public boolean dispatchKeyEvent(KeyEvent event)
	{
		Log.e(TAG, "dispatchKeyEvent");

		return super.dispatchKeyEvent(event);
	}*/

	public void RemoveInputView()
	{
		Log.e(TAG, "RemoveInputView");	
		
		if (mInputView == null)
		{
			return;
		}
		
		View youAddedView;
		ViewGroup rootView = (ViewGroup) findViewById(android.R.id.content);
		
		for (int i = 0; i < rootView.getChildCount(); i++)
		{
		    if(rootView.getChildAt(i) == mInputView)
		    {
		    	rootView.removeViewAt(i);
		    	break; 
		    }
		}
		
		mInputView = null;
	}
	
	public void CheckInternet()
	{
		ConnectivityManager connManager = (ConnectivityManager) getSystemService(CONNECTIVITY_SERVICE);
				
		int res = 1;

		/*if (connManager.isDefaultNetworkActive())
		{
			res = 1;
		}*/

		NetworkInfo net_info = connManager.getActiveNetworkInfo();
		
		if (net_info != null)
		{			
			if (!net_info.isConnected())
			{
				res = 0;				
			}
			
			if (!net_info.isAvailable())
			{
				res = 0;				
			}
		}
		else
		{
			res = 0;			
		}

		IonLib.SetInternetConnected(res);
	}	
	
	public void StartENgine()
	{
		ion_view = new IonView(this);
		ion_view.setPreserveEGLContextOnPause(true);
        setContentView(ion_view);
	}

	public void EnableCamera(String tex_id)
	{
		int id = Integer.parseInt( tex_id );

		ion_view.renderer.EnableCamera(id);
	}

	public void ShowMapUI()
	{
		//mMap = (SupportMapFragment) getSupportFragmentManager().findFragmentByTag("map");

        // We only create a fragment if it doesn't already exist.
        /*if (mMap == null)
        {
        	// btn_map = new Button(this);

        	//FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.FILL_PARENT,
				//	   FrameLayout.LayoutParams.FILL_PARENT);

        	//btn_map.setLayoutParams(lp);
        	//frameLayout.addView(btn_map);
        		
        	//addContentView(btn_map, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT));
        	
        	mMap = new MapView(this);
        	
        	mMap.setComponents(new Components());
        	
        	RasterDataSource dataSource = new HTTPRasterDataSource(new EPSG3857(), 0, 18, "http://otile1.mqcdn.com/tiles/1.0.0/osm/{zoom}/{x}/{y}.png");
            RasterLayer mapLayer = new RasterLayer(dataSource, 0);
            mMap.getLayers().setBaseLayer(mapLayer);
            
            addContentView(mMap, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT));
            
            //mMap.bringToFront();
            //mMap.
        
            
            
            // To programmatically add the map, we first create a SupportMapFragment.
        	//mMap = SupportMapFragment.newInstance();

            // Then we add it using a FragmentTransaction.
            //FragmentTransaction fragmentTransaction = getSupportFragmentManager().beginTransaction();
            //fragmentTransaction.add(android.R.id.content, mMap, "map");
            //fragmentTransaction.commit();  
                        
        }
        
        //if (mMap == null || mMap.getView() == null) return;        
        //mMap.getView().setVisibility(0);    
        
        mMap.setVisibility(0);*/
	}
	
	public void ShowMap()
	{										
		runOnUiThread(new Runnable()
		{
			public void run()
			{
				IonActivity.activity.ShowMapUI();							
			}
		});
	}
	
	
	String SetMapPosition_data;
		
	int map_prev_x = -1;
	int map_prev_y = -1;
	int map_prev_w = -1;
	int map_prev_h = -1;
					
	public void SetMapPositionUI()
	{	
		//if (mMap == null || mMap.getView() == null) return;		
		/*if (mMap == null) return;
								
		String[] strings = SetMapPosition_data.split(";", 4);
		
		int x = Integer.parseInt( strings[0] );
		int y = Integer.parseInt( strings[1] );
		int w = Integer.parseInt( strings[2] );
		int h = Integer.parseInt( strings[3] );		
    	
		if (map_prev_x == x && map_prev_y == y &&
			map_prev_w == w && map_prev_h == h)
		{
			//mMap.getView().setVisibility(0);
			//mMap.setVisibility(0);
		}
		else
		{				
			//mMap.getView().setVisibility(4);
			//mMap.setVisibility(4);
			
			FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(w, h);
		
			//lp.leftMargin = x;
			//lp.topMargin = y + 300;
									
			//btn_map.setLayoutParams(lp);
			//btn_map.requestLayout();
			
			lp.leftMargin = x;
			lp.topMargin = y;
			
			//frameLayout.updateViewLayout(mMap.getView(), lp);
			//mMap.getView().setLayoutParams(lp);
			//mMap.getView().requestLayout();		
			
			mMap.setLayoutParams(lp);
			mMap.requestLayout();
			
			map_prev_x = x;
			map_prev_y = y;
			map_prev_w = w;
			map_prev_h = h;
		}*/			
	}
	
	public void SetMapPosition(String data)
	{			
		SetMapPosition_data = data;
		
		runOnUiThread(new Runnable()
		{
			public void run()
			{
				IonActivity.activity.SetMapPositionUI();
			}
		});
	}
		
	String AddObjectToMap_data;
	
	public void AddObjectToMapUI()
	{			
		Resources res = getResources();
		
		String[] strings = AddObjectToMap_data.split(";", 3);
		
		float x = Float.parseFloat( strings[0] );
		float y = Float.parseFloat( strings[1] );				        

		//mMap.getMap().moveCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(x, y), 18));
		 
		//mMap.getMap().addMarker(new MarkerOptions().position(new LatLng(x, y)).title(strings[2]));
		
		/*Overlay overlay = new Overlay(mMap.getMapController());		
		
		final OverlayItem obj = new OverlayItem(new GeoPoint(x , y), res.getDrawable(R.drawable.pointer));
 
		BalloonItem balloonKremlin = new BalloonItem(this, obj.getGeoPoint());
		balloonKremlin.setText(strings[2]);
		
		obj.setBalloonItem(balloonKremlin);		
		overlay.addOverlayItem(obj);
		
		mMap.getMapController().getOverlayManager().addOverlay(overlay);*/
	}
	
	public void AddObjectToMap(String data)
	{
		//if (mMap == null) return;
		
		AddObjectToMap_data = data;
		
		runOnUiThread(new Runnable()
		{
			public void run()
			{
				IonActivity.activity.AddObjectToMapUI();
			}
		});
	}

	public void GetBatteryLevel()
	{
		IntentFilter ifilter = new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
		Intent batteryStatus = getApplicationContext().registerReceiver(null, ifilter);

		int level = batteryStatus.getIntExtra(BatteryManager.EXTRA_LEVEL, -1);
		int scale = batteryStatus.getIntExtra(BatteryManager.EXTRA_SCALE, -1);

		float batteryPct = level / (float)scale * 100.0f;

		IonLib.SetBatteryLevel(batteryPct);
	}

	public void GetBatterySatus()
	{
		IntentFilter ifilter = new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
		Intent batteryStatus = getApplicationContext().registerReceiver(null, ifilter);

		int status = batteryStatus.getIntExtra(BatteryManager.EXTRA_STATUS, -1);

		int res_status = 0;

		if (status == BatteryManager.BATTERY_STATUS_NOT_CHARGING)
		{
			res_status = 1;
		}

		if (status == BatteryManager.BATTERY_STATUS_CHARGING)
		{
			res_status = 2;
		}

		if (status == BatteryManager.BATTERY_STATUS_FULL)
		{
			res_status = 3;
		}

		IonLib.SetBatterySatus(res_status);
	}

	public void DeleteAllMapObjectsUI()
	{
		//if (mMap == null) return;
		
		//mMap.getMap().clear();
		
		/*List<Overlay> overlays = mMap.getMapController().getOverlayManager().getOverlays(); 
		
		for (int i=0; i<overlays.size(); i++)
		{
			mMap.getMapController().getOverlayManager().removeOverlay(overlays.get(i));
		}*/
	}		
	
	public void DeleteAllMapObjects()
	{
		//if (mMap != null) return;
						
		runOnUiThread(new Runnable()
		{
			public void run()
			{
				IonActivity.activity.DeleteAllMapObjectsUI();							
			}
		});
	}
	
	public void HideMapUI()
	{	
		//if (mMap == null || mMap.getView() == null) return;
		//if (mMap == null) return;
						
		//Log.e(TAG, "HideMapUI");
		//mMap.getView().setVisibility(4);
		//mMap.setVisibility(4);
		
		//mMap.addMarker(new MarkerOptions().position(new LatLng(0, 0)).title("Marker"));
		
		//frameLayout.removeView(mMap);
		//mMap = null;		
	}
	
	public void HideMap()
	{
		//if (mMap == null) return;		
						
		runOnUiThread(new Runnable()
		{
			public void run()
			{
				IonActivity.activity.HideMapUI();							
			}
		});
	}
	
	String ShowNativeMap_data;
	
	public void ShowNativeMapUI()
	{	
		String[] strings = ShowNativeMap_data.split(";", 3);
		
		float x = Float.parseFloat(strings[0]);
		float y = Float.parseFloat( strings[1] );
		
		IonMapActivity.x = x;
		IonMapActivity.y = y;
		IonMapActivity.desc = strings[2];
		
		Intent intent = new Intent();
		intent.setClass(this, IonMapActivity.class);
		startActivity(intent);
	}
	
	public void ShowNativeMap(String data)
	{			
		ShowNativeMap_data = data;
		
		runOnUiThread(new Runnable()
		{
			public void run()
			{
				IonActivity.activity.ShowNativeMapUI();
			}
		});
	}
	
	String ShowPopup_data;
	
	public void ShowPopupUI()
	{		
		String[] strings = ShowPopup_data.split(";", 3);			
		
		AlertDialog.Builder dialog = new AlertDialog.Builder(this);
		
		dialog.setTitle(strings[0]);
		
		dialog.setMessage(strings[1]);
		
		DialogInterface.OnClickListener listiner = new DialogInterface.OnClickListener() {
	        public void onClick(DialogInterface dialog, int which)
	        { 
	            IonLib.SetPopupResult(1);
	        }};
	        
	    dialog.setCancelable(false);
		dialog.setPositiveButton(strings[2], listiner);					   
		
		dialog.show();							
	}
	
	public void ShowPopup(String data)
	{	
		ShowPopup_data = data;
		
		runOnUiThread(new Runnable() {
			public void run() {
				IonActivity.activity.ShowPopupUI();
			}
		});
	}

	String ShowPopup2_data;
	
	public void ShowPopup2UI()
	{			
		String[] strings = ShowPopup2_data.split(";", 4);
						
		AlertDialog.Builder dialog = new AlertDialog.Builder(this);
		
		dialog.setTitle(strings[0]);
		
		dialog.setMessage(strings[1]);
		
		DialogInterface.OnClickListener listiner = new DialogInterface.OnClickListener() {
	        public void onClick(DialogInterface dialog, int which)
	        { 
	            IonLib.SetPopupResult(1);
	        }};
	        
		dialog.setPositiveButton(strings[2], listiner);
			
		DialogInterface.OnClickListener listiner2 = new DialogInterface.OnClickListener() {
	        public void onClick(DialogInterface dialog, int which)
	        { 
	            IonLib.SetPopupResult(2);
	        }};
	       
	    dialog.setCancelable(false);
	    dialog.setNegativeButton(strings[3], listiner2);	        	   
		
	    dialog.show();	
	}	
	
	public void ShowPopup2(String data)
	{	
		ShowPopup2_data = data;
		
		runOnUiThread(new Runnable()
		{
			public void run()
			{
				IonActivity.activity.ShowPopup2UI();							
			}
		});				
	}	
	
	public void InitDownloader()
	{
		downloader = new IonDownloader();
        
        setContentView(R.layout.downloader);
        
        UIHolder holder = new UIHolder();
        
        holder.progressBar = (ProgressBar) findViewById(R.id.progressBar);
        holder.statusText = (TextView) findViewById(R.id.statusText);
        holder.progressFraction = (TextView) findViewById(R.id.progressAsFraction);
        holder.progressPercent = (TextView) findViewById(R.id.progressAsPercentage);
        holder.averageSpeed = (TextView) findViewById(R.id.progressAverageSpeed);
        holder.timeRemaining = (TextView) findViewById(R.id.progressTimeRemaining);
        holder.dashboard = findViewById(R.id.downloaderDashboard);
        holder.cellMessage = findViewById(R.id.approveCellular);
        holder.pauseButton = (Button) findViewById(R.id.pauseButton);
        holder.WiFiSettingsButton = (Button) findViewById(R.id.wifiSettingsButton);
        holder.resumeOnCell = (Button) findViewById(R.id.resumeOverCellular);               
                        
        holder.not_enough_space_on_card = getString(R.string.not_enough_space_on_card);	
        holder.kilobytes_per_second = R.string.kilobytes_per_second;
        holder.time_remaining = R.string.time_remaining;		
		 
        downloader.Init(holder, this, IonDownloaderService.class, GetVersion(), GetExtSize());	
	}
	
	public boolean ReadFromApk()
	{
		return false;		
	}	
	
	public boolean UseDownloader()
	{
		return false;	
	}
	
	public int GetVersion()
	{
		return 1;	
	}
	
	public int GetExtSize()
	{
		return 1;	
	}
	
	public String GetPublicKey()
    {
        return "your public key";
    }
	
	private final byte[] salt = new byte[] { 14, 52, -31, -14, 49, 38, -100, -32, 12, 90, -11, -54, 93, 15, -63, -18, -32, 53, -124, 34 };
	
    public byte[] GetSALT()
    {
        return salt;
    }
    
    public void FinishActivity()
	{
    	runOnUiThread(new Runnable() {
			public void run() {
				IonActivity.this.finish();
			}
		});
	}

	public void OpenOKSession()
	{
		Log.e("ION", "OK LogIn");
		
		/*Intent intent = new Intent();
        intent.setClass(this, LoginActivity.class);
        startActivityForResult(intent, 1);*/       	
    }  
    
    public void OpenVKSession()
	{    	
		Log.e("ION", "VK LogIn");
		
		if (!IonVKLoginActivity.token.isEmpty())
		{			
			IonLib.SetSignInStatusVK(1, IonVKLoginActivity.token, IonVKLoginActivity.user_id);
		} else if (!IonVKLoginActivity.API_ID.equals(""))
		{	
			Intent intent = new Intent();
			intent.setClass(this, IonVKLoginActivity.class);
			startActivityForResult(intent, REQUEST_LOGIN);
		}
		else
		{
			Log.e(TAG, "VK app id not setted");
			IonLib.SetSignInStatusVK(0, "error", "noid");
		}
    }      
    
    public void OpenInstagramSession()
	{    	
		Log.e("ION", "Instagram LogIn");

		if (!IonInstagramLoginActivity.token.isEmpty())
		{			
			IonLib.SetSignInStatusInstagram(1, IonInstagramLoginActivity.token);
		}
		else				
		{	
			Intent intent = new Intent();
			intent.setClass(this, IonInstagramLoginActivity.class);
			startActivityForResult(intent, REQUEST_LOGIN_INST);
		}		
    }      
    
    public void SelectImage()
    {
		Intent pickPicIntent = new Intent();
		pickPicIntent.setType("image/*");
		pickPicIntent.setAction(Intent.ACTION_GET_CONTENT);
		startActivityForResult(pickPicIntent, SELECT_IMAGE);
    }
    
    public void SelectPhoto()
    {
    	Intent cameraIntent = new Intent(android.provider.MediaStore.ACTION_IMAGE_CAPTURE); 
    	startActivityForResult(cameraIntent, SELECT_PHOTO);
	}

	public void SelectImagePhoto() {
		runOnUiThread(new Runnable() {
			public void run() {

				final CharSequence[] items = {"Take Photo", "Choose from Library", "Cancel"};

				AlertDialog.Builder builder = new AlertDialog.Builder(IonActivity.this);
				builder.setTitle("Select Photo!");

				builder.setItems(items, new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int item) {
						if (items[item].equals("Take Photo")) {
							SelectPhoto();
						} else if (items[item].equals("Choose from Library")) {
							SelectImage();
						} else if (items[item].equals("Cancel")) {
							IonLib.SetSelectImageResult(1, "");
							dialog.dismiss();
						}
					}
				});

				builder.show();
			}
		});
    	}
    
    private void parseUrl(String url)
    {
        try
        {
            if(url==null) return;
            
            Log.i(TAG, "url="+url);
            
            /*if(url.startsWith(Auth.redirect_url))
            {
                if(!url.contains("error="))
                {
                    String[] auth=Auth.parseRedirectUrl(url);
                    Intent intent=new Intent();
                    intent.putExtra("token", auth[0]);
                    intent.putExtra("user_id", Long.parseLong(auth[1]));
                    setResult(Activity.RESULT_OK, intent);
                }
                
                finish();
            }*/
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }
    
    public void OpenFacebookSession()
	{		    									
		if (!facebook_app_id.equals(""))
		{
			if (AccessToken.getCurrentAccessToken() == null) {
				List lnkLst = new ArrayList();
				lnkLst.add("public_profile");

				LoginManager.getInstance().logInWithReadPermissions(IonActivity.this, lnkLst);
			}
			else
			{
				IonLib.SetSignInStatusFB(1, AccessToken.getCurrentAccessToken().getToken());
			}
		}
		else
		{
			Log.e(TAG, "Facebook app id not setted");
			IonLib.SetSignInStatusFB(0, "error");
		}
	}            
    
    public void CloseFacebookSession()
	{
    	runOnUiThread(new Runnable() {
			public void run() {
				LoginManager.getInstance().logOut();
			}
		});
	}

	public void ClearWebViewCache()
	{
    	Log.e(TAG, "ClearWebViewCache");
    	
    	CookieManager.getInstance().removeAllCookie();
    	CookieSyncManager.getInstance().sync();	
    	    	        
    	IonVKLoginActivity.token = "";
    	
    	SharedPreferences prefs = getSharedPreferences(getPackageName(), MODE_PRIVATE);

		SharedPreferences.Editor editor = prefs.edit();;
        
        editor.putString("VK_token", "");                    
        editor.putString("VK_user_id", "");
        editor.commit();
	}

	public void SendSms(String adressAndText)
    { 
    	 String SENT = "SMS_SENT";

    	 PendingIntent sentPI = PendingIntent.getBroadcast(this, 0, new Intent(SENT), 0);

   	     // ---when the SMS has been sent---
    	 BroadcastReceiver sentSMSStatusReciever = new BroadcastReceiver()
    	 {
    		 @Override
    	     public void onReceive(Context arg0, Intent arg1)
    	     {
    			 int res = getResultCode();
    	         
    			 switch (res) 
    	         {
    			 	case RESULT_OK:
    	            	Log.v("ION", "SendSms RESULT_OK");
    	            	break;
    	            case SmsManager.RESULT_ERROR_GENERIC_FAILURE:
    	            	Log.e("ION", "SendSms RESULT_ERROR_GENERIC_FAILURE");
    	            	break;
    	            case SmsManager.RESULT_ERROR_RADIO_OFF:
    	            	Log.e("ION", "SendSms RESULT_ERROR_RADIO_OFF");
    	            	break;
    	            case SmsManager.RESULT_ERROR_NULL_PDU:
    	            	Log.e("ION", "SendSms RESULT_ERROR_NULL_PDU");
    	            	break;
    	            case SmsManager.RESULT_ERROR_NO_SERVICE:
    	            	Log.e("ION", "SendSms RESULT_ERROR_NO_SERVICE");
    	            	break;
    	   
    	            default:
    	            
    	            Log.e("ION", "SendSms unknown code="+res);  	          
    	         }
    	            
    			 unregisterReceiver(this);
    	            
    	         if (res == RESULT_OK)
    	         {
    	        	 IonLib.SetSendSmsResult(0);
    	         }
    	         else
    	         {
    	        	 IonLib.SetSendSmsResult(-1);
    	         }
    	     }
    	};
    	
    	registerReceiver(sentSMSStatusReciever, new IntentFilter(SENT));

    	SmsManager smsManager = SmsManager.getDefault();

    	String[] strings = adressAndText.split(";", 2);
    	String number = strings[0];
    	String text = strings[1];

		//number = number.replace("+", "%2B");
    	    
    	Log.e("ION", "SMS TO " + number);
    	
		try
		{
			
			Intent intent = new Intent( Intent.ACTION_VIEW, Uri.parse( "sms:" + number ) );
			intent.putExtra( "sms_body", text );
			intent.putExtra("exit_on_sent", true);
			
			startActivity( intent );
			IonLib.SetSendSmsResult(0);
			
			//smsManager.sendTextMessage(number, null, text, sentPI, null);
		} 		
		catch (Exception e)
		{
			Log.e("ION",e.getMessage());
			IonLib.SetSendSmsResult(-1);
		}
    }
    
    public void StartShareText(String text)
    {
    	Intent sharingIntent = new Intent(android.content.Intent.ACTION_SEND); 
    	sharingIntent.setType("text/plain");    	
    	sharingIntent.putExtra(android.content.Intent.EXTRA_TEXT, text);    	
    	startActivity(Intent.createChooser(sharingIntent, "Share via"));        
    }
    
    public void StartShareImage(String image)
    {
    	File sd_file = Environment.getExternalStorageDirectory();
		
		String sd_path = sd_file.getAbsolutePath() + "/Android/data/" + getPackageName() + "/" + image;		
		
    	Intent share = new Intent(Intent.ACTION_SEND);
    	share.setType("image/*");
    	 
    	share.putExtra(Intent.EXTRA_STREAM, Uri.parse("file://" + sd_path));
    	 
    	startActivity(Intent.createChooser(share, "Share via"));
	}
    
    public void AddMediaPlayer(String opgltexid_and_path)
    { 
    	String str = opgltexid_and_path;
    	int index = str.indexOf(";");
    	String opgltexid = str.substring(0, index);
    	String path = str.substring(index+1, str.length());
    	path = "rtsp://v4.cache3.c.youtube.com/CjYLENy73wIaLQlW_ji2apr6AxMYDSANFEIJbXYtZ29vZ2xlSARSBXdhdGNoYOr_86Xm06e5UAw=/0/0/0/video.3gp";
    	int id = Integer.parseInt(opgltexid);
    	//mView.AddMediaPlayer(id, path);

	}

	public void RemoveMediaPlayer(String opgltexid)
    { 
    	int id = Integer.parseInt(opgltexid);
    	//mView.RemoveMediaPlayer(id);

    }

	public void StartMediaPlayer(String opgltexid)
    { 
    	int id = Integer.parseInt(opgltexid);
    	//mView.StartMediaPlayer(id);

    }

	public void PauseMediaPlayer(String opgltexid)
    { 
    	int id = Integer.parseInt(opgltexid);
    	//mView.PauseMediaPlayer(id);
    }    
    
    public void CallNumber(String number)
    { 
    	try 
		{
    		Intent callIntent = new Intent(Intent.ACTION_CALL);
		    callIntent.setData(Uri.parse("tel:"+number));
		    startActivity(callIntent);
		} 
		catch (ActivityNotFoundException e)
		{
			Log.e("helloandroid dialing example", "Call failed", e);
		}
    }
    
	public void ReadContacts()
   	{
    	Log.v("ION", "called ReadContacts");

		TelephonyManager tm = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
        
        if ( tm == null ||  tm.getPhoneType() == TelephonyManager.PHONE_TYPE_NONE)
        {
    		Log.e("ION", "Error ReadContacts" );
    		IonLib.SetContactsCount(0);
    		return;
        }
        
        runOnUiThread(new Runnable() {
			public void run() {
				ContentResolver cr = getContentResolver();

				Log.e("ION", "ReadContacts started");

				contact_reader = cr.query(ContactsContract.CommonDataKinds.Phone.CONTENT_URI,
						new String[]{Phone._ID,
								Phone.DISPLAY_NAME,
								Phone.DATA1,
								Phone.DATA2,
								Phone.PHOTO_ID}, null, null, Phone.DISPLAY_NAME);

				Log.e("ION", "ReadContacts Completed");

				if (contact_reader == null) {
					Log.e("ION", "Error ReadContacts");
					IonLib.SetContactsCount(0);
					return;
				}

				Log.e("ION", "ReadContacts Completed " + contact_reader.getCount());

				startManagingCursor(contact_reader);

				IonLib.SetContactsCount(contact_reader.getCount());

				contact_reader.moveToFirst();

				for (int i = 0; i < contact_reader.getCount(); i++) {
					String strType;
					if (Phone.TYPE_HOME == contact_reader.getInt(3)) {
						strType = "HOME";
					} else if (Phone.TYPE_MOBILE == contact_reader.getInt(3)) {
						strType = "MOBILE";
					} else if (Phone.TYPE_WORK == contact_reader.getInt(3)) {
						strType = "WORK";
					} else {
						strType = "OTHER";
					}

					String number = contact_reader.getString(2);
					String fnumber_tmp = PhoneNumberUtils.formatNumber(number);

					String prv = fnumber_tmp;

					String fnumber = "";

					for (int k = 0; k < prv.length(); k++) {
						if (fnumber_tmp.charAt(k) == '-' ||
								fnumber_tmp.charAt(k) == '(' ||
								fnumber_tmp.charAt(k) == '}' ||
								fnumber_tmp.charAt(k) == ' ') {
							continue;
						}

						fnumber += fnumber_tmp.charAt(k);
					}

					//Log.e(TAG, prv + " " + fnumber);        	

					IonLib.SetContactData(i, contact_reader.getString(1), strType + "=" + fnumber);

					contact_reader.moveToNext();
				}

				IonLib.FinalizeReadContacts();
			}

			;
		});
	}

	public void ReadAvatars()
   	{
    	contact_reader.moveToFirst();
    	
    	String prev_name = "HopeIsRandome!2!&NeverMatch";
    	
        for (int i =0; i < contact_reader.getCount(); ++i)
        {
        	String filename = "";
        	
        	for (int j =0; j < contact_reader.getColumnCount(); ++j)
            {
        		if (contact_reader.isNull(j))
        		{
        			continue;
        		}
        		
        		if ( j == 4 )
        		{
        			InputStream is = openPhoto(contact_reader.getLong(j));
        			if (is != null)
        			{
        				BitmapFactory bf = new BitmapFactory();
        				Bitmap bmp =  bf.decodeStream(is);        			
        			
        				String sd_path = "";
        			
        				File sd_file = Environment.getExternalStorageDirectory();
        			
        				sd_path = sd_file.getAbsolutePath() + "/Android/data/" + getPackageName() + "/";
        			
        				filename = sd_path + contact_reader.getString(j)+".png";        				
        				
        				try
        				{        				
        					FileOutputStream out = new FileOutputStream(filename);
        					bmp.compress(Bitmap.CompressFormat.PNG, 90, out);
        					out.close();
        					
        					filename = contact_reader.getString(j) + ".png";
        				}
        				catch (Exception e)
        				{
        					filename = "";
        					e.printStackTrace();
        				}        			        			
        			}
        		}
        		
        		String str = contact_reader.getString(j);
            }        	        	     
        	
        	String cur_name = contact_reader.getString(1);
        	
        	if (filename != "" && prev_name != cur_name)
        	{
        		IonLib.SetContactAvatar(cur_name, filename);
        	}
        	
        	prev_name = cur_name;        			
        	contact_reader.moveToNext();
        }
                
        IonLib.FinalizeReadAvatars();        
   	}            
    
    public InputStream openPhoto(long thumbnailId) {
		Uri uri = ContentUris.withAppendedId(ContactsContract.Data.CONTENT_URI, thumbnailId);


		Log.v("ION", "openPhoto photoUri =" + uri);

		Cursor cursor = null;
		try {
			cursor = getContentResolver().query(uri,
					new String[]{ContactsContract.CommonDataKinds.Photo.PHOTO},
					null, null, null);
		} catch (Exception e) {
			Log.e("ION", "openPhoto " + e.getMessage());
		}
		if (cursor == null) {
			return null;
		}
		try {
			if (cursor.moveToFirst()) {
				byte[] data = cursor.getBlob(0);
				if (data != null) {
					return new ByteArrayInputStream(data);
				}
			}
		} finally {
			cursor.close();
		}
		return null;
	}

	Bitmap DownsclaeBitmap(int scale_factor, Bitmap bitmap)
	{
		float wgt = bitmap.getWidth();
		float hgt = bitmap.getHeight();

		if (bitmap.getWidth() > 512 || bitmap.getHeight() > 512)
		{
			wgt = 512;
			hgt = 512;

			if (bitmap.getWidth() > bitmap.getHeight())
			{
				hgt = bitmap.getHeight() * 512 / bitmap.getWidth();
			}
			else
			{
				wgt = bitmap.getWidth() * 512 / bitmap.getHeight();
			}

			float scaleWidth = bitmap.getWidth() / wgt;
			float scaleHeight = bitmap.getHeight() / hgt;

			// CREATE A MATRIX FOR THE MANIPULATION
			Matrix matrix = new Matrix();
			// RESIZE THE BIT MAP
			matrix.postScale(scaleWidth, scaleHeight);

			// "RECREATE" THE NEW BITMAP
			Bitmap resizedBitmap = Bitmap.createScaledBitmap(bitmap, (int)wgt, (int)hgt, false);
			bitmap.recycle();

			return resizedBitmap;
		}

		return bitmap;
	}

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data)
	{    	       	    	
    	if (requestCode == REQUEST_LOGIN_INST)
    	{
    		if (resultCode == RESULT_OK)
            {            
    			String access_token = data.getStringExtra("token");
    			
    			Log.e("ION", "Instagram " + access_token);  
    			
    			IonLib.SetSignInStatusInstagram(1, access_token);  
            }
    		else
    		{
    			IonLib.SetSignInStatusInstagram(0, "error");
    		}
    	}
    	else
    	if (requestCode == REQUEST_LOGIN)
    	{    		    	
            if (resultCode == RESULT_OK)
            {
            	String access_token = data.getStringExtra("token");
                Long user_id=data.getLongExtra("user_id", 0);
                
                Log.e("ION", "VK " + access_token + " " + user_id);                                
                
                IonLib.SetSignInStatusVK(1, access_token, "" + user_id);          
            }
            else
            {
            	IonLib.SetSignInStatusVK(0, "error", "noid");             	
            }                        
        }
    	else
    	if (requestCode == SELECT_IMAGE)
    	{
    		int res = 1;
    		String image = "";
    	
    		if (resultCode == RESULT_OK)
			{
				Uri uri = data.getData();

				Bitmap photo_org = null;

				try
				{
					photo_org = MediaStore.Images.Media.getBitmap(getContentResolver(), uri);
				}
				catch (IOException e)
				{
					e.printStackTrace();
				}

				File sd_file = Environment.getExternalStorageDirectory();
     						
 				image = sd_file.getAbsolutePath() + "/Android/data/" + getPackageName() + "/" + "image" + image_index + ".jpg";
				image_index++;

 				try
 				{
 				 	FileOutputStream out = new FileOutputStream(image);
					Bitmap photo = DownsclaeBitmap(640, photo_org);
 					photo.compress(Bitmap.CompressFormat.JPEG, 90, out);
 					out.close();
					res = 0;
 				}
 				catch (Exception e)
 				{
 				 	image = "";
 					e.printStackTrace();
				}
            }
    		
    		IonLib.SetSelectImageResult(res, image);   	    
    	}
    	else
        if (requestCode == SELECT_PHOTO)
        {
        	int res = 1;
        	String image = "";
        	
        	if (resultCode == RESULT_OK)
            {
        		Bitmap photo_org = (Bitmap) data.getExtras().get("data");

        		File sd_file = Environment.getExternalStorageDirectory();
    						
				image = sd_file.getAbsolutePath() + "/Android/data/" + getPackageName() + "/" + "image" + image_index + ".jpg";
				image_index++;
				
				Log.e(TAG, image);
				
				try
				{        				
					FileOutputStream out = new FileOutputStream(image);
					Bitmap photo = DownsclaeBitmap(640, photo_org);
					photo.compress(Bitmap.CompressFormat.JPEG, 90, out);
					out.close();
					res = 0;
				}
				catch (Exception e)
				{
					image = "";
					e.printStackTrace();
				}				
            }
        		
        	IonLib.SetSelectImageResult(res, image);   	    
        }
    	else
    	{
			super.onActivityResult(requestCode, resultCode, data);

			if (!facebook_app_id.equals(""))
			{
				callbackManager.onActivityResult(requestCode, resultCode, data);
			}
    	}
    }
	
	public void AddEventToCalendar(String params)
	{
		Log.i(TAG, params);
		 
		String str = "";
		String[] parts = new String[12];
				
		int index = 0;
		
		for (int i=0;i<params.length();i++)
		{
			char c = params.charAt(i);
			
			if (c == '|')
			{
				parts[index] = str;						
				
				index++;
				str = "";				
			}
			else
			{
				str += params.charAt(i);
			}	
		}
		
		int day = Integer.parseInt( parts[0] );
		int month = Integer.parseInt( parts[1] ) - 1;
		int year = Integer.parseInt( parts[2] );
		int hour = Integer.parseInt( parts[3] );
		int minute = Integer.parseInt( parts[4] );
		int second = Integer.parseInt( parts[5] );
		
		long tm = 1000 * (second + 60 * minute + 60 * 60 * hour);
		
		Intent calIntent = new Intent(Intent.ACTION_INSERT);
		calIntent.setType("vnd.android.cursor.item/event");
		calIntent.putExtra("title", parts[6]);
		calIntent.putExtra("description", parts[7]);
		calIntent.putExtra("location", parts[8]);
		calIntent.putExtra("allDay", false);		
		GregorianCalendar calDate = new GregorianCalendar(year, month, day);
		calIntent.putExtra(CalendarContract.EXTRA_EVENT_ALL_DAY, true);
		calIntent.putExtra(CalendarContract.EXTRA_EVENT_BEGIN_TIME,
						   calDate.getTimeInMillis() + hour);
		calIntent.putExtra(CalendarContract.EXTRA_EVENT_END_TIME,
						   calDate.getTimeInMillis() + hour);
		startActivity(calIntent);
	}		
	
	@Override public boolean onTouchEvent(MotionEvent event)
    {		
		/*String names[] = { "DOWN" , "UP" , "MOVE" , "CANCEL" , "OUTSIDE" ,
                "POINTER_DOWN" , "POINTER_UP" , "7?" , "8?" , "9?" };
     StringBuilder sb = new StringBuilder();
     int action = event.getAction();
     int actionCode = action & MotionEvent.ACTION_MASK;

     sb.append("OnTouchEvent ACTION_" ).append(names[actionCode]);

     if (actionCode == MotionEvent.ACTION_POINTER_1_DOWN ||
    	 actionCode == MotionEvent.ACTION_POINTER_1_UP)
     {
    	 sb.append("(pid 1" );
     }
     
     if (actionCode == MotionEvent.ACTION_POINTER_2_DOWN ||
        	 actionCode == MotionEvent.ACTION_POINTER_2_UP)
         {
        	 sb.append("(pid 2" );
         }
     
     if (actionCode == MotionEvent.ACTION_POINTER_3_DOWN ||
        	 actionCode == MotionEvent.ACTION_POINTER_3_UP)
         {
        	 sb.append("(pid 3" );
         }
     
     /*
     if (actionCode == MotionEvent.ACTION_POINTER_DOWN
           || actionCode == MotionEvent.ACTION_POINTER_UP) {
        sb.append("(pid " ).append(
        action >> MotionEvent.ACTION_POINTER_ID_SHIFT);
        sb.append(")" );
     }
     
     sb.append("[" );

     for (int i = 0; i < event.getPointerCount(); i++) {
        sb.append("#" ).append(i);
        sb.append("(pid " ).append(event.getPointerId(i));
        sb.append(")=" ).append((int) event.getX(i));
        sb.append("," ).append((int) event.getY(i));
        if (i + 1 < event.getPointerCount())
        sb.append(";" );
     }
     sb.append("]" );

     Log.d("JAVA_DEBUGGING", sb.toString());*/
     
       int action = event.getAction();
       int actionCode = action & MotionEvent.ACTION_MASK;
  
       int index = 0;
       
       if (actionCode == MotionEvent.ACTION_POINTER_1_DOWN ||
    	    	 actionCode == MotionEvent.ACTION_POINTER_1_UP)
       {
    	   index = 1;
       }
    	     
    	     if (actionCode == MotionEvent.ACTION_POINTER_2_DOWN ||
    	        	 actionCode == MotionEvent.ACTION_POINTER_2_UP)
    	         {
    	    	 index = 2;
    	         }
    	     
    	     if (actionCode == MotionEvent.ACTION_POINTER_3_DOWN ||
    	        	 actionCode == MotionEvent.ACTION_POINTER_3_UP)
    	         {
    	    	 index = 3;
    	        }
    	     
       if (actionCode == MotionEvent.ACTION_POINTER_DOWN || actionCode == MotionEvent.ACTION_POINTER_UP)
       {
    	   //index = action >> MotionEvent.ACTION_POINTER_ID_SHIFT;         	   
       }        

       	 switch (actionCode)
         {
           case MotionEvent.ACTION_DOWN:
        	    IonLib.TouchStart(0, (int)(event.getX(0)), (int)(event.getY(0)));        	            	   
        	    
          		//Log.d("JAVA_DEBUGGING", "touch down " + index);
          	break;
          		
           case MotionEvent.ACTION_POINTER_DOWN:
				            					
           		IonLib.TouchStart(index, (int)(event.getX(index)), (int)(event.getY(index)));
           		//Log.d("JAVA_DEBUGGING", "point down " + index);
           			           						
           break;          

           case MotionEvent.ACTION_UP:    
        	   
        	    IonLib.TouchEnd(0);
         		//Log.d("JAVA_DEBUGGING", "touch up " + index);
         	
           break;
         		
           case MotionEvent.ACTION_POINTER_UP:
				           	            				
           		IonLib.TouchEnd(index);         		
           		//Log.d("JAVA_DEBUGGING", "point up " + index);
           	
           break;				 
       }
       
  	   int num = event.getPointerCount();
  		
  	   for (int a = 0; a < num; a++)
		{         		   
			IonLib.TouchMove(event.getPointerId(a), (int)(event.getX(a)), (int)(event.getY(a)));
			//Log.d("JAVA_DEBUGGING", "Move " + event.getPointerId(a) + " " +(int)(event.getX(a)) + " " + (int)(event.getY(a)));
		}
  
     	return true;  		
    }
		
	public void SetPushServerURL(String url)
	{		
	}
		
	public void StartRegisterInPushService()
	{
	}	
}