// Local JDI Host, as DLL.

#include "pch.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            EMUCtor();
            break;
        case DLL_PROCESS_DETACH:
            EMUDtor();
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}

#define endl    ( line[p] == 0 )
#define space   ( line[p] == 0x20 )
#define quot    ( line[p] == '\'' )
#define dquot   ( line[p] == '\"' )

static void Tokenize(char* line, std::vector<std::string>& args)
{
    int p, start, end;
    p = start = end = 0;

    args.clear();

    // while not end line
    while (!endl)
    {
        // skip space first, if any
        while (space) p++;
        if (!endl && (quot || dquot))
        {   // quotation, need special case
            p++;
            start = p;
            while (1)
            {
                if (endl)
                {
                    throw "Open quotation";
                    return;
                }

                if (quot || dquot)
                {
                    end = p;
                    p++;
                    break;
                }
                else p++;
            }

            args.push_back(std::string(line + start, end - start));
        }
        else if (!endl)
        {
            start = p;
            while (1)
            {
                if (endl || space || quot || dquot)
                {
                    end = p;
                    break;
                }

                p++;
            }

            args.push_back(std::string(line + start, end - start));
        }
    }
}

#undef space
#undef quot
#undef dquot
#undef endl

extern "C" __declspec(dllexport) char* CallJdi(char* request)
{
    Json json;
    std::vector<std::string> args;

    Tokenize(request, args);

    Json::Value* output = JDI::Hub.Execute(args);
    if (output == nullptr)
    {
        char* text = (char*)::CoTaskMemAlloc(3);
        assert(text);
        text[0] = '{';
        text[1] = '}';
        text[2] = 0;
        return text;
    }

    Json::Value* rootObj = json.root.AddObject(nullptr);
    rootObj->AddValue("reply", output);

    size_t jsonTextSize = 0;
    json.GetSerializedTextSize(nullptr, -1, jsonTextSize);

    char* jsonText = (char*)::CoTaskMemAlloc(jsonTextSize + 1);
    assert(jsonText);
    memset(jsonText, 0, jsonTextSize + 1);

    json.Serialize(jsonText, jsonTextSize, jsonTextSize);

    return jsonText;
}
