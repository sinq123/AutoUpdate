#ifndef MD5_H
#define MD5_H
/* Type define */
#include <afx.h>
typedef unsigned char byte;
typedef unsigned int uint32;
/* MD5 declaration. */
class MD5 {
private:
	void update(byte* input, int length);
	void final();
	void transform(const byte block[64]);
	void encode(const uint32* input, byte* output, size_t length);
	void decode(const byte* input, uint32* output, size_t length);
	CString showHEX(byte *pBuffer, int iCount);

	void INIT(byte* input, int length);
	MD5();
	MD5(CString str);
	void INIT(CString str);
	byte* digest();
	CString DATA();
	CString KEY(CString key);
	void reset();
	/* class uncopyable */
	MD5(const MD5&);
	MD5& operator=(const MD5&);

private:
	uint32 _state[4];	/* state (ABCD) */
	uint32 _count[2];	/* number of bits, modulo 2^64 (low-order word first) */
	byte _buffer[64];	/* input buffer */
	byte _digest[16];	/* message digest */
	bool _finished;		/* calculate finished ? */

	static byte PADDING[64];	/* padding for calculate */
	static char HEX[16];
	enum { BUFFER_SIZE = 1024 };
public:
	// 计算文件MD5校验值
	static CString getFileMD5(CString filePath);
};

#endif /*MD5_H*/
