
package com.ion.engine;

import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.view.View;
import android.widget.TextView;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.MarkerOptions;

public class IonMapActivity extends FragmentActivity
{
    private static final String TAG = "IonWebViewActivity";

    public static float x = 0;
    public static float y = 0;
    public static String desc = "";          
                       
    TextView url_txt = null;
    
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
    }        
    
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        
        setContentView(R.layout.mapview);                                                            
                        
        url_txt = (TextView) findViewById(R.id.BrowserPanelURL);
        url_txt.setText("Maps");
                                            
        listiner = new Listiner();
        
        View button = findViewById(R.id.closeWeb);
        button.setOnClickListener(listiner);                           
               
        SupportMapFragment mMap = (SupportMapFragment)getSupportFragmentManager().findFragmentById(R.id.map);
        
        mMap.getMap().moveCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(x, y), 18));
        mMap.getMap().addMarker(new MarkerOptions().position(new LatLng(x, y)).title(desc));       
    }         
}