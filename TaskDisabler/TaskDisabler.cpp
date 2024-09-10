#include <iostream>
#include "tasks.h"

int main()
{
    std::wstring path;
    std::cout << "Type Task Path: ";
    std::wcin >> path;
    Tasks tasks = Tasks();
    tasks.DisableTask(path);
    tasks.Close();
}

