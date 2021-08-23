package com.ion.appnow;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.google.android.gcm.GCMBroadcastReceiver;

public class GCMReceiver extends GCMBroadcastReceiver
{ 
    @Override
	protected String getGCMIntentServiceClassName(Context context)
    { 
		return "com.ion.appnow.GCMIntentService"; 
	} 
}
