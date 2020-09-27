/*
 * dbtest.cpp
 *
 *  Created on: 2015-1-14
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>

#include <iostream>
#include <string>
using namespace std;

// Common
#include <common/Arithmetic.h>
#include <common/StringHandle.h>
#include <common/CommonFunc.h>
#include <common/Math.h>

bool Parse(int argc, char *argv[]);

class CommonHeader {
public:
	CommonHeader(unsigned short paramHeaderSize) {
		this->paramHeaderSize = paramHeaderSize;
	}

	unsigned short Serialize(char *buffer, int size) {
		int new_size = 0;
		const char PacketStart[] = {0xAB, 0x00, 0x00};
		memcpy(buffer, (const void*)PacketStart, sizeof(PacketStart));
		new_size += sizeof(PacketStart);

		unsigned short headerSize = paramHeaderSize + 25;
		buffer[new_size + 0] = (headerSize >> 8) & 0xFF;
		buffer[new_size + 1] = (headerSize) & 0xFF;
		new_size += 2;

		const char Padding[] = {0x27,0x15,0x9f,0x9a,0x54,0x17,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
		memcpy(buffer + new_size, (const void*)Padding, sizeof(Padding));
		new_size += sizeof(Padding);

		buffer[new_size + 0] = (paramHeaderSize >> 8) & 0xFF;
		buffer[new_size + 1] = (paramHeaderSize) & 0xFF;
		new_size += 2;

		return new_size;
	}

	unsigned short paramHeaderSize;
};

class ParamHeader {
public:
	ParamHeader(const string &key, const string &value) {
		this->key = key;
		this->value = value;
		this->buffer_size = 0;
		this->buffer = NULL;
	}

	ParamHeader(const string &key, const char* buffer, int size) {
		this->key = key;
		this->buffer_size = size;
		this->buffer = new char[size];
		memcpy(this->buffer, buffer, size);
	}

	~ParamHeader() {
		if( this->buffer ) {
			delete[] this->buffer;
			this->buffer_size = 0;
		}
	}

	int Serialize(char *buffer, int size) {
		int new_size = 0;
		unsigned int header_key_len = key.size();
		buffer[new_size + 0] = (header_key_len >> 24) & 0xFF;
		buffer[new_size + 1] = (header_key_len >> 16) & 0xFF;
		buffer[new_size + 2] = (header_key_len >> 8) & 0xFF;
		buffer[new_size + 3] = (header_key_len) & 0xFF;
		new_size += 4;
		memcpy(buffer + new_size, (const void*)key.c_str(), key.size());
		new_size += key.size();

		if ( this->buffer ) {
			unsigned int header_value_len = this->buffer_size;
			if ( header_value_len > 0 ) {
				buffer[new_size + 0] = (header_value_len >> 24) & 0xFF;
				buffer[new_size + 1] = (header_value_len >> 16) & 0xFF;
				buffer[new_size + 2] = (header_value_len >> 8) & 0xFF;
				buffer[new_size + 3] = (header_value_len) & 0xFF;
				new_size += 4;

				memcpy(buffer + new_size, (const void*)this->buffer, this->buffer_size);
				new_size += this->buffer_size;
			} else {
				const char Padding[] = {0x00,0x00,0x00,0x00} ;
				memcpy(buffer + new_size, (const void*)Padding, sizeof(Padding));
				new_size += sizeof(Padding);
			}

		} else {
			unsigned int header_value_len = value.size();
			if ( header_value_len > 0 ) {
				buffer[new_size + 0] = (header_value_len >> 24) & 0xFF;
				buffer[new_size + 1] = (header_value_len >> 16) & 0xFF;
				buffer[new_size + 2] = (header_value_len >> 8) & 0xFF;
				buffer[new_size + 3] = (header_value_len) & 0xFF;
				new_size += 4;

				memcpy(buffer + new_size, (const void*)value.c_str(), value.size());
				new_size += value.size();
			} else {
				const char Padding[] = {0x00,0x00,0x00,0x00} ;
				memcpy(buffer + new_size, (const void*)Padding, sizeof(Padding));
				new_size += sizeof(Padding);
			}
		}

		return new_size;
	}

	string key;
	string value;
	char *buffer;
	int buffer_size;
};

const char AuthKey[] = {
		0x30,0x43,0x02,0x01,0x01,0x04,0x3c,0x30,0x3a,0x02,0x01,0x01,0x02,0x01, \
		0x01,0x02,0x04,0x17,0x54,0x9a,0x9f,0x02,0x03,0x1e,0xe2,0xb9,0x02,0x04,0x3c,0xdc, \
		0xcd,0xcb,0x02,0x04,0xad,0xf8,0xcd,0xcb,0x02,0x03,0x20,0x16,0xda,0x02,0x04,0x2a, \
		0x05,0xf8,0x24,0x02,0x04,0xa2,0x50,0xfb,0x3a,0x02,0x04,0x5f,0xbc,0x82,0xdb,0x02, \
		0x04,0x51,0xa7,0xef,0x39,0x04,0x00,
		};
static const int RANGE = 131071;
//static const unsigned char ENC_KEY[] = {0xFC, 0xCA, 0x36, 0x91, 0xB3, 0x9C, 0x02, 0xD9, 0xD8, 0xC0, 0xD7, 0xF8};
static unsigned char* ENC_KEY;
static unsigned int ENC_KEY_LENGTH;

int Req(int seq, char **buffer, int &size, int total_file_size) {
	char pBuffer[4096] = {0};
	int pSize = 0;

	string strseq = to_string(seq);
	int rangestart = (RANGE + 1) * seq;
	string srangestart = to_string(rangestart);
	int rangeend = rangestart + RANGE;
	if ( total_file_size > 0 ) {
		rangeend = MIN(rangeend, total_file_size);
	}
	string srangeend = to_string(rangeend);

	printf("# Req seq:%d, rangestart:%d, rangeend:%d, total_file_size:%d \n", seq, rangestart, rangeend, total_file_size);

	// mp4
	string url = "http://wxapp.tc.qq.com/251/20302/stodownload?encfilekey=G83YYE2iciaib491UK8yGibLXDKSgvVgk8CgnTwJLwVPt0Micv8OPgfPIic08nGRQA9npw9ZFgL8mCHWehQ8CZRW3Out2PA93mmz5ZMMX3zk7xAbBnH3iaDdPRWQ0HpEnOJxibHabxmpzwa6VGzHDEDQ5Hjacaw4S0c3icPae&bizid=1023&dotrans=934&hy=SZ&idx=1&m=219abab90465b969dbf235ffb5df6a41&token=AxricY7RBHdV3hUtSjd0bFD5NUmoTX72oZne2AHC9eltDhxEOKv4w7HqiceITynbJtLB2OrXjtpV0";
	string ticket = "1a38303602010104223020020200fb0402535a04107287eda7776030d7585757c25766a0240204013e4e75040d0000000462746673000000013122086832363566696c65280130bbd5b7053886fdb6fb05";
	ParamHeader params[] = {
		ParamHeader("ver", "1"),
		ParamHeader("weixinnum", "391420575"),
//		ParamHeader("weixinnum", "2815544115"),
		ParamHeader("province", "10344"),
		ParamHeader("city", "0"),
		ParamHeader("isp", "0"),
		ParamHeader("seq", strseq),
		ParamHeader("clientversion", "385879857"),
		ParamHeader("clientostype", "iOS13.7"),
//		ParamHeader("clientversion", "654316090"),
//		ParamHeader("clientostype", "android-21"),
		ParamHeader("authkey", (const char*)AuthKey, sizeof(AuthKey)),
		ParamHeader("nettype", "1"),
		ParamHeader("acceptdupack", "1"),
		ParamHeader("signal", ""),
		ParamHeader("scene", ""),
		ParamHeader("url", url),
		ParamHeader("rangestart", srangestart),
		ParamHeader("rangeend", srangeend),
		ParamHeader("lastretcode", "0"),
		ParamHeader("ipseq", "0"),
		ParamHeader("redirect_type", "1"),
		ParamHeader("lastvideoformat", "0"),
		ParamHeader("videoformat", "2"),
		ParamHeader("X-snsvideoflag", "V2"),
		ParamHeader("X-snsvideoticket", ticket),
	};

	for(int i = 0; i < _countof(params); i++) {
		int size = params[i].Serialize(pBuffer + pSize, sizeof(pBuffer));
		pSize += size;
	}

	char cBuffer[2046] = {0};
	CommonHeader c((unsigned short)pSize);
	int cSize = c.Serialize(cBuffer, sizeof(cBuffer));

	char *req = new char[cSize + pSize];
	memcpy(req, cBuffer, cSize);
	memcpy(req + cSize, pBuffer, pSize);

	*buffer = req;
	size = cSize + pSize;

	return size;
}

void FreeReq(char **buffer) {
	delete[] *buffer;
}

bool Parse(const char* buffer, int size, int &packetSize, int &fileSize, int &totalFileSize, FILE* fp, FILE* fpori) {
	int step = 0;

	// Read header
	if ( step + 3 > size ) {
		return false;
	}
	const unsigned char *p = (const unsigned char*)buffer;
	if ( p[0] != 0xAB || p[1] != 0x00 ) {
		printf("# Parse packet start error p:%02x,%02x \n", p[0], p[1]);
		return false;
	}
	p += 3;
	step += 3;

	// Read header len
	if ( step + 23 > size ) {
		return false;
	}
	unsigned short header_len;
	header_len = (p[0] << 8) | p[1];
//	printf("# Parse packet, header_len:%hu, p:%02x,%02x \n", header_len, p[0], p[1]);
	p += 2;
	step += 2;

	// Skip
	p += 20;
	step += 20;
	bool bHasFileData = false;
	bool bIsTotalsize = false;

	bool bIsSeq = false;
	unsigned int seq = 0;;

	bool bIsEnclen = false;
	unsigned int enclen	= 0;

	while (true) {
		// Read header len
		if ( step + 4 > size ) {
			return false;
		}
		unsigned int header_key_len;
		header_key_len = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
		p += 4;
		step += 4;

		// Read header key
		if ( step + header_key_len > size ) {
			return false;
		}

		char key[2048] = {0};
		memcpy(key, p, header_key_len);
		p += header_key_len;
		step += header_key_len;

//		printf("# Parse header, header_key_len:%u, key:%s \n", header_key_len, key);

		// totalsize
		if (bIsTotalsize) {
			totalFileSize = atoi(key);
			bIsTotalsize = false;
		}
		if ( strcmp("totalsize", key) == 0 ) {
			bIsTotalsize = true;
		}

		// seq
		if ( bIsSeq ) {
			seq = atoi(key);
			bIsSeq = false;
		}
		if ( strcmp("seq", key) == 0 ) {
			bIsSeq = true;
		}

		// enclen
		if ( bIsEnclen ) {
			enclen = atoi(key);
			bIsEnclen = false;
		}
		if ( strcmp("X-enclen", key) == 0 ) {
			bIsEnclen = true;
		}

		if ( strcmp("filedata", key) == 0 ) {
			bHasFileData = true;
			break;
		}
//		if ( step >= header_len ) {
//			printf("# Read packet ok, header_len:%u, step:%u \n", header_len, step);
//			break;
//		}
	}

	if ( bHasFileData ) {
		// Read filedata len
		if ( step + 4 > size ) {
			return false;
		}
		unsigned int filedata_len;
		filedata_len = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
		p += 4;
		step += 4;
//		printf("# Parse filedata size, filedata_len:%u, p:%02x,%02x,%02x,%02x \n", filedata_len, p[0], p[1], p[2], p[3]);

		// Read filedata
		if ( step + filedata_len > size ) {
			return false;
		}
		fileSize = filedata_len;
		packetSize = filedata_len + header_len;

//		if ( false ) {
		if ( enclen > 0 ) {
			unsigned char* dec = new unsigned char[filedata_len];
			for(int i = 0; i < filedata_len; i++) {
				int index = i % ENC_KEY_LENGTH;
				dec[i] = p[i] ^ ENC_KEY[index];
//				printf("# enc[i]:%02x, index:%u, enc:%02x, dec[i]:%02x \n", p[i], index, ENC_KEY[index], decode[i]);
			}
			fwrite(dec, 1, filedata_len, fp);
			delete[] dec;
		} else {
			fwrite(p, 1, filedata_len, fp);
		}
		fflush(fp);
		fwrite(p, 1, filedata_len, fpori);
		fflush(fpori);

		printf("# Parse seq:%u, X-enclen:%u, size:%d, packetSize:%d, fileSize:%d, totalFileSize:%d \n", seq, enclen, size, packetSize, fileSize, totalFileSize);

		return true;
	} else {
		packetSize = header_len;
		printf("# Parse packet ok without filedata, size:%d, packetSize:%d \n", size, packetSize);
	}

	return false;
}

int main(int argc, char *argv[]) {
	printf("############## json-test ############## \n");
	Parse(argc, argv);
	srand(time(0));

	FILE *fpkey = fopen("wechat_channel.key", "rb+");
	fseek(fpkey, 0, SEEK_END);
	ENC_KEY_LENGTH = ftell(fpkey);
	ENC_KEY = new unsigned char[ENC_KEY_LENGTH];
	fseek(fpkey, 0, SEEK_SET);
	int read = fread(ENC_KEY, 1, ENC_KEY_LENGTH, fpkey);
	printf("# Read key size:%d, read:%d \n", read, ENC_KEY_LENGTH);
	fclose(fpkey);

	FILE *fpreq = fopen("req.bin", "wb+");
	FILE *fpres = fopen("res.bin", "wb+");
	FILE *fpresdata = fopen("res.mp4", "wb+");

	string ip = "203.205.220.60";
	unsigned short port = 80;

	int client = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in server;
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if (-1 != client && !(inet_pton(AF_INET, ip.c_str(), &server.sin_addr) < 0)) {
		if (connect(client, (struct sockaddr*)&server, sizeof(server)) != -1) {
			printf("# Connected %s:%hu \n", ip.c_str(), port);

			bool connected = true;
			int seq = 0;
			int total_file_size = 0;
			int recv_total_data_size = 0;
			int recv_total_file_size = 0;
			while(connected) {
				char *req = NULL;
				int req_size = 0;
				Req(seq++, &req, req_size, total_file_size);
				fwrite(req, 1, req_size, fpreq);

				int sent = send(client, req, req_size, 0);
//				printf("# Send byte:%d \n", sent);
				FreeReq(&req);

				char recv_buffer[2*RANGE] = {0};
				int recv_buffer_offset = 0;
				int recv_data_size = 0;
				while (true) {
					int packet_size = 0;
					int filedataSize = 0;
					int received = recv(client, recv_buffer + recv_buffer_offset, sizeof(recv_buffer), 0);
					if (received > 0) {
						recv_data_size += received;
						recv_total_data_size += received;
//						printf("# Recv byte:%d, recv_total_size:%d, recv_total_data_size:%d \n", received, recv_data_size, recv_total_data_size);
						recv_buffer_offset += received;

						bool bParse = Parse(recv_buffer, recv_buffer_offset, packet_size, filedataSize, total_file_size, fpresdata, fpres);
						if ( packet_size > 0 ) {
//								printf("# Flush res file byte:%d \n", recv_buffer_offset);
//								fwrite(recv_buffer, 1, recv_buffer_offset, fpres);

							recv_total_file_size += filedataSize;
							printf("# Flush resdata filedataSize:%d, totalSize:%d \n", filedataSize, recv_total_file_size);

							recv_buffer_offset = 0;
							break;
						}

					} else {
						connected = false;
						break;
					}
				}

				if ( total_file_size > 0 && recv_total_file_size >= total_file_size ) {
					printf("# All data recv totalSize:%d \n", recv_total_file_size);
					break;
				}
			}
		}
	}
	printf("# Disconnected %s:%hu \n", ip.c_str(), port);

	fclose(fpreq);
	fclose(fpres);
	fclose(fpresdata);

	return EXIT_SUCCESS;
}

bool Parse(int argc, char *argv[]) {
	string key;
	string value;

	for( int i = 1; (i + 1) < argc; i+=2 ) {
		key = argv[i];
		value = argv[i+1];
	}

	return true;
}
