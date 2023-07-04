#include <iostream>
#include <iads.h>
#include <AdsHlp.h>
#include <atlbase.h>
#include "pch.h"
#include <codecvt>
#include "add_employee.h"
#include <jni.h>

using namespace std;

const BSTR USERNAME = SysAllocString(L"admin@cloud.com");
const BSTR PASSWORD = SysAllocString(L"JAGA@jaga2001");
const BSTR OU_NAME = SysAllocString(L"OU=zoho");



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

JNIEXPORT jboolean JNICALL Java_add_1employee_add_1emp
(JNIEnv* env, jobject obj, jstring ip_address,jstring employee_id, jstring employee_name, jstring employee_ph_number, jstring employee_dept_id, jstring employee_report) {
    jboolean isCopyUsername;
    std::cout << "JNI call" << std::endl;
    const char* ip_address_c = env->GetStringUTFChars(ip_address, &isCopyUsername);
    const char* emp_name = env->GetStringUTFChars(employee_name, &isCopyUsername);
    const char* emp_id = env->GetStringUTFChars(employee_id, &isCopyUsername);
    const char* emp_ph = env->GetStringUTFChars(employee_ph_number, &isCopyUsername);
    const char* emp_dept = env->GetStringUTFChars(employee_dept_id, &isCopyUsername);
    const char* emp_report = env->GetStringUTFChars(employee_report, &isCopyUsername);
    
    std::string emp_id_s = emp_id;
    std::string emp_name_s = emp_name;
    std::string emp_report_s = emp_report;
    std::string emp_user_cn = "CN=" + emp_name_s + "-" + emp_id + ",OU=zoho";
    BSTR bstrUserCN = ConvertStringToBSTR(emp_user_cn);
    BSTR bstr_emp_id = ConvertStringToBSTR(emp_id);
    BSTR bstr_emp_name = ConvertStringToBSTR(emp_name);
    BSTR bstr_emp_ph = ConvertStringToBSTR(emp_ph);
    BSTR bstr_emp_dept = ConvertStringToBSTR(emp_dept);
    BSTR bstr_ip_address = ConvertStringToBSTR(ip_address_c);

    BSTR bstrPassword = SysAllocString(L"JAASjaas");
    BSTR bstrOU = SysAllocString(L"zoho");
    std::cout << "JNI call10" << std::endl;

    std::cout << emp_name << std::endl;
    std::cout << emp_ph << std::endl;
    std::cout << emp_dept << std::endl;
    std::cout << emp_report << std::endl;
    
    jboolean result = false;
    IADsContainer* pCont = NULL;
    IADsUser* pUser = NULL;
    IDispatch* pDisp = NULL;
    BSTR bstrUserClass = NULL;
    VARIANT var;
    HRESULT hr = S_OK;
    CoInitialize(NULL);
    IDirectorySearch* pDSSearch = NULL;
    std::string report_manager = "";
	hr = ADsOpenObject(bstr_ip_address, USERNAME, PASSWORD, ADS_SECURE_AUTHENTICATION, IID_IDirectorySearch, (void**)&pDSSearch);
    if (FAILED(hr)) {
        std::cerr << "Failed to open directory object: " << hr << std::endl;
        return JNI_FALSE;
    }
    hr = pDSSearch->QueryInterface(IID_IADsContainer, (void**)&pCont);
    if (FAILED(hr)) {
        std::cout << "Failed to get container object: " << hr << std::endl;
        goto Cleanup;

    }
    bstrUserClass = SysAllocString(L"user");
    hr = pCont->Create(bstrUserClass, bstrUserCN, &pDisp);
    if (FAILED(hr)) {
        std::cout << "Failed to create user object: " << hr << std::endl;
        goto Cleanup;

    }
   

    hr = pDisp->QueryInterface(IID_IADsUser, (void**)&pUser);
    if (FAILED(hr)) {
        std::cout << "Failed to get IADsUser interface: " << hr << std::endl;
        goto Cleanup;

    }
    report_manager = manager_dn(emp_report_s, pDSSearch);

    VariantInit(&var);
    V_BSTR(&var) = bstr_emp_name;
    V_VT(&var) = VT_BSTR;
    hr = pUser->Put(CComBSTR("sAMAccountName"), var);
    VariantClear(&var);
    if (FAILED(hr)) {
        std::cout << "Failed sAMAcc: " << hr << std::endl;
        goto Cleanup;
    }
    V_BSTR(&var) = bstr_emp_id;
    V_VT(&var) = VT_BSTR;
    hr = pUser->Put(CComBSTR("employeeID"), var);
    VariantClear(&var);
    if (FAILED(hr)) {
        std::cout << "Failed id: " << hr << std::endl;
        goto Cleanup;
    }
    

    V_BSTR(&var) = bstr_emp_ph;
    V_VT(&var) = VT_BSTR;
    hr = pUser->Put(CComBSTR("mobile"), var);
    VariantClear(&var);
    if (FAILED(hr)) {
        std::cout << "Failed mo: " << hr << std::endl;
        goto Cleanup;
    }

    V_BSTR(&var) = SysAllocString(L"zoho");
    V_VT(&var) = VT_BSTR;
    hr = pUser->Put(CComBSTR("ou"), var);
    VariantClear(&var);
    if (FAILED(hr)) {
        std::cout << "Failed OU: " << hr << std::endl;
        goto Cleanup;
    }
    
    V_BSTR(&var) = bstr_emp_dept;
    V_VT(&var) = VT_BSTR;
    hr = pUser->Put(CComBSTR("department"), var);
    VariantClear(&var);
    if (FAILED(hr)) {
        std::cout << "Failed depart: " << hr << std::endl;
        goto Cleanup;
    }
    
    if(report_manager.empty())
    {
        goto Cleanup;
    }

    V_BSTR(&var) = ConvertStringToBSTR(report_manager);
    V_VT(&var) = VT_BSTR;
    hr = pUser->Put(CComBSTR("manager"), var);
    VariantClear(&var);

    if (FAILED(hr)) {
        std::cout << "Failed manager: " << hr << std::endl;
        goto Cleanup;
    }
    




    pUser->SetPassword(bstrPassword);
    SysFreeString(bstrPassword);
    hr = pUser->SetInfo();
    if (FAILED(hr)) {
        std::cout << "Failed to commit user object: " << hr << std::endl;
        goto Cleanup;
    }
    else
    {
        result = true;
        goto Cleanup;
    }

Cleanup:
    if (pUser) pUser->Release();
    if (pDisp) pDisp->Release();
    if (pCont) pCont->Release();
    if (pDSSearch) pDSSearch->Release();
    return result;
}
