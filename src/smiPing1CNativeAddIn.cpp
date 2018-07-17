/*
����������� https://infostart.ru/public/184119/
��� �������� ������ ������� ����� 
include ����� ������� � ������ '���������� ��������'
lib � ������ '�������� ���������'

�������� ������� (������ ������� ����) - �������� ������������ - �������� C++

���c��������, ��� ��� �������� ������ ��� Win
*/

#include "stdafx.h"

#include <stdio.h>
#include <wchar.h>
#include <string>

//������ �� ������� �� its.
//����� ��� �������������� ������ � �������
//������ ��� ���������������� ������ ��� win 
#include "WcharWrapper.h"

//����������� ������������ ����, � ������� ����� ��������� ���� ������
#include "smiPing1CNativeAddIn.h"

/*��������� ��������� ������� � ������� ��� en � ru, ����� �� ������� its
�� �������� �� ���� �����: Field1,2 ������������� ����1,2
� ���� ������� Method1,2 ������������� �����1,2
*/

static const wchar_t *g_PropNames[] = {
	L"ForWrite", /*�������� ������������� eTempProp*/
	L"ForRead"
};
static const wchar_t *g_MethodNames[] = {
	L"WriteField", //����� ������������� eTempMethod
	L"ReadField"   //����� ������������� eTempMethodFunc
};

static const wchar_t *g_PropNamesRu[] = {
	L"���������",  /*�������� ������������� eTempProp*/
	L"���������"   /*�������� ������������� eTempPropReadable*/
};
static const wchar_t *g_MethodNamesRu[] = {
	L"����������", //����� ������������� eTempMethod
	L"����������"  //����� ������������� eTempMethodFunc
};

//������ ��� ������ �� ������� CAddInNative �� ���� CTemplNative
//��� ���������� ������ ������ ���� ����� �������� CTemplNative �� ����� ���������� ���
//� ������ templNative.cpp, h, def. ����� � ������ ��������������� ��������������
//
//�� ������ ������������� ��� ������ �� ���� ����������� �������, 
//� ��� �� ������������ � �����������
//
static const wchar_t g_kClassNames[] = L"CTemplNative"; //|OtherClass1|OtherClass2";

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
	/* ��� ���� � �������
	static WCHAR_T* names = 0;
	if (!names)
		::convToShortWchar(&names, g_kClassNames);
	return names;
	*/
	//����� �� �������� its
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

	m_boolTempProp = false; //��������� �������� ��������������� eTempProp
						//��� ���������������� ����� ������ ��� ����� � ������ ����� �����
	m_boolTempPropReadable = false; //��������� �������� ��������������� eTempProp
							//��� ���������������� ����� ������ ��� ������ � ������ ����� �����
}
//---------------------------------------------------------------------------//
CTemplNative::~CTemplNative()
{
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
	//��� ������������ ����������� �������
	if (m_boolTempProp)
	{
		m_boolTempProp = !m_boolTempProp;
	}
}
//---------------------------------------------------------------------------//
bool CTemplNative::RegisterExtensionAs(WCHAR_T** wsExtensionName)
{ 
	const wchar_t *wsExtension = L"TemplNativeExtension"; //��� ����������
	//������� �� ����� �������� �� 1�
	//�������� ��������� = �����("AddIn.MyComponent.TemplNativeExtension"); 
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
	int iActualSize = 0;

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
bool CTemplNative::GetPropVal(const long lPropNum, tVariant* pvarPropVal)
{ 
	/*����� �� ������� its*/
	//���������� �������� ��������
	switch (lPropNum)
	{
	case eTempPropReadable://������ ���������� �������� ��������
		TV_VT(pvarPropVal) = VTYPE_BOOL;
		TV_BOOL(pvarPropVal) = m_boolTempPropReadable;
		break;
	case eTempProp: //����� �� ���������, �.�. ��-��������� ���� �� ��������
		return false;
	default:
		return false;
	}

	//�������� ���������
	return true;
}
//---------------------------------------------------------------------------//
bool CTemplNative::SetPropVal(const long lPropNum, tVariant* varPropVal)
{ 
	/*����� �� ������� its*/
	switch (lPropNum)
	{
	case eTempProp: //������������� ���������� �������� � ��������
		if (TV_VT(varPropVal) != VTYPE_BOOL)
			return false;
		m_boolTempProp = TV_BOOL(varPropVal);
		break;
	case eTempPropReadable:
		return false; //��������� ��� ������� ��������� �������� �� ���������� ��� ������
		//��������� ������� ��-���������
	default:
		return false;
	}

	//�������� �����������
	return true;
}
//---------------------------------------------------------------------------//
bool CTemplNative::IsPropReadable(const long lPropNum)
{ 
	/*����� �� ������� its*/
	switch (lPropNum)
	{
	/*������ �������� �� ������������ ����*/
	case eTempProp:
		return false;
	/*�������� ��� �������� ��������������� eTempPropReadable ����� ������*/
	case eTempPropReadable:
		return true;
	default:
	/*��-������� ������ ������*/
		return false;
	}

	return false;
}
//---------------------------------------------------------------------------//
bool CTemplNative::IsPropWritable(const long lPropNum)
{
	/*����� �� ������� its*/
	switch (lPropNum)
	{
	/*�������� ��� �������� ��������������� eTempProp ����� ����������*/
	case eTempProp:
		return true;
	/*������ �������� ���� �� ���������� ��� ������*/
	case eTempPropReadable:
		return false;
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
	switch (lMethodNum)
	{
	case eTempMethod:
		return 1; /*� ���� ������ ���� ��������*/
	
	case eTempMethodFunc: /*� ������� ��� ����������*/
		return 0;
	default:
		return 0;
	}

    return 0;
}
//---------------------------------------------------------------------------//
bool CTemplNative::GetParamDefValue(const long lMethodNum, const long lParamNum,
                          tVariant *pvarParamDefValue)
{ 
	/*����� �� ������� its
	���������� �������� ��-��������� ��� ���������� �������
	*/
	TV_VT(pvarParamDefValue) = VTYPE_EMPTY;

	switch (lMethodNum)
	{
	case eTempMethod:
	case eTempMethodFunc:
	/*case eMethShowInStatusLine:
	case eMethStartTimer:
	case eMethStopTimer:
	case eMethShowMsgBox:
	*/
		// There are no parameter values by default 
		break;
	default:
		return false;
	}

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
	case eTempMethod:
		return false;
	/*� ������ ���� ������������ ��������*/
	case eTempMethodFunc:
		return true;
	default:
		return false;
	}

	return false;
}
//---------------------------------------------------------------------------//
bool CTemplNative::CallAsProc(const long lMethodNum,
                    tVariant* paParams, const long lSizeArray)
{ 
	/*����� �� ������� its*/
	/*����� ������ ��� ���������*/
	switch (lMethodNum)
	{
	case eTempMethod: //����� ��������� ������ ��� ���������
		return fTempMethod(paParams, lSizeArray);
	//������ �� ������� its
	/*
	case eMethDisable:
		m_boolEnabled = false;
		break;
	case eMethShowInStatusLine:
		if (m_iConnect && lSizeArray)
		{
			tVariant *var = paParams;
			m_iConnect->SetStatusLine(var->pwstrVal);
		}
		break;
	case eMethStartTimer:
		pAsyncEvent = m_iConnect;
		m_hTimerQueue = CreateTimerQueue();
		CreateTimerQueueTimer(&m_hTimer, m_hTimerQueue,
			(WAITORTIMERCALLBACK)MyTimerProc, 0, 1000, 1000, 0);
		break;
	case eMethStopTimer:
		if (m_hTimer != 0)
		{
			DeleteTimerQueue(m_hTimerQueue);
			m_hTimerQueue = 0;
			m_hTimer = 0;
		}

		m_uiTimer = 0;
		pAsyncEvent = NULL;
		break;
	case eMethShowMsgBox:
	{
		if (eAppCapabilities1 <= g_capabilities)
		{
			IAddInDefBaseEx* cnn = (IAddInDefBaseEx*)m_iConnect;
			IMsgBox* imsgbox = (IMsgBox*)cnn->GetInterface(eIMsgBox);
			if (imsgbox)
			{
				IPlatformInfo* info = (IPlatformInfo*)cnn->GetInterface(eIPlatformInfo);
				assert(info);
				const IPlatformInfo::AppInfo* plt = info->GetPlatformInfo();
				if (!plt)
					break;
				tVariant retVal;
				tVarInit(&retVal);
				if (imsgbox->Confirm(plt->AppVersion, &retVal))
				{
					bool succeed = TV_BOOL(&retVal);
					WCHAR_T* result = 0;

					if (succeed)
						::convToShortWchar(&result, L"OK");
					else
						::convToShortWchar(&result, L"Cancel");

					imsgbox->Alert(result);
					delete[] result;

				}
			}
		}
	}
	break;
	*/
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

	switch (lMethodNum)
	{
		case eTempMethodFunc:
		{
			//������ ������ � ������������ ���������
			return fTempMethodFunc(pvarRetValue, paParams, lSizeArray);
		}
		break;

	}
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


//---------------------------------------------------------------------------//
//���������� ������ ����������
bool CTemplNative::fTempMethod(tVariant* paParams, const long lSizeArray) //��������� eTempMethod1
{
	//������������� �� ��������� ��� ������ �������� eTempProp1.
	//���������� � 1� ��� ���������, ��� 1� �� ����� ������������� ���������
	//� ���������� ���������� ������� ��������� ������ ������
	//������ ������ � �����������
	if (lSizeArray != 1 || !paParams) //���� ���-�� ���������� �� 1 ��� �������� �� ����������
		return false;

	if (TV_VT(paParams) != VTYPE_BOOL) //���� ��� ��������� �� ������
		return false;

	m_boolTempPropReadable = TV_BOOL(paParams);  //������������� �������� �����. eTempPropReadable �� �������� ���������
	return true;
}

bool CTemplNative::fTempMethodFunc(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) //��������� eTempMethod2
{
	//������ �� ��������� ��� ������ �������� eTempProp2.
	/*���������� �� 1�, ��� �������, � ���������� ���������� ������� ��������� ���������� ������*/

	//� ������ ������� ��� ���������� ������ ���������� ��������. ������ ������ � ����������� � fTempMethod
	//������������� ���������� �������� � ��� ������
	pvarRetValue->bVal = m_boolTempProp;
	TV_VT(pvarRetValue) = VTYPE_BOOL;
	return true;
}