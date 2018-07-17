#ifndef __WCHARWRAPPER_H__
#define __WCHARWRAPPER_H__

#include "ComponentBase.h"

/*
���������� �� ������� its. � ����� �����, ��� ��� ��� ��������������
������ ��� �������� ��� � 1� � ������.

������ ������ ������������� ��� linux

��� ����� �������� � ��������� ������, ��� �� �� ��������� �����
�������� ������� ����, � ������ ������ templNative.h
*/
class WcharWrapper
{
public:
	WcharWrapper(const wchar_t* str);
	~WcharWrapper();

	operator const wchar_t*() { return m_str_wchar; }
	operator wchar_t*() { return m_str_wchar; }
private:
	WcharWrapper & operator = (const WcharWrapper& other) {};
	WcharWrapper(const WcharWrapper& other) {};
private:
	wchar_t* m_str_wchar;
};

#endif //__WCHARWRAPPER_H__
