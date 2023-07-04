#include <iostream>
#include <iads.h>
#include <AdsHlp.h>
#include<atlbase.h>
#include "pch.h"

#include <codecvt>

#include "delete_employee.h"
#include <jni.h>
const BSTR USERNAME = SysAllocString(L"admin@cloud.com");
const BSTR PASSWORD = SysAllocString(L"JAGAjaga2310");
const BSTR OU_NAME = SysAllocString(L"OU=zoho");

using namespace std;
BSTR ConvertStringToBSTR(const std::string& str)
{
    const char* charStr = str.c_str();
    BSTR bstr = SysAllocStringLen(NULL, str.length());
    MultiByteToWideChar(CP_ACP, 0, charStr, str.length(), bstr, str.length());
    return bstr;
}

std::string lpwstr_to_string(LPWSTR lpwstr)
{
    std::wstring ws(lpwstr);
    std::wstring_convert<codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(ws);
}

std::string employee_cn(const std::string& report_id, IDirectorySearch* pDSSearch) {
    std::string dn = "";
    std::string employeeID = report_id;
    std::wstring filter = L"(&(objectCategory=person)(objectClass=user)(|(employeeID=" + std::wstring(employeeID.begin(), employeeID.end()) + L")))";
    LPOLESTR lpFilter = (LPOLESTR)filter.c_str();
    LPWSTR pszAttr[] = { (LPWSTR)L"cn" };
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

JNIEXPORT jboolean JNICALL Java_delete_1employee_delete_1emp
(JNIEnv* env, jobject obj, jstring ip_address, jstring emp_id)

{

    jboolean isCopyUsername;
    const char* ip_address_c = env->GetStringUTFChars(ip_address, &isCopyUsername);
    BSTR bstr_ip_address = ConvertStringToBSTR(ip_address_c);
    const char* emp_id_c = env->GetStringUTFChars(emp_id, &isCopyUsername);
    std::string emp_id_s = emp_id_c;
    jboolean result = false;
    IADsContainer* pCont = NULL;
    HRESULT hr = S_OK;
    CoInitialize(NULL);
    IDirectorySearch* pDSSearch = NULL;

    hr = ADsOpenObject(bstr_ip_address, L"admin@cloud.com", L"JAGA@jaga2001", 0, IID_IDirectorySearch, (void**)&pDSSearch);
    if (FAILED(hr)) {
        std::cout << "Failed to connect to Active Directory." << std::endl;
        JNI_FALSE;
    }
    std::string emp_cn = employee_cn(emp_id_s,pDSSearch);
    hr = pDSSearch->QueryInterface(IID_IADsContainer, (void**)&pCont);
    if (FAILED(hr)) {
        std::cout << "Failed to get container object: " << hr << std::endl;
        return JNI_FALSE;
    }

    BSTR bstrUserCN = ConvertStringToBSTR("CN="+emp_cn+",OU=zoho");
    BSTR bstrUserClass = SysAllocString(L"user");
    hr = pCont->Delete(bstrUserClass, bstrUserCN);
    if (FAILED(hr)) {
        std::cout << "Failed to create user object: " << hr << std::endl;
         goto Cleanup;
    }
    std::cout << "success to create user object: " << hr << std::endl;

    
    IADsDeleteOps* pOps;
    hr = pCont->QueryInterface(IID_IADsDeleteOps, (void**)&pOps);
    if (FAILED(hr)) {
        std::cout << "Failed to get IADsUser interface: " << hr << std::endl;
        goto Cleanup;
    }
    std::cout << "Success to get IADsUser interface: " << hr << std::endl;
    pCont->Release();


    pOps->DeleteObject(0);
    pOps->Release();
    if (FAILED(hr))
    {
        std::cout << "Failed User object created" << std::endl;
    }
    else
    {
        result = true;
    }

Cleanup:
    std::cout << "Cleaned." << std::endl;
    if (pCont) pCont->Release();
    if (pDSSearch) pDSSearch->Release();
    return result;
}