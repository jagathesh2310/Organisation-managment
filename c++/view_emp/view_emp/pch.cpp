#include <iostream>
#include <iads.h>
#include <AdsHlp.h>
#include<atlbase.h>
#include "pch.h"
#include <codecvt>
#include "view_employee.h"
#include "json.hpp"
#include <jni.h>
using namespace std;
using json = nlohmann::json;

std::string lpwstr_to_string(LPWSTR lpwstr)
{
    std::wstring ws(lpwstr);
    std::wstring_convert<codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(ws);
}
BSTR ConvertStringToBSTR(const std::string& str)
{
    const char* charStr = str.c_str();
    BSTR bstr = SysAllocStringLen(NULL, str.length());
    MultiByteToWideChar(CP_ACP, 0, charStr, str.length(), bstr, str.length());
    return bstr;
}

std::string manager_dn(const std::string& report_id, IDirectorySearch* pDSSearch) {
    std::string dn = "";
    std::string employeeID = report_id;
    std::wstring filter = L"(&(objectCategory=person)(objectClass=user)(|(employeeID=" + std::wstring(employeeID.begin(), employeeID.end()) + L")))";
    LPOLESTR lpFilter = (LPOLESTR)filter.c_str();
    LPWSTR pszAttr[] = { (LPWSTR)L"distinguishedname" };
    const DWORD dwCount = sizeof(pszAttr) / sizeof(LPWSTR);

    ADS_SEARCH_HANDLE hSearch = NULL;
    HRESULT hr = pDSSearch->ExecuteSearch(const_cast<LPWSTR>(filter.c_str()), const_cast<LPWSTR*>(pszAttr), dwCount, &hSearch);
    if (FAILED(hr)) {
        std::cerr << "Failed to execute search: " << hr << std::endl;
        return dn;
    }

    if (SUCCEEDED(hr)) {
        ADS_SEARCH_COLUMN col;
        if (SUCCEEDED(pDSSearch->GetNextRow(hSearch))) {
            if (SUCCEEDED(pDSSearch->GetColumn(hSearch, pszAttr[0], &col))) {
                dn = lpwstr_to_string(col.pADsValues[0].CaseIgnoreString);
                pDSSearch->FreeColumn(&col);
            }
        }
        pDSSearch->CloseSearchHandle(hSearch);
    }

    return dn;
}

std::string manager_details(const std::string& dn_name, IDirectorySearch* pDSSearch) {
    std::string details = "";
    std::string distinguishedname = dn_name;
    std::wstring filter = L"(&(objectCategory=person)(objectClass=user)(|(distinguishedname=" + std::wstring(distinguishedname.begin(), distinguishedname.end()) + L")))";
    LPOLESTR lpFilter = (LPOLESTR)filter.c_str();
    LPWSTR pszAttr[] = { (LPWSTR)L"sAMAccountName",(LPWSTR)L"employeeID"};
    const DWORD dwCount = sizeof(pszAttr) / sizeof(LPWSTR);

    ADS_SEARCH_HANDLE hSearch = NULL;
    HRESULT hr = pDSSearch->ExecuteSearch(const_cast<LPWSTR>(filter.c_str()), const_cast<LPWSTR*>(pszAttr), dwCount, &hSearch);
    if (FAILED(hr)) {
        std::cerr << "Failed to execute search: " << hr << std::endl;
        return details;
    }

    if (SUCCEEDED(hr)) {
        ADS_SEARCH_COLUMN col;
        if (SUCCEEDED(pDSSearch->GetNextRow(hSearch))) {
            if (SUCCEEDED(pDSSearch->GetColumn(hSearch, pszAttr[0], &col))) {
                details = lpwstr_to_string(col.pADsValues[0].CaseIgnoreString);
                pDSSearch->FreeColumn(&col);
            }
            if (SUCCEEDED(pDSSearch->GetColumn(hSearch, pszAttr[1], &col))) {
                
                details += "-" + lpwstr_to_string(col.pADsValues[0].CaseIgnoreString);
                pDSSearch->FreeColumn(&col);
            }
        }
        pDSSearch->CloseSearchHandle(hSearch);
    }

    return details;
}


JNIEXPORT jstring JNICALL Java_view_1employee_view_1emp
(JNIEnv* env, jobject obj,jstring ip_address, jstring manager)

{
    jboolean isCopyUsername;
    const char* ip_address_c = env->GetStringUTFChars(ip_address, &isCopyUsername);
    BSTR bstr_ip_address = ConvertStringToBSTR(ip_address_c);
    const char* manager_c = env->GetStringUTFChars(manager, &isCopyUsername);
    std::string manager_s = manager_c;
    std::wstring filter;
    HRESULT hr = S_OK;
    CoInitialize(NULL);
    IDirectorySearch* pDSSearch = NULL;

    hr = ADsOpenObject(bstr_ip_address, L"admin@cloud.com", L"JAGA@jaga2001", 0, IID_IDirectorySearch, (void**)&pDSSearch);

    if (FAILED(hr)) {
        std::cout << "Failed to connect to Active Directory." << std::endl;
        return NULL;
    }
    if (manager_s._Equal("all"))
    {

        filter = L"(&(objectCategory=person)(objectClass=user)(OU=zoho))";
    }
    else
    {
        std::string manager_cc = manager_dn(manager_s, pDSSearch);
        filter = L"(&(objectCategory=person)(objectClass=user)(OU=zoho)(manager=" + std::wstring(manager_cc.begin(), manager_cc.end()) + L"))";
    }

    LPWSTR pszAttr[] = { (LPWSTR)
    L"sAMAccountName",
    (LPWSTR)L"name",
   (LPWSTR)L"employeeID",
    (LPWSTR)L"department",
    (LPWSTR)L"manager",
    (LPWSTR)L"mobile"
    };

    ADS_SEARCH_HANDLE hSearch;
    DWORD dwCount = sizeof(pszAttr) / sizeof(LPWSTR);

    
    LPOLESTR lpFilter = (LPOLESTR)filter.c_str();

    HRESULT hResult = pDSSearch->ExecuteSearch(lpFilter, pszAttr, dwCount, &hSearch);
    if (FAILED(hResult))
    {
        std::cout << "Failed to execute search." << std::endl;
        pDSSearch->Release();
        return NULL;
    }
    int count = 0;
    json arr;
    ADS_SEARCH_COLUMN col;
    int size = 1;
    while (SUCCEEDED(pDSSearch->GetNextRow(hSearch)))
    {
        json myObject;
        
        if (SUCCEEDED(pDSSearch->GetColumn(hSearch, CComBSTR("sAMAccountName"), &col)))
        {
            
                LPWSTR sam_name = col.pADsValues[0].CaseIgnoreString;
                myObject["employee_name"] = lpwstr_to_string(col.pADsValues[0].CaseIgnoreString);
                
                size = WideCharToMultiByte(CP_UTF8, 0, sam_name, -1, NULL, 0, NULL, NULL);
            
            pDSSearch->FreeColumn(&col);
        }
        else
        {
            break;
        }
        if (SUCCEEDED(pDSSearch->GetColumn(hSearch, CComBSTR("name"), &col)))
        {
            
                myObject["name"] = lpwstr_to_string(col.pADsValues[0].CaseIgnoreString);
            
            pDSSearch->FreeColumn(&col);
        }
        if (SUCCEEDED(pDSSearch->GetColumn(hSearch, CComBSTR("employeeID"), &col)))
        {
            
            
                myObject["employee_id"] = lpwstr_to_string(col.pADsValues[0].CaseIgnoreString);
            
            pDSSearch->FreeColumn(&col);
        }
        if (SUCCEEDED(pDSSearch->GetColumn(hSearch, CComBSTR("department"), &col)))
        {
           
                myObject["department_id"] = lpwstr_to_string(col.pADsValues[0].CaseIgnoreString);
            
            pDSSearch->FreeColumn(&col);
        }
        if (SUCCEEDED(pDSSearch->GetColumn(hSearch, CComBSTR("manager"), &col)))
        {
            
                myObject["report"] = manager_details(lpwstr_to_string(col.pADsValues[0].CaseIgnoreString), pDSSearch);

            
            pDSSearch->FreeColumn(&col);
        }

        if (SUCCEEDED(pDSSearch->GetColumn(hSearch, CComBSTR("mobile"), &col)))
        {
            
                myObject["phone_number"] = lpwstr_to_string(col.pADsValues[0].CaseIgnoreString);
        
            
            pDSSearch->FreeColumn(&col);
        }
        if (size > 0) {
            arr[count] = myObject;

            count++;
        }
        else
        {
           
            break;
        }
    }
    pDSSearch->Release();
    CoUninitialize();
    return env->NewStringUTF(arr.dump().c_str());
}