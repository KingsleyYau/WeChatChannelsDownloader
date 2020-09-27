/*
 * Buffer.h
 *
 *  Created on: 2015-11-13
 *      Author: Max
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER_LEN 4096

class Buffer {
public:
	Buffer() {
		size = MAX_BUFFER_LEN;
		buffer = new char[MAX_BUFFER_LEN];
		Reset();
	}

	Buffer(unsigned int size) {
		this->size = size;
		if( size > 0 ) {
			buffer = new char[this->size];
		}
		Reset();
	}

	~Buffer() {
		if( buffer ) {
			delete[] buffer;
			buffer = NULL;
		}
	}

	void Reset() {
		len = 0;
		if( size > 0 && buffer ) {
			memset(buffer, '\0', size);
		}
	}

	int Size() {
		return size;
	}

	int Freespace() {
		return size - len;
	}

	void ReadZeroCopy(const void **data, int &size) {
		*data = buffer;
		size = len;
	}

	int Toss(int size) {
		int parsedLen = len<size?len:size;
		memmove((void *)buffer, (const void *)(buffer + parsedLen), len - parsedLen);
		len -= parsedLen;
		return parsedLen;
	}

	bool Write(const char* data, int size) {
		if( Freespace() > size ) {
			memcpy((void *)(buffer + len), (const void *)data, size);
			len += size;
			return true;
		}
		return false;
	}

	char *GetBuffer4Write() {
		return buffer + len;
	}

	void TossWrite(int size) {
		len += size;
	}

private:
	int		len;
	int 	size;
	char	*buffer;
};

#endif /* BUFFER_H_ */
