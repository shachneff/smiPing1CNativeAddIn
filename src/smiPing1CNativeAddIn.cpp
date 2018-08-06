/*
����������� https://infostart.ru/public/184119/
��� �������� ������ ������� ����� 
include ����� ������� � ������ '���������� ��������'
lib � ������ '�������� ���������'

���������� ping https://www.codeproject.com/Articles/647/Ping-for-Windows
�������� ������� (������ ������� ����) - �������� ������������ - �������� C++

���c��������, ��� ��� �������� ������ ��� Win
*/

#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>

#include <Windows.h>
#include <exception>
#include <iostream>
#include <stdlib.h>

#include <stdio.h>
#include <wchar.h>
#include <string>
#include <comdef.h>

#include <iphlpapi.h>
#include <icmpapi.h>
#include <WS2tcpip.h>

//������ �� ������� �� its.
//����� ��� �������������� ������ � �������
//������ ��� ���������������� ������ ��� win 
#include "WcharWrapper.h"
//����������� ������������ ����, � ������� ����� ��������� ���� ������
#include "smiPing1CNativeAddIn.h"

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

static const wchar_t *g_PropNames[] = {
	//���� ��� �������������
	L"Address", //ip ����� ��� ��� ���������� ��� �����. ��-��������� localhost
	L"PingCount", //���������� ������ ��-��������� 1
	L"PackageSizeB", //������ ��������� ������ � ������ ��-��������� 8B
	L"PingIsComplete", //��������� ���������� ���������� ��-��������� false
	//���� ����������� �����������
	L"GoodPingPercent", //������� �������� ������
	L"MinElapsedTime" //����������� ����� ����������� �����
	L"MaxElapsedTime",//����������� ����� ����������� �����
	L"IsError", //������� ������������� ������ ��� �����
	L"ErrMessage" //��������� �� ������� � �������� ����������
};
static const wchar_t *g_MethodNames[] = {
	L"SendPing"
};

static const wchar_t *g_PropNamesRu[] = {
	//���� ��� �������������
	L"�����", //ip ����� ��� ��� ���������� ��� �����. ��-��������� localhost
	L"����������������", //���������� ������ ��-��������� 1
	L"������������", //������ ��������� ������ � ������ ��-��������� 8B
	L"���������", //��������� ���������� ���������� ��-��������� false
	//���� ����������� �����������
	L"���������������������", //������� �������� ������
	L"�������������������", //����������� ����� ����������� �����
	L"��������������������",//����������� ����� ����������� �����
	L"����������", //������� ������������� ������ ��� �����
	L"��������������" //��������� �� ������� � �������� ����������
};
static const wchar_t *g_MethodNamesRu[] = {
	L"����"
};

static const wchar_t g_kClassNames[] = L"CTemplNative";

uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len = 0);
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len = 0);
uint32_t getLenShortWcharStr(const WCHAR_T* Source);

//��������� �� ������� its
static AppCapabilities g_capabilities = eAppCapabilitiesInvalid;
static WcharWrapper s_names(g_kClassNames);

//---------------------------------------------------------------------------//
/*������������ its. ��������� ������ ���� �������� ����������.*/
const WCHAR_T* GetClassNames()
{
	return s_names;
}
//---------------------------------------------------------------------------//
/*������������ its. �������� ���������� ������� ����������. ���� ������ �� ����� ���� ������ ��� �� ������ ������ � ��������� ������ � ������������ 0.*/
long GetClassObject(const wchar_t* wsName, IComponentBase** pInterface)
{
    if(!*pInterface)
    {
        *pInterface= new CTemplNative();
        return (long)*pInterface;
    }
    return 0;
}
//---------------------------------------------------------------------------//
/*������������ its. �������� ���������� ����� ���������� �������.���������� ������ ������ ���������� 
������� ������ � ���������� ������������ �� ������.��� �������� ���������� ������������ 0, 
����� � ��� ������(Runtime error).*/
long DestroyObject(IComponentBase** pIntf)
{
   if(!*pIntf)
      return -1;

   delete *pIntf;
   *pIntf = 0;
   return 0;
}
//---------------------------------------------------------------------------//
/*������������ its. ������������� ������ �������������� ���������� ������������.
���������� ������ ������� ������, � ������� ��� ����� ��������.
���� ������� �� �����������, �� ��� ���������� �� ����� �������� ����������� 
������ ���������, ������� ���������� � ���������.

<capabilities> ���: ������������ AppCapabilities. 
�������� ������������: eAppCapabilitiesInvalid = -1, 
						eAppCapabilities1 = 1, 
						eAppCapabilitiesLast = eAppCapabilities1
*/
AppCapabilities SetPlatformCapabilities(const AppCapabilities capabilities)
{
	g_capabilities = capabilities;
	return eAppCapabilitiesLast;
}
//---------------------------------------------------------------------------//
//CTemplNative
CTemplNative::CTemplNative()
{
	m_iMemory = 0;
	m_iConnect = 0;	

	//���� ��� �������������
	m_strAddress = L"localhost"; //ip ����� ��� ��� ���������� ��� �����. ��-��������� localhost
	m_intPingCount = 1; //���������� ������ ��-��������� 1
	m_intPackageSizeB = 8; //������ ��������� ������ � ������ ��-��������� 8B
	DropResultData();
}
//---------------------------------------------------------------------------//
CTemplNative::~CTemplNative()
{
}
//---------------------------------------------------------------------------//
void CTemplNative::DropResultData()
{
	m_boolPingIsComplete = false; //��������� ���������� ���������� ��-��������� false
								 //���� ����������� �����������
	m_boolIsError = false;	//������� ������������� ������ ��� �����
	m_strErrMessage = L"";	//��������� �� ������� � �������� ����������
	m_intGoodPingPercent = 0; //������� �������� ������
	m_intMinElapsedTime = 0; //����������� ����� ����������� �����
	m_intMaxElapsedTime = 0; //����������� ����� ����������� �����
}
//---------------------------------------------------------------------------//
bool CTemplNative::Init(void* pConnection)
{ 
	m_iConnect = (IAddInDefBase*)pConnection;
	return m_iConnect != NULL;
}
//---------------------------------------------------------------------------//
long CTemplNative::GetInfo()
{ 
    return 2000; 
}
//---------------------------------------------------------------------------//
void CTemplNative::Done()
{
}
//---------------------------------------------------------------------------//
bool CTemplNative::RegisterExtensionAs(WCHAR_T** wsExtensionName)
{ 
	const wchar_t *wsExtension = L"smiPingNativeExtension"; //��� ����������
	//������� �� ����� �������� �� 1�
	//�������� ��������� = �����("AddIn.MyComponent.smiPingNativeExtension"); 
	//MyComponent - ��� ����������, ������� �������� ��� �����������:
	//���������������������������(����, "MyComponent", ��������������������.Native);
	//
	int iActualSize = ::wcslen(wsExtension) + 1;
	WCHAR_T* dest = 0;

	if (m_iMemory)
	{
		if (m_iMemory->AllocMemory((void**)wsExtensionName, iActualSize * sizeof(WCHAR_T)))
			::convToShortWchar(wsExtensionName, wsExtension, iActualSize);
		return true;
	}

	return false;
}
//---------------------------------------------------------------------------//
long CTemplNative::GetNProps()
{ 
	//���������� ����� ���������� ������� 
	//������������ � ������������ ������������� �����
    return eLastProp;
}
//---------------------------------------------------------------------------//
long CTemplNative::FindProp(const WCHAR_T* wsPropName)
{ 
	//����� �� ������� its
	//����� ���� �� �����
	long plPropNum = -1;
	wchar_t* propName = 0;

	::convFromShortWchar(&propName, wsPropName);
	plPropNum = findName(g_PropNames, propName, eLastProp);

	if (plPropNum == -1)
		plPropNum = findName(g_PropNamesRu, propName, eLastProp);

	delete[] propName;

	return plPropNum;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CTemplNative::GetPropName(long lPropNum, long lPropAlias)
{ 
	/*����� �� ������� its*/
	if (lPropNum >= eLastProp)
		return NULL;

	wchar_t *wsCurrentName = NULL;
	WCHAR_T *wsPropName = NULL;
	size_t iActualSize = 0;

	switch (lPropAlias)
	{
	case 0: // First language
		wsCurrentName = (wchar_t*)g_PropNames[lPropNum];
		break;
	case 1: // Second language
		wsCurrentName = (wchar_t*)g_PropNamesRu[lPropNum];
		break;
	default:
		return 0;
	}

	iActualSize = wcslen(wsCurrentName) + 1;

	if (m_iMemory && wsCurrentName)
	{
		if (m_iMemory->AllocMemory((void**)&wsPropName, iActualSize * sizeof(WCHAR_T)))
			::convToShortWchar(&wsPropName, wsCurrentName, iActualSize);
	}

	return wsPropName;
}
//---------------------------------------------------------------------------//
wchar_t* CTemplNative::SetWCharPropertyVal(const WCHAR_T* src)
{
	wchar_t *wsAddress = NULL;
	int iActualSize = 0;

	iActualSize = wcslen(src) + 1;

	if (m_iMemory && src)
	{
		if (m_iMemory->AllocMemory((void**)&wsAddress, iActualSize * sizeof(WCHAR_T)))
			::convToShortWchar(&wsAddress, src, iActualSize);
	}

	return wsAddress;

}

//---------------------------------------------------------------------------//
bool CTemplNative::GetPropVal(const long lPropNum, tVariant* pvarPropVal)
{ 
	switch (lPropNum)
	{
	case eAddress:	
		TV_VT(pvarPropVal) = VTYPE_PWSTR;
		pvarPropVal->pwstrVal = SetWCharPropertyVal(m_strAddress);
		pvarPropVal->strLen = wcslen(m_strAddress);
		break;
	case ePingCount: 
		TV_VT(pvarPropVal) = VTYPE_I4;
		TV_INT(pvarPropVal) = m_intPingCount;
		break;
	case ePackageSizeB:
		TV_VT(pvarPropVal) = VTYPE_I4;
		TV_INT(pvarPropVal) = m_intPackageSizeB;
		break;
	case ePingIsComplete:
		TV_VT(pvarPropVal) = VTYPE_BOOL;
		TV_BOOL(pvarPropVal) = m_boolPingIsComplete;
		break;
	case eGoodPingPercent:
		TV_VT(pvarPropVal) = VTYPE_I4;
		TV_INT(pvarPropVal) = m_intGoodPingPercent;
		break;
	case eMinTTL:
		TV_VT(pvarPropVal) = VTYPE_I4;
		TV_INT(pvarPropVal) = m_intMinElapsedTime;
		break;
	case eMaxTTL:
		TV_VT(pvarPropVal) = VTYPE_I4;
		TV_INT(pvarPropVal) = m_intMaxElapsedTime;
		break;
	case eIsError:
		TV_VT(pvarPropVal) = VTYPE_BOOL;
		TV_INT(pvarPropVal) = m_boolIsError;
		break;
	case eErrMessage:
		TV_VT(pvarPropVal) = VTYPE_PWSTR;
		pvarPropVal->pwstrVal = SetWCharPropertyVal(m_strErrMessage);
		pvarPropVal->strLen = wcslen(m_strErrMessage);
		break;
	default:
		return false;
	}
	//�������� ���������
	return true;
}
//---------------------------------------------------------------------------//
bool CTemplNative::SetPropVal(const long lPropNum, tVariant* varPropVal)
{ 	
	switch (lPropNum)
	{
	case eAddress: 
		if (TV_VT(varPropVal) != VTYPE_PWSTR) return false;
		m_strAddress = SetWCharPropertyVal(varPropVal->pwstrVal);
		break;
	case ePingCount:
		if (TV_VT(varPropVal) != VTYPE_I4) return false;
		m_intPingCount = TV_INT(varPropVal);
		break;
	//case ePackageSizeB:
	//	if (TV_VT(varPropVal) != VTYPE_I4) return false;
	//	m_intPackageSizeB = TV_INT(varPropVal);
	//	break;
	default:
		return false;
	}

	//�������� �����������
	return true;
}
//---------------------------------------------------------------------------//
bool CTemplNative::IsPropReadable(const long lPropNum)
{ 
	//��� �������� ��������
	return true;
}
//---------------------------------------------------------------------------//
bool CTemplNative::IsPropWritable(const long lPropNum)
{
	/*����� �� ������� its*/
	switch (lPropNum)
	{
	/*�������� ��� ��� �������� ����� ����������*/
	case eAddress:
	case ePingCount:
	//case ePackageSizeB:
		return true;
	default:
		return false;
	}

	return false;
}
//---------------------------------------------------------------------------//
long CTemplNative::GetNMethods()
{ //���������� ���������� �������
    return eLastMethod;
}
//---------------------------------------------------------------------------//
long CTemplNative::findName(const wchar_t* names[], const wchar_t* name,
	const uint32_t size) const
{
	long ret = -1;
	for (uint32_t i = 0; i < size; i++)
	{
		if (!wcscmp(names[i], name))
		{
			ret = i;
			break;
		}
	}
	return ret;
}
//---------------------------------------------------------------------------//
long CTemplNative::FindMethod(const WCHAR_T* wsMethodName)
{ 
	/*����� �� ������� its*/
	//���� ����� �� �����
	long plMethodNum = -1;
	wchar_t* name = 0;

	::convFromShortWchar(&name, wsMethodName);

	plMethodNum = findName(g_MethodNames, name, eLastMethod);

	if (plMethodNum == -1)
		plMethodNum = findName(g_MethodNamesRu, name, eLastMethod);

	delete[] name;

	return plMethodNum;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CTemplNative::GetMethodName(const long lMethodNum, 
                            const long lMethodAlias)
{ 
	/*����� �� ������� its*/
	//���������� ��� ������ �� ��������� ������ � �����
	if (lMethodNum >= eLastMethod)
		return NULL;

	wchar_t *wsCurrentName = NULL;
	WCHAR_T *wsMethodName = NULL;
	int iActualSize = 0;

	switch (lMethodAlias)
	{
	case 0: // First language
		wsCurrentName = (wchar_t*)g_MethodNames[lMethodNum];
		break;
	case 1: // Second language
		wsCurrentName = (wchar_t*)g_MethodNamesRu[lMethodNum];
		break;
	default:
		return 0;
	}

	iActualSize = wcslen(wsCurrentName) + 1;

	if (m_iMemory && wsCurrentName)
	{
		if (m_iMemory->AllocMemory((void**)&wsMethodName, iActualSize * sizeof(WCHAR_T)))
			::convToShortWchar(&wsMethodName, wsCurrentName, iActualSize);
	}

	return wsMethodName;
}
//---------------------------------------------------------------------------//
long CTemplNative::GetNParams(const long lMethodNum)
{ 
	/*����� �� ������� its
	���������� ���������� ���������� ��� ��������� ������*/
	/*
	switch (lMethodNum)
	{
	case eTempMethod:
		return 1; //� ���� ������ ���� ��������
	
	case eTempMethodFunc: //� ������� ��� ����������
		return 0;
	default:
		return 0;
	}
	*/
	//� ������� ������ ��� ����������
    return 0;
}
//---------------------------------------------------------------------------//
bool CTemplNative::GetParamDefValue(const long lMethodNum, const long lParamNum,
                          tVariant *pvarParamDefValue)
{ 
	return false;
} 
//---------------------------------------------------------------------------//
bool CTemplNative::HasRetVal(const long lMethodNum)
{ 
	/*����� �� ������� its
	���������� ������� ���� �� � ������ ������������ ��������*/
	switch (lMethodNum)
	{
	/*� ������ ��� ������������� ��������*/
	case ePing:
		return false;
	/*� ������ ���� ������������ ��������*/
	//case eTempMethodFunc:
	//	return true;
	default:
		return false;
	}

	return false;
}
//---------------------------------------------------------------------------//
bool CTemplNative::CallAsProc(const long lMethodNum,
                    tVariant* paParams, const long lSizeArray)
{ 
	switch (lMethodNum)
	{
	case ePing: 
		return Ping();
	default:
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------//
bool CTemplNative::CallAsFunc(const long lMethodNum,
                tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{ 
	bool ret = false;//������� ��������� ������ ������
    return ret; 
}
//---------------------------------------------------------------------------//
void CTemplNative::SetLocale(const WCHAR_T* loc)
{
	//����� �� ������� its
	_wsetlocale(LC_ALL, loc);
}
//---------------------------------------------------------------------------//
bool CTemplNative::setMemManager(void* mem)
{	//����� �� ������� its
	m_iMemory = (IMemoryManager*)mem;
	return m_iMemory != 0;
}
//---------------------------------------------------------------------------//
uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len)
{
    if (!len)
        len = ::wcslen(Source)+1;

    if (!*Dest)
        *Dest = new WCHAR_T[len];

    WCHAR_T* tmpShort = *Dest;
    wchar_t* tmpWChar = (wchar_t*) Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, len*sizeof(WCHAR_T));
    do
    {
        *tmpShort++ = (WCHAR_T)*tmpWChar++;
        ++res;
    }
    while (len-- && *tmpWChar);

    return res;
}
//---------------------------------------------------------------------------//
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len)
{
    if (!len)
        len = getLenShortWcharStr(Source)+1;

    if (!*Dest)
        *Dest = new wchar_t[len];

    wchar_t* tmpWChar = *Dest;
    WCHAR_T* tmpShort = (WCHAR_T*)Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, len*sizeof(wchar_t));
    do
    {
        *tmpWChar++ = (wchar_t)*tmpShort++;
        ++res;
    }
    while (len-- && *tmpShort);

    return res;
}
//---------------------------------------------------------------------------//
uint32_t getLenShortWcharStr(const WCHAR_T* Source)
{
    uint32_t res = 0;
    WCHAR_T *tmpShort = (WCHAR_T*)Source;

    while (*tmpShort++)
        ++res;

    return res;
}
//---------------------------------------------------------------------------//

wchar_t* CTemplNative::to_wstring(std::string const src)
{
	wchar_t *wstrResult;
	const char* ptrSrc;
	ptrSrc = src.c_str();
	int reqSize = mbstowcs(NULL, ptrSrc, 0)+1;
	
	if (m_iMemory && reqSize) 
	{
		if (m_iMemory->AllocMemory((void **)wstrResult, reqSize * sizeof(WCHAR_T)))
		{
			mbstowcs(wstrResult, ptrSrc, reqSize);
		}
	}
	
	return wstrResult;
}

//---------------------------------------------------------------------------//
//���������� ������ ����������
bool CTemplNative::Ping() //��������� ePing
{		
	/*
	UINT nRetries = m_intPingCount;
	_bstr_t pstrHost(m_strAddress);
	SOCKET	  rawSocket;
	LPHOSTENT lpHost;
	UINT	  nLoop;
	int       nRet;
	struct    sockaddr_in saDest;
	struct    sockaddr_in saSrc;
	DWORD	  dwTimeSent;
	DWORD	  dwElapsed;
	u_char    cTTL;
	wchar_t* wstrResult;
	*/
	HANDLE hIcmpFile;
	unsigned long ipaddr = INADDR_NONE;
	DWORD dwRetVal = 0;
	char SendData[64] = "Data Buffer";
	LPVOID ReplyBuffer = NULL;
	DWORD ReplySize = 64;
	//_bstr_t pstrHost(L"uhuuihn99.ru");
	_bstr_t pstrHost(m_strAddress);
	DWORD dwRetval;
	struct addrinfo *result = NULL;
	struct addrinfo hints;
	WSADATA wsaData;
	int iResult;
	UINT	  nLoop;
	UINT nRetries = m_intPingCount;

	try
	{
		DropResultData(); //�������� ���������� ����������

		int iResult;
		WSADATA wsaData;
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {

			m_boolIsError = true;
			FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, WSAGetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPWSTR)&m_strErrMessage, 0, NULL);

			return true;
		}

		//--------------------------------
		// Setup the hints address info structure
		// which is passed to the getaddrinfo() function
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		//���������� ����� �� ��� ��������
		dwRetval = getaddrinfo(pstrHost, NULL, &hints, &result);
		if (dwRetval != 0) {
			m_boolIsError = true;
			m_strErrMessage = L"������ ����������� ������ ������� getaddrinfo(...)";
			WSACleanup();
			return true;
		}
		ipaddr = ((struct sockaddr_in *) result->ai_addr)->sin_addr.S_un.S_addr;

		hIcmpFile = IcmpCreateFile();
		if (hIcmpFile == INVALID_HANDLE_VALUE) {
			m_boolIsError = true;
			m_strErrMessage = L"�� ������� ���������������� ICMP";
			return true;
		}

		ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
		ReplyBuffer = (VOID*)malloc(ReplySize);
		if (ReplyBuffer == NULL) {
			m_boolIsError = true;
			m_strErrMessage = L"������ ��������� ������ ��� ������ ������ ��������";
			return true;
		}

		m_intGoodPingPercent = 0;
		m_intPackageSizeB = sizeof(SendData);
		// Ping multiple times
		for (nLoop = 0; nLoop < nRetries; nLoop++)
		{
			dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), NULL, ReplyBuffer, ReplySize, 1000);
			if (dwRetVal != 0) {
				//ping -- ������, ������� ���������
				PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
				struct in_addr ReplyAddr;
				ReplyAddr.S_un.S_addr = pEchoReply->Address;

				m_intGoodPingPercent++;
				//������������� ��� � ���� ����� �������� ������
				if (m_intMinElapsedTime > pEchoReply->RoundTripTime || m_intMinElapsedTime == 0) m_intMinElapsedTime = pEchoReply->RoundTripTime;
				if (m_intMaxElapsedTime < pEchoReply->RoundTripTime) m_intMaxElapsedTime = pEchoReply->RoundTripTime;
			}
			else {
				//ping -- �� ������, �������� �� ��������
				m_strErrMessage = L"�������� �������";
			}			
		}

		m_intGoodPingPercent = m_intGoodPingPercent / m_intPingCount * 100;
		m_boolPingIsComplete = true;

		freeaddrinfo(result);
		WSACleanup();

		return true;
	}
	catch(std::exception &e)
	{
		m_boolIsError = true;
		m_strErrMessage = to_wstring(e.what());
	}
}

int CTemplNative::SendEchoRequest(SOCKET s, LPSOCKADDR_IN lpstToAddr)
{
	static ECHOREQUEST echoReq;
	static int nId = 1;
	static int nSeq = 1;
	int nRet;

	// Fill in echo request
	echoReq.icmpHdr.Type = ICMP_ECHOREQ;
	echoReq.icmpHdr.Code = 0;
	echoReq.icmpHdr.Checksum = 0;
	echoReq.icmpHdr.ID = nId++;
	echoReq.icmpHdr.Seq = nSeq++;

	// Fill in some data to send
	for (nRet = 0; nRet < REQ_DATASIZE; nRet++)
		echoReq.cData[nRet] = ' ' + nRet;

	// Save tick count when sent
	echoReq.dwTime = GetTickCount();

	// Put data in packet and compute checksum
	echoReq.icmpHdr.Checksum = in_cksum((u_short *)&echoReq, sizeof(ECHOREQUEST));

	// Send the echo request  								  
	nRet = sendto(s,						/* socket */
		(LPSTR)&echoReq,			/* buffer */
		sizeof(ECHOREQUEST),
		0,							/* flags */
		(LPSOCKADDR)lpstToAddr, /* destination */
		sizeof(SOCKADDR_IN));   /* address length */

	if (nRet == SOCKET_ERROR)
	{
		m_boolIsError = true;
		m_strErrMessage = L"������ ������ � �������";
	}
	return (nRet);
}

DWORD CTemplNative::RecvEchoReply(SOCKET s, LPSOCKADDR_IN lpsaFrom, u_char * pTTL)
{
	ECHOREPLY echoReply;
	int nRet;
	int nAddrLen = sizeof(struct sockaddr_in);

	// Receive the echo reply	
	nRet = recvfrom(s,					// socket
		(LPSTR)&echoReply,	// buffer
		sizeof(ECHOREPLY),	// size of buffer
		0,					// flags
		(LPSOCKADDR)lpsaFrom,	// From address
		&nAddrLen);			// pointer to address len

							// Check return value
	if (nRet == SOCKET_ERROR)
	{
		m_boolIsError = true;
		m_strErrMessage = L"������ ������ � �������";
	}

	// return time sent and IP TTL
	*pTTL = echoReply.ipHdr.TTL;

	return(echoReply.echoRequest.dwTime);
}

int CTemplNative::WaitForEchoReply(SOCKET s)
{
	struct timeval Timeout;
	fd_set readfds;

	readfds.fd_count = 1;
	readfds.fd_array[0] = s;
	Timeout.tv_sec = 1;
	Timeout.tv_usec = 0;

	return(select(1, &readfds, NULL, NULL, &Timeout));
}

u_short CTemplNative::in_cksum(u_short * addr, int len)
{
	register int nleft = len;
	register u_short *w = addr;
	register u_short answer;
	register int sum = 0;

	/*
	*  Our algorithm is simple, using a 32 bit accumulator (sum),
	*  we add sequential 16 bit words to it, and at the end, fold
	*  back all the carry bits from the top 16 bits into the lower
	*  16 bits.
	*/
	while (nleft > 1) {
		sum += *w++;
		nleft -= 2;
	}

	/* mop up an odd byte, if necessary */
	if (nleft == 1) {
		u_short	u = 0;

		*(u_char *)(&u) = *(u_char *)w;
		sum += u;
	}

	/*
	* add back carry outs from top 16 bits to low 16 bits
	*/
	sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
	sum += (sum >> 16);			/* add carry */
	answer = ~sum;				/* truncate to 16 bits */
	return (answer);
}
