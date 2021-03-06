 
#import "Tracking.h"
#import <SystemConfiguration/SCNetworkReachability.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <net/if_dl.h>
#include <ifaddrs.h>
#include "OpenUDID.h"
#import <CommonCrypto/CommonDigest.h>

@implementation Tracking

@synthesize ClientId;
@synthesize URLScheme;
@synthesize AppleId;
@synthesize UserAgent;

#define IFT_ETHER 0x6
#define SERVER @"https://ad-x.co.uk"
#define CONNECTION_TIMEOUT 4.0
//#define SERVER @"http://164.177.132.156"


- (void)sendEvent:(NSString*)event withData:(NSString*)data {
    NSMutableArray *arr1 = [[NSMutableArray alloc] initWithCapacity: 2];
    [arr1 addObject:event];
    [arr1 addObject:data];
    [self performSelectorInBackground:@selector(reportAppEventToAdX:) withObject:arr1];
    [arr1 release];
   
}

- (void)sendEvent:(NSString*)event withData:(NSString*)data andCurrency:(NSString*)currency {
    
    NSMutableArray *arr1 = [[NSMutableArray alloc] initWithCapacity: 3];
    [arr1 addObject:event];
    [arr1 addObject:data];
    [arr1 addObject:currency];
    [self performSelectorInBackground:@selector(reportAppEventToAdX:) withObject:arr1];
    [arr1 release];
}   

- (id)init {
    //referral = [[NSMutableString alloc] init];
    return self;
}

- (void)dealloc
{
    [referral release];
	[super dealloc];
}

- (NSString *) macAddress 
{
    char* macAddressString = (char*)malloc(18);
    
    int  success;
    struct ifaddrs * addrs;
    struct ifaddrs * cursor;
    const struct sockaddr_dl * dlAddr;
    const unsigned char* base;
    int i;
    
    success = getifaddrs(&addrs) == 0;
    if (success) {
        cursor = addrs;
        while (cursor != 0) {
            if ( (cursor->ifa_addr->sa_family == AF_LINK)
                && (((const struct sockaddr_dl *) cursor->ifa_addr)->sdl_type == IFT_ETHER) && strcmp("en0",  cursor->ifa_name)==0 ) {
                dlAddr = (const struct sockaddr_dl *) cursor->ifa_addr;
                base = (const unsigned char*) &dlAddr->sdl_data[dlAddr->sdl_nlen];
                strcpy(macAddressString, ""); 
                for (i = 0; i < dlAddr->sdl_alen; i++) {
                    if (i != 0) {
                        strcat(macAddressString, ":");
                    }
                    char partialAddr[3];
                    sprintf(partialAddr, "%02X", base[i]);
                    strcat(macAddressString, partialAddr);
                    
                }
            }
            cursor = cursor->ifa_next;
        }
        
        freeifaddrs(addrs);
    }    
    
    NSString* macAddress= [[[NSString alloc] initWithCString:macAddressString
                                                    encoding:NSMacOSRomanStringEncoding] autorelease];
    
    return macAddress;
}

- (NSString *) Odin1 
{
    char* macAddressString = (char*)malloc(18);
    
    int  success;
    struct ifaddrs * addrs;
    struct ifaddrs * cursor;
    const struct sockaddr_dl * dlAddr;
    const unsigned char* base;
    int i;
    NSMutableString* output = [NSMutableString stringWithCapacity:CC_SHA1_DIGEST_LENGTH * 2];
    
    success = getifaddrs(&addrs) == 0;
    if (success) {
        cursor = addrs;
        while (cursor != 0) {
            if ( (cursor->ifa_addr->sa_family == AF_LINK)
                && (((const struct sockaddr_dl *) cursor->ifa_addr)->sdl_type == IFT_ETHER) && strcmp("en0",  cursor->ifa_name)==0 ) {
                dlAddr = (const struct sockaddr_dl *) cursor->ifa_addr;
                base = (const unsigned char*) &dlAddr->sdl_data[dlAddr->sdl_nlen];
                strcpy(macAddressString, ""); 
                for (i = 0; i < dlAddr->sdl_alen; i++) {
                    if (i != 0) {
                        strcat(macAddressString, ":");
                    }
                    char partialAddr[3];
                    sprintf(partialAddr, "%02X", base[i]);
                    strcat(macAddressString, partialAddr);
                    
                }
            }
            cursor = cursor->ifa_next;
        }
        
        uint8_t digest[CC_SHA1_DIGEST_LENGTH];
        CC_SHA1(base, 6, digest);
                
        for(int i = 0; i < CC_SHA1_DIGEST_LENGTH; i++)
            [output appendFormat:@"%02x", digest[i]];
                       
        freeifaddrs(addrs);
    }    
    
    return [output copy];
}


- (void)reportAppOpenToAdXNow {
    [self reportAppOpenToAdX:true];
}

- (void)reportAppOpenToAdX:(bool)now {
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init]; // we're in a new thread here, so we need our own autorelease pool
	// Have we already reported an app open?
	NSString *documentsDirectory = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
																		NSUserDomainMask, YES) objectAtIndex:0];
	NSString *appOpenPath = [documentsDirectory stringByAppendingPathComponent:@"adx_app_open"];
	NSFileManager *fileManager = [NSFileManager defaultManager];
	NSError *parseError = nil;
    /*
     * Dont report app open until we've got the referral information.
     */
    NSString *deviceKeyPath = [documentsDirectory stringByAppendingPathComponent:@"DeviceKeyFound"];    
    if(!now && ![fileManager fileExistsAtPath:deviceKeyPath]) 
    {
	    [pool release];
        return;   
    }
    
    if(![fileManager fileExistsAtPath:appOpenPath]) {
		// Not yet reported -- report now
		NSString *appname    = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleIdentifier"];
		NSString *appversion = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleVersion"];
        
        NSString *deviceID = [[NSUserDefaults standardUserDefaults] objectForKey:@"adxurl"];    
        NSString *isu      = [self Odin1];
        NSString *macAddress = [self macAddress];
        NSString *openUDID = [OpenUDID value];
        
		NSString *appOpenEndpoint = [NSString stringWithFormat:@"%@/atrk/iOSapp?isu=%@&app_id=%@&app_name=%@&app_version=%@&clientid=%@&deviceKey=%@&tag_version=2.3&macAddress=%@&udid=%@",
                                     SERVER, isu, AppleId, appname, appversion, ClientId, deviceID, macAddress,openUDID];
        NSLog(@"%@",appOpenEndpoint);
		NSURLRequest *request = [NSURLRequest requestWithURL:[NSURL URLWithString:appOpenEndpoint]];
		NSURLResponse *response;
		NSError *ierror = nil;
		NSData *responseData = [NSURLConnection sendSynchronousRequest:request returningResponse:&response error:&ierror];
		NSString *respStr = [[NSString alloc] initWithData:responseData encoding:NSUTF8StringEncoding];
        NSLog(@"HTTP response %d %@",[(NSHTTPURLResponse *)response statusCode],respStr);
		if((!ierror) && ([(NSHTTPURLResponse *)response statusCode] == 200) ) {
            [self parseResponse:responseData error:&parseError];
		}
		[respStr release];
	}
	[pool release];
}

- (void)reportAppEventToAdX:(NSArray *)eventAndDataReference {
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init]; // we're in a new thread here, so we need our own autorelease pool
	// Have we already reported an app open?
    
    NSString *appname    = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleIdentifier"];
    
    NSString *event = [eventAndDataReference objectAtIndex:0];
    NSString *data  = [eventAndDataReference objectAtIndex:1];
    NSString *currency = ([eventAndDataReference count] > 2) ? [eventAndDataReference objectAtIndex:2] : @"";
    
    NSString *isu      = [self Odin1];
    NSString *macAddress = [self macAddress];
    NSString* openUDID  = [OpenUDID value];
    NSString *deviceKey = [[NSUserDefaults standardUserDefaults] objectForKey:@"adxurl"];    

    NSString *appOpenEndpoint = [NSString stringWithFormat:@"%@/API/event/%@/%@/%@/%@/%@/%@?macAddress=%@&udid=%@&deviceKey=%@",
                                 SERVER, ClientId, isu, appname, event, data, currency, macAddress, openUDID, deviceKey];
    NSURLRequest *request = [NSURLRequest requestWithURL:[NSURL URLWithString:appOpenEndpoint]];
    NSURLResponse *response;
    NSError *ierror = nil;
    [NSURLConnection sendSynchronousRequest:request returningResponse:&response error:&ierror];
    
    [pool release];
}

- (BOOL) connectedToNetwork
{
    // Create zero addy
    struct sockaddr_in zeroAddress;
    bzero(&zeroAddress, sizeof(zeroAddress));
    zeroAddress.sin_len = sizeof(zeroAddress);
    zeroAddress.sin_family = AF_INET;
	
    // Recover reachability flags
    SCNetworkReachabilityRef defaultRouteReachability = SCNetworkReachabilityCreateWithAddress(NULL, (struct sockaddr *)&zeroAddress);
    SCNetworkReachabilityFlags flags;
	
    BOOL didRetrieveFlags = SCNetworkReachabilityGetFlags(defaultRouteReachability, &flags);
    CFRelease(defaultRouteReachability);
	
    if (!didRetrieveFlags)
    {
        //printf("Error. Could not recover network reachability flags\n");
        return 0;
    }
	
    BOOL isReachable = flags & kSCNetworkFlagsReachable;
    BOOL needsConnection = flags & kSCNetworkFlagsConnectionRequired;
    BOOL nonWiFi = flags & kSCNetworkReachabilityFlagsTransientConnection;
    return ((isReachable && !needsConnection) || nonWiFi) ? 
    (([[[NSURLConnection alloc] initWithRequest:[NSURLRequest 
                                                requestWithURL: [NSURL URLWithString:SERVER] 
                                                cachePolicy:NSURLRequestReloadIgnoringLocalCacheData timeoutInterval:20.0] 
                                      delegate:self] autorelease]) ? YES : NO) : NO;
}

- (BOOL)recentSwish
{    
    NSNumber *deviceKeyInProgress = [[NSUserDefaults standardUserDefaults] objectForKey:@"DeviceKeyInProgress"];   
    // If we started a swich already within the last 5 minutes, don't repeat...
    if (deviceKeyInProgress != NULL && [deviceKeyInProgress longValue]+300 > [[NSDate date] timeIntervalSince1970])
        return TRUE;    
    return FALSE;
}

- (int)swishAttempts
{
    NSNumber *attempts = [[NSUserDefaults standardUserDefaults] objectForKey:@"SwishAttempts"];   
    if (attempts != NULL) return [attempts intValue]; 
    return 0;
}

-(NSString*)userAgentString
{
    NSString *s = [[NSUserDefaults standardUserDefaults] objectForKey:@"UserAgent"];   
    if (s == NULL)
        {
        UIWebView *webView = [[UIWebView alloc] initWithFrame:CGRectZero];
        NSString* secretAgent = [webView stringByEvaluatingJavaScriptFromString:@"navigator.userAgent"];
        NSLog(@"User Agent is %@",secretAgent);
        [[NSUserDefaults standardUserDefaults] setObject:secretAgent forKey:@"UserAgent"];
        [[NSUserDefaults standardUserDefaults] synchronize];
        return secretAgent;
        }
    return s;
}

- (void)reportAppOpen{
    
    UserAgent = [self userAgentString];
    [self performSelectorInBackground:@selector(doReportAppOpen) withObject:nil];
}

- (void)doReportAppOpen
{
	NSError *parseError = nil;

    referral = [[NSMutableString alloc] init];
    
    // We've done the swish, just report the App open to Ad-X.
    NSNumber *n = [[NSUserDefaults standardUserDefaults] objectForKey:@"SwishDone"];   
    if ((n != NULL) && [n boolValue] == true)
        {
        [self reportAppOpenToAdX:TRUE];
        return;
        }
    
    if ([self recentSwish]) return;
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init]; // we're in a new thread here, so we need our own autorelease pool
    
	// Record the time we're starting so we can decide is a swish is ok.
    connectionStartTime = [NSDate date];
    
    NSString *appname    = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleIdentifier"];        
    NSString *macAddress = [self macAddress];
    NSString *openUDID  = [OpenUDID value];
    NSString *odin1 = [self Odin1];
    NSString *version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleVersion"];    
    
    NSString * encodedUAgent = (NSString *)CFURLCreateStringByAddingPercentEscapes(NULL,
                                                                                   (CFStringRef)UserAgent,
                                                                                   NULL,
                                                                                   (CFStringRef)@"!*'();:@&=+$,/?%#[]",
                                                                                   kCFStringEncodingUTF8 );
    
    NSString *appOpenEndpoint = [NSString stringWithFormat:@"http://ad-x.co.uk/atrk/iOSAppOpen?isu=%@&clientID=%@&app_name=%@&app_id=%@&tag_version=2.6&macAddress=%@&udid=%@&uagent=%@&version=%@",
                                 odin1, ClientId, appname, AppleId, macAddress, openUDID, encodedUAgent,version];
    NSLog(@"%@",appOpenEndpoint);
    NSURLRequest *request = [NSURLRequest requestWithURL:[NSURL URLWithString:appOpenEndpoint]];
    NSURLResponse *response;
    NSError *ierror = nil;
    NSData *responseData = [NSURLConnection sendSynchronousRequest:request returningResponse:&response error:&ierror];
    
    NSString *respStr = [[NSString alloc] initWithData:responseData encoding:NSUTF8StringEncoding];
    NSLog(@"HTTP response %d %@",[(NSHTTPURLResponse *)response statusCode],respStr);
    if((!ierror) && ([(NSHTTPURLResponse *)response statusCode] == 200) ) {
        [self parseResponse:responseData error:&parseError];
    }
    [respStr release];  
    [pool release];    
    
}

- (BOOL)doSwish
{
    NSString *documentsDirectory = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
                                                                    NSUserDomainMask, YES) objectAtIndex:0];
    NSString *deviceKeyPath = [documentsDirectory stringByAppendingPathComponent:@"DeviceKeyFound"];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    
    // If we've got the referral don't do it again.
    if([fileManager fileExistsAtPath:deviceKeyPath]) 
        return TRUE;
   
    if ([self recentSwish]) return TRUE;
    int attempts = [self swishAttempts];
    if (attempts > 3) return TRUE;    

    NSNumber *newattempts;
    if (attempts == 0) newattempts = [NSNumber numberWithInt:1];
    else               newattempts = [NSNumber numberWithInt:attempts+1];

    //NSString *deviceKeyInProgress = [documentsDirectory stringByAppendingPathComponent:@"DeviceKeyInProgress"];
    //if([fileManager fileExistsAtPath:deviceKeyInProgress]) 
    //return TRUE;

    NSString *appname    = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleIdentifier"];
    NSString *odin1      = [self Odin1];

    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"%@/atrk/Rconv.php?tag=%@&appid=%@&adxid=%@",SERVER,URLScheme, appname, odin1]];   
    
    if ([self connectedToNetwork])
    {   
        // Save the time and number of attempts.
        NSNumber *ts = [NSNumber numberWithLong:[[NSDate date] timeIntervalSince1970]];
        [[NSUserDefaults standardUserDefaults] setObject:ts forKey:@"DeviceKeyInProgress"];
        [[NSUserDefaults standardUserDefaults] setObject:newattempts forKey:@"SwishAttempts"];
        [[NSUserDefaults standardUserDefaults] synchronize];

        //  [fileManager createFileAtPath:deviceKeyInProgress contents:nil attributes:nil];
        if (![[UIApplication sharedApplication] openURL:url]) {
            // NSLog(@"%@%@",@"Failed to open url:",[url description]);
        }
        return TRUE;
    }
    return TRUE;
}

/**
 Ad-X third party is going out to Safari and Safari is starting our application through a URL.
 This mechanism is handled here.
 */
- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url
{
	NSString *documentsDirectory = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
																		NSUserDomainMask, YES) objectAtIndex:0];
    NSString *deviceKeyPath = [documentsDirectory stringByAppendingPathComponent:@"DeviceKeyFound"];
    
    //if (!url) {  return NO; }
    
    /* Retrieve the ADX UDID */
    NSString *urlquery = [url absoluteString];    
    NSMutableDictionary *params = [[NSMutableDictionary alloc] init];
    for (NSString *param in [urlquery componentsSeparatedByString:@"&"]) {
        NSArray *elts = [param componentsSeparatedByString:@"="];
        if([elts count] < 2) continue;
        [params setObject:[elts objectAtIndex:1] forKey:[elts objectAtIndex:0]];
    }
        
    NSString *URLString = [url absoluteString];
    // Save the information to UserDefaults.
    [[NSUserDefaults standardUserDefaults] setObject:URLString forKey:@"adxurl"];
    [[NSUserDefaults standardUserDefaults] synchronize];
    
    
    
    // Create a marker file to prevent us doing this each time.
	NSFileManager *fileManager = [NSFileManager defaultManager];
    [fileManager createFileAtPath:deviceKeyPath contents:nil attributes:nil]; // successful report, mark it as such
    NSLog(@"Got the URL as %@",URLString);
    [self performSelectorInBackground:@selector(reportAppOpenToAdXNow) withObject:nil];
    return YES;
}

/*
 * Take the XML Puzzle String and parse it.
 */
- (void)parseResponse:(NSData*)data error:(NSError **)error
{	
    NSXMLParser *parser = [[NSXMLParser alloc] initWithData:data];
    // Set self as the delegate of the parser so that it will receive the parser delegate methods callbacks.
    [parser setDelegate:self];
    // Depending on the XML document you're parsing, you may want to enable these features of NSXMLParser.
    [parser setShouldProcessNamespaces:NO];
    [parser setShouldReportNamespacePrefixes:NO];
    [parser setShouldResolveExternalEntities:NO];
    
    [parser parse];
    
    NSError *parseError = [parser parserError];
    if (parseError && error) {
        *error = parseError;
    }
    
    [parser release];
}


- (void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName attributes:(NSDictionary *)attributeDict
{
    if (qName) {
        elementName = qName;
    }
	
	currentElement = elementName;
}

- (void)parser:(NSXMLParser *)parser didEndElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName
{     
    if (qName) {
        elementName = qName;
    }
    if ([currentElement isEqualToString:@"Referral"])
        {
        NSLog(@"Got Referral from server %@",referral);
        // Save the information to UserDefareportults.
        [[NSUserDefaults standardUserDefaults] setObject:referral forKey:@"url"];
        [[NSUserDefaults standardUserDefaults] synchronize];
        }
}
- (void)parser:(NSXMLParser *)parser foundCDATA:(NSData *)CDATABlock
{
}

- (void)parser:(NSXMLParser *)parser foundCharacters:(NSString *)string
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *documentsDirectory = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
																		NSUserDomainMask, YES) objectAtIndex:0];
    NSString *appOpenPath = [documentsDirectory stringByAppendingPathComponent:@"adx_app_open"];
    
    if ([currentElement isEqualToString:@"Referral"]) 
        {
        [referral appendString:string];
        }
    if ([currentElement isEqualToString:@"Success"]) 
        {
        if ([string compare:@"true"] == NSOrderedSame || [string compare:@"stop"] == NSOrderedSame)
            [fileManager createFileAtPath:appOpenPath contents:nil attributes:nil]; // successful report, mark it as such	

        // We've had a response back from an App Open - set swish to no if not already done.
        NSNumber *n = [NSNumber numberWithBool:true];
        [[NSUserDefaults standardUserDefaults] setObject:n forKey:@"SwishDone"];
        [[NSUserDefaults standardUserDefaults] synchronize];
            
        NSLog(@"Success is %@",string);
        }
    if ([currentElement isEqualToString:@"Swish"])
        {
        NSDate *secondDate = [NSDate date];
        NSTimeInterval interval = [secondDate timeIntervalSinceDate:connectionStartTime];
        if ([string compare:@"true"] == NSOrderedSame)
            {
            if (interval < CONNECTION_TIMEOUT)
                {
                NSLog(@"Swish is a go - %f seconds",interval);
                [self doSwish];
                }
            else             
                NSLog(@"Connection took %f seconds - defer",interval);
            }
        else
            {
            // Swish is 'no' so record it.
            NSNumber *n = [NSNumber numberWithBool:true];
            [[NSUserDefaults standardUserDefaults] setObject:n forKey:@"SwishDone"];
            [[NSUserDefaults standardUserDefaults] synchronize];
                    
            NSLog(@"Ok - ready to send to Ad-X");
            // Mark the swish information as found so we do send Open notifications.
            NSString *deviceKeyPath = [documentsDirectory stringByAppendingPathComponent:@"DeviceKeyFound"];        
            NSFileManager *fileManager = [NSFileManager defaultManager];
            [fileManager createFileAtPath:deviceKeyPath contents:nil attributes:nil]; // successful report, mark it as such
            }
        }

}


@end
