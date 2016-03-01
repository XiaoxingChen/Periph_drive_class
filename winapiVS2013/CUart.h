#ifndef CUART_H
#define CUART_H
#include <windows.h>
#include <iostream>
#include <stdint.h>
class CUart
{
public:
	CUart(_In_ LPCSTR lpFileName)
		:lpFileName_(lpFileName)
			//RXDBUF_SIZE_(50),
			//RxdBuf_StrLen_(0)
	{}
	void Init(DWORD BaudRate);
	void printf(const char* lpOutBuffer);
	void SendArray(LPCVOID lpOutBuffer, DWORD dwBytesWrite);
	void PopRxdQueue(char* RxdBuf, uint16_t maxLength = 0);
	DWORD Get_cbInQue() const;
	void Clear_RxQue() const;
	//char RxdBuf_[50];
	void exit();
	void cout_RxdBuf();
private:
	BOOL Open();
	void Config(DWORD BaudRate);
	//const unsigned short RXDBUF_SIZE_;

	_In_ LPCSTR lpFileName_;
	HANDLE hCom_;
};

//extern CUart COM1;
#endif 
/*End of File*/
