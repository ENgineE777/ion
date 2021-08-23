//
//  IPhoneAppDelegate.m
//  IPhone
//
//  Created by Eugene Solyanov on 1/9/12.
//  Copyright __MyCompanyName__ 2012. All rights reserved.
//

#import "IOSAppDelegate.h"
#import "IONViewController.h"

#import <FBSDKCoreKit/FBSDKCoreKit.h>
#import <FBSDKLoginKit/FBSDKLoginKit.h>

#include "Core/Core.h"

NSString* flurry_app_id = nil;
NSString* appnow_app_id = nil;
NSString* appnow_app_secret = nil;

bool fb_login_view_logined = false;
int  fb_login_view_status = 0;
bool fb_login_need_call_ion = true;
std::string fb_login_view_token;

vector<std::string> ios_event_name;
vector<std::string> ios_event_param;

char push_server_url[256];

extern int project_orintation;

OnlineUserStatsSystem::ItemBuyListiner* last_caller;
void* last_caller_data;

OnlineUserStatsSystem::ItemRestoreListiner* last_rs_caller;
void* last_rs_caller_data;

IOSAppDelegate* app_delegate = null;

@implementation IOSAppDelegate

@synthesize window;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    app_delegate = self;
    
    self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
    
    viewController = [[IONViewController alloc] init];
    
    window.rootViewController = viewController;
    [self.window makeKeyAndVisible];
    NSBundle* main_bundle = [NSBundle mainBundle];
    
    NSDictionary* cfg = nil;
    
    cfg = [main_bundle objectForInfoDictionaryKey:@"ModuleData_info"];
    
    if (cfg)
    {
        appnow_app_id = [cfg objectForKey:@"app_id"];
        
        if (appnow_app_id != null)
        {
            if (appnow_app_id.length == 0)
            {
                appnow_app_id = null;
            }
        }
        
        appnow_app_secret = [cfg objectForKey:@"app_secret"];
        
        if (appnow_app_secret != null)
        {
            if (appnow_app_secret.length == 0)
            {
                appnow_app_secret = null;
            }
        }
    }
    
    cfg = [main_bundle objectForInfoDictionaryKey:@"Flurry"];
    
    //flurry_app_id = @"JF7Y7N5PW8PK6M479X3B";
    
    if (cfg)
    {
        flurry_app_id = [cfg objectForKey:@"app_id"];
    }

    if (flurry_app_id && [flurry_app_id length] != 0)
    {
        //[Flurry setCrashReportingEnabled:YES];
        //note: iOS only allows one crash reporting tool per app; if using another, set to: NO
        //[Flurry startSession:flurry_app_id];
        
        NSString* ns_text = [NSString stringWithCString:core->info.device_id encoding:NSUTF8StringEncoding];
        //[Flurry setUserID:ns_text];
    }
    
    NSString* str = [NSString stringWithFormat:@"app%@", appnow_app_id];
    
    strcpy(push_server_url, "http://dev.appnow.com/push/api.php");
    
    return [[FBSDKApplicationDelegate sharedInstance] application:application didFinishLaunchingWithOptions:launchOptions];
}

- (void)StartRegisterInPush
{
    if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 8.0)
    {
        [[UIApplication sharedApplication] registerUserNotificationSettings:[UIUserNotificationSettings settingsForTypes:(UIUserNotificationTypeSound | UIUserNotificationTypeAlert | UIUserNotificationTypeBadge) categories:nil]];
        [[UIApplication sharedApplication] registerForRemoteNotifications];
    }
    else
    {
        [[UIApplication sharedApplication] registerForRemoteNotificationTypes:
         (UIUserNotificationTypeBadge | UIUserNotificationTypeSound | UIUserNotificationTypeAlert)];
    }
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken
{
    if (appnow_app_id == null) return;
    
    int len = [deviceToken length];
    const byte* devTokenBytes = (const byte*)[deviceToken bytes];
    
    char token[1024];
    token[0] = 0;
    
    for (int i=0; i<len; i++)
    {
        char tmp[8];
        
        sprintf(tmp, "%02x", devTokenBytes[i]);
        strcat(token, tmp);
    }
    
    core->TraceTo("app", token);
    
    const char* app_id = [appnow_app_id UTF8String];
    const char* app_secret = [appnow_app_secret UTF8String];
    
    char post_param[1024];
    
    sprintf(post_param, "cmd=register_push_device&deviceid=%s&device_token=%s&app_id=%s&app_secret=%s&platform=ios", core->info.device_id, token, app_id, app_secret);
    
    core->TraceTo("app", post_param);
    
    core->Network()->StartRequest(NULL, null, "POST", push_server_url, post_param, "token_sign");
}

- (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error
{
    NSLog(@"Error in registration, %@", error);
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    core->TraceTo("app", "from bg");
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    core->TraceTo("app", "to bg");
    
    en_core.Mobile()->SetOnPause(true);
    en_core.Mobile()->Work(0,0);
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    [UIApplication sharedApplication].applicationIconBadgeNumber = 0;
    core->TraceTo("app", "from bg");
   [FBSDKAppEvents activateApp];
}

- (void)applicationWillTerminate:(UIApplication *)application
{    
}

- (void)dealloc
{
    [window release];

    [super dealloc];
}

- (BOOL)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo
{
    core->TraceTo("app", "didReceiveRemoteNotification");
}

- (BOOL)application:(UIApplication *)application
            openURL:(NSURL *)url
  sourceApplication:(NSString *)sourceApplication
         annotation:(id)annotation
{    
    return [[FBSDKApplicationDelegate sharedInstance] application:application
                                                        openURL:url
                                              sourceApplication:sourceApplication
                                                     annotation:annotation];
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    //[self reportAppOpen];
    
    en_core.Mobile()->SetOnPause(false);
    en_core.Mobile()->Work(0,0);
}

- (void)applicationDidFinishLaunching:(UIApplication *)application
{
    //[self reportAppOpen];
}

-(void) CloseFB
{
    FBSDKLoginManager *login = [[FBSDKLoginManager alloc] init];
    
    [login logOut];

    [[NSNotificationCenter defaultCenter] removeObserver:self name:FBSDKAccessTokenDidChangeNotification object:nil];
}

- (void)observeTokenChange:(NSNotification *)notfication
{
    if ([FBSDKAccessToken currentAccessToken])
    {
        fb_login_view_token = [[FBSDKAccessToken currentAccessToken].tokenString UTF8String];
        en_core.OnlineUserStats()->Facebook()->SetSignInStatus(true, fb_login_view_token.c_str());
    }    
}

- (void)LoginToFacebook:(NSArray*)writePermissions
{
    if ([FBSDKAccessToken currentAccessToken])
    {
        bool need_login = false;
        
        if (writePermissions)
        {
            for (int i=0;i<writePermissions.count;i++)
            {
                if ([[FBSDKAccessToken currentAccessToken].permissions containsObject:writePermissions[i]])
                {
                    continue;
                }
            
                need_login = true;
                break;
            }
        }
        
        if (!need_login)
        {
            fb_login_view_token = [[FBSDKAccessToken currentAccessToken].tokenString UTF8String];
        
            fb_login_view_status = 1;
            fb_login_view_logined = true;
        
            if (fb_login_need_call_ion)
            {
                en_core.OnlineUserStats()->Facebook()->SetSignInStatus(true, fb_login_view_token.c_str());
            }
        
            [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(observeTokenChange:) name:FBSDKAccessTokenDidChangeNotification object:nil];

            return;
        }
    }
    
    FBSDKLoginManager *login = [[FBSDKLoginManager alloc] init];

    NSArray* permissions = writePermissions;
    
    if (!permissions)
    {
        permissions = @[@"public_profile"];
    }
    
    
    [login logInWithReadPermissions:permissions handler:^(FBSDKLoginManagerLoginResult *result, NSError *error)
        {
            if (error)
            {
                fb_login_view_status = 1;
                
                if (fb_login_need_call_ion)
                {
                    en_core.OnlineUserStats()->Facebook()->SetSignInStatus(false, "");
                }
            }
            else
            if (result.isCancelled)
            {
                fb_login_view_status = 1;
                
                if (fb_login_need_call_ion)
                {
                    en_core.OnlineUserStats()->Facebook()->SetSignInStatus(false, "");
                }
            }
            else
            {
                fb_login_view_token = [[FBSDKAccessToken currentAccessToken].tokenString UTF8String];
                
                fb_login_view_status = 1;
                fb_login_view_logined = true;
                
                if (fb_login_need_call_ion)
                {
                    en_core.OnlineUserStats()->Facebook()->SetSignInStatus(true, fb_login_view_token.c_str());
                }
                
                NSUserDefaults* def = [NSUserDefaults standardUserDefaults];
                
                [def setObject: @"logged" forKey:@"fb_was_logged"];
                
                [def synchronize];

                [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(observeTokenChange:) name:FBSDKAccessTokenDidChangeNotification object:nil];        
            }
    }];
}

- (void)RequestFBPermissions:(NSArray*)writePermissions
{
    if ([FBSDKAccessToken currentAccessToken])
    {
        bool need_login = false;
        
        for (int i=0;i<writePermissions.count;i++)
        {
            if ([[FBSDKAccessToken currentAccessToken].permissions containsObject:writePermissions[i]])
            {
                continue;
            }
            
            need_login = true;
            break;
        }
        
        if (!need_login)
        {
            en_core.OnlineUserStats()->Facebook()->SetRequestPermistionsRes(true);
        
            return;
        }
    }
    
    FBSDKLoginManager *login = [[FBSDKLoginManager alloc] init];
    
    [login logInWithPublishPermissions:writePermissions handler:^(FBSDKLoginManagerLoginResult *result, NSError *error)
     {
         en_core.OnlineUserStats()->Facebook()->SetRequestPermistionsRes(true);
     }];
}

- (void)TrackUserAge:(int)age
{
    if (flurry_app_id)
    {
        //[Flurry setAge:age];
    }
}

- (void)TrackUserGender:(NSString*)gender
{
    if (flurry_app_id)
    {
        //[Flurry setGender:gender];
    }
}

- (void)TrackEvent:(NSString*)name
{
    if (flurry_app_id)
    {
        if (ios_event_name.size() > 0)
        {
            NSMutableDictionary* params = [[NSMutableDictionary alloc] init];
            
            for (int i=0; i<ios_event_name.size();i++)
            {
                NSString* param_name = [[NSString alloc] initWithFormat:@"%s", ios_event_name[i].c_str()];
                NSString* param_value = [[NSString alloc] initWithFormat:@"%s", ios_event_param[i].c_str()];
                
                [params setObject:param_name forKey:param_value];
            }
    
            //[Flurry logEvent:name withParameters:params];

            ios_event_name.resize(0);
            ios_event_param.resize(0);
        }
        else
        {
            //[Flurry logEvent:name];
        }
    }
    
    //[ga_tracker send:[[GAIDictionaryBuilder createEventWithCategory:@"Default" action:name label:@"" value:nil] build]];
}

#pragma mark PushWoosh

//succesfully registered for push notifications
- (void) onDidRegisterForRemoteNotificationsWithDeviceToken:(NSString *)token
{
	NSLog(@"Registered with push token: %@", token);
}

//failed to register for push notifications
- (void) onDidFailToRegisterForRemoteNotificationsWithError:(NSError *)error
{
	NSLog(@"Failed to register: %@", [error description]);
}

/*
//user pressed OK on the push notification
- (void) onPushAccepted:(PushNotificationManager *)pushManager withNotification:(NSDictionary *)pushNotification
{
	[PushNotificationManager clearNotificationCenter];
	
	NSLog(@"Received push notification: %@", pushNotification);
    NSString* userdata = [pushNotification objectForKey:@"u"];

    if (userdata)
    {
        en_core.PushNotification()->ProcessData([userdata UTF8String]);
    }
    else
    {
        en_core.PushNotification()->ProcessData("");
    }
}*/

#pragma mark -

@end

@implementation InAppPurchaseManager

// InAppPurchaseManager.m
- (void)requestProUpgradeProductData
{    
    NSSet *productIdentifiers = [NSSet setWithObject:productId ];
    productsRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:productIdentifiers];
    productsRequest.delegate = self;
    [productsRequest start];
    
    // we will release the request object in the delegate callback
}

#pragma mark -
#pragma mark SKProductsRequestDelegate methods

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
    NSArray *products = response.products;
    proUpgradeProduct = [products count] == 1 ? [[products firstObject] retain] : nil;
    
    if (proUpgradeProduct)
    {
        NSLog(@"Product title: %@" , proUpgradeProduct.localizedTitle);
        NSLog(@"Product description: %@" , proUpgradeProduct.localizedDescription);
        NSLog(@"Product price: %@" , proUpgradeProduct.price);
        NSLog(@"Product id: %@" , proUpgradeProduct.productIdentifier);
    }
    
    for (NSString *invalidProductId in response.invalidProductIdentifiers)
    {
        NSLog(@"Invalid product id: %@" , invalidProductId);
    }
    
    // finally release the reqest we alloc/init’ed in requestProUpgradeProductData
    [productsRequest release];
    
    [[NSNotificationCenter defaultCenter] postNotificationName:kInAppPurchaseManagerProductsFetchedNotification object:self userInfo:nil];
}

//
// call this method once on startup
//
- (void)loadStore
{
    // restarts any purchases if they were interrupted last time the app was open
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    
    // get the product description (defined in early sections)
    //[self requestProUpgradeProductData];
}

//
// call this before making a purchase
//
- (BOOL)canMakePurchases
{
    return [SKPaymentQueue canMakePayments];
}

//
// kick off the upgrade transaction
//
- (void)purchase:(NSString*)set_productId
{
    productId = set_productId;
    
    //[self requestProUpgradeProductData];
    
    SKPayment *payment = [SKPayment paymentWithProductIdentifier:productId];
    [[SKPaymentQueue defaultQueue] addPayment:payment];
}

#pragma -
#pragma Purchase helpers

//
// saves a record of the transaction by storing the receipt to disk
//
- (void)recordTransaction:(SKPaymentTransaction *)transaction
{
    if ([transaction.payment.productIdentifier isEqualToString:productId])
    {
        // save the transaction receipt to disk
        //[[NSUserDefaults standardUserDefaults] setValue:transaction.transactionReceipt forKey:@"proUpgradeTransactionReceipt" ];
        //[[NSUserDefaults standardUserDefaults] synchronize];
    }
}

//
// enable pro features
//
- (void)provideContent:(NSString*)productId
{
    if ([productId isEqualToString:productId])
    {
        // enable the pro features
        //[[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"isProUpgradePurchased" ];
        //[[NSUserDefaults standardUserDefaults] synchronize];
        
        if (last_caller)
        {
            last_caller->OnItemBuySuccess(last_caller_data);
            last_caller = null;
        }
    }
}

//
// removes the transaction from the queue and posts a notification with the transaction result
//
- (void)finishTransaction:(SKPaymentTransaction *)transaction wasSuccessful:(BOOL)wasSuccessful
{
    // remove the transaction from the payment queue.
    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
    
    NSDictionary *userInfo = [NSDictionary dictionaryWithObjectsAndKeys:transaction, @"transaction" , nil];
    if (wasSuccessful)
    {
        // send out a notification that we’ve finished the transaction
        [[NSNotificationCenter defaultCenter] postNotificationName:kInAppPurchaseManagerTransactionSucceededNotification object:self userInfo:userInfo];
    }
    else
    {
        // send out a notification for the failed transaction
        [[NSNotificationCenter defaultCenter] postNotificationName:kInAppPurchaseManagerTransactionFailedNotification object:self userInfo:userInfo];
    }
}

//
// called when the transaction was successful
//
- (void)completeTransaction:(SKPaymentTransaction *)transaction
{
    [self recordTransaction:transaction];
    [self provideContent:transaction.payment.productIdentifier];
    [self finishTransaction:transaction wasSuccessful:YES];
}

//
// called when a transaction has been restored and and successfully completed
//
- (void)restoreTransaction:(SKPaymentTransaction *)transaction
{
    [self recordTransaction:transaction.originalTransaction];
    [self provideContent:transaction.originalTransaction.payment.productIdentifier];
    [self finishTransaction:transaction wasSuccessful:YES];
}

//
// called when a transaction has failed
//
- (void)failedTransaction:(SKPaymentTransaction *)transaction
{
    if (transaction.error.code != SKErrorPaymentCancelled)
    {
        // error!
        [self finishTransaction:transaction wasSuccessful:NO];    
        
        if (last_caller)
        {
            last_caller->OnItemBuyFail(last_caller_data);
            last_caller = null;
        }
    }
    else
    {
        // this is fine, the user just cancelled, so don’t notify
        [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
        
        last_caller = null;        
    }    
}

#pragma mark -
#pragma mark SKPaymentTransactionObserver methods

//
// called when the transaction status is updated
//
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
    for (SKPaymentTransaction *transaction in transactions)
    {
        switch (transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchased:
                [self completeTransaction:transaction];
                break;
            case SKPaymentTransactionStateFailed:
                [self failedTransaction:transaction];
                break;
            case SKPaymentTransactionStateRestored:
                [self restoreTransaction:transaction];
                break;
            default:
                break;
        }
    }
}

- (void)RestorePurchase
{
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    [[SKPaymentQueue defaultQueue]restoreCompletedTransactions];
}

- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue
{
    NSLog(@"%@",queue );
    NSLog(@"Restored Transactions are once again in Queue for purchasing %@",[queue transactions]);  
    
    NSMutableArray *purchasedItemIDs = [[NSMutableArray alloc] init];
    NSLog(@"received restored transactions: %i", queue.transactions.count);
    
    for (SKPaymentTransaction *transaction in queue.transactions)
    {                
        NSString *productID = transaction.payment.productIdentifier;
        [purchasedItemIDs addObject:productID];
        NSLog (@"product id is %@" , productID);
        // here put an if/then statement to write files based on previously purchased items
        // example if ([productID isequaltostring: @"youruniqueproductidentifier]){write files} else { nslog sorry}
        
        const char* product_id = [productID UTF8String];
        
        if (last_rs_caller)
        {
            last_rs_caller->OnItemRestoreSuccess(last_rs_caller_data, product_id);            
        }
    }  
    
    last_rs_caller = null;
}

@end
