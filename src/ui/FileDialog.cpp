#include "Base.h"
#include "FileDialog.h"

#include <cstdio>
#include <cstring>

namespace Esox
{
    String FileDialog::Open()
    {
#if defined(__linux__)
        FILE* fp = popen("zenity --file-selection", "r");
        if(!fp)
        {
            ESOX_LOG_ERROR("popen returned NULL");
            return "";
        }
        char fileName[1024];
        if(fgets(fileName, 1024 * sizeof(char), fp))
        {
            fileName[strcspn(fileName, "\n")] = '\0';
            pclose(fp);
            return String(fileName);
        }
        else
        {
            pclose(fp);
            ESOX_LOG_ERROR("fgets returned NULL");
            return "";
        }
#elif defined(_WIN32)
        return "";
#endif
    }
}
