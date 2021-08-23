
package com.ion.appnow;

import java.util.ArrayList;
import java.util.List;

import net.robotmedia.billing.BillingController;

import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ActivityInfo;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.pm.ResolveInfo;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Parcelable;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gcm.GCMRegistrar;
import com.ion.engine.IonActivity;
import com.ion.engine.IonLib;
import com.ion.packagename.R;

import java.util.HashMap;

public class AppNowActivity extends IonActivity
{
	public static String share_text = "";
	public static String share_link = "";
	public static String share_vk = "";
	
	public static String app_id = "";
	public static String app_secret = "";
	
	AsyncTask<Void, Void, Void> mRegisterTask;
	ConnectionDetector cd;
	  
	GridView gvMain;
	ArrayAdapter<String> adapter;
	  
	List<Intent> targetedShareIntents = new ArrayList<Intent>();
			
	String eventName = "";
	HashMap<String, String> eventParams = new HashMap<String, String>();    	    
    
	@Override
	public boolean ReadFromApk()
	{
		return true;
	}

	private static Intent createShareIntent(String text)
	{
		Intent share = new Intent(android.content.Intent.ACTION_SEND);
	    share.setType("text/plain");
	        
	    if (text != null)
	    {
	    	share.putExtra(Intent.EXTRA_TEXT, text);
	    }        
	        
	    return share;
	}

	private int getPixels(int dipValue)
	{
		DisplayMetrics displayMetrics = new DisplayMetrics();
        WindowManager windowManager = (WindowManager)getSystemService(Context.WINDOW_SERVICE);
        windowManager.getDefaultDisplay().getMetrics(displayMetrics);
        
		//int px = getResources().getDimensionPixelSize(dipValue);
		
		//Resources r = getResources();
        //int px = (int)TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, dipValue, 
        //r.getDisplayMetrics());
		
        return (int)((float)dipValue * displayMetrics.density);
	}

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

	public class AlertCancelListener implements DialogInterface.OnCancelListener
	{
		public void onCancel(DialogInterface dialog)
		{
			IonLib.SetShareRes("failed");
		};
	}

	AlertCancelListener alertCancelListener = new AlertCancelListener();

	public class ImageAdapter extends BaseAdapter
	{
	    private Context mContext;
	    private LayoutInflater mInflater;

	    public List<ActivityInfo> activities = new ArrayList<ActivityInfo>();
	    
	    class ViewHolder
	    {  
            TextView title;  
            ImageView icon;
        }
	    
	    public ImageAdapter(Context c)
	    {
	        mContext = c;
	        mInflater = LayoutInflater.from(c);
	        
	        List<ResolveInfo> resInfo = getApplicationContext().getPackageManager().queryIntentActivities(createShareIntent("fasdsf"), 0x00010000);
	        
	        for (int i = 0; i<resInfo.size(); i++)
            {         
	        	ResolveInfo info = resInfo.get(i);        		        	
	        	
                if (/*!info.activityInfo.packageName.contains(".vk") && !info.activityInfo.packageName.contains(".facebook") && */
                	!info.activityInfo.packageName.contains("com.ion") && !info.activityInfo.packageName.contains("appnow"))                		                		
                {                	                
                	activities.add(info.activityInfo);
                }                                
            }		        
	        
	        ApplicationInfo appInfo;
	        
	        try
			{
	        	appInfo = getPackageManager().getApplicationInfo(getPackageName(), 0);
			}
			catch (NameNotFoundException e)
			{
				e.printStackTrace();
				throw new RuntimeException("Unable to locate assets, aborting...");
			}
	        	        
	        ActivityInfo[] list = getPackageManager().getPackageArchiveInfo(appInfo.sourceDir, PackageManager.GET_ACTIVITIES).activities;
	        
	        /*for (int i=0; i<list.length;i++)
	        {	        		        
	        	if (list[i].name.equalsIgnoreCase("com.ion.appnow.VKShareActivity") ||
	        		list[i].name.equalsIgnoreCase("com.ion.appnow.FBShareActivity"))
	        	{
	        		activities.add(list[i]);     			        		
	        	}
	        }*/
	    }

	    public int getCount()
	    {
	    	return activities.size();
	        //return mThumbIds.length;
	    }

	    public Object getItem(int position)
	    {
	        return null;
	    }

	    public long getItemId(int position)
	    {
	        return 0;
	    }	  
	    
	    // create a new ImageView for each item referenced by the Adapter
	    public View getView(int position, View convertView, ViewGroup parent)
	    {
	    	ViewHolder holder;
	    	
            if (convertView == null)
            {  // if it's not recycled,     
                 convertView = mInflater.inflate(R.layout.item, null);  
                 convertView.setLayoutParams(new GridView.LayoutParams(getPixels(100), getPixels(110))); 
              
                 holder = new ViewHolder();
                 holder.title = (TextView) convertView.findViewById(R.id.categoryText);  
                 holder.icon = (ImageView )convertView.findViewById(R.id.categoryimage); 
                 convertView.setTag(holder);
             }
            else
            {  
                 holder = (ViewHolder) convertView.getTag(); 
            }
            
            holder.icon.setAdjustViewBounds(true);  
            holder.icon.setScaleType(ImageView.ScaleType.CENTER_INSIDE);     
            holder.icon.setPadding(getPixels(4), getPixels(4), getPixels(4), getPixels(4));  
            
            ActivityInfo info = activities.get(position);                                    
            
            if (info.name.equalsIgnoreCase("com.ion.appnow.VKShareActivity"))
            {
            	holder.icon.setImageResource(R.drawable.icon_vk);
            	holder.title.setText("VK");
            }
            else
            if (info.name.equalsIgnoreCase("com.ion.appnow.FBShareActivity"))
            {            
            	holder.icon.setImageResource(R.drawable.icon_fb);
            	holder.title.setText("Facebook");
            }
            else
            {
            	holder.icon.setImageDrawable(info.loadIcon(getPackageManager()));
            	holder.title.setText(info.loadLabel(getPackageManager()));
            }	        		                       

            return convertView;
	    }
	}
	
	ImageAdapter image_adapter;
	AlertDialog  alertDialog;
	
	private void LaunchComponent(String packageName, String name)
	{
	    Intent launch_intent = new Intent("android.intent.action.MAIN");
	    	   	    		
	    launch_intent.setAction(Intent.ACTION_SEND);
	    launch_intent.setType("text/plain");
	    		
	    launch_intent.addCategory("android.intent.category.LAUNCHER");
	    launch_intent.setComponent(new ComponentName(packageName, name));
	    launch_intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);

	    launch_intent.putExtra(android.content.Intent.EXTRA_TEXT, share_text);    	    	
	    
	    activity.startActivity(launch_intent);
	    	    
	    //IonLib.CallFuncInScene2(share_callback_module, share_callback, name);
	}
	
	@Override
	public void StartShareText(String text)
    {			
		int index = 0;
		int last_index = 0;
		
		for (int i=0; i<text.length() - 1; i++)
		{
			if (text.charAt(i)== '|' && text.charAt(i + 1) == '<')
			{	    				
				if (index == 0)
				{
					share_text = text.substring(last_index, i);
				}
				else
				if (index == 1)
				{
					share_link = text.substring(last_index, i);					
				
					last_index = i+2;
					
					share_vk = text.substring(last_index);
					break;
				}
									
				index++;
				last_index = i+2;			
			}			
		}	
		
		Log.e(TAG, share_text);
		Log.e(TAG, share_link);
		Log.e(TAG, share_vk);
		
		runOnUiThread(new Runnable()
		{			
			public void run()
			{										    		
				AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(AppNowActivity.this);
 
				alertDialogBuilder.setTitle("Share via");
 
				
				LayoutInflater inflater = AppNowActivity.this.getLayoutInflater();
				
				View layout = inflater.inflate(R.layout.sharelist, null);
				
				alertDialogBuilder.setView(layout);

				alertDialogBuilder.setOnCancelListener(alertCancelListener);

				alertDialog = alertDialogBuilder.create();
 
				gvMain = (GridView)layout.findViewById(R.id.gvMain);
				
				image_adapter = new ImageAdapter(AppNowActivity.this);
				gvMain.setAdapter(image_adapter);

				gvMain.setOnItemClickListener(new OnItemClickListener() {
					public void onItemClick(AdapterView<?> parent, View v, int position, long id) {
						ActivityInfo info = image_adapter.activities.get(position);
						LaunchComponent(info.packageName, info.name);

						AppNowActivity.this.alertDialog.dismiss();

						IonLib.SetShareRes(info.name);
					}
				});

				gvMain.setNumColumns(GridView.AUTO_FIT);
				gvMain.setVerticalSpacing(getPixels(3));
				gvMain.setHorizontalSpacing(getPixels(3));
				gvMain.setColumnWidth(getPixels(90));
				gvMain.setStretchMode(GridView.STRETCH_SPACING_UNIFORM);

				// show it
				alertDialog.show();	
			}
		});
    }
	
	@Override
	public void ReadModules(String module_name, Bundle metaData)
    {
		if (module_name.equals("info"))
		{
			String tmp;
			
			tmp = metaData.getString("app_id");
			if (tmp == null || tmp.length() < 5) return;
				
			app_id = tmp.substring("val:".length());
			
			tmp = metaData.getString("app_secret");
			if (tmp == null || tmp.length() < 5) return;
				
			app_secret = tmp.substring("val:".length());
			
			Log.d("AppNowActivity", "app_id: " + app_id + " app_secret: " + app_secret);
		}
    }
		
	private final BroadcastReceiver mHandleMessageReceiver = new BroadcastReceiver()
	{
		@Override
		public void onReceive(Context context, Intent intent)
		{
			String newMessage = intent.getExtras().getString("message");
			// Waking up mobile if it is sleeping
			WakeLocker.acquire(getApplicationContext());
			
			/**
			 * Take appropriate action on this message
			 * depending upon your app requirement
			 * For now i am just displaying it on the screen
			 * */
			
			// Showing received message
			//lblMessage.append(newMessage + "\n");			
			Toast.makeText(getApplicationContext(), "New Message: " + newMessage, Toast.LENGTH_LONG).show();
			
			// Releasing wake lock
			WakeLocker.release();
		}
	};

	Boolean handle_registered = false;

	@Override
	protected void onCreate(Bundle icicle)
	{
		super.onCreate(icicle);			
	}
	
	@Override
	protected void onStart()
	{			
		Log.e(TAG, "onStart()");		

		super.onStart();
	}
	 
	@Override
	protected void onStop()
	{
		Log.e(TAG, "onStop()");		

		super.onStop();
	}	
	
	@Override
    protected void onDestroy()
    {   
    	Log.e(TAG, "onDestroy()");

    	if (handle_registered)
		{
			unregisterReceiver(mHandleMessageReceiver);
		}
    	    	    	
        super.onDestroy();

		android.os.Process.killProcess(android.os.Process.myPid());
    }

	@Override
	public void TrackUserAge(String age)
 	{
		int age_number = Integer.parseInt(age);
    }
	
	@Override
	public void TrackUserGender(String gender)
 	{
    }
	
	@Override
	public void AddEventParamName(String name)
	{
		eventName = name;			    	    	    							
 	}
	
	@Override
	public void AddEventParamValue(String value)
 	{	
		eventParams.put(eventName, value);								
    }
	
	@Override
	public void SetPushServerURL(String url)
	{
		ServerUtilities.SERVER_URL = url;
	}
	
	@Override
	public void StartRegisterInPushService()
	{
		Log.e(TAG, "StartRegisterInPushService()");
		
		if (!app_id.isEmpty())
		{
			cd = new ConnectionDetector(getApplicationContext());
				
			// Make sure the device has the proper dependencies.
			GCMRegistrar.checkDevice(this);

			// Make sure the manifest was properly set - comment out this line
			// while developing the app, then uncomment it when it's ready.
			GCMRegistrar.checkManifest(this);
		
			registerReceiver(mHandleMessageReceiver, new IntentFilter(GCMIntentService.DISPLAY_MESSAGE_ACTION));

			handle_registered = true;

			// Get GCM registration id
			final String regId = GCMRegistrar.getRegistrationId(this);

			// Check if regid already presents
			//if (regId.equals(""))
			{
				// Registration is not present, register now with GCM			
				GCMRegistrar.register(this, GCMIntentService.SENDER_ID);
			}
			/*else
			{
				Log.i(TAG, "push token:" + regId);
				
				// Device is already registered on GCM
				if (GCMRegistrar.isRegisteredOnServer(this))
				{
					// Skips registration.				
					//Toast.makeText(getApplicationContext(), "Already registered with GCM", Toast.LENGTH_LONG).show();
				}
				else
				{
					// Try to register again, but not in the UI thread.
					// It's also necessary to cancel the thread onDestroy(),
					// hence the use of AsyncTask instead of a raw thread.
					final Context context = this;
				
					mRegisterTask = new AsyncTask<Void, Void, Void>()
					{
						@Override
						protected Void doInBackground(Void... params)
						{
							// Register on our server
							// On server creates a new user
							ServerUtilities.register(context, regId);
							return null;
						}

						@Override
						protected void onPostExecute(Void result)
						{
							mRegisterTask = null;
						}

					};
					mRegisterTask.execute(null, null, null);
				}
			}*/
		}
	}
	
	@Override
	public void TrackEvent(String event_name)
 	{
    }
};