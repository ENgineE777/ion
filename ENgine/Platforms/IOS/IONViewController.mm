//
//  EAGLView.m
//  IPhone
//
//  Created by Eugene Solyanov on 1/9/12.
//  Copyright __MyCompanyName__ 2012. All rights reserved.
//

#include <mach/mach_time.h>

#include "Core/Script/ScriptModule.h"
#include "Core/Network/NetLogger.h"

#import "Core/Core.h"

#import "IONViewController.h"
#import "vkLoginViewController.h"
#import "WebViewController.h"
//#import "MMInterstitial.h"
#import "MediaPlayerViewController.h"
#import "VKShareActivity.h"
#import "FBShareActivity.h"
#import "Platforms/IOS/IOSAppDelegate.h"
#import "Platforms/IOS/IOSAppDelegate.h"

#import "MapKit/MapKit.h"

#import "TileOverlay.h"
#import "TileOverlayView.h"

TileOverlay* overlay;

//#include "Flurry.h"

float allow_first_responder = -0.5f;

extern bool vk_login_view_logined;
extern std::string vk_login_view_token;
extern std::string vk_login_view_user_id;
extern bool vk_login_need_call_ion;
extern int vk_login_view_status;

int   vk_share_stage = 0;
float vk_share_stage_time = 0.0f;

int   fb_share_stage = 0;
float fb_share_stage_time = 0.0f;

extern bool fb_login_view_logined;
extern std::string fb_login_view_token;
extern int  fb_login_view_status;
extern bool fb_login_need_call_ion;

int project_orintation = 0;
int project_status_bar = 0;

extern NSString* ads_mmedia_appid;
extern NSString* ads_admob_appid;

extern string mediaplayer_path;
extern string mediaplayer_videoName;

extern IOSAppDelegate* app_delegate;

extern int ym_external_amount;
extern NSString* ym_external_reciver;
extern NSString* ym_external_message;

std::string ios_share_text;
std::string ios_share_extra_data;
std::string ios_share_type;

IONViewController* ion_view_controller = null;
MKMapView* mapView = null;


vector<MKPointAnnotation*> annotations;

double last_time = 0.0;
int debug_info_type = 0;

float timer_to_open_photo_library = -1.0f;

bool need_cache_maps = true;

bool first_run = true;

bool allow_ym = true;

struct BeaconData
{
    int minor;
    int major;
    int proximity;
    float dist;
};

struct RegionData
{
    char onEnterCallback[128];
    bool enterCalled;
    char onExitCallback[128];
    CLBeaconRegion* raw_data;
    ScriptModule* exec_module;
    vector<BeaconData> beaconsData;
    
    RegionData()
    {
        onEnterCallback[0] = 0;
        onExitCallback[0] = 0;
        raw_data = null;
        exec_module = null;
        enterCalled = false;
    }
};

vector<RegionData> regionsData;

struct ActTouch
{
    UITouch* touch;
    bool need_end;
    int counter;
    
    ActTouch()
    {
        touch = 0;
        need_end = false;
        counter = 0;
        
    };
};

ActTouch act_touches[10];

double GetCurrentTime()
{
    static mach_timebase_info_data_t sTimebaseInfo;
    uint64_t time = mach_absolute_time();
    uint64_t nanos;
    
    // If this is the first time we've run, get the timebase.
    // We can use denom == 0 to indicate that sTimebaseInfo is
    // uninitialised because it makes no sense to have a zero
    // denominator is a fraction.
    if (sTimebaseInfo.denom == 0) {
        (void)mach_timebase_info(&sTimebaseInfo);
    }
    
    // Do the maths.  We hope that the multiplication doesn't
    // overflow; the price you pay for working in fixed point.
    nanos = time * sTimebaseInfo.numer / sTimebaseInfo.denom;
    return ((double)nanos / 1000000000.0) * 0.01f;
}

UITextField* tfiled = null;

@implementation IONViewController

- (float)GetScreenScale
{
    if (floor(NSFoundationVersionNumber) > NSFoundationVersionNumber_iOS_7_1)
    {
        return [UIScreen mainScreen].nativeScale;
    }

    return [UIScreen mainScreen].scale;
}

- (void)dealloc
{
    [self shutdownION];
    
    if ([EAGLContext currentContext] == context)
    {
        [EAGLContext setCurrentContext:nil];
    }
    
    [context release];
    [super dealloc];
}

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string
{
    core->TraceTo("app", string.UTF8String);
    
    if (strlen(string.UTF8String) == 0)
    {
        wchar_t wstr[8];
        wstr[0] = 8;
     
        core->Controls()->SetLastChar(wstr[0]);
        
        return false;
    }
    
    
    const char* str = [string UTF8String];
    
    if (str[0] == '\n')
    {
        wchar_t chr = 9;
        
        core->Controls()->SetLastChar(chr);
    }
    else
    {
        wstring wstr;
        String::Utf8toUtf16(wstr, str);
        
        core->Controls()->SetLastChar(wstr.c_str()[0]);
    }
    
    core->TraceTo("App","insertText %s", str);
    
    return false;
}

- (MKOverlayView*)mapView:(MKMapView*)mapView viewForOverlay:(id<MKOverlay>)ovl
{
    TileOverlayView* view = [[TileOverlayView alloc] initWithOverlay:ovl];
    view.tileAlpha = 1.0f;
    return [view autorelease];
    
}

- (void)viewDidLoad
{
    NSLog(@"viewDidLoad");
    
    ion_view_controller = self;
    
    //ion_view_controller.keyboardType = UIKeyboardTypeURL;
    
    [super viewDidLoad];
    
    
    //mapView = [[MKMapView alloc] initWithFrame:CGRectMake(0, 0, 300, 300)];
    //mapView.delegate = self;
    //mapView.pitchEnabled = false;
    //mapView.rotateEnabled = false;
    //[self.view addSubview:mapView];
    
    overlay = [[TileOverlay alloc] initOverlay];
    [mapView addOverlay:overlay];
    
    MKMapRect visibleRect = [mapView mapRectThatFits:overlay.boundingMapRect];
    visibleRect.size.width /= 2;
    visibleRect.size.height /= 2;
    visibleRect.origin.x += visibleRect.size.width / 2;
    visibleRect.origin.y += visibleRect.size.height / 2;
    mapView.visibleMapRect = visibleRect;
    
    [self AddObjectToMap:50 set_y:10 set_text:@"ssdffsd"];
    

    
    context = [[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2] autorelease];

    
    if (!context)
    {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat16;
    
    self.preferredFramesPerSecond = 60;
    
    //if (floor(NSFoundationVersionNumber) > NSFoundationVersionNumber_iOS_6_1)
    {
        tfiled = [[UITextField alloc] initWithFrame:CGRectMake(7107, 10, 100, 30)];
        tfiled.delegate = self;
        tfiled.text = @"ION";
        
        if (floor(NSFoundationVersionNumber) > NSFoundationVersionNumber_iOS_7_1)
        {
            tfiled.autocorrectionType = UITextAutocorrectionTypeNo;
        }
        
        [self.view addSubview:tfiled];
    }
    
    [self resignFirstResponder];
    
    [self initION];
    
    self.locationManager = null;
    
    //if (floor(NSFoundationVersionNumber) > NSFoundationVersionNumber_iOS_7_1)
    //{
    //   [[UIApplication sharedApplication] setStatusBarStyle:UIStatusBarStyleBlackOpaque animated:NO];
    //}
    
    /*if (need_cache_maps)
    {
        mapView = [[MKMapView alloc] initWithFrame:CGRectMake(0,0, 200, 200)];
        mapView.pitchEnabled = false;
        mapView.rotateEnabled = false;
        //mapView.hidden = true;
        [self.view.superview addSubview:mapView];
    }*/
    
    /*NSUUID* uuid = [[NSUUID alloc] initWithUUIDString:@"F7826DA6-4FA2-4E98-8024-BC5B71E0893E"];
    CLBeaconRegion* beaconRegion = [[CLBeaconRegion alloc] initWithProximityUUID:uuid identifier:@"Kontakt"];
    [self.locationManager startMonitoringForRegion:beaconRegion];*/
    
    //[[EBBeaconsTracker sharedInstance] setAppID:@"ThisIsID"];
    //[[EBBeaconsTracker sharedInstance] startMonitoring];
    //[[EBBeaconsTracker sharedInstance] setUuids:@[@"4DFE6B3C-6292-AEEE-159C-55E4818382CD"]];
    
    //tets_filed = [[UITextField alloc] initWithFrame:CGRectMake(0, 10, 20, 55)];
    //tets_filed.inputDelegate = self;
    
    // Register notification when the keyboard will be show
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(keyboardWillShow:)
                                                 name:UIKeyboardWillShowNotification
                                               object:nil];
    
    // Register notification when the keyboard will be hide
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(keyboardWillHide:)
                                                 name:UIKeyboardWillHideNotification
                                               object:nil];

}

- (void)EnableTrackGPS:(bool)enable
{
    if (enable)
    {
        if (!self.locationManager)
        {
            self.locationManager = [[CLLocationManager alloc] init];
            self.locationManager.desiredAccuracy = kCLLocationAccuracyBest;
            self.locationManager.delegate = self;
        }
    
        if ([self.locationManager respondsToSelector:@selector(requestWhenInUseAuthorization)])
        {
            [self.locationManager requestWhenInUseAuthorization];
        }
        
        [self.locationManager startUpdatingLocation];
    }
    else
    {
        //[self.locationManager stopUpdatingLocation];
    }
}

- (void)keyboardWillShow:(NSNotification *)notification
{
    CGSize keyboardSize = [[[notification userInfo] objectForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue].size;
    
    core->Controls()->SetSoftKeyboardHeight(keyboardSize.height * [self GetScreenScale]);
}

- (void)keyboardWillHide:(NSNotification *)notification
{
    CGRect keyboardBounds;
    
    [[notification.userInfo valueForKey:UIKeyboardFrameBeginUserInfoKey] getValue:&keyboardBounds];
    
    core->Controls()->HideSoftKeyboard();
    
    // Do something with keyboard height
}

- (void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray *)locations
{
    CLLocation* loc = manager.location;
    
    en_core.Mobile()->SetGPSData(0, loc.coordinate.latitude, loc.coordinate.longitude);
}

- (void)StartMonitoringRegion:(NSString*)id uuid:(NSString*)uuid
{
    for (int i=0;i<regionsData.size();i++)
    {
        CLBeaconRegion* beaconRegion = regionsData[i].raw_data;
        
        if (String::IsEqual(beaconRegion.identifier.UTF8String, id.UTF8String))
        {
            return;
        }
    }
    
    /*NSUUID* nuuid = [[NSUUID alloc] initWithUUIDString:@"F7826DA6-4FA2-4E98-8024-BC5B71E0893E"];
    CLBeaconRegion* beaconRegion = [[CLBeaconRegion alloc] initWithProximityUUID:nuuid identifier:@"Kontakt"];
    [self.locationManager startMonitoringForRegion:beaconRegion];*/
    
    regionsData.push_back(RegionData());
    
    RegionData& regionData = regionsData[regionsData.size()-1];
    
    NSUUID* nuuid = [[NSUUID alloc] initWithUUIDString:uuid];
    regionData.raw_data = [[CLBeaconRegion alloc] initWithProximityUUID:nuuid identifier:id];
    regionData.raw_data.notifyEntryStateOnDisplay = true;
    [self.locationManager startMonitoringForRegion:regionData.raw_data];
    
    //[self locationManager:self.locationManager didStartMonitoringForRegion:regionData.raw_data];
}

- (void)locationManager:(CLLocationManager *)manager didDetermineState:(CLRegionState)state forRegion:(CLRegion *)region
{
    // present local notification
    UILocalNotification *notification = [[UILocalNotification alloc] init];
    notification.alertBody = @"didDetermineState";
    notification.soundName = UILocalNotificationDefaultSoundName;
    
    for (int i=0;i<regionsData.size();i++)
    {
        CLBeaconRegion* beaconRegion = regionsData[i].raw_data;
        
        if (String::IsEqual(beaconRegion.identifier.UTF8String, region.identifier.UTF8String))
        {
            if (state == CLRegionStateInside)
            {
                [manager startRangingBeaconsInRegion:beaconRegion];
            }
            else
            {
                [manager stopRangingBeaconsInRegion:beaconRegion];
            }
            
            break;
        }
    }
}

- (void)StopMonitoringRegion:(const char*)id
{
    for (int i=0;i<regionsData.size();i++)
    {
        CLBeaconRegion* beaconRegion = regionsData[i].raw_data;

        if (String::IsEqual(beaconRegion.identifier.UTF8String, id))
        {
            [self.locationManager stopMonitoringForRegion:beaconRegion];
            
            if (i!= regionsData.size()-1)
            {
                regionsData[i] = regionsData[regionsData.size()-1];
            }
            
            regionsData.pop_back();
            
            [beaconRegion release];
            
            break;
        }
    }
}

-(void)locationManager:(CLLocationManager *)manager didStartMonitoringForRegion:(CLRegion *)region
{
    for (int i=0;i<regionsData.size();i++)
    {
        CLBeaconRegion* beaconRegion = regionsData[i].raw_data;
        
        if (String::IsEqual(beaconRegion.identifier.UTF8String, region.identifier.UTF8String))
        {
            [self.locationManager startRangingBeaconsInRegion:beaconRegion];
            break;
        }
    }
}

-(void)SetCallbackOnEnterRegion:(const char*)id callback:(const char*)name
{
    for (int i=0;i<regionsData.size();i++)
    {
        CLBeaconRegion* beaconRegion = regionsData[i].raw_data;
        
        if (String::IsEqual(beaconRegion.identifier.UTF8String, id))
        {
            String::Copy(regionsData[i].onEnterCallback, 128, name);
            regionsData[i].exec_module = ScriptModule::cur_exec_module;
        }
    }
}

-(void)SetCallbackOnExitRegion:(const char*)id callback:(const char*)name
{
    for (int i=0;i<regionsData.size();i++)
    {
        CLBeaconRegion* beaconRegion = regionsData[i].raw_data;
        
        if (String::IsEqual(beaconRegion.identifier.UTF8String, id))
        {
            String::Copy(regionsData[i].onExitCallback, 128, name);
            regionsData[i].exec_module = ScriptModule::cur_exec_module;
        }
    }
}

-(void)locationManager:(CLLocationManager *)manager didEnterRegion:(CLRegion *)region
{
    // present local notification
    UILocalNotification *notification = [[UILocalNotification alloc] init];
    notification.alertBody = @"У нас сегодня большие скидки, ждем Вас!";
    notification.soundName = UILocalNotificationDefaultSoundName;
    
    [[UIApplication sharedApplication] presentLocalNotificationNow:notification];
    
    for (int i=0;i<regionsData.size();i++)
    {
        CLBeaconRegion* beaconRegion = regionsData[i].raw_data;
        
        if (String::IsEqual(beaconRegion.identifier.UTF8String, region.identifier.UTF8String))
        {
            if (regionsData[i].onEnterCallback[0] && regionsData[i].exec_module && !regionsData[i].enterCalled)
            {
                //regionsData[i].exec_module->Call(regionsData[i].onEnterCallback);
                regionsData[i].enterCalled = true;
            }
 
            break;
        }
    }
}

-(void)locationManager:(CLLocationManager *)manager didExitRegion:(CLRegion *)region
{
    // present local notification
    UILocalNotification *notification = [[UILocalNotification alloc] init];
    notification.alertBody = @"Спасибо за визит, ждем Вас снова у нас!";
    notification.soundName = UILocalNotificationDefaultSoundName;
    
    [[UIApplication sharedApplication] presentLocalNotificationNow:notification];

    
    for (int i=0;i<regionsData.size();i++)
    {
        CLBeaconRegion* beaconRegion = regionsData[i].raw_data;
        
        if (String::IsEqual(beaconRegion.identifier.UTF8String, region.identifier.UTF8String))
        {
            if (regionsData[i].onExitCallback[0] && regionsData[i].exec_module)
            {
                //regionsData[i].exec_module->Call(regionsData[i].onExitCallback);
                regionsData[i].enterCalled = false;
            }
            
            break;
        }
    }
}

-(void)locationManager:(CLLocationManager *)manager didRangeBeacons:(NSArray *)beacons inRegion:(CLBeaconRegion *)region
{
    for (int i=0;i<regionsData.size();i++)
    {
        CLBeaconRegion* beaconRegion = regionsData[i].raw_data;
 
        if (String::IsEqual(beaconRegion.identifier.UTF8String, region.identifier.UTF8String))
        {
            if (regionsData[i].onEnterCallback[0] && regionsData[i].exec_module && !regionsData[i].enterCalled && beacons.count > 0)
            {
                //regionsData[i].exec_module->Call(regionsData[i].onEnterCallback);
                regionsData[i].enterCalled = true;
            }
            
            regionsData[i].beaconsData.resize(beacons.count);
    
            for (int j=0; j<beacons.count; j++)
            {
                CLBeacon* beacon = [beacons objectAtIndex:j];
    
                int prx = -1;
    
                if (beacon.proximity == CLProximityImmediate)
                {
                    prx = 1;
                }
                else
                if (beacon.proximity == CLProximityNear)
                {
                    prx = 0;
                }
                else
                if (beacon.proximity == CLProximityFar)
                {
                    prx = 2;
                }
        
                regionsData[i].beaconsData[j].major = beacon.major.integerValue;
                regionsData[i].beaconsData[j].minor = beacon.minor.integerValue;
                regionsData[i].beaconsData[j].proximity = prx;
                regionsData[i].beaconsData[j].dist = beacon.accuracy;
        
                //core->TraceTo("app", "%s %s", region.reg, region.proximityUUID.UUIDString.UTF8String, prx);
                //core->TraceTo("app", "%i %i %i %4.3f", beacon.major.integerValue, beacon.minor.integerValue, prx, beacon.accuracy);
            }
            
            break;
        }
    }
}

- (int)GetRegionBeaconsCount:(const char*)id
{
    for (int i=0;i<regionsData.size();i++)
    {
        CLBeaconRegion* beaconRegion = regionsData[i].raw_data;
        
        if (String::IsEqual(beaconRegion.identifier.UTF8String, id))
        {
            return regionsData[i].beaconsData.size();
        }
    }
    
    return 0;
}

- (int)GetRegionBeaconMinor:(const char*)id index:(int)index
{
    for (int i=0;i<regionsData.size();i++)
    {
        CLBeaconRegion* beaconRegion = regionsData[i].raw_data;
        
        if (String::IsEqual(beaconRegion.identifier.UTF8String, id))
        {
            return regionsData[i].beaconsData[index].minor;
        }
    }
    
    return 0;
}

- (int)GetRegionBeaconMajor:(const char*)id index:(int)index
{
    for (int i=0;i<regionsData.size();i++)
    {
        CLBeaconRegion* beaconRegion = regionsData[i].raw_data;
        
        if (String::IsEqual(beaconRegion.identifier.UTF8String, id))
        {
            return regionsData[i].beaconsData[index].major;
        }
    }
    
    return 0;
}

- (int)GetRegionBeaconProximity:(const char*)id index:(int)index
{
    for (int i=0;i<regionsData.size();i++)
    {
        CLBeaconRegion* beaconRegion = regionsData[i].raw_data;
        
        if (String::IsEqual(beaconRegion.identifier.UTF8String, id))
        {
            return regionsData[i].beaconsData[index].proximity;
        }
    }
    
    return 0;

}

- (float)GetRegionBeaconAccuracy:(const char*)id index:(int)index
{
    for (int i=0;i<regionsData.size();i++)
    {
        CLBeaconRegion* beaconRegion = regionsData[i].raw_data;
        
        if (String::IsEqual(beaconRegion.identifier.UTF8String, id))
        {
            return regionsData[i].beaconsData[index].dist;
        }
    }
    
    return 0;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
}

/*
- (BOOL)shouldAutorotate
{
    if ( project_orintation == 0)
    {
        return false;
    }
    
    return true;
}

- (NSUInteger)supportedInterfaceOrientations
{
    if ( project_orintation == 0)
    {
        return UIInterfaceOrientationLandscapeLeft;
    }
    else
    if ( project_orintation == 1)
    {
        return UIInterfaceOrientationPortrait;
    }

    return UIInterfaceOrientationLandscapeLeft|UIInterfaceOrientationLandscapeRight| UIInterfaceOrientationPortrait;
}

- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation
{
    if ( project_orintation == 0)
    {
        return UIInterfaceOrientationLandscapeLeft;
    }
    else
    if ( project_orintation == 1)
    {
        return UIInterfaceOrientationPortrait;
    }
    
    return UIInterfaceOrientationLandscapeLeft|UIInterfaceOrientationLandscapeRight| UIInterfaceOrientationPortrait;
}*/

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    
    if ([self isViewLoaded] && ([[self view] window] == nil))
    {
        NSLog(@"didReceiveMemoryWarning");
        
        self.view = nil;
        
        [self shutdownION];
        
        if ([EAGLContext currentContext] == context)
        {
            [EAGLContext setCurrentContext:nil];
        }
        
        context = nil;
    }
}

float gyro[3];
float prev_gyro[3];

NSTimer* timer;

CMAttitude *referenceAttitude;

-(void)doGyroUpdate
{
    CMAttitude* attitude = mmanager.deviceMotion.attitude;
    if (referenceAttitude != nil)
    {
        if (project_orintation == 0)
        {
            gyro[1] = attitude.roll - prev_gyro[0];
            gyro[2] = attitude.pitch - prev_gyro[1];
            gyro[0] = attitude.yaw - prev_gyro[2];
        }
        else
        {
            [attitude multiplyByInverseOfAttitude:referenceAttitude];
            
            gyro[0] = attitude.roll - prev_gyro[0];
            gyro[1] = attitude.pitch - prev_gyro[1];
            gyro[2] = attitude.yaw - prev_gyro[2];
        }
    }
    else
    {
        referenceAttitude = [attitude retain];
    }
    
    prev_gyro[0] = attitude.roll;
    prev_gyro[1] = attitude.pitch;
    prev_gyro[2] = attitude.yaw;
}

#include <sys/utsname.h>
NSString *machineName()
{
    struct utsname systemInfo;
    if (uname(&systemInfo) < 0) {
        return nil;
    } else {
        return [NSString stringWithCString:systemInfo.machine
                                  encoding:NSUTF8StringEncoding];
    }
}

int sv_camera_width;
int sv_camera_height;
byte* sv_buffer = 0;
bool sv_ready = false;

- (void)captureOutput:(AVCaptureOutput *)captureOutput
didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
       fromConnection:(AVCaptureConnection *)connection
{
    if (!sv_ready)
    {
        CVImageBufferRef cameraFrame = CMSampleBufferGetImageBuffer(sampleBuffer);
        CVPixelBufferLockBaseAddress(cameraFrame, 0);
    
        sv_camera_width = CVPixelBufferGetWidth(cameraFrame);
        sv_camera_height = CVPixelBufferGetHeight(cameraFrame);
    
        
        //core->TraceTo("Log", "%4.3f", (float)CVPixelBufferGetBytesPerRow(cameraFrame) / (float)sv_camera_height);
        
        if (!sv_buffer)
        {
            sv_buffer = (byte*)malloc(sv_camera_width * sv_camera_height * 4);
        }
        
        void *src_buff = CVPixelBufferGetBaseAddress(cameraFrame);
        
        memcpy(sv_buffer, src_buff, sv_camera_width * sv_camera_height * 4);
        
        CVPixelBufferUnlockBaseAddress(cameraFrame, 0);
        
        sv_ready = true;
    }
                       
    

    
    
    
    //NSLog(@"Frame...");


    /*static const GLfloat squareVertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f,  1.0f,
        1.0f,  1.0f,
    };
    
    static const GLfloat textureVertices[] = {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f,  1.0f,
        0.0f,  0.0f,
    };
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, videoFrameTexture);
    
    // Update uniform values
    glUniform1i(videoFrameUniform, 0);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, squareVertices);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, textureVertices);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);*/
}

- (void)prepareCameraFeed
{
    //holding an error
    NSError *error = nil;
    
    //create a new capture session, set the preset, create + add the video camera input
    AVCaptureSession *captureSession = [[AVCaptureSession alloc] init];
    [captureSession setSessionPreset:AVCaptureSessionPreset640x480];
    AVCaptureDevice *videoCamera = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    AVCaptureDeviceInput *videoInput = [AVCaptureDeviceInput deviceInputWithDevice:videoCamera error:&error];
    AVCaptureMetadataOutput *output = [[AVCaptureMetadataOutput alloc] init];
    [captureSession addInput:videoInput];
    [captureSession addOutput:output];
    
    [output setMetadataObjectsDelegate:self queue:dispatch_get_main_queue()];
    [output setMetadataObjectTypes:@[AVMetadataObjectTypeQRCode]];
    
    
    //set up the data ouput object, tell it to discard late video frames for no lag
    AVCaptureVideoDataOutput *dataOutput = [[AVCaptureVideoDataOutput alloc] init];
    dataOutput.alwaysDiscardsLateVideoFrames = YES;
    
    //create a new dispatch queue for all the sample buffers to be called into.grapher
    dispatch_queue_t queue;
    queue = dispatch_queue_create("cameraQueue", NULL);
    [dataOutput setSampleBufferDelegate:self queue:queue];
    

    dispatch_release(queue);
    
    //set some settings on the video data output
    NSString *key = (NSString *)kCVPixelBufferPixelFormatTypeKey;
    NSNumber *value = [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA];
    NSDictionary* videoSettings = [NSDictionary dictionaryWithObject:value forKey:key];
    [dataOutput setVideoSettings:videoSettings];
    
    //add the data output
    [captureSession addOutput:dataOutput];
    
    AVCaptureConnection *conn = [dataOutput connectionWithMediaType:AVMediaTypeVideo];
    [conn setVideoOrientation:AVCaptureVideoOrientationPortrait];
    
    //start the capture session running
    [captureSession startRunning];
}

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputMetadataObjects:(NSArray *)metadataObjects fromConnection:(AVCaptureConnection *)connection
{
    NSArray* metadataObjectTypes = @[AVMetadataObjectTypeQRCode];
    
    for (AVMetadataObject *current in metadataObjects)
    {
        if ([current isKindOfClass:[AVMetadataMachineReadableCodeObject class]]
            && [metadataObjectTypes containsObject:current.type])
        {
            NSString *scannedResult = [(AVMetadataMachineReadableCodeObject *) current stringValue];
            
            //NSLog(scannedResult);
            
            if (en_core.Mobile()->scan_listener)
            {
                en_core.Mobile()->scan_listener->OnEvent(en_core.Mobile()->scan_data, scannedResult.UTF8String);
            }
            /*if (_completionBlock) {
                _completionBlock(scannedResult);
            }*/
            
            break;
        }
    }
}

- (void)initION
{
    NSArray *arr = [NSLocale preferredLanguages];
    
    for (NSString *lan in arr)
    {
        NSLog(@"%@: %@ %@",lan, [NSLocale canonicalLanguageIdentifierFromString:lan], [[[NSLocale alloc] initWithLocaleIdentifier:lan] displayNameForKey:NSLocaleIdentifier value:lan]);
        
        en_core.SetLocale([lan UTF8String]);
        
        break;
    }
    //NSLog(language);
    
    mmanager = [[CMMotionManager alloc] init];
    
    CMDeviceMotion *deviceMotion = mmanager.deviceMotion;
    referenceAttitude = nil;
    
    CMAttitude *attitude = deviceMotion.attitude;
    referenceAttitude = [attitude retain];
    
    prev_gyro[0] = deviceMotion.attitude.roll;
    prev_gyro[1] = deviceMotion.attitude.pitch;
    prev_gyro[2] = deviceMotion.attitude.yaw;
    
    [mmanager startGyroUpdates];
    [mmanager startDeviceMotionUpdates];
    
    timer = [NSTimer scheduledTimerWithTimeInterval:1/30.0
                                             target:self
                                           selector:@selector(doGyroUpdate)
                                           userInfo:nil
                                            repeats:YES];
    
    self.view.multipleTouchEnabled = true;
    [EAGLContext setCurrentContext:context];
    
    core->Init();
    
    en_core.EN_Scene()->RegisterSceneName("Editor/system_declations.srt");
     
    IFileBuffer* fb = core->Files()->LoadFile("project");
    
    {
        float scale = 1;
        
        if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)])
        {
            scale = [[UIScreen mainScreen] scale];
        }
        
        float dpi;
        
        if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
        {
            bool isMini = false;
            
            NSString *machName = machineName();
  
            if (machName != nil)
            {
                if ([machName isEqualToString:@"iPad2,5"] ||
                    [machName isEqualToString:@"iPad2,6"] ||
                    [machName isEqualToString:@"iPad2,7"])
                {
                    isMini = true;
                }
            }
            
            if (isMini)
            {
                dpi = 163 * scale;
            }
            else
            {
                dpi = 132 * scale;
            }
        }
        else
        if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
        {
            dpi = 163 * scale;
        }
        else
        {
            dpi = 160 * scale;
        }
        
        core->TraceTo("app", "dpi - %4.3f", dpi);
        
        en_core.Render()->Display()->SetScreenDPI(dpi);
    }
    
    if (fb)
    {
        byte* ptr = fb->GetBuffer();
     
        int sz = 0;
        sz = *((int*)ptr);
        ptr += 4;
     
        for (int i=0;i<sz;i++)
        {
            en_core.EN_Scene()->RegisterSceneName((const char*)ptr);
            ptr += 256;
        }
        
        debug_info_type = *((int*)ptr);
        ptr += 4;
     
        //debug_info_type = 2;
        
        sz = *((int*)ptr);
        ptr += 4;
        
        //en_core.Render()->SetTextureType(RenderServiceIOS::tex_pvrtc);
        
        project_status_bar = *((int*)ptr);
        ptr += 4;
        
        project_orintation = *((int*)ptr);
        ptr += 4;
        
        int ref_height = *((int*)ptr);
        ptr += 4;
        
        en_core.Render()->Display()->SetReferenceHeight(ref_height);
        en_core.EN_Scene()->Load((const char*)ptr);
        //core.Scene()->Load("menu");
        
        fb->Release();
    }
    
    NSUserDefaults* def = [NSUserDefaults standardUserDefaults];
    
    NSString* vk_token = [def objectForKey:@"vk_token"];
    
    if (vk_token && vk_token.UTF8String[0])
    {
        vk_login_view_token = vk_token.UTF8String;
        
        NSString* vk_user = [def objectForKey:@"vk_user_id"];

        vk_login_view_user_id = vk_user.UTF8String;
    }
    
    NSString* cur_agentname = [[NSString alloc] initWithFormat:@"%s", core->info.device_name];
    NSString* agentname = [NSString stringWithFormat:@"Mozilla/5.0 %@ ION ENgine", cur_agentname];
    
    NSDictionary* dictionary = [NSDictionary dictionaryWithObjectsAndKeys:agentname, @"UserAgent", nil];
    [[NSUserDefaults standardUserDefaults] registerDefaults:dictionary];

    if (debug_info_type > 0)
        core->CreateNetLogger();
    
    last_time = GetCurrentTime();
}

- (void)shutdownION
{
    [EAGLContext setCurrentContext:context];
}

- (int) GetBarOffeset
{
    int offset = 0;
    
    if (![UIApplication sharedApplication].statusBarHidden)
    {
        if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 7.0f)
        {
            if (project_status_bar == 1)
            {
                offset = [UIApplication sharedApplication].statusBarFrame.size.height;
            }
        }
    }
    
    return offset;
}

- (void) RemoveDeadTouches:(NSSet*)touches
{
    for (int i=0; i<10; i++)
    {
        if (act_touches[i].touch == null)
        {
            continue;
        }

        int count = [touches count];
    
        UITouch* touch = null;
        
        for (int k=0;k<count;k++)
        {
            touch = [[touches allObjects] objectAtIndex:k];
            
            if (act_touches[i].touch == touch)
            {
                touch = act_touches[i].touch;
                break;
            }
        }
        
        if (!touch)
        {
            core->Controls()->TouchEnd(i);
            act_touches[i].touch = 0;
        }
    }
}

- (int)FindTouchIndex:(UITouch*)touch
{
    int index = -1;
    
    for (int k=0; k<10; k++)
    {
        if (act_touches[k].touch == touch)
        {
            index = k;
            break;
        }
    }
    
    return index;
}

- (void) touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
    int offset = [self GetBarOffeset];
    [self RemoveDeadTouches:[event allTouches]];
    
    UITouch* touch = null;
    CGPoint currentTouch;
	
	int count = [touches count];
	
    for (int i=0;i<count;i++)
    {
        touch = [[touches allObjects] objectAtIndex:i];
        currentTouch = [touch locationInView: [touch view]];
        
        int index = -1;
        
        for (int k=0; k<10; k++)
        {
            if (!act_touches[k].touch)
            {
                index = k;
                break;
            }
        }

        
        if (index != -1)
        {
            act_touches[index].touch = touch;
            act_touches[index].counter = 0;
            act_touches[index].need_end = false;
         
            core->Controls()->TouchStart(index, currentTouch.x * [self GetScreenScale],
                                     (currentTouch.y - offset) * [self GetScreenScale]);
        }
		
        //core->TraceTo("App","StartClick %i %4.3f %4.3f %4.3f", index, currentTouch.x, currentTouch.y, [touch timestamp]);
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    int offset = [self GetBarOffeset];
    [self RemoveDeadTouches:[event allTouches]];
    
    UITouch* touch = null;
    CGPoint currentTouch;
	
	int count = [touches count];
    
    for (int i=0;i<count;i++)
    {
        touch = [[touches allObjects] objectAtIndex:i];
        currentTouch = [touch locationInView: [touch view]];
    
        int index = [self FindTouchIndex:touch];
        
        if (index != -1)
        {
            core->Controls()->TouchMove(index, currentTouch.x * [self GetScreenScale],
                                    (currentTouch.y - offset) * [self GetScreenScale]);
        }
        
		//core->TraceTo("App","MoveClick %i %4.3f %4.3f %4.3f %4.3f %4.3f", index, prevTouch.x, prevTouch.y, currentTouch.x, currentTouch.y, [touch timestamp]);
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self touchesEnded:touches withEvent:event];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    int offset = [self GetBarOffeset];
    [self RemoveDeadTouches:[event allTouches]];
    
    UITouch* touch = null;
    CGPoint currentTouch;
    
	int count = [touches count];
	
    for (int i=0;i<count;i++)
    {
        touch = [[touches allObjects] objectAtIndex:i];
        
        currentTouch = [touch locationInView: [touch view]];
        
        int index = [self FindTouchIndex:touch];
        
        if (index != -1)
        {
            if (act_touches[index].counter != 0)
            {
                core->Controls()->TouchEnd(index);
                act_touches[index].touch = 0;
            }
            else
            {
                act_touches[index].need_end = true;
            }
        }
        
        //core->TraceTo("debug", "EndClick %i %i %4.3f %4.3f %4.3f", index, act_touches[index].counter, currentTouch.x, currentTouch.y, [touch timestamp]);
	}
}

/*
- (BOOL)hasText
{
    core->TraceTo("App","hasText");
    // Return whether there's any text present
    return YES;
}*/

- (void)ShowMap:(bool)show
{
    if (show)
    {
        self.preferredFramesPerSecond = 30;
    }
    else
    {
        self.preferredFramesPerSecond = 60;
    }
    
    if (need_cache_maps && mapView != null)
    {
        mapView.hidden = !show;
        
        if (!show)
        {
            [self DeleteAllMapObjects];
        }
    }
    else
    {
        if (show && mapView == null)
        {
            mapView = [[MKMapView alloc] initWithFrame:CGRectMake(0, 0, 1, 1)];
            mapView.delegate = self;
            //mapView.pitchEnabled = false;
            //mapView.rotateEnabled = false;
            [self.view addSubview:mapView];
        }
        else
        if (!show && mapView != null)
        {
            [mapView removeFromSuperview];
            [mapView release];
            mapView = null;
        }
    }
}

- (void)SetMapPosition:(int)x set_y:(int)y set_w:(int)w set_h:(int)h
{
    if (mapView == null) return;
    
    float scale = [self GetScreenScale];
    mapView.frame = CGRectMake(x / scale, y / scale, w / scale, h / scale);
}

- (void)AddObjectToMap:(float)x set_y:(float)y set_text:(NSString*)text
{
    if (mapView == null) return;
    
    CLLocationCoordinate2D coord;
    coord.latitude = x;
    coord.longitude = y;
    
    MKCoordinateRegion region;
    MKCoordinateSpan span;
    span.latitudeDelta = 0.00002f;
    span.longitudeDelta = 0.00002f;
    region.span = span;
    region.center = coord;
    
    MKPointAnnotation* annotation = [[MKPointAnnotation alloc] init];
    [annotation setCoordinate:coord];
    [annotation setTitle:text];
    [mapView addAnnotation:annotation];
    
    [mapView setRegion:region animated:false];
    [mapView regionThatFits: region];
    
    annotations.push_back(annotation);
}

- (void)DeleteAllMapObjects
{
    if (mapView == null) return;
    
    for (int i=0; i<annotations.size(); i++)
    {
        [mapView removeAnnotation:annotations[i]];
        [annotations[i] release];
    }
    
    annotations.clear();
}

- (void)ShowPopup:(NSString*)title set_text:(NSString*)text set_ok_text:(NSString*)ok_text
{
    allow_first_responder = -10.0f;
    UIAlertView* alert = [[[UIAlertView alloc] initWithTitle:title message:text delegate:self cancelButtonTitle:null otherButtonTitles:ok_text, nil] autorelease];
    [alert show];
}

- (void)ShowPopup:(NSString*)title set_text:(NSString*)text set_ok_text:(NSString*)ok_text set_cancel_text:(NSString*)cancel_text
{
    allow_first_responder = -10.0f;
    UIAlertView* alert = [[[UIAlertView alloc] initWithTitle:title message:text delegate:self cancelButtonTitle:cancel_text otherButtonTitles:ok_text, nil] autorelease];
    [alert show];
}

-(void)alertView:(UIAlertView*)alertView
                 clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (buttonIndex == [alertView cancelButtonIndex])
    {
        en_core.Mobile()->SetPopupResult(2);
    }
    else
    {
        en_core.Mobile()->SetPopupResult(1);
    }
    
    allow_first_responder = 1.0f;
}

#import <mach/mach.h>
#import <mach/mach_host.h>

- (void)ProcessVKShare
{
    if (vk_share_stage == 1)
    {
        if (!vk_login_view_logined)
        {
            vk_share_stage_time += en_core.GetDeltaTime();
            if (vk_share_stage_time < 1.0f) return;

            vk_share_stage = 0;
            vk_login_need_call_ion = false;
            
            [self openVKLogin];
            vk_share_stage = 2;
        }
        else
        {
            vk_share_stage = 3;
        }
    }
    else
    if (vk_share_stage == 2)
    {
        if (vk_login_view_status == 1)
        {
            if (!vk_login_view_logined)
            {
                vk_share_stage = 0;
            }
            else
            {
                vk_share_stage = 3;
            }
        }
    }
    else
    if (vk_share_stage == 3)
    {
        std::string req_out_url;
        std::string post_params;
        std::string share_vk;
        
        int pos = ios_share_extra_data.find("|<");
        
        if (pos != string::npos)
        {
            share_vk = ios_share_extra_data.substr(pos + 2);
        }
        
        if (share_vk.size() == 0)
        {
            req_out_url = "https://api.vk.com/method/wall.post?access_token=" + vk_login_view_token;
            post_params = "message=" + ios_share_text;
        }
        else
        {
            req_out_url = "https://api.vk.com/method/wall.repost?access_token=" + vk_login_view_token + "&object=wall" + share_vk;
        }

        core->Network()->StartRequest(null, null, null, req_out_url.c_str(), post_params.c_str(), "vk_post_share");
        
        vk_share_stage = 0;
    }
}

- (void)ProcessFBShare
{
    if (fb_share_stage == 1)
    {
        if (!fb_login_view_logined)
        {
            fb_share_stage_time += en_core.GetDeltaTime();
            if (fb_share_stage_time < 1.0f) return;
            
            fb_share_stage = 0;
            fb_login_need_call_ion = false;
            
            //[app_delegate LoginToFacebook:@[@"publish_actions"]];
            [app_delegate LoginToFacebook:@[@"publish_actions"] callWrite:true];
            fb_share_stage = 2;
        }
        else
        {
            fb_share_stage = 3;
        }
    }
    else
    if (fb_share_stage == 2)
    {
        if (fb_login_view_status == 1)
        {
            if (!fb_login_view_logined)
            {
                fb_share_stage = 0;
            }
            else
            {
                fb_share_stage = 3;
            }
        }
    }
    else
    if (fb_share_stage == 3)
    {
        std::string req_out_url = "https://graph.facebook.com/me/feed?access_token=" + fb_login_view_token;
        std::string post_params;
        std::string share_link;
        
        int pos = ios_share_extra_data.find("|<");
        
        if (pos != string::npos)
        {
            share_link = ios_share_extra_data.substr(0, pos);
        }
        
        post_params = "message=" + ios_share_text;
        
        if (share_link.size() > 0)
        {
            post_params += "&link=" + share_link;
        }
                
        core->Network()->StartRequest(null, null, null, req_out_url.c_str(), post_params.c_str(), "fb_post_share");
                
        fb_share_stage = 0;
    }
}


- (int)GetTitleBarHeight
{
    if (project_status_bar == 2 && [[[UIDevice currentDevice] systemVersion] floatValue] >= 7.0f)
    {
        float k = [UIApplication sharedApplication].statusBarFrame.size.height;
        k = k * [self GetScreenScale];
        
        return (int)(k + 0.5f);
    }
    
    return 0;
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    int offset = 0;
    UIScreen* mainscr = [UIScreen mainScreen];
    
    if (![UIApplication sharedApplication].statusBarHidden)
    {
        if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 7.0f)
        {
            if (project_status_bar == 1)
            {
                if (project_orintation == 0)
                {
                    if (mainscr.currentMode.size.height > mainscr.currentMode.size.width)
                    {
                        offset = [UIApplication sharedApplication].statusBarFrame.size.width * [self GetScreenScale];
                    }
                    else
                    {
                        offset = [UIApplication sharedApplication].statusBarFrame.size.height * [self GetScreenScale];
                    }
                }
                else
                {
                    if (mainscr.currentMode.size.height < mainscr.currentMode.size.width)
                    {
                        offset = [UIApplication sharedApplication].statusBarFrame.size.width * [self GetScreenScale];
                    }
                    else
                    {
                        offset = [UIApplication sharedApplication].statusBarFrame.size.height * [self GetScreenScale];
                    }
                }
        
                offset = [UIApplication sharedApplication].statusBarFrame.size.height * [self GetScreenScale];
            }
        }
    }
    
    RenderService::VideoMode mode;
    
    mode.width = rect.size.width * [self GetScreenScale];
    mode.height = rect.size.height * [self GetScreenScale];
    
    /*if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 7.1f)
    {
        if (project_orintation == 0)
        {
            mode.height = rect.size.width * [self GetScreenScale];
            mode.width = rect.size.height * [self GetScreenScale];
        }
    }*/
    
    if (ios_share_type.size() > 0)
    {
        en_core.Mobile()->SetShareRes(ios_share_type.c_str());
        ios_share_type = "";
    }
    
    
    /*core->TraceTo("log", "mode.width = %d mode.height=%d offset=%d , [UIApplication sharedApplication].statusBarFrame.size.height =%f,statusBarFrame.size.width = %f, scale = %f",
                  mode.width, mode.height, offset, [UIApplication sharedApplication].statusBarFrame.size.height , [UIApplication sharedApplication].statusBarFrame.size.width , [[UIScreen mainScreen] scale]);*/
    
    
    //mode.height -= offset;
	
    en_core.Render()->Display()->ApplyVideoMode(mode);
    
    double cur_time = GetCurrentTime();
    
	en_core.SetDeltaTime(cur_time - last_time);
    en_core.SetDeltaTime(self.timeSinceLastDraw);
    
    en_core.CountDeltaTime(true);
    
    if (timer_to_open_photo_library > 0.0f)
    {
        timer_to_open_photo_library -= en_core.GetDeltaTime();
        
        if (timer_to_open_photo_library < 0.0f)
        {
            [self StartSelectImageFromGalarery];
        }
    }

    
    [self ProcessVKShare];
    [self ProcessFBShare];
    
    if (allow_first_responder > 0.0f)
    {
        allow_first_responder -= en_core.GetDeltaTime();
        
        if (allow_first_responder < 0.01f)
        {
            allow_first_responder = -0.5f;
        }
    }
    
    /*mach_port_t host_port;
    mach_msg_type_number_t host_size;
    vm_size_t pagesize;
    
    host_port = mach_host_self();
    host_size = sizeof(vm_statistics_data_t) / sizeof(integer_t);
    host_page_size(host_port, &pagesize);
    
    vm_statistics_data_t vm_stat;
    
    if (host_statistics(host_port, HOST_VM_INFO, (host_info_t)&vm_stat, &host_size) != KERN_SUCCESS)
        NSLog(@"Failed to fetch vm statistics");
    
    natural_t mem_used = (vm_stat.active_count +
                          vm_stat.inactive_count +
                          vm_stat.wire_count) * pagesize;
    natural_t mem_free = vm_stat.free_count * pagesize;
    natural_t mem_total = mem_used + mem_free;
    //NSLog(@"used: %u free: %u total: %u", mem_used, mem_free, mem_total);
    
    //core.Render()->DrawSprite(txfg, 0.0f, 250.0f, up_left, COLOR_WHITE, 0.0f);
    core.Render()->PrintText(10, 10, up_right, COLOR_GREEN, "%i %i %i %i", core.GetFPS(), mem_used, mem_free, mem_total);
    */
    
    if (debug_info_type > 0)
    {
        en_core.Render()->Debug()->PrintText(10, 35, up_right, COLOR_GREEN, "%i", core->GetFPS());
    }
    
    if (debug_info_type == 2)
    {
        int tex_total = 0;
        int tex_loading = 0;
        int tex_memory = 0;
        
        en_core.Render()->GetTextureInfo(tex_total, tex_loading, tex_memory);
        
        float flt_tex_memory = tex_memory / (1024 * 1024);
        
        en_core.Render()->Debug()->PrintText(10, 60, up_right, COLOR_GREEN, "C: %i L: %i M: %4.1f Mb", tex_total, tex_loading, flt_tex_memory);
    }
    
    //en_core.Render()->Debug()->PrintText(10, 10, up_right, COLOR_GREEN, "%i", core->GetFPS());
    
    //en_core.Render()->PrintText(10, 10, up_right, COLOR_GREEN, "%4.3f %4.3f %4.3f", prev_gyro[0], prev_gyro[1], prev_gyro[2]);
    
    //en_core.Render()->PrintText(10, 10, up_right, COLOR_GREEN, "%i %i", mode.width, mode.height);
    
    en_core.Controls()->SetGyro(0, gyro[0] * 35);
    en_core.Controls()->SetGyro(1, gyro[1] * 35);
    en_core.Controls()->SetGyro(2, gyro[2] * 35);
    
    if (first_run)
    {
        en_core.Work();
        first_run = false;
    }
    
	en_core.Work();
    
    for (int k=0; k<10; k++)
    {
        if (!act_touches[k].touch) continue;
        act_touches[k].counter++;
        
        if (act_touches[k].need_end && act_touches[k].counter == 2)
        {
            act_touches[k].touch = 0;
            core->Controls()->TouchEnd(k);
            core->TraceTo("app", "hack on");
        }
    }
    
    //self.framesPerSecond = 60;
}

- (void)openVKLogin
{
    if (vk_login_view_token.size() > 0)
    {
        vk_login_view_status = 1;
        vk_login_view_logined = true;
        en_core.OnlineUserStats()->VK()->SetSignInStatus(true, vk_login_view_token.c_str(), vk_login_view_user_id.c_str());
    }
    else
    {
        allow_first_responder = -10.0f;
    
        NSLog(@"openVKLogin");
    
        vkLoginViewController *vk = [[vkLoginViewController alloc] init];
    
        vk.delegate = self;
        [self presentModalViewController:vk animated:YES];
        [vk release];
    }
}

- (void)StartSelectPhoto
{
    allow_first_responder = -10.0f;
    
    UIImagePickerController* picker = [[UIImagePickerController alloc] init];
    picker.delegate = self;
    
    //picker.allowsEditing = YES;
    picker.sourceType = UIImagePickerControllerSourceTypeCamera;
    
    picker.cameraDevice = UIImagePickerControllerCameraDeviceRear;
    
    [self presentModalViewController:picker animated: TRUE];
}

- (void)StartSelectImageFromGalarery
{
    allow_first_responder = -10.0f;

    UIImagePickerController* picker = [[UIImagePickerController alloc] init];
    picker.delegate = self;

    //picker.allowsEditing = YES;
    picker.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
    
    //picker.wantsFullScreenLayout = YES;
    //picker.toolbarHidden = YES;
    //picker.navigationBarHidden = YES;
    
    //[self presentModalViewController:picker animated: TRUE];
    
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad)
    {
        UIPopoverController* popover = [[UIPopoverController alloc] initWithContentViewController:picker];
    
        CGRect popoverRect = CGRectMake(10, 10, 200, 200);
    
        [popover presentPopoverFromRect:popoverRect inView:self.view permittedArrowDirections: UIPopoverArrowDirectionAny animated:YES];
    }
    else
    {
        //[self presentViewController:picker animated: TRUE completion: nil];
        [self presentModalViewController:picker animated: TRUE];
    }
}

- (void)StartSelectImage:(int)type
{
    allow_first_responder = -10.0f;
    
    if (type == 0)
    {
        [self StartSelectPhoto];
    }
    else
    if (type == 1)
    {
        [self StartSelectImageFromGalarery];
    }
    else
    if (type == 2)
    {
        UIActionSheet* actionSheet = [[UIActionSheet alloc] init] ;

        actionSheet.delegate = self;

        if (String::IsEqual(core->GetLocale(), "ru"))
        {
            actionSheet.title = @"Выберите изображение";
        
            [actionSheet addButtonWithTitle:@"Сделать фото"];
            [actionSheet addButtonWithTitle:@"Выбрать из библиотеки"];
            
            [actionSheet addButtonWithTitle:@"Отмена"];
        }
        else
        {
            actionSheet.title = @"Select Image";
            
            [actionSheet addButtonWithTitle:@"Take photo"];
            [actionSheet addButtonWithTitle:@"Chose from library"];
        
            [actionSheet addButtonWithTitle:@"Cancel"];
        }

        [actionSheet setCancelButtonIndex:(actionSheet.numberOfButtons - 1)];
    
        //[self presentModalViewController:actionSheet animated: TRUE];
        
        [actionSheet showInView:self.view];

        //[actionSheet release];
    }
}

-(void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    [actionSheet dismissWithClickedButtonIndex:buttonIndex animated:YES];
    
    if (buttonIndex == 0)
    {
        [self StartSelectPhoto];
    }
    else
    if (buttonIndex == 1)
    {
        [self StartSelectImageFromGalarery];
            
    }
    else
    if (buttonIndex == 2)
    {
        en_core.Mobile()->SetSelectImageResult(1, "");
        allow_first_responder = 1.0f;
    }
}

-(void)imagePickerController:(UIImagePickerController*)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    
    /*
    // Retrieve the image orientation from the ALAsset
    UIImageOrientation orientation = UIImageOrientationUp;
    NSNumber* orientationValue = [result valueForProperty:@"ALAssetPropertyOrientation"];
    if (orientationValue != nil) {
        orientation = [orientationValue intValue];
    }
    
    CGFloat scale  = 1;
    int angle = 0;
    if ( orientation == UIImageOrientationUp)            // default orientation
        angle = 0;
    if ( orientation == UIImageOrientationDown)          // 180 deg rotation
        angle = 180;
    if ( orientation == UIImageOrientationLeft)          // 90 deg CCW
        angle = +90;
    if ( orientation == UIImageOrientationRight)         // 90 deg CW
        angle = -90;
    //if (orientation == 3)
    CGImageRef imageRef = nil;
:imageRef
     scale:scale orientation:orientation];
    
    UIImage* image2 = [UIImage imageWithCGImage:result.defaultRepresentation.fullScreenImage
                                          scale:scale orientation:orientation];*/
    
    //UIImagePickerControllerOriginalImage

	UIImage* image;

    UIImage *sourceImage = [info valueForKey:UIImagePickerControllerOriginalImage];

	if (sourceImage.imageOrientation == 0)
    {
    	image = sourceImage;
    }
    else
    if (sourceImage.imageOrientation == 3)
    {
        CGSize imgsize = sourceImage.size;
       UIGraphicsBeginImageContext(imgsize);
       [sourceImage drawInRect:CGRectMake(0.0, 0.0, imgsize.width, imgsize.height)];
       image = UIGraphicsGetImageFromCurrentImageContext();
       UIGraphicsEndImageContext();
   }
   else
   if (sourceImage.imageOrientation == 1)
   {
       image = [UIImage imageWithCGImage:[sourceImage CGImage] scale:1.0 orientation:1];
   }

    float scale_factor = 640.0f;
    
    if (image.size.width > scale_factor || image.size.height > scale_factor)
    {
        float wgt = scale_factor;
        float hgt = scale_factor;
        
        if (image.size.width > image.size.height)
        {
            hgt = image.size.height * scale_factor / image.size.width;
        }
        else
        {
            wgt = image.size.width * scale_factor / image.size.height;
        }
        
        CGSize scaleSize = CGSizeMake(wgt, hgt);
        UIGraphicsBeginImageContextWithOptions(scaleSize, NO, 0.0f);
        [image drawInRect:CGRectMake(0.0f, 0.0f, scaleSize.width, scaleSize.height)];
        
        UIImage* img = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
        
        //[image release];
        image = img;
    }
    
    //core->TraceTo("app", [[url absoluteString] UTF8String]);
    NSData* imga_raw = UIImageJPEGRepresentation(image, 0.9);
    
    static int rand_number = 0;
    rand_number++;
    
    char name[128];
    sprintf(name, "image_%i.jpg", rand_number);
    
    IFile* file = core->Files()->OpenFile(name, IFile::write);
    
	if (file)
	{
		file->Write((byte*)imga_raw.bytes, imga_raw.length);
		file->Release();
	}
    
    en_core.Mobile()->SetSelectImageResult(0, name);
                                               
    [picker dismissViewControllerAnimated:YES completion:null];
    
    //[image release];
    allow_first_responder = 1.0f;
}

-(void)imagePickerControllerDidCancel:(UIImagePickerController*)picker
{
    en_core.Mobile()->SetSelectImageResult(1, "");
    
    [picker dismissViewControllerAnimated:YES completion:null];
    
    allow_first_responder = 1.0f;
}

-(void)navigationController:(UINavigationController *)navigationController willShowViewController:(UIViewController *)viewController animated:(BOOL)animated
{
    [[UIApplication sharedApplication] setStatusBarHidden:YES];
}

- (void)openURL
{
    allow_first_responder = -10.0f;
    
    WebViewController *url = [[WebViewController alloc] init];
    
    url.delegate = self;
    [self presentModalViewController:url animated:YES];
    [url release];
}


- (void)openVideo:(string)path
{
    allow_first_responder = -10.0f;
    int pos = path.find_last_of(";", path.size()-1);
    mediaplayer_path = path.substr(0, pos);
    mediaplayer_videoName = "";
    if (pos != path.length()-1)
        mediaplayer_videoName = path.substr(pos+1, path.length() - (pos+1) );
    //mediaplayer_path = "rtsp://r1---sn-5hn7su76.c.youtube.com/CiILENy73wIaGQlZ3Vu_YhygrxMYESARFEgGUgZ2aWRlb3MM/0/0/0/video.3gp";
    
    [MediaPlayerViewController showKxMoviViewController:self];
}

- (void)ShareTextResponce:(NSString*)activeType completed:(BOOL)completed
{
    if (completed)
    {
        ios_share_type = activeType.UTF8String;
    }
    else
    {
        ios_share_type = "failed";
    }
}

- (void)StartShareText:(NSString*)text
{
    NSArray* activityItems = [NSArray arrayWithObjects:text, nil];

    VKShareActivity* vk = [[VKShareActivity alloc] init];
    //FBShareActivity* fb = [[FBShareActivity alloc] init];
    NSArray* activities = @[vk];//, fb];
    
    UIActivityViewController* activityViewController = [[UIActivityViewController alloc] initWithActivityItems: activityItems applicationActivities:activities];

    activityViewController.modalTransitionStyle = UIModalTransitionStyleCoverVertical;
    
    [activityViewController setCompletionHandler:^(NSString* activityType, BOOL completed) { [self ShareTextResponce: activityType completed:completed]; }];
    
    //UIActivityTypePostToFacebook
    
    if (floor(NSFoundationVersionNumber) > NSFoundationVersionNumber_iOS_6_1)
    {
        activityViewController.excludedActivityTypes = @[UIActivityTypeSaveToCameraRoll, UIActivityTypeAssignToContact, UIActivityTypePrint, UIActivityTypeAirDrop, UIActivityTypeCopyToPasteboard];
    }
    else
    {
        activityViewController.excludedActivityTypes = @[UIActivityTypeSaveToCameraRoll, UIActivityTypeAssignToContact, UIActivityTypePrint, UIActivityTypeCopyToPasteboard];
    }

/*
 UIKIT_EXTERN NSString *const UIActivityTypePostToFacebook     NS_AVAILABLE_IOS(6_0);
 UIKIT_EXTERN NSString *const UIActivityTypePostToTwitter      NS_AVAILABLE_IOS(6_0);
 UIKIT_EXTERN NSString *const UIActivityTypePostToWeibo        NS_AVAILABLE_IOS(6_0);    // SinaWeibo
 UIKIT_EXTERN NSString *const UIActivityTypeMessage            NS_AVAILABLE_IOS(6_0);
 UIKIT_EXTERN NSString *const UIActivityTypeMail               NS_AVAILABLE_IOS(6_0);
 UIKIT_EXTERN NSString *const UIActivityTypePrint              NS_AVAILABLE_IOS(6_0);
 UIKIT_EXTERN NSString *const UIActivityTypeCopyToPasteboard   NS_AVAILABLE_IOS(6_0);
 UIKIT_EXTERN NSString *const UIActivityTypeAssignToContact    NS_AVAILABLE_IOS(6_0);
 UIKIT_EXTERN NSString *const UIActivityTypeSaveToCameraRoll   NS_AVAILABLE_IOS(6_0);
 UIKIT_EXTERN NSString *const UIActivityTypeAddToReadingList   NS_AVAILABLE_IOS(7_0);
 UIKIT_EXTERN NSString *const UIActivityTypePostToFlickr       NS_AVAILABLE_IOS(7_0);
 UIKIT_EXTERN NSString *const UIActivityTypePostToVimeo        NS_AVAILABLE_IOS(7_0);
 UIKIT_EXTERN NSString *const UIActivityTypePostToTencentWeibo NS_AVAILABLE_IOS(7_0);
 UIKIT_EXTERN NSString *const UIActivityTypeAirDrop            NS_AVAILABLE_IOS(7_0);*/

    [self presentViewController: activityViewController animated: YES completion: nil];
    
    if ([activityViewController respondsToSelector:@selector(popoverPresentationController)])
    {
        // iOS 8+
        UIPopoverPresentationController *presentationController = [activityViewController popoverPresentationController];
        
        presentationController.sourceView = self.view; // if button or change to self.view.
    }
}

- (void)ShareImageResponce:(NSString*)activeType completed:(BOOL)completed
{
    if (completed)
    {
        ios_share_type = activeType.UTF8String;
    }
    else
    {
        ios_share_type = "failed";
    }
}

- (void)StartShareImage:(NSString*)image
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *path = [NSString stringWithFormat:@"%@/%@", [paths objectAtIndex:0], image];
    
    
    UIImage* shareImage = [UIImage imageNamed:path];
    NSArray* activityItems = [NSArray arrayWithObjects:shareImage, nil];
    
    UIActivityViewController* activityViewController = [[UIActivityViewController alloc] initWithActivityItems: activityItems applicationActivities:null];
    
    activityViewController.modalTransitionStyle = UIModalTransitionStyleCoverVertical;
    
    activityViewController.excludedActivityTypes = @[UIActivityTypePrint, UIActivityTypeCopyToPasteboard, UIActivityTypeAssignToContact];
    
    [activityViewController setCompletionHandler:^(NSString* activityType, BOOL completed) { [self ShareImageResponce: activityType completed:completed]; }];
    
    [self presentViewController: activityViewController animated: YES completion: nil];
    
    if ([activityViewController respondsToSelector:@selector(popoverPresentationController)])
    {
        // iOS 8+
        UIPopoverPresentationController *presentationController = [activityViewController popoverPresentationController];
        
        presentationController.sourceView = self.view; // if button or change to self.view.
    }
}

-(void) didAuthWithToken:(NSString*)token
{
    NSLog(token);
    en_core.OnlineUserStats()->Instagram()->SetSignInStatus(true, token.UTF8String);
}

-(void) checkInstagramAuth
{
    InstagramAuthController *instagramAuthController = [InstagramAuthController new];
    instagramAuthController.authDelegate = self;
    
    instagramAuthController.modalPresentationStyle = UIModalPresentationFormSheet;
    instagramAuthController.modalTransitionStyle = UIModalTransitionStyleCoverVertical;
    
    [self presentViewController:instagramAuthController animated:YES completion:^{ } ];
    
    InstagramAuthController *weakAuthController = instagramAuthController;
    
    instagramAuthController.completionBlock = ^(void) {
        [weakAuthController dismissViewControllerAnimated:YES completion:nil];
    };
}

@end

/*(
@implementation EAGLView

@synthesize animating;
@dynamic animationFrameInterval;



// You must implement this method
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

//The EAGL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder
{    
    if ((self = [super initWithCoder:coder]))
    {
        {
            float scale = [UIScreen mainScreen].scale;
            self.contentScaleFactor = scale;
            self.layer.contentsScale = scale;            
        }
        
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;

        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];

        renderer = [[ES2Renderer alloc] init];

        if (!renderer)
        {
            [self release];
            return nil;
        }

        animating = FALSE;
        displayLinkSupported = FALSE;
        animationFrameInterval = 1;
        displayLink = nil;
        animationTimer = nil;

        // A system version of 3.1 or greater is required to use CADisplayLink. The NSTimer
        // class is used as fallback when it isn't available.
        NSString *reqSysVer = @"3.1";
        NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
        if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending)
            displayLinkSupported = TRUE;		
    }

    return self;
}

- (void)Update:(id)sender
{	
    double cur_time = GetCurrentTime();
    
	core.SetDeltaTime(cur_time - last_time);
    core.CountDeltaTime();
    //core.Render()->PrintText(10, 10, up_right, COLOR_GREEN, "%i", core.GetFPS());
	core.Work();
	[renderer render];
    
    last_time = cur_time;
}

- (void)layoutSubviews
{
	RenderService::VideoMode mode;	
	    
    UIScreen* mainscr = [UIScreen mainScreen];
    
    if (mainscr.currentMode.size.height > mainscr.currentMode.size.width)
    {
        mode.width = mainscr.currentMode.size.height;
        mode.height = mainscr.currentMode.size.width;
    }
    else
	{
        mode.width = mainscr.currentMode.size.width;
        mode.height = mainscr.currentMode.size.height;
	}
	
	core.Trace("video set - %i x %i",mode.width, mode.height);
	
	core.Render()->ApplyVideoMode(mode);	
	
	core.Init();
       
	/*FileBuffer* fb = core.Files()->LoadFile("tagscene");
	
	if (fb)
    {
		core.Scene()->Load((const char*)fb->GetBuffer());
		fb->Release();
	}*/
    
    /*core.Scene()->RegisterSceneName("Editor/system_declations.srt");
    
	FileBuffer* fb = core.Files()->LoadFile("project");
	
    if (fb)
    {
        byte* ptr = fb->GetBuffer();
        
        int sz = 0;
        sz = *((int*)ptr);
        ptr += 4;                
        
        for (int i=0;i<sz;i++)
        {
            core.Scene()->RegisterSceneName((const char*)ptr);
            ptr += 256;
        }
                        
        core.Scene()->Load((const char*)ptr);
        
        fb->Release();
    }
	    
    last_time = GetCurrentTime();
    
    [renderer resizeFromLayer:(CAEAGLLayer*)self.layer];
    [self Update:nil];         
}

- (NSInteger)animationFrameInterval
{
    return animationFrameInterval;
}

- (void)setAnimationFrameInterval:(NSInteger)frameInterval
{
    // Frame interval defines how many display frames must pass between each time the
    // display link fires. The display link will only fire 30 times a second when the
    // frame internal is two on a display that refreshes 60 times a second. The default
    // frame interval setting of one will fire 60 times a second when the display refreshes
    // at 60 times a second. A frame interval setting of less than one results in undefined
    // behavior.
    if (frameInterval >= 1)
    {
        animationFrameInterval = frameInterval;

        if (animating)
        {
            [self stopAnimation];
            [self startAnimation];
        }
    }
}

- (void)startAnimation
{
    if (!animating)
    {
        if (displayLinkSupported)
        {
            // CADisplayLink is API new to iPhone SDK 3.1. Compiling against earlier versions will result in a warning, but can be dismissed
            // if the system version runtime check for CADisplayLink exists in -initWithCoder:. The runtime check ensures this code will
            // not be called in system versions earlier than 3.1.

            displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(Update:)];
            [displayLink setFrameInterval:animationFrameInterval];
            [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        }
        else
            animationTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((1.0 / 60.0) * animationFrameInterval) target:self selector:@selector(Update:) userInfo:nil repeats:TRUE];

        animating = TRUE;
    }
}

- (void)stopAnimation
{
    if (animating)
    {
        if (displayLinkSupported)
        {
            [displayLink invalidate];
            displayLink = nil;
        }
        else
        {
            [animationTimer invalidate];
            animationTimer = nil;
        }

        animating = FALSE;
    }
}

- (void) touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{     
	[super touchesBegan:touches withEvent:event];
	
	NSArray *allTouches = [touches allObjects]; 
	
	int count = [allTouches count];	
	
	if (count == 1)
	{		
		CGPoint tch = [[allTouches objectAtIndex:0] locationInView:self];
		core.Controls()->StartClick(tch.x * [UIScreen mainScreen].scale,
                                    tch.y * [UIScreen mainScreen].scale);
		//core.Trace("StartClick %4.3f %4.3f", tch.x, tch.y);
	}	
} 

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	[super touchesMoved:touches withEvent:event];
		
	NSArray *allTouches = [touches allObjects]; 
	int count = [allTouches count]; Platforms
	if (count == 1)
	{
		CGPoint tch = [[allTouches objectAtIndex:0] locationInView:self];
		core.Controls()->MoveClick(tch.x * [UIScreen mainScreen].scale,
                                   tch.y * [UIScreen mainScreen].scale);

		//core.Trace("MoveClick %4.3f %4.3f", tch.x, tch.y);
	}	
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	[super touchesEnded:touches withEvent:event];
	
	
	NSArray *allTouches = [touches allObjects]; 
	int count = [allTouches count]; 
	
	if (count == 1)
	{
		core.Controls()->EndClick();
	}	
}

- (void)dealloc
{
	core.Release();
	
    [renderer release];

    [super dealloc];
}

@end*/