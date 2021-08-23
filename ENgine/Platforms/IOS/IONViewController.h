//
//  EAGLView.h
//  IPhone
//
//  Created by Eugene Solyanov on 1/9/12.
//  Copyright __MyCompanyName__ 2012. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <GLKit/GLKit.h>
#import <CoreMotion/CoreMotion.h>

//#import "GADInterstitial.h"
//#import "GADInterstitialDelegate.h"

#import "InstagramAuthController.h"

#import "MapKit/MapKit.h"
#import "AVFoundation/AVFoundation.h"

    
@interface IONViewController : GLKViewController<UIAlertViewDelegate, UITextFieldDelegate, CLLocationManagerDelegate, UIActionSheetDelegate, UINavigationControllerDelegate, UIImagePickerControllerDelegate, MKMapViewDelegate, InstagramAuthDelegate, AVCaptureVideoDataOutputSampleBufferDelegate,AVCaptureMetadataOutputObjectsDelegate>
{
    EAGLContext *context;
    CMMotionManager* mmanager;
}

- (void)openFBLogin;
- (void)openVKLogin;
- (void)StartMonitoringRegion:(NSString*)id uuid:(NSString*)uuid;
- (void)StopMonitoringRegion:(const char*)id;
- (void)SetCallbackOnEnterRegion:(const char*)id callback:(const char*)name;
- (void)SetCallbackOnExitRegion:(const char*)id callback:(const char*)name;
- (int)GetRegionBeaconsCount:(const char*)id;
- (int)GetRegionBeaconMinor:(const char*)id index:(int)index;
- (int)GetRegionBeaconMajor:(const char*)id index:(int)index;
- (int)GetRegionBeaconProximity:(const char*)id index:(int)index;
- (float)GetRegionBeaconAccuracy:(const char*)id index:(int)index;
- (void)ShowMap:(bool)show;
- (void)SetMapPosition:(int)x set_y:(int)y set_w:(int)w set_h:(int)h;
- (void)AddObjectToMap:(float)x set_y:(float)y set_text:(NSString*)text;
- (void)DeleteAllMapObjects;
- (void)ShowPopup:(NSString*)title set_text:(NSString*)text set_ok_text:(NSString*)ok_text;
- (void)ShowPopup:(NSString*)title set_text:(NSString*)text set_ok_text:(NSString*)ok_text set_cancel_text:(NSString*)cancel_text;
- (float)GetScreenScale;
- (int)GetTitleBarHeight;

@property (strong, nonatomic) CLLocationManager* locationManager;

@end
