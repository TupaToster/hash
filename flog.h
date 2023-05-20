#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>
#include <time.h>
#include <string.h>
#include <assert.h>

// DEFINE NDEBUG TO DISABLE LOGS

extern FILE *logOutf;

const unsigned char POISON1 = 0xBD;                    ///< 1 byte poison
const unsigned short POISON2 = 0xBDCF;                 ///< 2 byte poison
const unsigned int POISON4 = 0xBADC0FEE;               ///< 4 byte poison
const unsigned long long POISON8 = 0xBADC0FEEF04DED32; ///< 8 byte poison

#define setPoison(var) setPoisonInside(&var, sizeof(var)) ///< Macros for auto-poison

#define isPoison(var) isPoisonInside(&var, sizeof(var)) ///< Macros for checking for poison

void setPoisonInside(void *var, size_t sizeofVar);

bool isPoisonInside(void *var, size_t sizeofVar);

unsigned int countHash(void *from, void *to, unsigned int mult = 107u);

void flogIntern(const void *val, const char *varType, const char *varName, size_t varSize, const char *fileName, const char *funcName, size_t line);

const char *getFormatIntern(const char *varType);

#define getFormat(var) getFormatIntern(typeid(var).name())

#define autoRefresh 

#ifdef autoRefresh
#define refresher fprintf(logOutf, "<meta http-equiv=\"refresh\" content=\"5\">\n")
#else
#define refresher ;
#endif

#ifndef NDEBUG
#define flog(a)                                                                                                                 \
    {                                                                                                                           \
        if (logOutf == NULL)                                                                                                    \
        {                                                                                                                       \
            logOutf = fopen("logs_out.html", "w");                                                                              \
            setvbuf(logOutf, NULL, _IONBF, 0);                                                                                  \
            refresher;                                                                                                          \
            fprintf(logOutf, "<pre>----------------------------------------\n"                                                  \
                             "<style> body {padding: 25px; background-color: #252525; color: white; font-size: 25px;} </style>" \
                             "Logging session at compiled time : %s %s<br><br></pre>\n\n",                                      \
                    __TIME__, __DATE__);                                                                                        \
        }                                                                                                                       \
        flogIntern(&a, typeid(a).name(), #a, sizeof(a), __FILE__, __FUNCTION__, __LINE__);                                      \
    }

#define flogprintf(...)                                                                                                         \
    {                                                                                                                           \
        if (logOutf == NULL)                                                                                                    \
        {                                                                                                                       \
            logOutf = fopen("logs_out.html", "w");                                                                              \
            setvbuf(logOutf, NULL, _IONBF, 0);                                                                                  \
            refresher;                                                                                                          \
            fprintf(logOutf, "<pre>----------------------------------------\n"                                                  \
                             "<style> body {padding: 25px; background-color: #252525; color: white; font-size: 25px;} </style>" \
                             "Logging session at compiled time : %s %s<br><br></pre>\n\n",                                      \
                    __TIME__, __DATE__);                                                                                        \
        }                                                                                                                       \
        fprintf(logOutf, __VA_ARGS__);                                                                                          \
    }
#else
#define flog(a) ;
#define flogprintf(...) ;
#endif