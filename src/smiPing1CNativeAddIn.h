
//���������� ��������� �������� ���������
//�������� ����� ���������� ���, ������ ���������� ��� ����� ���������
#ifndef __SMIPING1CNATIVEADDIN_H__
#define __SMIPING1CNATIVEADDIN_H__

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
		ePingIsComplit, //��-��������� false
		//���� ����������� �����������
		eGoodPingPercent, //������� �������� ������
		eMinTTL, //����������� ����� ����������� �����
		eMaxTTL, //����������� ����� ����������� �����
        eLastProp      // Always last
    };

    enum Methods
    {
		ePing = 0, //�������� ���� �� �������� ���������� 
		/*
		��� ������� ������������� ePingIsComplit � false
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

    // Attributes
	//����� �� ������� its
	IAddInDefBase * m_iConnect;
	IMemoryManager *m_iMemory;

	//���� ��� �������������
	char* m_strAddress; //ip ����� ��� ��� ���������� ��� �����. ��-��������� localhost
	int m_intPingCount; //���������� ������ ��-��������� 1
	int	m_intPackageSizeB; //������ ��������� ������ � ������ ��-��������� 8B
	bool m_boolPingIsComplit; //��������� ���������� ���������� ��-��������� false
	//���� ����������� �����������
	int m_intGoodPingPercent; //������� �������� ������
	int m_intMinTTL; //����������� ����� ����������� �����
	int m_intMaxTTL; //����������� ����� ����������� �����
		 
	//---------------------------------------------------------------------------//
	//���������� ������ ����������
	bool SendPing(); // �������� ���� �� �������� ���������� 
					 // ��� ������� ������������� ePingIsComplit � false
					 // ��� ���������� � true
					 
	
};
#endif //__SMIPING1CNATIVEADDIN_H__
