
//���������� ��������� �������� ���������
//�������� ����� ���������� ���, ������ ���������� ��� ����� ���������
#ifndef __SMIPING1CNATIVEADDIN_H__
#define __SMIPING1CNATIVEADDIN_H__

#include "stdafx.h"

#define ICMP_ECHOREPLY	0
#define ICMP_ECHOREQ	8
#define REQ_DATASIZE  32

#include "ComponentBase.h"
#include "AddInDefBase.h"
#include "IMemoryManager.h"

//������ ��� ������ �� ������� CAddInNative �� ���� CTemplNative
///////////////////////////////////////////////////////////////////////////////
// class CTemplNative
class CTemplNative : public IComponentBase
{
public:
    enum Props
    {
		//���� ��� �������������
		eAddress = 0, //ip ����� ��� ��� ���������� ��� �����. ��-��������� localhost
		ePingCount, //���������� ������ ��-��������� 1
		ePackageSizeB, //������ ��������� ������ � ������ ��-��������� 8B
		//��������� ���������� ����������
		ePingIsComplete, //��-��������� false
		//���� ����������� �����������
		eGoodPingPercent, //������� �������� ������
		eMinTTL, //����������� ����� ����������� �����
		eMaxTTL, //����������� ����� ����������� �����
		eIsError, //������� ������������� ������ ��� �����
		eErrMessage, //��������� �� ������� � �������� ����������
        eLastProp      // Always last
    };

    enum Methods
    {
		ePing = 0, //�������� ���� �� �������� ���������� 
		/*
		��� ������� ������������� ePingIsComplete � false
		��� ���������� � true
		*/
        eLastMethod      // Always last
    };

	//������ ��� ������������ �� ������� CAddInNative �� ���� CTemplNative
	CTemplNative(void);
    virtual ~CTemplNative();
    // IInitDoneBase
    virtual bool ADDIN_API Init(void*);
    virtual bool ADDIN_API setMemManager(void* mem);
    virtual long ADDIN_API GetInfo();
    virtual void ADDIN_API Done();
    // ILanguageExtenderBase
    virtual bool ADDIN_API RegisterExtensionAs(WCHAR_T** wsLanguageExt);
    virtual long ADDIN_API GetNProps();
    virtual long ADDIN_API FindProp(const WCHAR_T* wsPropName);
    virtual const WCHAR_T* ADDIN_API GetPropName(long lPropNum, long lPropAlias);
    virtual bool ADDIN_API GetPropVal(const long lPropNum, tVariant* pvarPropVal);
    virtual bool ADDIN_API SetPropVal(const long lPropNum, tVariant* varPropVal);
    virtual bool ADDIN_API IsPropReadable(const long lPropNum);
    virtual bool ADDIN_API IsPropWritable(const long lPropNum);
    virtual long ADDIN_API GetNMethods();
    virtual long ADDIN_API FindMethod(const WCHAR_T* wsMethodName);
    virtual const WCHAR_T* ADDIN_API GetMethodName(const long lMethodNum, 
                            const long lMethodAlias);
    virtual long ADDIN_API GetNParams(const long lMethodNum);
    virtual bool ADDIN_API GetParamDefValue(const long lMethodNum, const long lParamNum,
                            tVariant *pvarParamDefValue);
    virtual bool ADDIN_API HasRetVal(const long lMethodNum);
    virtual bool ADDIN_API CallAsProc(const long lMethodNum,
                    tVariant* paParams, const long lSizeArray);
    virtual bool ADDIN_API CallAsFunc(const long lMethodNum,
                tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
    operator IComponentBase*() { return (IComponentBase*)this; }
    // LocaleBase
    virtual void ADDIN_API SetLocale(const WCHAR_T* loc);
private:
	long findName(const wchar_t* names[], const wchar_t* name, const uint32_t size) const;
	void DropResultData();
	wchar_t* SetWCharPropertyVal(const WCHAR_T* src);
	wchar_t* to_wstring(std::string const str); //�������������� C-string � wchar_t, ������� ���������� WinAPI
	 
    // Attributes
	//����� �� ������� its
	IAddInDefBase * m_iConnect;
	IMemoryManager *m_iMemory;

	//���� ��� �������������
	wchar_t* m_strAddress; //ip ����� ��� ��� ���������� ��� �����. ��-��������� localhost
	int m_intPingCount; //���������� ������ ��-��������� 1
	int	m_intPackageSizeB; //������ ��������� ������ � ������ ��-��������� 8B
	bool m_boolPingIsComplete; //��������� ���������� ���������� ��-��������� false
	//���� ����������� �����������
	int m_intGoodPingPercent; //������� �������� ������
	int m_intMinElapsedTime; //����������� ����� ����������� �����
	int m_intMaxElapsedTime; //����������� ����� ����������� �����
	
	bool m_boolIsError; //������� ������������� ������ ��� �����
	wchar_t* m_strErrMessage; //��������� �� ������� � �������� ����������

	//---------------------------------------------------------------------------//
	//���������� ������ ����������
	bool Ping(); // �������� ���� �� �������� ���������� 
					 // ��� ������� ������������� ePingIsComplete � false
					 // ��� ���������� � true
					 
	int SendEchoRequest(SOCKET s, LPSOCKADDR_IN lpstToAddr);
	DWORD RecvEchoReply(SOCKET s, LPSOCKADDR_IN lpsaFrom, u_char *pTTL);
	int WaitForEchoReply(SOCKET s);
	u_short in_cksum(u_short *addr, int len);

};

// IP Header -- RFC 791
typedef struct tagIPHDR
{
	u_char  VIHL;			// Version and IHL
	u_char	TOS;			// Type Of Service
	short	TotLen;			// Total Length
	short	ID;				// Identification
	short	FlagOff;		// Flags and Fragment Offset
	u_char	TTL;			// Time To Live
	u_char	Protocol;		// Protocol
	u_short	Checksum;		// Checksum
	struct	in_addr iaSrc;	// Internet Address - Source
	struct	in_addr iaDst;	// Internet Address - Destination
}IPHDR, *PIPHDR;


// ICMP Header - RFC 792
typedef struct tagICMPHDR
{
	u_char	Type;			// Type
	u_char	Code;			// Code
	u_short	Checksum;		// Checksum
	u_short	ID;				// Identification
	u_short	Seq;			// Sequence
	char	Data;			// Data
}ICMPHDR, *PICMPHDR;

// ICMP Echo Request
typedef struct tagECHOREQUEST
{
	ICMPHDR icmpHdr;
	DWORD	dwTime;
	char	cData[REQ_DATASIZE];
}ECHOREQUEST, *PECHOREQUEST;


// ICMP Echo Reply
typedef struct tagECHOREPLY
{
	IPHDR	ipHdr;
	ECHOREQUEST	echoRequest;
	char    cFiller[256];
}ECHOREPLY, *PECHOREPLY;

#endif //__SMIPING1CNATIVEADDIN_H__
