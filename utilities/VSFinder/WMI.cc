#include "VSFinder.h"
#include <Windows.h>
#include <wbemidl.h>
#include <memory>

template <typename T>
bool SomeWSTRToString(T inBSTR, std::string &outString)
{
    DWORD size = WideCharToMultiByte(CP_UTF8,
                                     0,
                                     inBSTR,
                                     -1,
                                     nullptr,
                                     0,
                                     nullptr,
                                     nullptr);

    if (size == 0)
    {
        return false;
    }

    std::unique_ptr<char[]> res = std::make_unique<char[]>(size);
    DWORD result = WideCharToMultiByte(CP_UTF8,
                                       0,
                                       inBSTR,
                                       -1,
                                       res.get(),
                                       size,
                                       nullptr,
                                       nullptr);

    if (result == 0)
    {
        return false;
    }

    outString = std::string(res.get());

    return true;
}

bool GetVsInfo(std::vector<std::map<std::string, val>> &groupQueryResult)
{

    // Initialize COm
    HRESULT hRes = CoInitializeEx(0, COINIT_MULTITHREADED);

    if (hRes < 0)
    {
        return false;
    }

    // Set general COM security levels
    hRes = CoInitializeSecurity(
        NULL,
        -1,
        NULL,
        NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE,
        NULL);

    if (hRes < 0)
    {
        CoUninitialize();
        return false;
    }

    IWbemLocator *pLoc = NULL;

    // Obtain the initial locator to WMI
    hRes = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator,
        reinterpret_cast<void **>(&pLoc));

    if (hRes < 0)
    {
        CoUninitialize();
        return false;
    }

    // Connect to WMI
    IWbemServices *pSvc = NULL;

    hRes = pLoc->ConnectServer(
        L"ROOT\\CIMV2",
        NULL,
        NULL,
        0,
        NULL,
        0,
        0,
        &pSvc);

    if (hRes < 0)
    {
        pLoc->Release();
        CoUninitialize();
        return false;
    }

    IEnumWbemClassObject *pClsObject = NULL;

    hRes = pSvc->ExecQuery(
        L"WQL",
        L"SELECT * FROM MSFT_VSInstance",
        WBEM_FLAG_RETURN_WBEM_COMPLETE,
        NULL,
        &pClsObject);

    if (hRes < 0)
    {
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return false;
    }

    HRESULT wmiIteratorResult;

    do
    {
        IWbemClassObject *wbemClsObj[10];
        ULONG pReturned;

        wmiIteratorResult = pClsObject->Next(
            WBEM_INFINITE,
            10,
            wbemClsObj,
            &pReturned);

        for (ULONG i = 0; i < pReturned; i++)
        {
            auto &&wbemObj = std::move(wbemClsObj[i]);
            std::vector<std::wstring> wbemObjClsNameCollection;
            // Find all query result key
            HRESULT wbemObjEnumResult = wbemObj->BeginEnumeration(0);

            do
            {
                BSTR wbemObjClsName = NULL;
                VARIANT wbemVariant;
                CIMTYPE wbemCimtype;
                long wbemPFlavor;

                wbemObjEnumResult = wbemObj->Next(
                    0,
                    &wbemObjClsName,
                    &wbemVariant,
                    &wbemCimtype,
                    &wbemPFlavor);

                if (wbemObjEnumResult >= 0)
                {
                    wbemObjClsNameCollection.emplace_back(std::wstring(wbemObjClsName, SysStringLen(wbemObjClsName)));
                }
                VariantClear(&wbemVariant);
            } while (wbemObjEnumResult == WBEM_S_NO_ERROR);

            std::map<std::string, val> wbemObjQueryKeyValueMappingCollection;

            for (auto &&wbemObjClsName : wbemObjClsNameCollection)
            {

                VARIANT wbemVariant;
                CIMTYPE wbemCimtype;
                long wbemPFlavor;
                long wbemObjQueryResult = wbemObj->Get(
                    wbemObjClsName.data(),
                    0,
                    &wbemVariant,
                    &wbemCimtype,
                    &wbemPFlavor);

                if (wbemObjQueryResult >= 0)
                {
                    std::string outString;

                    unsigned short valType = wbemVariant.vt;

                    std::string wbemObjClsNameUtf8Val;
                    SomeWSTRToString<wchar_t *>(const_cast<wchar_t *>(wbemObjClsName.data()), wbemObjClsNameUtf8Val);
                    val _v;
                    switch (wbemVariant.vt)
                    {
                    case VT_I2:
                    {
                        _v.valType = NUMBER_TYPE;
                        _v.numberVal = static_cast<double>(wbemVariant.iVal);
                        wbemObjQueryKeyValueMappingCollection.insert(std::pair<std::string, val>(wbemObjClsNameUtf8Val, _v));
                        break;
                    }
                    case VT_I4:
                    {
                        _v.valType = NUMBER_TYPE;
                        _v.numberVal = static_cast<double>(wbemVariant.lVal);
                        wbemObjQueryKeyValueMappingCollection.insert(std::pair<std::string, val>(wbemObjClsNameUtf8Val, _v));
                        break;
                    }
                    case VT_R4:
                    {
                        _v.valType = NUMBER_TYPE;
                        _v.numberVal = static_cast<double>(wbemVariant.fltVal);
                        wbemObjQueryKeyValueMappingCollection.insert(std::pair<std::string, val>(wbemObjClsNameUtf8Val, _v));
                        break;
                    }
                    case VT_R8:
                    {
                        _v.valType = NUMBER_TYPE;
                        _v.numberVal = static_cast<double>(wbemVariant.dblVal);
                        wbemObjQueryKeyValueMappingCollection.insert(std::pair<std::string, val>(wbemObjClsNameUtf8Val, _v));
                        break;
                    }
                    case VT_BSTR:
                    {
                        std::string _val;
                        SomeWSTRToString<BSTR>(wbemVariant.bstrVal, _val);
                        _v.valType = STRING_TYPE;
                        _v.stringVal = _val;
                        wbemObjQueryKeyValueMappingCollection.insert(std::pair<std::string, val>(wbemObjClsNameUtf8Val, _v));
                        break;
                    }
                    case VT_BOOL:
                    {
                        _v.valType = BOOLEAN_TYPE;
                        _v.boolVal = wbemVariant.boolVal;
                        wbemObjQueryKeyValueMappingCollection.insert(std::pair<std::string, val>(wbemObjClsNameUtf8Val, _v));
                        break;
                    }
                    default:
                    {
                        _v.valType = NULL_TYPE;
                        wbemObjQueryKeyValueMappingCollection.insert(std::pair<std::string, val>(wbemObjClsNameUtf8Val, _v));
                        break;
                    }
                    }
                }

                VariantClear(&wbemVariant);
            }

            wbemObj->EndEnumeration();
            wbemObj->Release();

            groupQueryResult.emplace_back(wbemObjQueryKeyValueMappingCollection);
        }
    } while (wmiIteratorResult == WBEM_S_NO_ERROR);

    pLoc->Release();
    pSvc->Release();
    pClsObject->Release();
    CoUninitialize();

    return true;
}
