#pragma once

#include "ChatCommon.h"

#include <openssl/bn.h>
#include <openssl/dsa.h>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include "Core/Network/NetworkDataBuffer.h"

typedef byte OTRSSID[8];
typedef byte OTRAESKey[16];
typedef byte OTRHash1[20];
typedef byte OTRHash2[32];
typedef byte OTRMac[EVP_MAX_MD_SIZE];
typedef byte OTRMac160[20];

class OTRStaticBuffer;

class OTRNumber
{
public:
	OTRNumber(BN_CTX* ctx = NULL);
	virtual ~OTRNumber();

public:
	void Init(BN_CTX* ctx);

	void FromHEX(const char* hex);
	void FromRandom(int bit);
	void FromBuffer(const OTRStaticBuffer& buffer);

	const byte* ToBinary() const;

	int GetSize();

	void Pow(OTRNumber& res, const OTRNumber& x, const OTRNumber& p);
	int Compare(const OTRNumber& rhs);

public:
	BIGNUM* bn;
	BN_CTX* ctx;

private:
	OTRNumber(const OTRNumber&);
	void operator=(const OTRNumber&);
};

struct AESState
{
	unsigned int num;
	byte ivec[16];
	byte ecount[16];

	uint64 ctr_inital_counter;

	AESState() : ctr_inital_counter(0), num(0) {}

	void Reset()
	{
		num = 0;
		::memset(ivec, 0, sizeof(ivec));
		::memset(ecount, 0, sizeof(ecount));

		/*ivec[15] = ((ctr_inital_counter >> 56) & 0xFF);
		ivec[14] = ((ctr_inital_counter >> 48) & 0xFF);
		ivec[13] = ((ctr_inital_counter >> 40) & 0xFF);
		ivec[12] = ((ctr_inital_counter >> 32) & 0xFF);
		ivec[11] = ((ctr_inital_counter >> 24) & 0xFF);
		ivec[10] = ((ctr_inital_counter >> 16) & 0xFF);
		ivec[9] = ((ctr_inital_counter >> 8) & 0xFF);
		ivec[8] = ((ctr_inital_counter >> 0) & 0xFF);*/

		ivec[8] = ((ctr_inital_counter >> 56) & 0xFF);
		ivec[9] = ((ctr_inital_counter >> 48) & 0xFF);
		ivec[10] = ((ctr_inital_counter >> 40) & 0xFF);
		ivec[11] = ((ctr_inital_counter >> 32) & 0xFF);
		ivec[12] = ((ctr_inital_counter >> 24) & 0xFF);
		ivec[13] = ((ctr_inital_counter >> 16) & 0xFF);
		ivec[14] = ((ctr_inital_counter >> 8) & 0xFF);
		ivec[15] = ((ctr_inital_counter >> 0) & 0xFF);
	}
};

class OTRStaticBuffer : public NetStaticBuffer
{
public:
	OTRStaticBuffer(int size = 8192) : NetStaticBuffer(size)
	{
	}

	virtual void PackMPI(BIGNUM* bn);
	virtual void PackMPI(const OTRNumber& num);

	virtual void PackDATA(const OTRNumber& data);

	virtual void PackSIG(DSA* priv_key, const byte* data, int data_len);
	virtual void PackSIG(DSA* priv_key, const OTRStaticBuffer& data);

	virtual void PackAES(const byte* key, AESState& aes_state, const byte* data, int data_len);
	virtual void PackAES(const OTRNumber& key, AESState& aes_state, const byte* data, int data_len);
	virtual void PackAES(const byte* key, AESState& aes_state, const NetStaticBuffer& data);
	virtual void PackAES(const OTRNumber& key, AESState& aes_state, const NetStaticBuffer& data);

	virtual void UnpackMPI(OTRNumber& num);
	virtual void UnpackMAC160(OTRMac160 mac);

	virtual void UnpackAES(const byte* key, AESState& aes_state, NetStaticBuffer& buffer);
	virtual void UnpackAES(const OTRNumber& key, AESState& aes_state, NetStaticBuffer& buffer);

	virtual void UnpackPublicKey(OTRStaticBuffer& buffer);
};

class OTRBlockBuffer : public OTRStaticBuffer
{
public:
	OTRBlockBuffer(int block_size, int size = 8192);

	void ResetToSize(int size);

protected:
	virtual bool ProcessOverflow(int overflow_size);

private:
	int block_size_;
};

struct OTRHeader
{
	word version;
	byte type;
	dword sender_tag;
	dword receiver_tag;
};

enum OTRState
{
	kOTRInit,
	kOTRKeyExchange,
	kOTRError,

	kOTRProcessMessages,
	kOTRReadMessageMarker,
	kOTRReadMessageBody,

	kOTRGotNewMessage,
};

class OTRChatClientImpl
{	
public:
	OTRChatClientImpl();
	virtual ~OTRChatClientImpl();

	OTRState GetState() const { return state; }
	bool IsDisconnected() const { return disconnected; }

	bool Connect(const string& host, int port);
	void Disconnect();

	bool ProcessDataStream(std::vector<std::string>& new_messages);

	void SendMessage(const char* message);
	void SendCommand(const char* message);

private:
	OTRChatClientImpl(const OTRChatClientImpl&);
	void operator=(const OTRChatClientImpl&);

	static int GetMac(byte* key, int key_len, byte* data, int data_len, OTRMac mac, const EVP_MD* mode = NULL);
	static int GetMac160(byte* key, int key_len, byte* data, int data_len, OTRMac160 mac);

	static void GetHash1(byte* src, int len, OTRHash1 hash, const byte* byte = NULL);
	static void GetHash2(byte* src, int len, OTRHash2 hash, const byte* byte = NULL);

	static void GetSSID(byte* src, int len, OTRSSID ssid);
	static void GetAESKeys(byte* src, int len, OTRAESKey keya, OTRAESKey keyb);
	static void GetAESSessionKey(byte* src, int len, OTRAESKey key, const byte* byte = NULL);

	void SendData(const char* header, const char* message);

	bool ReadHeader(OTRStaticBuffer& buffer, OTRHeader& header);
	void UnpackHeader(OTRStaticBuffer& buffer, OTRHeader& header);
	void PackHeader(NetStaticBuffer& buffer, byte type);

	bool DHCommitMessage();
	bool DHReadKeyMessage();

	bool DHRevealSignatureMessage();
	bool DHReadSignatureMessage();

	bool DHCaclulateSessionKeys();

	bool ProcessMessage(const NetStaticBuffer& buffer);
	bool ProcessChar(byte ch, NetStaticBuffer& buffer);

	bool disconnected;

	OTRState state;
	int sockfd;

	dword sender_tag;
	dword receiver_tag;

	dword keyidb;
	dword keyida;

	BN_CTX* ctx;

	DSA* privb;
	OTRStaticBuffer pubb;
	OTRStaticBuffer puba;

	OTRNumber g;
	OTRNumber p;

	OTRNumber x;
	OTRNumber r;
	OTRNumber gx;

	OTRNumber gy;

	OTRStaticBuffer gxmpi;

	AESState aes_state;

	OTRStaticBuffer secbytes;

	OTRSSID ssid;
	OTRAESKey ca, cb;
	OTRHash2 m1a, m2a, m1b, m2b;

	OTRAESKey sending_aes_key;
	OTRHash1 sending_mac_key;

	OTRAESKey receiving_aes_key;
	OTRHash1 receiving_mac_key;

	NetBlockBuffer last_message;
};
