
#ifdef PC
#include <WinSock2.h>
#endif

#include "OTRChatClientImpl.h"

static const char* DH1536_MODULUS_S =
    "FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1"
    "29024E088A67CC74020BBEA63B139B22514A08798E3404DD"
    "EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245"
    "E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED"
    "EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE45B3D"
    "C2007CB8A163BF0598DA48361C55D39A69163FA8FD24CF5F"
    "83655D23DCA3AD961C62F356208552BB9ED529077096966D"
    "670C354E4ABC9804F1746C08CA237327FFFFFFFFFFFFFFFF";

static const char *DH1536_GENERATOR_S = "02";

#ifndef PC
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <poll.h>
#endif
#include "Core/Network/Sockets.h"

#if defined(OSUNIX) || defined(ANDROID)
#include <unistd.h>
#endif

OTRNumber::OTRNumber(BN_CTX* ctx) : bn(NULL), ctx(NULL)
{
	Init(ctx);
}

OTRNumber::~OTRNumber()
{
	if (bn)
	{
		BN_free(bn);
		bn = NULL;
	}
}

void OTRNumber::Init(BN_CTX* set_ctx)
{
	ctx = set_ctx;

	if (ctx)
	{
		bn = BN_new();
	}
}

void OTRNumber::FromHEX(const char* hex)
{
	BN_hex2bn(&bn, hex);
}

void OTRNumber::FromRandom(int bit)
{
	BN_rand(bn, bit, 0, 1);
}

void OTRNumber::FromBuffer(const OTRStaticBuffer& buffer)
{
	BN_bin2bn(&buffer[0], buffer.GetSize(), bn);
}

int OTRNumber::GetSize()
{
	return BN_num_bytes(bn);
}

void OTRNumber::Pow(OTRNumber& res, const OTRNumber& x, const OTRNumber& p)
{
	BN_mod_exp(res.bn, bn, x.bn, p.bn, ctx);
}

int OTRNumber::Compare(const OTRNumber& rhs)
{
	return BN_cmp(bn, rhs.bn);
}

const byte* OTRNumber::ToBinary() const
{
	byte* buffer = new byte[BN_num_bytes(bn)];
	BN_bn2bin(bn, buffer);
	return buffer;
}

void OTRStaticBuffer::PackMPI(BIGNUM* bn)
{
	const int len = BN_bn2mpi(bn, NULL);

	if (GetSize() + len >= max_size_ && !ProcessOverflow(len))
	{
		return;
	}

	BN_bn2mpi(bn, pos_);
	pos_ += len;
}

void OTRStaticBuffer::PackMPI(const OTRNumber& num)
{
	PackMPI(num.bn);
}

void OTRStaticBuffer::PackDATA(const OTRNumber& data)
{
	const int sz = BN_num_bytes(data.bn);

	PackDWORD(sz);

	if (GetSize() + sz >= max_size_ && !ProcessOverflow(sz))
	{
		return;
	}

	BN_bn2bin(data.bn, pos_);
	pos_ += sz;
}

void OTRStaticBuffer::PackSIG(DSA* priv_key, const byte* data, int data_len)
{
	BIGNUM *kinv=NULL,*r=NULL,*s=NULL;
	BIGNUM m;
	BIGNUM xr;
	BN_CTX *ctx=NULL;

	BN_init(&m);
	BN_init(&xr);

	s = BN_new();
	ctx = BN_CTX_new();
	
	DSA_sign_setup(priv_key,ctx,&kinv,&r);

	BN_bin2bn(data,data_len,&m);

	BN_mod_mul(&xr,priv_key->priv_key,r,priv_key->q,ctx);
	BN_add(s, &xr, &m);
	BN_mod_mul(s,s,kinv,priv_key->q,ctx);

	PackMPI(r);
	PackMPI(s);

	if (ctx != NULL) BN_CTX_free(ctx);
	BN_clear_free(&m);
	BN_clear_free(&xr);
	if (kinv != NULL) BN_clear_free(kinv);
}

void OTRStaticBuffer::PackSIG(DSA* priv_key, const OTRStaticBuffer& data)
{
	PackSIG(priv_key, &data[0], data.GetSize());
}

void OTRStaticBuffer::PackAES(const byte* key, AESState& aes_state, const byte* data, int data_len)
{
	AES_KEY aes_key;
	AES_set_encrypt_key(key, 128, &aes_key);

	PackDWORD(data_len);

	if (GetSize() + data_len >= max_size_ && !ProcessOverflow(data_len))
	{
		return;
	}

	aes_state.Reset();
	AES_ctr128_encrypt(data, pos_, data_len, &aes_key, aes_state.ivec, aes_state.ecount, &aes_state.num);

	pos_ += data_len;
}

void OTRStaticBuffer::PackAES(const OTRNumber& key, AESState& aes_state, const byte* data, int data_len)
{
	const byte* key_buf = key.ToBinary();

	PackAES(key_buf, aes_state, data, data_len);
	
	delete[] key_buf;
}

void OTRStaticBuffer::PackAES(const byte* key, AESState& aes_state, const NetStaticBuffer& data)
{
	PackAES(key, aes_state, &data[0], data.GetSize());
}

void OTRStaticBuffer::PackAES(const OTRNumber& key, AESState& aes_state, const NetStaticBuffer& data)
{
	PackAES(key, aes_state, &data[0], data.GetSize());
}

void OTRStaticBuffer::UnpackMPI(OTRNumber& num)
{
	dword len = UnpackDWORD();
	BN_bin2bn(pos_, len, num.bn);
	pos_ += len;
}

void OTRStaticBuffer::UnpackMAC160(OTRMac160 mac)
{
	::memcpy(pos_, mac, sizeof(OTRMac160));
	pos_ += sizeof(OTRMac160);
}

void OTRStaticBuffer::UnpackAES(const byte* key, AESState& aes_state, NetStaticBuffer& dst)
{
	AES_KEY aes_key;
	AES_set_encrypt_key(key, 128, &aes_key);

	aes_state.Reset();
	AES_ctr128_encrypt(buffer_, &dst[0], used_size_, &aes_key, aes_state.ivec, aes_state.ecount, &aes_state.num);

	dst.SetUsedSize(used_size_);
}

void OTRStaticBuffer::UnpackAES(const OTRNumber& key, AESState& aes_state, NetStaticBuffer& dst)
{
	const byte* key_buf = key.ToBinary();

	UnpackAES(key_buf, aes_state, dst);
	
	delete[] key_buf;
}

void OTRStaticBuffer::UnpackPublicKey(OTRStaticBuffer& buffer)
{
	int total_len = sizeof(short), len = 0;
	
	BIN2DWORD(pos_ + total_len, len); total_len += len + sizeof(len);
	BIN2DWORD(pos_ + total_len, len); total_len += len + sizeof(len);
	BIN2DWORD(pos_ + total_len, len); total_len += len + sizeof(len);
	BIN2DWORD(pos_ + total_len, len); total_len += len + sizeof(len);
	
	::memcpy(&buffer[0], pos_, total_len);
	buffer.used_size_ = total_len;

	pos_ += total_len;
}

OTRBlockBuffer::OTRBlockBuffer(int block_size, int size) : OTRStaticBuffer(size), block_size_(block_size)
{
}

void OTRBlockBuffer::ResetToSize(int size)
{
	Reset();

	if (max_size_ > size)
	{
		delete[] buffer_;

		buffer_ = new byte[size];
		max_size_ = size;
		pos_ = buffer_;
	}
}

bool OTRBlockBuffer::ProcessOverflow(int overflow_size)
{
	const int offset = (int)(pos_ - buffer_);
	const int inc_size = overflow_size > block_size_ ? (overflow_size + block_size_) : block_size_;

	const int new_size = max_size_ + inc_size;
	byte* new_buffer = new byte[new_size];

	::memcpy(new_buffer, buffer_, max_size_);
	delete[] buffer_;

	buffer_ = new_buffer;
	max_size_ = new_size;

	pos_ = buffer_ + offset;

	return true;
}

DSA *DSA_generate_parameters2(int bits,
		unsigned char *seed_in, int seed_len,
		int *counter_ret, unsigned long *h_ret,
		void (*callback)(int, int, void *),
		void *cb_arg)
	{
	BN_GENCB cb;
	DSA *ret;

	if ((ret=DSA_new()) == NULL) return NULL;

	BN_GENCB_set_old(&cb, callback, cb_arg);

	if(DSA_generate_parameters_ex(ret, bits, seed_in, seed_len,
				counter_ret, h_ret, &cb))
		return ret;
	DSA_free(ret);
	return NULL;
	}

OTRChatClientImpl::OTRChatClientImpl()
	:disconnected(true)
	,state(kOTRInit)
	,sockfd(-1)
	,puba(1024)
	,pubb(1024)
	,gxmpi(1024)
	,secbytes(1024)
	,last_message(1024)
{
	sender_tag = rnd_range(0x00000100, 0xFFFFFFFF);
	receiver_tag = 0;

	keyidb = 1;

	ctx = BN_CTX_new();

	p.Init(ctx);
	p.FromHEX(DH1536_MODULUS_S);

	g.Init(ctx);
	g.FromHEX(DH1536_GENERATOR_S);

	x.Init(ctx);
	x.FromRandom(320);

	r.Init(ctx);
	r.FromRandom(128);

	gx.Init(ctx);
	g.Pow(gx, x, p);

	gxmpi.PackMPI(gx);

	gy.Init(ctx);

	privb = null;	
}

OTRChatClientImpl::~OTRChatClientImpl()
{
	ReleaseSockets();

	DSA_free(privb);
	BN_CTX_free(ctx);
}

bool OTRChatClientImpl::Connect(const string& host, int port)
{
	if (!privb)
	{
		privb = DSA_generate_parameters2(1024, NULL, 0, NULL, NULL, NULL, NULL);

		//privb = DSA_generate_parameters(NULL, 1024, NULL, 0, NULL, NULL, NULL);

/*	
#ifndef OPENSSL_NO_DEPRECATED
DSA *	DSA_generate_parameters(int bits,
		unsigned char *seed,int seed_len,
		int *counter_ret, unsigned long *h_ret,void
		(*callback)(int, int, void *),void *cb_arg);
#endif 


int	DSA_generate_parameters_ex(DSA *dsa, int bits,
		const unsigned char *seed,int seed_len,
		int *counter_ret, unsigned long *h_ret, BN_GENCB *cb);*/

		DSA_generate_key(privb);

		pubb.PackSHORT(0);
		pubb.PackMPI(privb->p);
		pubb.PackMPI(privb->q);
		pubb.PackMPI(privb->g);
		pubb.PackMPI(privb->pub_key);
	}

	InitSockets();

	if (sockfd != -1)
	{
		return true;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	struct sockaddr_in serv_addr;

	::memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(host.c_str());
    serv_addr.sin_port = htons(port);

    if (::connect(sockfd, (const sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
    {
		state = kOTRError;
		return false;
    }

	state = kOTRInit;
	disconnected = false;

	return true;
}

void OTRChatClientImpl::Disconnect()
{
	if (sockfd != -1)
	{
#ifdef PC
		::closesocket(sockfd);
#else
		::close(sockfd);
#endif
		sockfd = -1;
	}

	disconnected = true;
}

bool OTRChatClientImpl::ProcessDataStream(std::vector<std::string>& new_messages)
{
	static NetBlockBuffer buffer(1024);
	static byte recv_buffer[16384];

	while (!disconnected)
	{
		if (state == kOTRInit)
		{
			state = kOTRKeyExchange;

			::send(sockfd, "?OTRv3?", 7, 0);
		}
		else if (state == kOTRKeyExchange)
		{
			aes_state.ctr_inital_counter = 0;

			if (!DHCommitMessage() || !DHReadKeyMessage() || !DHRevealSignatureMessage() || !DHReadSignatureMessage() || !DHCaclulateSessionKeys())
			{
				state = kOTRError;
				continue;
			}

			state = kOTRProcessMessages;
			break;
		}
		else if (state == kOTRError)
		{
			break;
		}
		else if (state >= kOTRProcessMessages)
		{
			pollfd poll_fd;
			poll_fd.fd = sockfd;
			poll_fd.events = POLLRDNORM;
			poll_fd.revents = 0;

#ifdef PC
			int res = ::WSAPoll(&poll_fd, 1, 0);
#else
            int res = poll(&poll_fd, 1, 0);
#endif
			if (res < 0)
			{
				state = kOTRError;
				continue;
			}

			if ((poll_fd.revents & POLLERR) || (poll_fd.revents & POLLHUP))
			{
				state = kOTRError;
				continue;
			}

			if (!(poll_fd.revents & POLLRDNORM))
			{
				// Nothing to read
				ThreadWorker::Sleep(50);
				break;
			}

			const int bytes_recved = ::recv(sockfd, (char*)&recv_buffer, sizeof(recv_buffer), 0);
			if (bytes_recved <= 0)
			{
				state = kOTRError;
				continue;
			}

			for (int i = 0; i < bytes_recved; ++i)
			{
				if (!ProcessChar(recv_buffer[i], buffer))
				{
					break;
				}

				if (state == kOTRGotNewMessage)
				{
					state = kOTRProcessMessages;
					new_messages.push_back( std::string((const char*)&last_message[0], last_message.GetSize()) );

					buffer.Reset();
					last_message.Reset();
				}
			}
		}

		if (!new_messages.empty())
		{
			break;
		}
	}

	return !new_messages.empty();
}

bool OTRChatClientImpl::ProcessChar(byte ch, NetStaticBuffer& buffer)
{
	switch (ch)
	{
	case '?':
	case 'O':
	case 'T':
	case 'R':
		{
			if (state == kOTRProcessMessages)
			{
				state = kOTRReadMessageMarker;
			}

			if (state == kOTRReadMessageMarker || state == kOTRReadMessageBody)
			{
				buffer.PackBYTE(ch);
			}
			else
			{
				state = kOTRProcessMessages;
			}
		}
		break;

	case ':':
	case '|':
		{
			if (state == kOTRReadMessageMarker)
			{
				if (buffer.IsEqual("?OTR"))
				{
					if (ch == ':')
					{
						state = kOTRReadMessageBody;
						buffer.Reset();
					}
					else if (ch == '|')
					{
						// TODO: Process Fragment Messages
						state = kOTRProcessMessages;
					}
				}
				else
				{
					state = kOTRProcessMessages;
				}
			}
			else
			{
			}
		}
		break;

	case '.':
		{
			if (state == kOTRReadMessageBody)
			{
				if (ProcessMessage(buffer))
				{
					state = kOTRGotNewMessage;
				}
			}
			else
			{
				state = kOTRProcessMessages;
			}
		}
		break;

	default:
		{
			if (state == kOTRReadMessageBody && ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '=' || ch == '/' || ch == '+'))
			{
				buffer.PackBYTE(ch);
			}
			else
			{
				state = kOTRProcessMessages;
			}
		}
		break;
	}

	return state != kOTRProcessMessages;
}

bool OTRChatClientImpl::ProcessMessage(const NetStaticBuffer& message)
{
	static OTRStaticBuffer buffer;
	static OTRStaticBuffer message_enc;
	static OTRHeader header;

	buffer.Reset();

	message.UnpackBase64(buffer);

	UnpackHeader(buffer, header);

	byte flags = buffer.UnpackBYTE();
	byte message_keyida = buffer.UnpackDWORD();
	byte message_keyidb = buffer.UnpackDWORD();

	OTRNumber gy_next(ctx);
	buffer.UnpackMPI(gy_next);

	aes_state.ctr_inital_counter = buffer.UnpackQWORD();

	buffer.UnpackDATA(message_enc);

	OTRMac160 authenticator;
	buffer.UnpackMAC160(authenticator);

	message_enc.UnpackAES(receiving_aes_key, aes_state, last_message);

	return last_message.GetSize() > 0;
}

void OTRChatClientImpl::SendMessage(const char* message)
{
	SendData("?OTR:", message);
}

void OTRChatClientImpl::SendCommand(const char* message)
{
	SendData("?OTRCmd:", message);
}

void OTRChatClientImpl::SendData(const char* header, const char* message)
{
	static OTRBlockBuffer buffer(8192);
	static OTRBlockBuffer message_buffer(8192);

	buffer.ResetToSize(8192);
	message_buffer.ResetToSize(8192);

	++aes_state.ctr_inital_counter;

	OTRNumber x_next(ctx), gx_next(ctx);
	x_next.FromRandom(320);
	g.Pow(gx_next, x, p);

	PackHeader(buffer, 0x03);

	buffer.PackBYTE(0x00);
	buffer.PackDWORD(keyida);
	buffer.PackDWORD(keyidb);
	buffer.PackMPI(gx_next);
	buffer.PackQWORD(aes_state.ctr_inital_counter);
	buffer.PackAES(sending_aes_key, aes_state, (const byte*)message, strlen(message));

	OTRMac mac;
	int mac_len = GetMac(sending_mac_key, sizeof(OTRHash1), &buffer[0], buffer.GetSize(), mac, EVP_sha1());

	buffer.PackRAW(mac, mac_len);

	message_buffer.PackRAW((const byte*)header, strlen(header));
	message_buffer.PackBase64(&buffer[0], buffer.GetSize());
	message_buffer.PackRAW((const byte*)".", 1);

	message_buffer.Send(sockfd);
}

void OTRChatClientImpl::GetHash1(byte* src, int len, OTRHash1 hash, const byte* byte)
{
	static OTRStaticBuffer buffer;

	if (byte)
	{
		buffer.Reset();
		buffer.PackBYTE(*byte);
		buffer.PackRAW(src, len);

		SHA1(&buffer[0], buffer.GetSize(), hash);
	}
	else
	{
		SHA1(src, len, hash);
	}
}

void OTRChatClientImpl::GetSSID(byte* src, int len, OTRSSID ssid)
{
	static OTRHash2 hash;
	static const byte extra_byte = 0x00;

	GetHash2(src, len, hash, &extra_byte);

	::memcpy(ssid, hash, sizeof(OTRSSID));
}

void OTRChatClientImpl::GetAESKeys(byte* src, int len, OTRAESKey keya, OTRAESKey keyb)
{
	static OTRHash2 hash;
	static const byte extra_byte = 0x01;

	GetHash2(src, len, hash, &extra_byte);

	::memcpy(keyb, hash, sizeof(OTRAESKey));
	::memcpy(keya, hash + sizeof(OTRAESKey), sizeof(OTRAESKey));
}

void OTRChatClientImpl::GetAESSessionKey(byte* src, int len, OTRAESKey key, const byte* extra_byte)
{
	static OTRHash1 hash;

	GetHash1(src, len, hash, extra_byte);

	::memcpy(key, hash, sizeof(OTRAESKey));
}

void OTRChatClientImpl::GetHash2(byte* src, int len, OTRHash2 hash, const byte* byte)
{
	static OTRStaticBuffer buffer;

	if (byte)
	{
		buffer.Reset();
		buffer.PackBYTE(*byte);
		buffer.PackRAW(src, len);

		SHA256(&buffer[0], buffer.GetSize(), hash);
	}
	else
	{
		SHA256(src, len, hash);
	}
}

int OTRChatClientImpl::GetMac(byte* key, int key_len, byte* data, int data_len, OTRMac mac, const EVP_MD* mode)
{
	if (!mode)
	{
		mode = EVP_sha256();
	}

	unsigned int result_len = 0;
    HMAC(mode, key, key_len, data, data_len, mac, &result_len);
	return result_len;
}

int OTRChatClientImpl::GetMac160(byte* key, int key_len, byte* data, int data_len, OTRMac160 mac_160)
{
	static OTRMac mac;
	GetMac(key, key_len, data, data_len, mac);
	::memcpy(mac_160, mac, sizeof(OTRMac160));
	return sizeof(OTRMac160);
}

bool OTRChatClientImpl::ReadHeader(OTRStaticBuffer& buffer, OTRHeader& header)
{
	buffer.RecvDATA(sockfd);

	UnpackHeader(buffer, header);

	return true;
}

void OTRChatClientImpl::UnpackHeader(OTRStaticBuffer& buffer, OTRHeader& header)
{
	header.version = buffer.UnpackSHORT();
	header.type = buffer.UnpackBYTE();
	header.sender_tag = buffer.UnpackDWORD();
	header.receiver_tag = buffer.UnpackDWORD();
}

void OTRChatClientImpl::PackHeader(NetStaticBuffer& buffer, byte type)
{
	buffer.PackSHORT(3);
	buffer.PackBYTE(type);
	buffer.PackDWORD(sender_tag);
	buffer.PackDWORD(receiver_tag);
}

bool OTRChatClientImpl::DHCommitMessage()
{
	static OTRStaticBuffer buffer;

	buffer.Reset();

	PackHeader(buffer, 0x02);

	buffer.PackAES(r, aes_state, gxmpi);

	OTRHash2 hash;
	GetHash2(&gxmpi[0], gxmpi.GetSize(), hash);

	((NetStaticBuffer*)(&buffer))->PackDATA(hash, sizeof(hash));

	buffer.SendDATA(sockfd);

	return true;
}

bool OTRChatClientImpl::DHReadKeyMessage()
{
	static OTRStaticBuffer buffer;
	static OTRHeader header;

	buffer.Reset();

	ReadHeader(buffer, header);

	receiver_tag = header.sender_tag;

	buffer.UnpackMPI(gy);

	return true;
}

bool OTRChatClientImpl::DHRevealSignatureMessage()
{
	static OTRStaticBuffer xb;
	static OTRStaticBuffer buffer;

	xb.Reset();
	buffer.Reset();
	secbytes.Reset();

	OTRNumber s(ctx);
	gy.Pow(s, x, p);

	secbytes.PackMPI(s);

	byte extra_byte = 0x00;

	GetSSID(&secbytes[0], secbytes.GetSize(), ssid);
	GetAESKeys(&secbytes[0], secbytes.GetSize(), ca, cb);

	extra_byte = 0x02;
	GetHash2(&secbytes[0], secbytes.GetSize(), m1b, &extra_byte);

	extra_byte = 0x03;
	GetHash2(&secbytes[0], secbytes.GetSize(), m2b, &extra_byte);

	extra_byte = 0x04;
	GetHash2(&secbytes[0], secbytes.GetSize(), m1a, &extra_byte);

	extra_byte = 0x05;
	GetHash2(&secbytes[0], secbytes.GetSize(), m2a, &extra_byte);

	buffer.PackMPI(gx);
	buffer.PackMPI(gy);
	buffer.PackRAW(pubb);
	buffer.PackDWORD(keyidb);

	OTRMac mb;
	int mb_len = GetMac(m1b, sizeof(OTRHash2), &buffer[0], buffer.GetSize(), mb);

	buffer.Reset();

	buffer.PackRAW(pubb);
	buffer.PackDWORD(keyidb);
	buffer.PackSIG(privb, mb, mb_len);

	xb.PackAES(cb, aes_state, buffer);

	buffer.Reset();

	PackHeader(buffer, 0x11);

	buffer.PackDATA(r);
	buffer.PackRAW(xb);

	OTRMac xb_mac;
	int xb_mac_len = GetMac160(m2b, sizeof(OTRHash2), &xb[0], xb.GetSize(), xb_mac);

	buffer.PackRAW(xb_mac, xb_mac_len);

	buffer.SendDATA(sockfd);

	return true;
}

bool OTRChatClientImpl::DHReadSignatureMessage()
{
	static OTRStaticBuffer xa;
	static OTRStaticBuffer tmp;
	static OTRStaticBuffer buffer;
	static OTRHeader header;

	xa.Reset();
	tmp.Reset();
	buffer.Reset();
	
	ReadHeader(buffer, header);

	buffer.UnpackDATA(tmp);
	tmp.UnpackAES(ca, aes_state, xa);

	xa.UnpackPublicKey(puba);
	keyida = xa.UnpackDWORD();

	// TODO: Verification Signature message

	return true;
}

bool OTRChatClientImpl::DHCaclulateSessionKeys()
{
	OTRNumber puba_num(ctx), pubb_num(ctx);

	puba_num.FromBuffer(puba);
	pubb_num.FromBuffer(pubb);

	bool high = pubb_num.Compare(puba_num) > 0;

	byte sendbyte = high ? 0x01 : 0x02;
	byte recvbyte = high ? 0x02 : 0x01;

	GetAESSessionKey(&secbytes[0], secbytes.GetSize(), sending_aes_key, &sendbyte);
	GetHash1(sending_aes_key, sizeof(OTRAESKey), sending_mac_key);

	GetAESSessionKey(&secbytes[0], secbytes.GetSize(), receiving_aes_key, &recvbyte);
	GetHash1(receiving_aes_key, sizeof(OTRAESKey), receiving_mac_key);

	return true;
}