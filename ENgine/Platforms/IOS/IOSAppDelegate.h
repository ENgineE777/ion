//
//  IPhoneAppDelegate.h
//  IPhone
//
//  Created by Eugene Solyanov on 1/9/12.
//  Copyright __MyCompanyName__ 2012. All rights reserved.
//

#import <UIKit/UIKit.h>

@class IONViewController;

@interface IOSAppDelegate : NSObject <UIApplicationDelegate/*, PushNotificationDelegate*/>
{
    UIWindow* window;
    IONViewController* viewController;
}

- (void)LoginToFacebook;

@end


#import <StoreKit/StoreKit.h>

#define kInAppPurchaseManagerProductsFetchedNotification @"kInAppPurchaseManagerProductsFetchedNotification"
#define kInAppPurchaseManagerTransactionFailedNotification @"kInAppPurchaseManagerTransactionFailedNotification"
#define kInAppPurchaseManagerTransactionSucceededNotification @"kInAppPurchaseManagerTransactionSucceededNotification"

@interface InAppPurchaseManager : NSObject <SKProductsRequestDelegate, SKPaymentTransactionObserver>
{
    SKProduct* proUpgradeProduct;
    SKProductsRequest* productsRequest;
    NSString* productId;
};

// public methods
- (void)loadStore;
- (BOOL)canMakePurchases;
- (void)purchase:(NSString*)set_productId;
- (void)RestorePurchase;

@end