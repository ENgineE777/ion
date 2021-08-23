#pragma once

#include "Core/Network/INetworkService.h"

class AzureNet : public TaskHoldersOwner
{
public: 
	
	AzureNet();
	virtual bool Init();		
	virtual void Work(float dt, int level);
	virtual void Reset();
	virtual void Release();

	virtual void SetAuthenticationData(const char* account_name, const char* secret_key);
	virtual void StartRequestCreateCloudContainer(INetworkService::RequestListiner* listiner, 
		void* reqest_data, const char* containerName, const char* responseFileName);
	virtual void StartRequestCloudUploadFile(INetworkService::RequestListiner* listiner, 
		void* reqest_data, const char* containerName, const char* uploadFile,const char* outputName,  const char* responseFileName);

	virtual void StartRequestCloudLoadFile(INetworkService::RequestListiner* listiner, 
		void* reqest_data, const char* containerName, const char* file_name, const char* responseFileName);

	virtual void StartRequestCloudDeleteFile(INetworkService::RequestListiner* listiner, 
		void* reqest_data, const char* containerName, const char* file_name, const char* responseFileName);
	virtual void StartRequestCloudCreateTable(INetworkService::RequestListiner* listiner, 
		void* reqest_data, const char* tableName, const char* responseFileName);
	virtual void StartRequestCloudDeleteTable(INetworkService::RequestListiner* listiner, 
		void* reqest_data, const char* tableName, const char* responseFileName);

	virtual void StartRequestCloudInsertOrMergeRow(INetworkService::RequestListiner* listiner, 
		void* reqest_data, const char* tableName, const char* partitionKey, const char* rowKey,
		const char* row, const char* responseFileName);

	virtual void StartRequestCloudDeleteRow(INetworkService::RequestListiner* listiner, 
		void* reqest_data, const char* tableName, const char* partitionKey, const char* rowKey, const char* responseFileName);

	virtual void StartRequestCloudInsertRow(INetworkService::RequestListiner* listiner, 
		void* reqest_data, const char* tableName, const char* partitionKey, const char* rowKey,
		const char* row, const char* responseFileName);

	virtual void StartRequestCloudQueryRows(INetworkService::RequestListiner* listiner, 
		void* reqest_data, const char* tableName, const char* params, const char* responseFileName);

private:
	char account_name[64];
	char secret_key[256];
};
