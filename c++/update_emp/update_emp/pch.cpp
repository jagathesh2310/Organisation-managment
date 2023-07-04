
#include <iostream>
#include <iads.h>
#include <AdsHlp.h>
#include <atlbase.h>
#include <codecvt>
#include "pch.h"
#include "update_employee.h"
#include <jni.h>


const BSTR USERNAME = SysAllocString(L"admin@cloud.com");
const BSTR PASSWORD = SysAllocString(L"JAGA@jaga2001");
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

JNIEXPORT jboolean JNICALL Java_update_1employee_update_1emp(JNIEnv* env, jobject obj, jstring ip_address, jstring employee_id, jstring employee_name, jstring employee_ph_number, jstring employee_dept_id, jstring employee_report) {
    const char* emp_name = env->GetStringUTFChars(employee_name, NULL);
    const char* ip_address_c = env->GetStringUTFChars(ip_address, NULL);
    const char* emp_id = env->GetStringUTFChars(employee_id, NULL);
    const char* emp_ph = env->GetStringUTFChars(employee_ph_number, NULL);
    const char* emp_dept = env->GetStringUTFChars(employee_dept_id, NULL);
    const char* emp_report = env->GetStringUTFChars(employee_report, NULL);
    jboolean result = false;

    std::string emp_id_s = emp_id;
    std::string emp_name_s = emp_name;
    std::string emp_report_s = emp_report;
    
    BSTR bstr_emp_id = ConvertStringToBSTR(emp_id);
    BSTR bstr_emp_ph = ConvertStringToBSTR(emp_ph);
    BSTR bstr_emp_dept = ConvertStringToBSTR(emp_dept);
    BSTR bstr_ip_address = ConvertStringToBSTR(ip_address_c);

    VARIANT var;
    IADsContainer* pCont = NULL;
    IADsUser* pUser = NULL;

    if (emp_name == NULL || emp_id == NULL || emp_ph == NULL || emp_dept == NULL || emp_report == NULL) {
        std::cerr << "Failed to retrieve string parameter(s) from Java method." << std::endl;
        return JNI_FALSE;
    }

    CoInitialize(NULL);
    IDirectorySearch* pDSSearch = NULL;
    HRESULT hr = ADsOpenObject(bstr_ip_address, USERNAME, PASSWORD, ADS_SECURE_AUTHENTICATION, IID_IDirectorySearch, (void**)&pDSSearch);
    if (FAILED(hr)) {
        std::cerr << "Failed to open directory object: " << hr << std::endl;
        return JNI_FALSE;
    }

    std::string emp_dn = manager_dn(emp_report, pDSSearch);
    if (emp_dn.empty()) {
        std::cerr << "Failed to retrieve manager's DN for employee with report ID ";
        return JNI_FALSE;
    }


    hr=pDSSearch->QueryInterface(IID_IADsContainer, (void**)&pCont);
    if (FAILED(hr)) {
        std::cout << "Failed to get container object: " << hr << std::endl;
        return JNI_FALSE;
    }
    std::string temp = "/";

    BSTR bstrUserPath = ConvertStringToBSTR(ip_address_c+ temp + manager_dn(emp_id_s, pDSSearch));
    std::cout << manager_dn(emp_id_s, pDSSearch) << std::endl;
    std::string report_manager = manager_dn(emp_report_s,pDSSearch );
    if (report_manager.empty())
    {
        std::cout << "failed manager" << std::endl;
    }
    hr = ADsOpenObject(bstrUserPath, L"admin@cloud.com", L"JAGA@jaga2001", 0, IID_IADsUser, (void**)&pUser);
    if (FAILED(hr)) {

        std::cout << "Failed to bind to user objet 1" << hr << std::endl;
        goto Cleanup;
    }


    VariantInit(&var);
    V_BSTR(&var) = ConvertStringToBSTR(emp_name_s);
    V_VT(&var) = VT_BSTR;

    hr = pUser->Put(CComBSTR("sAMAccountName"), var);

    VariantClear(&var);

    if (FAILED(hr)) {
        std::cout << "Failed sAMAcc: " << hr << std::endl;
        goto Cleanup;
    }



    std::cout << "7";
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



    V_BSTR(&var) = bstr_emp_dept;
    V_VT(&var) = VT_BSTR;
    hr = pUser->Put(CComBSTR("department"), var);
    VariantClear(&var);
    if (FAILED(hr)) {
        std::cout << "Failed depart: " << hr << std::endl;
        //goto Cleanup;
    }
    

    V_BSTR(&var) = ConvertStringToBSTR(report_manager);
    V_VT(&var) = VT_BSTR;
    hr = pUser->Put(CComBSTR("manager"), var);
    VariantClear(&var);

    if (FAILED(hr)) {
        std::cout << "Failed manager: " << hr << std::endl;
        return JNI_FALSE;
    }

    hr = pUser->SetInfo();
    if (FAILED(hr)) {
        std::cout << "Failed to save user changes: " << hr << std::endl;
    }
    else
    {
        result = true;
        std::cout << "User object updated successfully." << std::endl;
    }

    

Cleanup:
    std::cout << "Cleaned" << std::endl;
    if (pUser) pUser->Release();
    if (pCont) pCont->Release();
    if (pDSSearch) pDSSearch->Release();
    return result;
}
