#include "Core/Core.h"

#include <curl/curl.h>
#include <curl/easy.h>


#include "openssl/hmac.h"


//#include "lib\curl_base64.h"
//#include "lib\memdebug.h"
#include "Azure.h"

//unsigned char *HMAC(const EVP_MD *evp_md, const void *key, int key_len,
//                    const unsigned char *d, size_t n, unsigned char *md,
//                    unsigned int *md_len);

AzureNet::AzureNet()
{
}

bool AzureNet::Init()
{
	core->AddTask(this, this, (TaskHolder::Task)&AzureNet::Work, -320, -1.f);

	return true;
}


void AzureNet::Work(float dt, int level)
{
}



void AzureNet::Reset()
{
	
}

void AzureNet::Release()
{
	Reset();
}

std::string rfc1123_datetime( time_t time )
{
	struct tm * timeinfo;
	char buffer [80];

	timeinfo = gmtime ( &time );
	strftime(buffer,80,"%a, %d %b %Y %H:%M:%S GMT",timeinfo);

	return buffer;
}

//std::string iso8601_datetime_milisecs( time_t time)
//{
//	struct tm * timeinfo;
//	char buffer [80];
//
//	timeinfo = gmtime ( &time );
//	strftime (buffer,80,"%Y-%m-%dT%H:%M:%SZ",timeinfo);
//
//
//#include <sys/time.h>
//	timeval time;
//	gettimeofday(&time, NULL);
//	long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
//
//
//#include "windows.h"
//		SYSTEMTIME time;
//	GetSystemTime(&time);
//	WORD millis = (time.wSeconds * 1000) + time.wMilliseconds;
//
//
//	return currentTime;
//}


void ParceIso8601()
{
	//ftp://ftp.ripe.net/test-traffic/ROOT/delayplots/iso8601.C
	//// ISO-8601 from a PostgreSQL database query.
	//std::string stime("2004-04-20 20:13:20.962245+01");

	//std::tm brokentime;

	//// Parse with strptime(3) using GNU ISO-8601 %z (timezone offset)
	//// extension.
	//char *unparsed =
	//	strptime(stime.c_str(), "%Y-%m-%d %H:%M:%S+%z", &brokentime);

	//std::string leftover = (unparsed) ? unparsed : "none";
	//std::cout << "Unparsed: " << leftover << '\n';

	//// Normalise.
	//mktime(&brokentime);

	//// Print with strftime(3) and GNU ISO-8601 %z (timezone offset)
	//// extension.
	//char buffer[40];
	//strftime(&buffer[0], 40, "%d/%m/%Y %H:%M%z", &brokentime);

	//std::cout << "Time: " << &buffer[0] << '\n'; 
}


void AzureNet::SetAuthenticationData(const char* account_name, const char* secret_key)
{
	String::Copy(this->account_name, 64, account_name);
	String::Copy(this->secret_key, 256, secret_key);
}

void AzureNet::StartRequestCreateCloudContainer(INetworkService::RequestListiner* listiner, 
	void* reqest_data, const char* containerName, const char* responseFileName)
{
	/*
	Request Syntax:
	PUT https://myaccount.blob.core.windows.net/mycontainer?restype=container HTTP/1.1

	Request Headers:
	x-ms-version: 2011-08-18
	x-ms-date: Sun, 25 Sep 2011 22:50:32 GMT
	x-ms-meta-Name: StorageSample
	Authorization: SharedKey myaccount:Z5043vY9MesKNh0PNtksNc9nbXSSqGHueE00JdjidOQ=
	*/

	//const char* request = "https://olegprog.blob.core.windows.net/test?restype=container";
	string request = string("https://")+ account_name + ".blob.core.windows.net/"
		+ containerName + "?restype=container";
	std::string GMTime = rfc1123_datetime( time(NULL) );

	/* signature
	GET\n\n\n\n\n\n\n\n\n\n\n\n
	x-ms-date:Sun, 11 Oct 2009 21:49:13 GMT\n
	x-ms-version:2009-09-19\n
	/myaccount/myaccount/mycontainered\n
	comp:metadata\n
	restype:container\n
	timeout:20
	*/

	string signature = "PUT\n\n\n0\n\n\n\n\n\n\n\n\n";
	signature += "x-ms-blob-public-access:container\n";
	signature += (string("x-ms-date:") + GMTime + "\n");
	signature += "x-ms-version:2009-09-19\n";
	//signature += "/olegprog/test\n";
	signature += string("/") + account_name + "/"+ containerName + "\n";
	signature += "restype:container";

	string secret_key(this->secret_key);

	string decoded;
	string encoded;

	decoded = base64_decode( secret_key);


	byte hash[32];
	unsigned int len = 0;
    
    //EXP
    /*EXP
	HMAC( EVP_sha256(), decoded.c_str(), strlen(decoded.c_str()),
		(byte*)signature.c_str(), strlen(signature.c_str()), hash,
		&len);
    */

	encoded = base64_encode(hash, 32);



	string headers;
	headers += string( "x-ms-blob-public-access: container" ) + "\n";
	headers += string( (string("x-ms-date: ") + GMTime).c_str() ) + "\n";
	headers += string( "x-ms-version: 2009-09-19" ) + "\n";
	headers += string( "Content-Length: 0" ) + "\n";
	//headerList.push_back( (string("Authorization: SharedKey olegprog:") + encoded).c_str());
	headers += string( (string("Authorization: SharedKey ")+ account_name+":" + encoded).c_str());

	core->Network()->StartRequest(listiner, reqest_data, "PUT",
		request.c_str(), null, headers.c_str(), null, responseFileName);

}




void AzureNet::StartRequestCloudUploadFile(INetworkService::RequestListiner* listiner, 
	void* reqest_data, const char* containerName, const char* uploadFile, const char* outputName, const char* responseFileName)
{
	/*
	Request Syntax:
	PUT https://myaccount.blob.core.windows.net/mycontainer/myblockblob HTTP/1.1

	Request Headers:
	x-ms-version: 2011-08-18
	x-ms-date: Sun, 25 Sep 2011 22:33:35 GMT
	Content-Type: text/plain; charset=UTF-8
	x-ms-blob-type: BlockBlob
	x-ms-meta-m1: v1
	x-ms-meta-m2: v2
	Authorization: SharedKey myaccount:YhuFJjN4fAR8/AmBrqBz7MG2uFinQ4rkh4dscbj598g=
	Content-Length: 11

	Request Body:
	hello world
	*/
	//const char* request = "https://olegprog.blob.core.windows.net/test/testblob1";

	char output[256];
	String::ExtractFileName(outputName, output);

	char uploadFileName[256];
	String::ExtractFileName(uploadFile, uploadFileName);
	string request = string("https://") + account_name + ".blob.core.windows.net/"+
		 containerName + "/"+ output;
	std::string GMTime = rfc1123_datetime( time(NULL) );

	//core->TraceTo("azure", "StartRequestCloudUploadFile: %s", uploadFile);
	IFile* file = core->Files()->OpenFile(uploadFile, IFile::read);

	char sizeStr[64];
	sizeStr[0] = 0;

	if (file)
	{
		sprintf(sizeStr, "%d", file->GetSize());	
		file->Release();
	}
	else
		core->TraceTo("azure", "can not open upload file: %s", uploadFile);

	

	/* signature
	GET\n\n\n\n\n\n\n\n\n\n\n\n
	x-ms-date:Sun, 11 Oct 2009 21:49:13 GMT\n
	x-ms-version:2009-09-19\n
	/myaccount/myaccount/mycontainered\n
	comp:metadata\n
	restype:container\n
	timeout:20
	*/

	string signature = string("PUT\n\n\n") + sizeStr +  "\n\n\n\n\n\n\n\n\n";
	signature += "x-ms-blob-type:BlockBlob\n";
	signature += (string("x-ms-date:") + GMTime + "\n");
	signature += "x-ms-version:2009-09-19\n";
	//signature += "/olegprog/test/testblob1";
	signature += string("/")+ account_name + "/" + containerName+"/"+ output;

	string secret_key(this->secret_key);

	string decoded;
	string encoded;

	decoded = base64_decode( secret_key);


	byte hash[32];
	unsigned int len = 0;
    
    /*EXP
	HMAC( EVP_sha256(), decoded.c_str(), strlen(decoded.c_str()),
		(byte*)signature.c_str(), strlen(signature.c_str()), hash,
		&len);
     */

	encoded = base64_encode(hash, 32);

	string headers;
	headers += string("x-ms-blob-type: BlockBlob" ) + "\n";
	headers += string((string("x-ms-date: ") + GMTime).c_str() ) + "\n";
	headers += string("x-ms-version: 2009-09-19" ) + "\n";
	headers += string((string("Content-Length: ") + sizeStr).c_str()) + "\n";
	headers += "Expect:\n";
	headers += ((string("Authorization: SharedKey ")+ account_name +":" + encoded).c_str());

	core->Network()->StartRequest(listiner, reqest_data, "PUT",
		request.c_str(), null, headers.c_str(), uploadFile, responseFileName);

}

void AzureNet::StartRequestCloudLoadFile(INetworkService::RequestListiner* listiner, 
	void* reqest_data, const char* containerName, const char* file_name, const char* responseFileName)
{
	string request = string("https://") + account_name + ".blob.core.windows.net/"+
		containerName + "/"+ file_name;

	core->Network()->StartRequest(listiner, reqest_data, "GET",
		request.c_str(), null, null, null, responseFileName);

}

void AzureNet::StartRequestCloudDeleteFile(INetworkService::RequestListiner* listiner, 
	void* reqest_data, const char* containerName, const char* file_name, const char* responseFileName)
{
	/*
	Request Syntax:
	PUT https://myaccount.blob.core.windows.net/mycontainer/myblockblob HTTP/1.1

	Request Headers:
	x-ms-version: 2011-08-18
	x-ms-date: Sun, 25 Sep 2011 22:33:35 GMT
	Content-Type: text/plain; charset=UTF-8
	x-ms-blob-type: BlockBlob
	x-ms-meta-m1: v1
	x-ms-meta-m2: v2
	Authorization: SharedKey myaccount:YhuFJjN4fAR8/AmBrqBz7MG2uFinQ4rkh4dscbj598g=
	Content-Length: 11

	Request Body:
	hello world
	*/
	//const char* request = "https://olegprog.blob.core.windows.net/test/testblob1";

	char output[256];
	String::ExtractFileName(file_name, output);


	string request = string("https://") + account_name + ".blob.core.windows.net/"+
		 containerName + "/"+ output;
	std::string GMTime = rfc1123_datetime( time(NULL) );

	/* signature
	GET\n\n\n\n\n\n\n\n\n\n\n\n
	x-ms-date:Sun, 11 Oct 2009 21:49:13 GMT\n
	x-ms-version:2009-09-19\n
	/myaccount/myaccount/mycontainered\n
	comp:metadata\n
	restype:container\n
	timeout:20
	*/

	string signature = "DELETE\n\n\n\n\n\n\n\n\n\n\n\n";
	signature += (string("x-ms-date:") + GMTime + "\n");
	signature += "x-ms-version:2009-09-19\n";
	signature += string("/")+ account_name + "/" + containerName+"/"+ output;

	string secret_key(this->secret_key);

	string decoded;
	string encoded;

	decoded = base64_decode( secret_key);


	byte hash[32];
	unsigned int len = 0;
    
    /*EXP
	HMAC( EVP_sha256(), decoded.c_str(), strlen(decoded.c_str()),
		(byte*)signature.c_str(), strlen(signature.c_str()), hash,
		&len);
     */

	encoded = base64_encode(hash, 32);

	string headers;
	headers += string((string("x-ms-date: ") + GMTime).c_str() ) + "\n";
	headers += string("x-ms-version: 2009-09-19" ) + "\n";
	headers += "Expect:\n";
	headers += ((string("Authorization: SharedKey ")+ account_name +":" + encoded).c_str());

	core->Network()->StartRequest(listiner, reqest_data, "DELETE",
		request.c_str(), null, headers.c_str(), null , responseFileName);

}

static int callsCounter = -1;
void AzureNet::StartRequestCloudCreateTable(INetworkService::RequestListiner* listiner, 
	void* reqest_data, const char* tableName,  const char* responseFileName)
{
	++callsCounter;

	char uploadFileName[256];
	//String::ExtractFileName(uploadFile, uploadFileName);
	string request = string("https://") + account_name + ".table.core.windows.net/Tables";
	std::string GMTime = rfc1123_datetime( time(NULL) );

	char tempFileName[256];
	String::Printf(tempFileName, 256, "%s%d%s",  "createTable_", callsCounter, ".xml");
	IFile* file = core->Files()->OpenFile(tempFileName, IFile::writeText);

	string xmlText;
	xmlText += "<?xml version='1.0' encoding='utf-8' standalone='yes'?> \
		<entry xmlns:d='http://schemas.microsoft.com/ado/2007/08/dataservices' \
xmlns:m='http://schemas.microsoft.com/ado/2007/08/dataservices/metadata' \
	  xmlns='http://www.w3.org/2005/Atom'> \
	  <title /> \
	  <updated>";
	//"2009-03-18T11:48:34.9840639-07:00
	  xmlText += iso8601_datetime(time(NULL));
	  xmlText += "</updated> \
	  <author> \
	  <name/> \
	  </author> \
	  <id/> \
	  <content type='application/xml'> \
	  <m:properties> \
	  <d:TableName>";
	xmlText += tableName;
	xmlText += "</d:TableName> \
	  </m:properties> \
	  </content> \
	  </entry>";

	file->Write(xmlText.c_str(), strlen(xmlText.c_str()));



	file->Release();

	file = core->Files()->OpenFile(tempFileName, IFile::read);
	char sizeStr[64];
	sizeStr[0] = 0;

	if (file)
	{
		sprintf(sizeStr, "%d", file->GetSize());		
	}
	file->Release();
	/* signature
	StringToSign = VERB + "\n" + 
	Content-MD5 + "\n" + 
	Content-Type + "\n" +
	Date + "\n" +
	CanonicalizedResource;
	*/

	string signature = string("POST\n\napplication/atom+xml\n") + GMTime +  "\n";
	signature += string("/")+ account_name + "/Tables";

	string secret_key(this->secret_key);

	string decoded;
	string encoded;

	decoded = base64_decode( secret_key);


	byte hash[32];
	unsigned int len = 0;
    
    /*EXP
	HMAC( EVP_sha256(), decoded.c_str(), strlen(decoded.c_str()),
		(byte*)signature.c_str(), strlen(signature.c_str()), hash,
		&len);
     */

	encoded = base64_encode(hash, 32);

	string headers;
	headers += string((string("Date: ") + GMTime).c_str() ) + "\n";
	headers += string("x-ms-version: 2012-02-12" ) + "\n";
	headers += string("Content-Type: application/atom+xml" ) + "\n";
	headers += string((string("Content-Length: ") + sizeStr).c_str()) + "\n";
	headers += string("DataServiceVersion: 1.0;NetFx") + "\n";
	headers += string("MaxDataServiceVersion: 1.0;NetFx") + "\n";
	////disable "Accept: */*" puted by curl 
	//headers += "Accept:\n"; 
	////disable "Expect: 100-continue" puted by curl
	//headers += "Expect:\n"; 
	headers += ((string("Authorization: SharedKey ")+ account_name +":" + encoded).c_str());

	core->Network()->StartRequest(listiner, reqest_data, 
		"POST", request.c_str(), "", headers.c_str(), tempFileName, responseFileName);

}

void AzureNet::StartRequestCloudDeleteTable(INetworkService::RequestListiner* listiner, 
	void* reqest_data, const char* tableName,  const char* responseFileName)
{
	// DELETE https://myaccount.table.core.windows.net/Tables('mytable') 
	string request = string("https://") + account_name + ".table.core.windows.net/Tables('"+tableName+"')";
	std::string GMTime = rfc1123_datetime( time(NULL) );

	
	/* signature
	StringToSign = VERB + "\n" + 
	Content-MD5 + "\n" + 
	Content-Type + "\n" +
	Date + "\n" +
	CanonicalizedResource;
	*/

	string signature = string("DELETE\n\napplication/atom+xml\n") + GMTime +  "\n";
	signature += string("/")+ account_name + "/Tables('"+tableName+"')";

	string secret_key(this->secret_key);

	string decoded;
	string encoded;

	decoded = base64_decode( secret_key);


	byte hash[32];
	unsigned int len = 0;
    
    /*EXP
	HMAC( EVP_sha256(), decoded.c_str(), strlen(decoded.c_str()),
		(byte*)signature.c_str(), strlen(signature.c_str()), hash,
		&len);*/

	encoded = base64_encode(hash, 32);

	string headers;
	headers += string((string("Date: ") + GMTime).c_str() ) + "\n";
	headers += string("x-ms-version: 2012-02-12" ) + "\n";
	headers += string("Content-Type: application/atom+xml" ) + "\n";
	headers += string("DataServiceVersion: 1.0;NetFx") + "\n";
	headers += string("MaxDataServiceVersion: 1.0;NetFx") + "\n";
	////disable "Accept: */*" puted by curl 
	//headers += "Accept:\n"; 
	////disable "Expect: 100-continue" puted by curl
	//headers += "Expect:\n"; 
	headers += ((string("Authorization: SharedKey ")+ account_name +":" + encoded).c_str());

	core->Network()->StartRequest(listiner, reqest_data, 
		"DELETE", request.c_str(), "", headers.c_str(), null, responseFileName);

}

void AzureNet::StartRequestCloudInsertOrMergeRow(INetworkService::RequestListiner* listiner, 
	void* reqest_data, const char* tableName, const char* partitionKey, const char* rowKey, const char* row, const char* responseFileName)
{
	//MERGE https://myaccount.table.core.windows.net/mytable(PartitionKey='myPartitionKey',RowKey='myRowKey1')

	string primaryKey = string("PartitionKey='") + partitionKey + "',RowKey='"+ rowKey +"'";
	string resource = string(tableName) + "(" + primaryKey + ")";
	string request = string("https://") + account_name + ".table.core.windows.net/" + resource;
	std::string GMTime = rfc1123_datetime( time(NULL) );

	const char* bodyFile = "InsertOrMerge.xml";
	IFile* file = core->Files()->OpenFile( bodyFile, IFile::writeText);

	string xmlText;
	xmlText += "<?xml version='1.0' encoding='utf-8' standalone='yes'?> \
		<entry xmlns:d='http://schemas.microsoft.com/ado/2007/08/dataservices' \
xmlns:m='http://schemas.microsoft.com/ado/2007/08/dataservices/metadata' \
	  xmlns='http://www.w3.org/2005/Atom'> \
	  <title /> \
	  <updated>";
	//"2009-03-18T11:48:34.9840639-07:00
	  xmlText += iso8601_datetime(time(NULL));
	  xmlText += "</updated> \
	  <author> \
	  <name/> \
	  </author> \
	  <id>";
	  xmlText += request;
	  xmlText += "</id> \
	  <content type='application/xml'> \
	  <m:properties>";
	  xmlText += string("<d:PartitionKey>") + partitionKey + "</d:PartitionKey>";
	  xmlText += string("<d:RowKey>")+ rowKey + "</d:RowKey>";
	  xmlText += row;
	  xmlText += "</m:properties> \
	  </content> \
	  </entry>";

	file->Write(xmlText.c_str(), strlen(xmlText.c_str()));



	file->Release();

	file = core->Files()->OpenFile( bodyFile, IFile::read);
	char sizeStr[64];
	sizeStr[0] = 0;

	if (file)
	{
		sprintf(sizeStr, "%d", file->GetSize());		
	}
	file->Release();
	/* signature
	StringToSign = VERB + "\n" + 
	Content-MD5 + "\n" + 
	Content-Type + "\n" +
	Date + "\n" +
	CanonicalizedResource;
	*/

	string signature = string("MERGE\n\napplication/atom+xml\n") + GMTime +  "\n";
	signature += string("/")+ account_name + "/" + resource;

	string secret_key(this->secret_key);

	string decoded;
	string encoded;

	decoded = base64_decode( secret_key);


	byte hash[32];
	unsigned int len = 0;
    
    /*
	HMAC( EVP_sha256(), decoded.c_str(), strlen(decoded.c_str()),
		(byte*)signature.c_str(), strlen(signature.c_str()), hash,
		&len);*/

	encoded = base64_encode(hash, 32);

	string headers;
	headers += string((string("Date: ") + GMTime).c_str() ) + "\n";
	headers += string("x-ms-version: 2012-02-12" ) + "\n";
	headers += string("Content-Type: application/atom+xml" ) + "\n";
	headers += string((string("Content-Length: ") + sizeStr).c_str()) + "\n";
	headers += string("DataServiceVersion: 1.0;NetFx") + "\n";
	headers += string("MaxDataServiceVersion: 1.0;NetFx") + "\n";
	////disable "Accept: */*" puted by curl 
	//headers += "Accept:\n"; 
	////disable "Expect: 100-continue" puted by curl
	//headers += "Expect:\n"; 
	headers += ((string("Authorization: SharedKey ")+ account_name +":" + encoded).c_str());

	core->Network()->StartRequest(listiner, reqest_data, 
		"MERGE", request.c_str(), "", headers.c_str(), bodyFile, responseFileName);

}

	void AzureNet::StartRequestCloudDeleteRow(INetworkService::RequestListiner* listiner, 
	void* reqest_data, const char* tableName, const char* partitionKey, const char* rowKey, const char* responseFileName)
{
	//  DELETE https://myaccount.table.core.windows.net/mytable(PartitionKey='myPartitionKey', RowKey='myRowKey1') 
	string primaryKey = string("PartitionKey='") + partitionKey + "',RowKey='"+ rowKey +"'";
	string resource = string(tableName) + "(" + primaryKey + ")";
	string request = string("https://") + account_name + ".table.core.windows.net/" + resource;
	std::string GMTime = rfc1123_datetime( time(NULL) );

	
	/* signature
	StringToSign = VERB + "\n" + 
	Content-MD5 + "\n" + 
	Content-Type + "\n" +
	Date + "\n" +
	CanonicalizedResource;
	*/

	string signature = string("DELETE\n\n\n") + GMTime +  "\n";
	signature += string("/")+ account_name + "/" + resource;

	string secret_key(this->secret_key);

	string decoded;
	string encoded;

	decoded = base64_decode( secret_key);


	byte hash[32];
	unsigned int len  = 0;
    
    /*
	HMAC( EVP_sha256(), decoded.c_str(), strlen(decoded.c_str()),
		(byte*)signature.c_str(), strlen(signature.c_str()), hash,
		&len);*/

	encoded = base64_encode(hash, 32);

	string headers;
	headers += string((string("Date: ") + GMTime).c_str() ) + "\n";
	headers += string("x-ms-version: 2012-02-12" ) + "\n";
	headers += string("DataServiceVersion: 1.0;NetFx") + "\n";
	headers += string("MaxDataServiceVersion: 1.0;NetFx") + "\n";
	headers += string("If-Match: *") + "\n";
	
	////disable "Accept: */*" puted by curl 
	//headers += "Accept:\n"; 
	////disable "Expect: 100-continue" puted by curl
	//headers += "Expect:\n"; 
	headers += ((string("Authorization: SharedKey ")+ account_name +":" + encoded).c_str());

	core->Network()->StartRequest(listiner, reqest_data, 
		"DELETE", request.c_str(), "", headers.c_str(), null, responseFileName);

}

void AzureNet::StartRequestCloudInsertRow(INetworkService::RequestListiner* listiner, 
	void* reqest_data, const char* tableName, const char* partitionKey, const char* rowKey, const char* row, const char* responseFileName)
{
	//POST https://myaccount.table.core.windows.net/mytable

	string resource = string(tableName);
	string request = string("https://") + account_name + ".table.core.windows.net/" + resource;
	std::string GMTime = rfc1123_datetime( time(NULL) );

	const char* bodyFile = "Insert.xml";
	IFile* file = core->Files()->OpenFile( bodyFile, IFile::writeText);

	string xmlText;
	xmlText += "<?xml version='1.0' encoding='utf-8' standalone='yes'?> \
		<entry xmlns:d='http://schemas.microsoft.com/ado/2007/08/dataservices' \
xmlns:m='http://schemas.microsoft.com/ado/2007/08/dataservices/metadata' \
	  xmlns='http://www.w3.org/2005/Atom'> \
	  <title /> \
	  <updated>";
	//"2009-03-18T11:48:34.9840639-07:00
	  xmlText += iso8601_datetime(time(NULL));
	  xmlText += "</updated> \
	  <author> \
	  <name/> \
	  </author> \
	  <id>";
	  xmlText += request;
	  xmlText += "</id> \
	  <content type='application/xml'> \
	  <m:properties>";
	  xmlText += string("<d:PartitionKey>") + partitionKey + "</d:PartitionKey>";
	  xmlText += string("<d:RowKey>")+ rowKey + "</d:RowKey>";
	  xmlText += row;
	  xmlText += "</m:properties> \
	  </content> \
	  </entry>";

	file->Write(xmlText.c_str(), strlen(xmlText.c_str()));



	file->Release();

	file = core->Files()->OpenFile( bodyFile, IFile::read);
	char sizeStr[64];
	sizeStr[0] = 0;

	if (file)
	{
		sprintf(sizeStr, "%d", file->GetSize());		
	}
	file->Release();
	/* signature
	StringToSign = VERB + "\n" + 
	Content-MD5 + "\n" + 
	Content-Type + "\n" +
	Date + "\n" +
	CanonicalizedResource;
	*/

	string signature = string("POST\n\napplication/atom+xml\n") + GMTime +  "\n";
	signature += string("/")+ account_name + "/" + resource;

	string secret_key(this->secret_key);

	string decoded;
	string encoded;

	decoded = base64_decode( secret_key);


	byte hash[32];
	unsigned int len = 0;
    //unsigned char *HMAC(const EVP_MD *evp_md, const void *key, int key_len,
     //                   const unsigned char *d, size_t n, unsigned char *md,
       //                 unsigned int *md_len);
    
    /*EXP
	HMAC( EVP_sha256(), decoded.c_str(), strlen(decoded.c_str()),
		(byte*)signature.c_str(), strlen(signature.c_str()), hash,
		&len);*/

	encoded = base64_encode(hash, 32);

	string headers;
	headers += string((string("Date: ") + GMTime).c_str() ) + "\n";
	headers += string("x-ms-version: 2012-02-12" ) + "\n";
	headers += string("Content-Type: application/atom+xml" ) + "\n";
	headers += string((string("Content-Length: ") + sizeStr).c_str()) + "\n";
	headers += string("DataServiceVersion: 1.0;NetFx") + "\n";
	headers += string("MaxDataServiceVersion: 1.0;NetFx") + "\n";
	////disable "Accept: */*" puted by curl 
	//headers += "Accept:\n"; 
	////disable "Expect: 100-continue" puted by curl
	//headers += "Expect:\n"; 
	headers += ((string("Authorization: SharedKey ")+ account_name +":" + encoded).c_str());

	core->Network()->StartRequest(listiner, reqest_data, 
		"POST", request.c_str(), "", headers.c_str(), bodyFile, responseFileName);

}

void AzureNet::StartRequestCloudQueryRows(INetworkService::RequestListiner* listiner, 
	void* reqest_data, const char* tableName, const char* params, const char* responseFileName)
{
	//GET https://myaccount.table.core.windows.net/mytable()
	//?$filter=<query-expression>&$select=<comma-separated-property-names>
	string param_str = params;
	for (int i = 0; i < param_str.size(); ++i)
	{
		if (param_str[i] == ' ')
			param_str.replace(i, 1, "%20");
	}

	string resource = string(tableName) + "()";
	string request;
	if (params && params[0])
		request = string("https://") + account_name + ".table.core.windows.net/" + resource + "?$filter="+param_str;
	else
		request = string("https://") + account_name + ".table.core.windows.net/" + resource;

	std::string GMTime = rfc1123_datetime( time(NULL) );

	
	/* signature
	StringToSign = VERB + "\n" + 
	Content-MD5 + "\n" + 
	Content-Type + "\n" +
	Date + "\n" +
	CanonicalizedResource;
	*/

	string signature = string("GET\n\n\n") + GMTime +  "\n";
	signature += string("/")+ account_name + "/" + resource;
	//if (params && params[0])
	//	signature += string("\n")+ "$filter:" + param_str;

	string secret_key(this->secret_key);

	string decoded;
	string encoded;

	decoded = base64_decode( secret_key);


	byte hash[32];
	unsigned int len = 0;
    
    /*EXP
	HMAC( EVP_sha256(), decoded.c_str(), strlen(decoded.c_str()),
		(byte*)signature.c_str(), strlen(signature.c_str()), hash,
		&len);
     */


	encoded = base64_encode(hash, 32);

	string headers;
	headers += string((string("Date: ") + GMTime).c_str() ) + "\n";
	headers += string("x-ms-version: 2012-02-12" ) + "\n";
	headers += string("DataServiceVersion: 1.0;NetFx") + "\n";
	headers += string("MaxDataServiceVersion: 1.0;NetFx") + "\n";
	headers += string("Accept: application/atom+xml") + "\n";
	headers += ((string("Authorization: SharedKey ")+ account_name +":" + encoded).c_str());

	core->Network()->StartRequest(listiner, reqest_data, 
		"GET", request.c_str(), null, headers.c_str(), null, responseFileName);

}
