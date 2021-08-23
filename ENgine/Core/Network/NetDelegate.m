

#import "NetDelegate.h"
#include "Core/Core.h"


extern NetworkService::CURLRequest* hack_CURLRequest[7];

@implementation NetDelegate

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
    
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
    if (hack_CURLRequest[self.index] && !hack_CURLRequest[self.index]->stream)
    {
        char fl_nm[512];
        sprintf(fl_nm, "%s_tmp", hack_CURLRequest[self.index]->full_file_name);
        
        hack_CURLRequest[self.index]->stream=fopen(fl_nm, "wb");
        
        /*if (!hack_CURLRequest->stream)
        {
            core->TraceTo("Network", "can't open %s", out->file_name);
        }*/
    }
    
    fwrite([data bytes], [data length], 1, hack_CURLRequest[self.index]->stream);
}

- (NSCachedURLResponse *)connection:(NSURLConnection *)connection
                  willCacheResponse:(NSCachedURLResponse*)cachedResponse
{
    return nil;
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
    hack_CURLRequest[self.index]->state = NetworkService::rq_finished;
    hack_CURLRequest[self.index]->res = 0;
    
    if (hack_CURLRequest[self.index]->stream)
    {
        fclose(hack_CURLRequest[self.index]->stream);
        hack_CURLRequest[self.index]->stream = null;
        
        if (hack_CURLRequest[self.index]->res != 0)
        {
            char fl_nm[512];
            sprintf(fl_nm, "%s_tmp", hack_CURLRequest[self.index]->full_file_name);
            
            DeleteFile(fl_nm);
        }
    }
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
    hack_CURLRequest[self.index]->state = NetworkService::rq_finished;
    hack_CURLRequest[self.index]->res = 1;
    
    if (hack_CURLRequest[self.index]->stream)
    {
        fclose(hack_CURLRequest[self.index]->stream);
        hack_CURLRequest[self.index]->stream = null;
    }
}

@end
