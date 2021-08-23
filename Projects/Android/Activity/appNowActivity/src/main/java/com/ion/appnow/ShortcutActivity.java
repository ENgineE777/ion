
package com.ion.appnow;

import android.app.Activity;
import android.content.Intent;
import android.content.Intent.ShortcutIconResource;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import com.ion.packagename.R;
import android.os.Parcelable;

public class ShortcutActivity extends Activity
{
	@Override
	protected void onCreate(final Bundle savedInstanceState)
    {
	    super.onCreate(savedInstanceState);
	    
	    Log.e("Fuckkkk!!!!", "ShortcutIconResource icon");

		Intent shortcutIntent = new Intent(Intent.ACTION_MAIN);
		shortcutIntent.setClassName(this, this.getClass().getName());
		//shortcutIntent.putExtra(EXTRA_KEY, "ApiDemos Provided This Shortcut");

		// Then, set up the container intent (the response to the caller)

		Intent intent = new Intent();
		intent.putExtra(Intent.EXTRA_SHORTCUT_INTENT, shortcutIntent);
		intent.putExtra(Intent.EXTRA_SHORTCUT_NAME, "getString(R.string.shortcut_name)");
		Parcelable iconResource = Intent.ShortcutIconResource.fromContext(this,  R.drawable.ic_launcher);

		intent.putExtra(Intent.EXTRA_SHORTCUT_ICON_RESOURCE, iconResource);

		setResult(RESULT_OK, intent);


		/*ShortcutIconResource icon = Intent.ShortcutIconResource.fromContext(this, R.drawable.ic_launcher);

	    if (icon == null)
	    {
	    	Log.e("Fuckkkk!!!!", "icon is bull fucccckkkkk");
	    }
	    
	    Intent intent = new Intent();

	    Intent launchIntent = new Intent(this, AppNowActivity.class);

	    Log.e("Fuckkkk!!!!", "ShortcutIconResource EXTRA_SHORTCUT_INTENT");
	    intent.putExtra(Intent.EXTRA_SHORTCUT_INTENT, launchIntent);
	    
	    Log.e("Fuckkkk!!!!", "ShortcutIconResource EXTRA_SHORTCUT_NAME " + getString(R.string.ion_activity));
	    intent.putExtra(Intent.EXTRA_SHORTCUT_NAME, getString(R.string.ion_activity));	    
	    
	    Log.e("Fuckkkk!!!!", "ShortcutIconResource EXTRA_SHORTCUT_ICON_RESOURCE " + icon.packageName);
	    intent.putExtra(Intent.EXTRA_SHORTCUT_ICON_RESOURCE, icon);

	    setResult(RESULT_OK, intent);*/
	    
	    finish();
	}
}