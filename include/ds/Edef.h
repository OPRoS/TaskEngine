#if defined(WIN32) || defined(WIN64)
    #define strcasecmp(x,y) stricmp((x),(y))
    #define strncasecmp(x,y,z) strnicmp((x),(y),(z))
#else
    #define stricmp(x,y) strcasecmp((x),(y))
    #define strnicmp(x,y,z) strncasecmp((x),(y),(z))
#endif

