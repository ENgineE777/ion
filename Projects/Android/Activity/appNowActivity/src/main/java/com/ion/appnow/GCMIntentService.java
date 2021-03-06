package com.ion.appnow;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.util.Log;

import com.ion.engine.IonActivity;

import com.google.android.gcm.GCMBaseIntentService;

import com.ion.packagename.R;

public class GCMIntentService extends GCMBaseIntentService
{	 
    // Google project id
    public static final String SENDER_ID = "986201009374";    
	
    public static final String DISPLAY_MESSAGE_ACTION = "com.ion.appnow.DISPLAY_MESSAGE";
    
	private static final String TAG = "GCMIntentService";

    public GCMIntentService()
    {
        super(SENDER_ID);
    }

    /**
     * Method called on device registered
     **/
    @Override
    protected void onRegistered(Context context, String registrationId)
    {
        Log.i(TAG, "Device registered: regId = " + registrationId);
        //displayMessage(context, "Your device registred with GCM");
        //Log.d("NAME", MainActivity.name);
        ServerUtilities.register(context, registrationId);
    }

    /**
     * Method called on device un registred
     * */
    @Override
    protected void onUnregistered(Context context, String registrationId)
    {
        Log.i(TAG, "Device unregistered");
        //displayMessage(context, "getString(R.string.gcm_unregistered)");
        ServerUtilities.unregister(context, registrationId);
    }

    /**
     * Method called on Receiving a new message
     * */
    @Override
    protected void onMessage(Context context, Intent intent)
    {        
        String message = intent.getExtras().getString("message");

        Log.i(TAG, "Received message " + message);
        
        //displayMessage(context, message);
        // notifies user
        generateNotification(context, message);
    }

    /**
     * Method called on receiving a deleted message
     * */
    @Override
    protected void onDeletedMessages(Context context, int total) {
        Log.i(TAG, "Received deleted messages notification");
        String message = "getString(R.string.gcm_deleted, total)";
        //displayMessage(context, message);
        // notifies user
        generateNotification(context, message);
    }

    /**
     * Method called on Error
     * */
    @Override
    public void onError(Context context, String errorId)
    {
        Log.i(TAG, "Received error: " + errorId);
        //displayMessage(context, "getString(R.string.gcm_error, errorId)");
    }

    @Override
    protected boolean onRecoverableError(Context context, String errorId)
    {
        // log message
        Log.i(TAG, "Received recoverable error: " + errorId);
        //displayMessage(context, "getString(R.string.gcm_recoverable_error, errorId)");
        
        return super.onRecoverableError(context, errorId);
    }

    /**
     * Issues a notification to inform the user that server has sent a message.
     */
    private static void generateNotification(Context context, String message)
    {    	    	
        int icon = R.drawable.ic_launcher;
        long when = System.currentTimeMillis();
        NotificationManager notificationManager = (NotificationManager)
                context.getSystemService(Context.NOTIFICATION_SERVICE);
        
        String title = context.getString(R.string.ion_activity);
        
        Intent notificationIntent = new Intent(context, AppNowActivity.class);
        // set intent so it does not start a new activity
        notificationIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP |
                Intent.FLAG_ACTIVITY_SINGLE_TOP);
        PendingIntent intent =
                PendingIntent.getActivity(context, 0, notificationIntent, 0);

        //Notification notification = new Notification(icon, message, when);
        //notification.setLatestEventInfo(context, title, message, intent);

        Notification notification = new Notification.Builder(context)
                                    .setContentTitle(title)
                                    .setContentText(message)
                                    .setSmallIcon(icon)
                                    .setContentIntent(intent)
                                    .build();

        notification.flags |= Notification.FLAG_AUTO_CANCEL;
        
        // Play default notification sound
        //notification.defaults |= Notification.DEFAULT_SOUND;
        
        //notification.sound = Uri.parse("android.resource://" + context.getPackageName() + "your_sound_file_name.mp3");
        
        // Vibrate if vibrate is enabled
        notification.defaults |= Notification.DEFAULT_VIBRATE;
        notificationManager.notify(0, notification);
    }

}
