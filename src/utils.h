#ifndef UTILS_H
#define UTILS_H

#include "app_state.h"

// Platform-specific directory separator
#if defined(_WIN32)
    #define DIR_SEPARATOR "\\"
#else
    #define DIR_SEPARATOR "/"
#endif

// Function declarations
char* GetDefaultProjectPath(void);
void EnsureDirectoryExists(const char* path);
FileBrowser InitFileBrowser(const char* directory);
void UnloadFileBrowser(FileBrowser* browser);
void SaveProject(AppState* app);
void LoadProject(AppState* app, const char* path);

#endif // UTILS_H
