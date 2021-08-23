
package com.ion.engine;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.AsyncTask;
import android.os.Environment;
import android.os.Messenger;
import android.os.StatFs;
import android.provider.Settings;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.google.android.vending.expansion.downloader.DownloadProgressInfo;
import com.google.android.vending.expansion.downloader.DownloaderClientMarshaller;
import com.google.android.vending.expansion.downloader.DownloaderServiceMarshaller;
import com.google.android.vending.expansion.downloader.Helpers;
import com.google.android.vending.expansion.downloader.IDownloaderClient;
import com.google.android.vending.expansion.downloader.IDownloaderService;
import com.google.android.vending.expansion.downloader.IStub;
import com.ion.engine.IonDownloader.Decompress;
//import com.ion.engine.WildWestDownloaderService;
import com.ion.engine.IonDownloader.ValidationTask;
import com.ion.engine.IonDownloader.XAPKFile;

public class IonDownloader implements IDownloaderClient
{	
	private static final String TAG = "LVLDownloader";
	
	public static class UIHolder
	{
		public ProgressBar progressBar;

		public TextView statusText;
		public TextView progressFraction;
		public TextView progressPercent;
		public TextView averageSpeed;
		public TextView timeRemaining;

		public View dashboard;
		public View cellMessage;

		public Button pauseButton;
		public Button WiFiSettingsButton;
		public Button resumeOnCell;
		
		public String not_enough_space_on_card;	
		public int kilobytes_per_second;
		public int time_remaining;
		
		public UIHolder()
		{	
		};
	};

	UIHolder ui_holder;
	
	public IonDownloader()
	{		
	};
	
    private boolean mStatePaused;
    private int mState;

    private IDownloaderService mRemoteService;

    private IStub mDownloaderClientStub; 
    
    public static String zipFileName = " ";
    public static int enoughSpace = 1;
    static int filesFoundOnStart = 0;
    static Boolean allreadyUnpucked = false;
    
    static private final float SMOOTHING_FACTOR = 0.005f;
    private boolean mCancelValidation;
    static int numCallsDoInBackground = 0;
    
    public static IonActivity main_activity;
    
    Boolean init_gui = false; 
    
    public static class XAPKFile
    {
        public boolean mIsMain;
        public int mFileVersion;
        public long mFileSize;

        XAPKFile(boolean isMain, int fileVersion, long fileSize)
        {
            mIsMain = isMain;
            mFileVersion = fileVersion;
            mFileSize = fileSize;
        }
    }
    
    private static final XAPKFile[] xAPKS =
    	{
        	new XAPKFile(true, // true signifies a main file
        				 12, // the version of the APK that the file was uploaded against
        				 39962020 // the length of the file in bytes
        			)       
    	};

    
    boolean expansionFilesDelivered()
    {
        for (XAPKFile xf : xAPKS)
        {
            String fileName = Helpers.getExpansionAPKFileName(main_activity, xf.mIsMain, xf.mFileVersion);
        
            if (!Helpers.doesFileExist(main_activity, fileName, xf.mFileSize, false))
            {
            	return false;
            }
        }
        
        return true;
    }
    
    private void setState(int newState)
    {
        if (mState != newState)
        {
            mState = newState;
            ui_holder.statusText.setText(Helpers.getDownloaderStringResourceIDFromState(newState));
        }
    }

    private void setButtonPausedState(boolean paused)
    {
        mStatePaused = paused;
        int stringResourceID = paused ? R.string.text_button_resume :
                R.string.text_button_pause;
        ui_holder.pauseButton.setText(stringResourceID);
    }            

    @Override
    public void onServiceConnected(Messenger m)
    {
    	mRemoteService = DownloaderServiceMarshaller.CreateProxy(m);
    	mRemoteService.onClientUpdated(mDownloaderClientStub.getMessenger());
    }

    @Override
    public void onDownloadStateChanged(int newState)
    {    	
    	setState(newState);
    
    	if (enoughSpace == 0)
    	{
    		ui_holder.statusText.setText(ui_holder.not_enough_space_on_card);
    	}
	
    	boolean showDashboard = true;
    	boolean showCellMessage = false;
    	boolean paused;
    	boolean indeterminate;
    
    	switch (newState)
    	{
        	case IDownloaderClient.STATE_IDLE:
        		// STATE_IDLE means the service is listening, so it's
        		//safe to start making calls via mRemoteService.
        		paused = false;
        		indeterminate = true;
            break;
        	case IDownloaderClient.STATE_CONNECTING:
        	case IDownloaderClient.STATE_FETCHING_URL:
        		showDashboard = true;
        		paused = false;
        		indeterminate = true;
        	break;
        	case IDownloaderClient.STATE_DOWNLOADING:
        		paused = false;
        		showDashboard = true;
        		indeterminate = false;
            break;

        	case IDownloaderClient.STATE_FAILED_CANCELED:
        	case IDownloaderClient.STATE_FAILED:
        	case IDownloaderClient.STATE_FAILED_FETCHING_URL:
        	case IDownloaderClient.STATE_FAILED_UNLICENSED:
        		paused = true;
        		showDashboard = false;
        		indeterminate = false;
            break;
        	case IDownloaderClient.STATE_PAUSED_NEED_CELLULAR_PERMISSION:
        	case IDownloaderClient.STATE_PAUSED_WIFI_DISABLED_NEED_CELLULAR_PERMISSION:
        		showDashboard = false;
        		paused = true;
        		indeterminate = false;
        		showCellMessage = true;
            break;

        	case IDownloaderClient.STATE_PAUSED_BY_REQUEST:
        		paused = true;
        		indeterminate = false;
            break;
        	case IDownloaderClient.STATE_PAUSED_ROAMING:
        	case IDownloaderClient.STATE_PAUSED_SDCARD_UNAVAILABLE:
        		paused = true;
        		indeterminate = false;
            break;
        	case IDownloaderClient.STATE_COMPLETED:
        		showDashboard = false;
        		paused = false;
        		indeterminate = false;
        		validateXAPKZipFiles();
            return;
        
        	default:
        		paused = true;
        		indeterminate = true;
        		showDashboard = true;
    	}
    
    	int newDashboardVisibility = showDashboard ? View.VISIBLE : View.GONE;
    
    	if (ui_holder.dashboard.getVisibility() != newDashboardVisibility)
    	{
    		ui_holder.dashboard.setVisibility(newDashboardVisibility);
    	}
    
    	int cellMessageVisibility = showCellMessage ? View.VISIBLE : View.GONE;
    	if (ui_holder.cellMessage.getVisibility() != cellMessageVisibility)
    	{
    		ui_holder.cellMessage.setVisibility(cellMessageVisibility);
    	}

    	ui_holder.progressBar.setIndeterminate(indeterminate);
    	setButtonPausedState(paused);    
    }

	@Override
	public void onDownloadProgress(DownloadProgressInfo progress)
	{		
		if (enoughSpace == 0)
		{
			ui_holder.statusText.setText(R.string.not_enough_space_on_card);
		}
		
				  
		ui_holder.averageSpeed.setText(main_activity.getString(ui_holder.kilobytes_per_second,
									   Helpers.getSpeedString(progress.mCurrentSpeed)));
		
		ui_holder.timeRemaining.setText(main_activity.getString(ui_holder.time_remaining,
									    Helpers.getTimeRemaining(progress.mTimeRemaining)));

		progress.mOverallTotal = progress.mOverallTotal;
		ui_holder.progressBar.setMax((int) (progress.mOverallTotal >> 8));
		ui_holder.progressBar.setProgress((int) (progress.mOverallProgress >> 8));
		
		ui_holder.progressPercent.setText(Long.toString(progress.mOverallProgress * 100 / progress.mOverallTotal) + "%");
		ui_holder.progressFraction.setText(Helpers.getDownloadProgressString(progress.mOverallProgress,
																			 progress.mOverallTotal));
	}

	void validateXAPKZipFiles()
	{	    
		ValidationTask validationTask = new ValidationTask();
		validationTask.execute(new Object());	
	}

    public class Decompress
    {
    	private String _zipFile; 
    	private String _location; 
    	 
    	public Decompress(String zipFile, String location)
    	{
    		_zipFile = zipFile; 
    	    _location = location; 
    	 
    	    _dirChecker("");
    	} 
    	 
    	public Boolean unzip( ValidationTask validationTask) 
    	{
    		try
    		{
    			FileInputStream fin = new FileInputStream(_zipFile); 
    	        ZipInputStream zin = new ZipInputStream(fin); 
    	        ZipEntry ze = null; 
    	        IonDownloader.allreadyUnpucked = false;
    	    
    	        while ((ze = zin.getNextEntry()) != null) 
    	        {
    	        	Log.e("unzip", "ze.getName() " + ze.getName()); 
    	 
    	        	if (ze.isDirectory())
    	        	{ 
    	        		_dirChecker(ze.getName()); 
    	        	} 
    	        	else
    	        	{
    	        		File f = new File(_location + ze.getName());
    	        		Log.e("unzip", "FileOutput name " + (_location + ze.getName())); 
    	        		
    	        		if(f.exists())
    	        		{
    	        			IonDownloader.allreadyUnpucked = true;
    	        			Log.e("unzip", "!!!!!FileOutput allready unpacked");
    	        			return false;
    	        		}
    	            	            	          
    	        		long size = ze.getSize();
    	        		long i = 0;
    	        		long percent = 0;
    	        		byte[] buf = new byte[1024];

    	        		StatFs stat_fs = new StatFs(Environment.getExternalStorageDirectory().getPath());
    	        		long avail_sd_space = (long)stat_fs.getAvailableBlocks() * (long)stat_fs.getBlockSize();
    	        
    	        		Log.e("unzip", "size=" + size + " avail_sd_space"+avail_sd_space);
    	        		
    	        		//avail_sd_space = 0;
    	        
    	        		if (size > avail_sd_space)
    	        		{
    	        			IonDownloader.enoughSpace = 0;
    	        			return false;    	        	    	        	
    	        		}

    	        		FileOutputStream fout = new FileOutputStream(_location + ze.getName());
    	      
    	        		int c = 0;
    	          
    	        		for (  c = zin.read(buf); c != -1; c = zin.read(buf), i += c) 
    	        		{ 
    	        			if (c > 0)
    	        			{
    	        				fout.write(buf, 0, c);
    	        				float fcurPercent = (float)i/(float)size * 100;
    	        				long curPercent = (long)(fcurPercent);
    	        				
    	        				if (curPercent != percent)
    	        				{
    	        					percent = curPercent;
    	        					Log.v("Progress", "Progress " + curPercent); 
    	        					
    	        					validationTask.CallPublishProgress(new DownloadProgressInfo(size, i, size - i, 1) );
    	        				}    	                	        		 
    	        			}
    	        		} 
    	 
    	        		zin.closeEntry(); 
    	        		fout.close(); 
    	        	}     	         
    	        } 
    	        
    	        zin.close(); 
    	    }
    		catch(Exception e)
    		{ 
    			Log.e("Decompress", "unzip", e); 
    		}
    		
    	    return true;
        }
    	
    	private void _dirChecker(String dir)
    	{ 
    	    File f = new File(_location + dir); 
    	 
    	    if(!f.isDirectory())
    	    {
    	    	f.mkdirs(); 
    	    } 
    	} 
    }      

    public class ValidationTask extends AsyncTask<Object, DownloadProgressInfo, Boolean>
	{	          
    	public void	CallPublishProgress( DownloadProgressInfo progressInfo)
    	{
             this.publishProgress(progressInfo);
    	}
    	
        @Override
        protected void onPreExecute()
        {
        	if (init_gui)
        	{
        		ui_holder.dashboard.setVisibility(View.VISIBLE);
        		ui_holder.cellMessage.setVisibility(View.GONE);
        		ui_holder.statusText.setText(R.string.text_exctracting);
        		
        		ui_holder.pauseButton.setOnClickListener(new View.OnClickListener()
        		{
        			@Override
        			public void onClick(View view)
        			{
        				mCancelValidation = true;
        			}
        		});
        		
        		ui_holder.pauseButton.setText(R.string.text_button_cancel_verify);
        	}
        	
        	
            super.onPreExecute();
        }

        @Override
        protected Boolean doInBackground(Object... params)
        {
        	Log.e("doInBackground", "numCallsDoInBackground=" + numCallsDoInBackground);
        	
        	if ( IonDownloader.numCallsDoInBackground > 0)
        		return false;
        	
        	IonDownloader.numCallsDoInBackground++;
        	
        	
        	//if (true)
        		//return true;
        			
            for (XAPKFile xf : xAPKS) 
            {            	
                String fileName = Helpers.getExpansionAPKFileName(
                		IonDownloader.main_activity,
                        xf.mIsMain, xf.mFileVersion);
                if (!Helpers.doesFileExist(IonDownloader.main_activity, fileName,
                        xf.mFileSize, false))
                    return false;
                
                zipFileName = Helpers.generateSaveFileName(IonDownloader.main_activity, fileName);
                
                Log.e("doInBackground",
        				"fileName=" + fileName + " zipFileName="+zipFileName+
        				" zipFileName.lastIndexOf('Resource.pack')="+ zipFileName.lastIndexOf("Resource.pack"));
                
            	if (zipFileName.lastIndexOf("Resource.pack") == -1 )
            	{
            	
            		String zipFile = zipFileName; 
            			//Environment.getExternalStorageDirectory() + "/files.zip"; 
            		String unzipLocation = zipFile;
            		int lastIndexOf = unzipLocation.lastIndexOf("/");
            		char[] bytes = unzipLocation.toCharArray();
            		unzipLocation = new String(bytes, 0, lastIndexOf+1);
            		Log.e("doInBackground",
            				"unzipLocation" + unzipLocation);
            		//unzipLocation = unzipLocation + "Resource.pack";
            		//for(int i = 0; i <= lastIndexOf; i++)
            		//{
            		//unzipLocation.concat(new String(bytes[i]));
            		//}
            	
            	 
            		Decompress d = new Decompress(zipFile, unzipLocation);
            		Boolean result = d.unzip(this);
            		
            		if (result == false && IonDownloader.allreadyUnpucked && IonDownloader.numCallsDoInBackground == 1)
            			return true;
            		
            		return result;
            	}
                /*
                fileName = Helpers
                        .generateSaveFileName(WildWestActivity.this, fileName);
                ZipResourceFile zrf;
                byte[] buf = new byte[1024 * 256];
                try {
                	zipFileName = fileName;
                    Log.e(Constants.TAG,
                            "zipFileName: " + zipFileName);
                    zrf = new ZipResourceFile(fileName);
                    ZipEntryRO[] entries = zrf.getAllEntries();
                    
                     // First calculate the total compressed length
                     
                     
                    long totalCompressedLength = 0;
                    for (ZipEntryRO entry : entries) {
                        totalCompressedLength += entry.mCompressedLength;
                    }
                    float averageVerifySpeed = 0;
                    long totalBytesRemaining = totalCompressedLength;
                    long timeRemaining;
                    
                     // Then calculate a CRC for every file in the Zip file,
                     // comparing it to what is stored in the Zip directory.
                     // Note that for compressed Zip files we must extract
                     // the contents to do this comparison.
                     //
                      
                    for (ZipEntryRO entry : entries) {
                        if (-1 != entry.mCRC32) {
                            //long length = entry.mUncompressedLength;
                        	long length = entry.mCompressedLength;
                            CRC32 crc = new CRC32();
                            DataInputStream dis = null;
                            try {
                                dis = new DataInputStream(
                                        zrf.getInputStream(entry.mFileName));

                                long startTime = SystemClock.uptimeMillis();
                                while (length > 0) {
                                    int seek = (int) (length > buf.length ? buf.length
                                            : length);
                                    dis.readFully(buf, 0, seek);
                                    crc.update(buf, 0, seek);
                                    length -= seek;
                                    long currentTime = SystemClock.uptimeMillis();
                                    long timePassed = currentTime - startTime;
                                    if (timePassed > 0) {
                                        float currentSpeedSample = (float) seek
                                                / (float) timePassed;
                                        if (0 != averageVerifySpeed) {
                                            averageVerifySpeed = SMOOTHING_FACTOR
                                                    * currentSpeedSample
                                                    + (1 - SMOOTHING_FACTOR)
                                                    * averageVerifySpeed;
                                        } else {
                                            averageVerifySpeed = currentSpeedSample;
                                        }
                                        totalBytesRemaining -= seek;
                                        timeRemaining = (long) (totalBytesRemaining / averageVerifySpeed);
                                        this.publishProgress(
                                                new DownloadProgressInfo(
                                                        totalCompressedLength,
                                                        totalCompressedLength
                                                                - totalBytesRemaining,
                                                        timeRemaining,
                                                        averageVerifySpeed)
                                                );
                                    }
                                    startTime = currentTime;
                                    if (mCancelValidation)
                                        return true;
                                }
                                if (crc.getValue() != entry.mCRC32) {
                                    Log.e(Constants.TAG,
                                            "CRC does not match for entry: "
                                                    + entry.mFileName);
                                    Log.e(Constants.TAG,
                                            "In file: " + entry.getZipFileName());
                                    return true;
                                }
                            } finally {
                                if (null != dis) {
                                    dis.close();
                                }
                            }
                        }
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                    return false;
                }
            }
        */
            }
            return true;
        }
        
        @Override
        protected void onProgressUpdate(DownloadProgressInfo... values)
        {
            onDownloadProgress(values[0]);
            super.onProgressUpdate(values);
        }
        
        @Override
        protected void onPostExecute(Boolean result)
        {
        	 Log.e("onPostExecute", "result="+result + " enoughSpace=" + enoughSpace +" init_gui=" + init_gui );
            if (result)
            {
            	//if (init_gui)
            	{
            		ui_holder.dashboard.setVisibility(View.VISIBLE);
            		ui_holder.cellMessage.setVisibility(View.GONE);
            		//mStatusText.setText(R.string.text_validation_complete);
                	if (enoughSpace == 0)
                	{
                		
                		ui_holder.statusText.setText(ui_holder.not_enough_space_on_card);
                		
                		if (IonDownloader.filesFoundOnStart == 1)
                			this.publishProgress(
                                new DownloadProgressInfo(
                                        100,
                                        100,
                                        0,
                                        0));
                	}
                	
                	ui_holder.pauseButton.setOnClickListener(new View.OnClickListener()
                	{
            			@Override
            			public void onClick(View view)
            			{
            				//StartENgine();
            		    	 if (enoughSpace == 0)
            		    	 {
            		    		 android.os.Process.killProcess(android.os.Process.myPid());
            		    	 }            		    		 
            			}
            		});
            		
                	ui_holder.pauseButton.setText(android.R.string.ok);            		
            	}

       			if (IonDownloader.enoughSpace == 1)
       			{
       				Log.e("onPostExecute", "StartENgine()");
       				StartENgine();
       			}
       			else
       			if (!init_gui)
       			{
       				/*
       				mStatusText.setText(R.string.not_enough_space_on_card);
            		mPauseButton.setOnClickListener(new View.OnClickListener() {
            			@Override
            			public void onClick(View view)
            			{
            				//StartENgine();
            		    	 if (enoughSpace == 0)
            		    	 	 finish();
            			}
            		});
            		mPauseButton.setText(android.R.string.ok); 
            		

       				AlertDialog ad = new AlertDialog.Builder(WildWestActivity.this).create();
       				ad.setCancelable(false); // This blocks the 'BACK' button
       				ad.setMessage("Please reinstal the application");
       				ad.setButton("OK", new DialogInterface.OnClickListener() {
       					@Override
       					public void onClick(DialogInterface dialog, int which) 
       					{
       						dialog.dismiss();
       						finish();
       					}
       				});
       				ad.show();
       				*/
       			}


            }
            
            else
            {
            	if (enoughSpace == 0)
            	{
            		
            		ui_holder.statusText.setText(R.string.not_enough_space_on_card);
            		
            		ui_holder.pauseButton.setOnClickListener(new View.OnClickListener()
            		{
            			@Override
            			public void onClick(View view)
            			{
            				//StartENgine();
            		    	 if (enoughSpace == 0)
            		    		 main_activity.finish();
            			}
            		});
            		
            		if (IonDownloader.filesFoundOnStart == 1)
            		{
            			this.publishProgress(
                            new DownloadProgressInfo(
                                    100,
                                    100,
                                    0,
                                    0));
            		}
            	}
            	
            	/*
            	if (init_gui)
            	{
            		mDashboard.setVisibility(View.VISIBLE);
            		mCellMessage.setVisibility(View.GONE);
            		mStatusText.setText(R.string.text_validation_failed);
            		mPauseButton.setOnClickListener(new View.OnClickListener()
            		{
            			@Override
            			public void onClick(View view)
            			{
            				finish();
            			}
            		});
            		mPauseButton.setText(android.R.string.cancel);
            	}
            	*/
            }
            super.onPostExecute(result);            
        }
	}
    
    public void Init(UIHolder set_holder, IonActivity set_activity, Class<?> downloaderService, int version, int exp_size)
    {
    	Log.e(TAG, "Init");
    	
    	xAPKS[0].mFileVersion = version;
    	xAPKS[0].mFileSize = exp_size;
    	
    	ui_holder = set_holder;    
        main_activity = set_activity;
        	
        init_gui = true;
        	
        mDownloaderClientStub = DownloaderClientMarshaller.CreateStub(this, downloaderService);
                    
        ui_holder.pauseButton.setOnClickListener(new View.OnClickListener()
        {
        	@Override
            public void onClick(View view)
            {
        		if (mStatePaused)
                {
        			mRemoteService.requestContinueDownload();
                }
                else
                {
                	mRemoteService.requestPauseDownload();
                }
                    
                setButtonPausedState(!mStatePaused);
            }
        });

        ui_holder.WiFiSettingsButton.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
              	main_activity.startActivity(new Intent(Settings.ACTION_WIFI_SETTINGS));
            }
        });          
            
        ui_holder.resumeOnCell.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                mRemoteService.setDownloadFlags(IDownloaderService.FLAGS_DOWNLOAD_OVER_CELLULAR);
                mRemoteService.requestContinueDownload();
                ui_holder.cellMessage.setVisibility(View.GONE);
            }
        });                       
            
        if (expansionFilesDelivered())
        {
          	filesFoundOnStart = 1;          	
        }
        
        Log.e(TAG, "expansionFilesDelivered " + filesFoundOnStart);
                                                
        if (filesFoundOnStart == 0)
        {            	            	
            try
            {
            	Intent launchIntent = main_activity.getIntent();
                    
                Intent intentToLaunchThisActivityFromNotification = new Intent(main_activity, main_activity.getClass());
                intentToLaunchThisActivityFromNotification.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TOP);
                intentToLaunchThisActivityFromNotification.setAction(launchIntent.getAction());

                if (launchIntent.getCategories() != null)
                {
                	for (String category : launchIntent.getCategories())
                    {
                		intentToLaunchThisActivityFromNotification.addCategory(category);
                    }
                }

                PendingIntent pendingIntent = PendingIntent.getActivity(main_activity,
                            											0, intentToLaunchThisActivityFromNotification,
                            											PendingIntent.FLAG_UPDATE_CURRENT);
                                        
                int startResult = DownloaderClientMarshaller.startDownloadServiceIfRequired(main_activity, pendingIntent, downloaderService);                               

                
                
                if (startResult != DownloaderClientMarshaller.NO_DOWNLOAD_REQUIRED)
                {
                	Log.e(TAG, "DownloaderClientMarshaller != NO_DOWNLOAD_REQUIRED");
                	
                	// The DownloaderService has started downloading the files,
                    // show progress
                    //initializeDownloadUI();
                    //return;
                } // otherwise, download not needed so we fall through to
                // starting the movie
           }
           
           catch (NameNotFoundException e)
           {
        	   Log.e(TAG, "Cannot find own package! MAYDAY!");
               e.printStackTrace();
           }
       }       
       else 
       {
    	   validateXAPKZipFiles();
       }
    }
    
    public void OnStart()
    {
    	if (null != mDownloaderClientStub)
    	{
    		mDownloaderClientStub.connect(main_activity);
    	}
    }
    
    public void OnStop()
    {    
        if (null != mDownloaderClientStub)
        {
            mDownloaderClientStub.disconnect(main_activity);
        }
    }
    
    public void OnDestroy()
    {
        mCancelValidation = true;         
    }
    
    public void StartENgine()
	{
    	ApplicationInfo appInfo = null;
    	PackageManager packMgmr = main_activity.getPackageManager();					
	
    	String fileName = Helpers.getExpansionAPKFileName( main_activity, xAPKS[0].mIsMain, xAPKS[0].mFileVersion);
    
    	if (Helpers.doesFileExist(main_activity, fileName, xAPKS[0].mFileSize, false))
    	{
    		String sv_file = Helpers.getSaveFilePath(main_activity);
    		
    		Log.e(TAG, "saved file is " + sv_file);
    		IonLib.SetBasePath(sv_file + "/");
		
    		//IonLib.ActivateResourcePack("fileName");
    		IonLib.ActivateResourcePack("Resource.pack");
    	}
    	
    	main_activity.StartENgine();
	}
}
