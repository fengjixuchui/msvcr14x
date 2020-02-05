#ifdef _DEBUG
#pragma comment(linker, "/EXPORT:__CrtSetDbgBlockType")
#pragma comment(linker, "/EXPORT:__mbsdup_dbg=__strdup_dbg")
#endif