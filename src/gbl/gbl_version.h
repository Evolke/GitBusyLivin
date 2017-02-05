#ifndef GBL_VERSION_H
#define GBL_VERSION_H

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)
#define GBL_APP_VERSION STR(VERSION_MAJOR) "." STR(VERSION_MINOR) "." STR(VERSION_BUILD)
#define GBL_APP_COMPANY_NAME "Evolke"
#define GBL_APP_NAME "GitBusyLivin"

#endif // GBL_VERSION_H
