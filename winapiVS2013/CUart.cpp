#include "CUart.h"
using namespace std;
BOOL CUart::Open()
{
	hCom_ = CreateFile(lpFileName_, //COM1 口
		GENERIC_READ | GENERIC_WRITE, //允 许 读 和 写
		0, //独 占 方 式
		NULL,
		OPEN_EXISTING, //打 开 而 不 是 创 建
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, //重 叠 方 式
		NULL);
	if (hCom_ == INVALID_HANDLE_VALUE)
	{
		cout << "open failed..." << endl;
		return FALSE;
	}
	cout << "open successfully! " << endl;
	return TRUE;
}

void CUart::exit()
{
	CloseHandle(hCom_);
}

void CUart::Config(DWORD BaudRate)
{
	SetupComm(hCom_, 4096, 4096); //输 入 缓 冲 区 和 输 出 缓 冲 区 的 大 小 都 是 1024
	COMMTIMEOUTS TimeOuts;
	//设 定 读 超 时
	TimeOuts.ReadIntervalTimeout = 1000;
	TimeOuts.ReadTotalTimeoutMultiplier = 500;
	TimeOuts.ReadTotalTimeoutConstant = 5000;
	//设 定 写 超 时
	TimeOuts.WriteTotalTimeoutMultiplier = 500;
	TimeOuts.WriteTotalTimeoutConstant = 2000;
	SetCommTimeouts(hCom_, &TimeOuts); //设 置 超 时

	DCB dcb;

	GetCommState(hCom_, &dcb);
	dcb.fBinary = TRUE;
	dcb.BaudRate = BaudRate; //波 特 率
	dcb.ByteSize = 8; //每 个 字 节 有 8 位
	dcb.Parity = NOPARITY; //无 奇 偶 校 验 位
	dcb.StopBits = ONESTOPBIT; //1 个 停 止 位
	SetCommState(hCom_, &dcb);

	PurgeComm(hCom_, PURGE_TXCLEAR | PURGE_RXCLEAR);
}
void CUart::Init(DWORD BaudRate)
{
	Open();
	Config(BaudRate);
}

void CUart::SendArray(LPCVOID lpOutBuffer, DWORD dwBytesWrite)
{
	OVERLAPPED m_osWrite;
	memset(&m_osWrite, 0, sizeof(OVERLAPPED));
	m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	/*char lpOutBuffer[100] = "hey chenxx!";
	DWORD dwBytesWrite = 100;*/
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	BOOL bWriteStat;
	ClearCommError(hCom_, &dwErrorFlags, &ComStat);
	bWriteStat = WriteFile(hCom_, lpOutBuffer, dwBytesWrite, &dwBytesWrite, &m_osWrite);
	if (!bWriteStat)
	{
		/*cout << "write to UART failed!" << endl;*/
		if (GetLastError() == ERROR_IO_PENDING)
		{
			WaitForSingleObject(m_osWrite.hEvent, 1000);
		}
	}
	PurgeComm(hCom_, PURGE_TXABORT |PURGE_TXCLEAR);
}

void CUart::printf(const char* lpOutBuffer)
{
	SendArray(lpOutBuffer, strlen(lpOutBuffer));
}

void CUart::PopRxdQueue(char* RxdBuf, uint16_t maxLength)
{
	OVERLAPPED m_osRead;
	memset(&m_osRead, 0, sizeof(OVERLAPPED));
	m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	DWORD dwBytesRead = Get_cbInQue();//the total unread bytes
	BOOL bReadStat;

	if (dwBytesRead == 0)
		return;
	if (maxLength != 0)
		dwBytesRead = min(dwBytesRead, maxLength);
	
	bReadStat = ReadFile(hCom_, RxdBuf, dwBytesRead, &dwBytesRead, &m_osRead);
	if (!bReadStat)
	{
		cout << "bReadStat false" << endl;
		if (GetLastError() == ERROR_IO_PENDING)
			//GetLastError()函 数 返 回 ERROR_IO_PENDING,表明串口正在进行读操作
		{
			WaitForSingleObject(m_osRead.hEvent, 2000);
			cout << "Error IO pending"<< endl;
		}
	}
	PurgeComm(hCom_, PURGE_TXABORT | PURGE_RXABORT );
	
#if 0
	cout << "read from UART successfully!" << endl;
#endif
}

//
//DWORD Get_cbInQue()
//
DWORD CUart::Get_cbInQue() const
{
	COMSTAT ComStat;
	DWORD dwErrorFlags;

	ClearCommError(hCom_, &dwErrorFlags, &ComStat);

	return ComStat.cbInQue;
}

//
//void Clear_RxQue()
//
void CUart::Clear_RxQue() const
{
	PurgeComm(hCom_, PURGE_RXCLEAR);
}


void CUart::cout_RxdBuf()
{
	//for (size_t i = 0; i < RxdBuf_StrLen_; i++)
	//{
	//	cout << hex<<(int)(unsigned char)RxdBuf_[i];
	//}
	//cout << endl;
}

//CUart COM1("COM1");
/*End of File*/
