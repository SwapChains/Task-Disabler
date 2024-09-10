#pragma once

#include <windows.h>
#include <taskschd.h>
#include <comdef.h>
#include <string>

#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")

class Tasks
{
private:
    ITaskService* pService = nullptr;
    ITaskFolder* pRootFolder = nullptr;

    void HandleError(HRESULT hr)
    {
        if (FAILED(hr))
        {
            _com_error err(hr);
            MessageBox(NULL, err.ErrorMessage(), L"Erro", MB_OK);
            exit(hr);
        }
    }
public:
    Tasks()
    {
        HandleError(CoInitializeEx(NULL, COINIT_MULTITHREADED));
        HandleError(CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pService));
        HandleError(pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t()));
        HandleError(pService->GetFolder(_bstr_t(L"\\"), &pRootFolder));
    }

    void Close()
    {
        if (pRootFolder) pRootFolder->Release();
        if (pService) pService->Release();
        CoUninitialize();
    }

    void DisableTask(std::wstring id)
    {
        IRegisteredTask* pTask = nullptr;
        HandleError(pRootFolder->GetTask(_bstr_t(id.c_str()), &pTask));

        ITaskDefinition* pTaskDef = nullptr;
        HandleError(pTask->get_Definition(&pTaskDef));

        ITriggerCollection* pTriggerCollection = nullptr;
        HandleError(pTaskDef->get_Triggers(&pTriggerCollection));

        long triggerCount = 0;
        HandleError(pTriggerCollection->get_Count(&triggerCount));

        for (long i = 1; i <= triggerCount; ++i) 
        {
            ITrigger* pTrigger = nullptr;
            HandleError(pTriggerCollection->get_Item(i, &pTrigger));
            HandleError(pTrigger->put_Enabled(VARIANT_FALSE));
            pTrigger->Release();
        }

        IRegisteredTask* pNewTask = nullptr;
        HandleError(pRootFolder->RegisterTaskDefinition(_bstr_t(id.c_str()), pTaskDef, TASK_UPDATE, _variant_t(), _variant_t(), TASK_LOGON_INTERACTIVE_TOKEN, _variant_t(L""), &pNewTask));

        if (pNewTask) pNewTask->Release();
        if (pTriggerCollection) pTriggerCollection->Release();
        if (pTaskDef) pTaskDef->Release();
        if (pTask) pTask->Release();
    }
};