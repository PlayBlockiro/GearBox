#include "utils.h"
#include <sys/stat.h>

#if defined(_WIN32)
    #include <windows.h>
    #include <direct.h>  // For _mkdir
    #define DEFAULT_PROJECT_PATH "C:/ProgramFiles/BlockiroLLC/HarmonyBox/Projects"
    #define mkdir(path, mode) _mkdir(path)
#else
    #include <unistd.h>
    #include <pwd.h>
    #define DEFAULT_PROJECT_PATH_PREFIX "/.HarmonyBox/Projects"
#endif

char* GetDefaultProjectPath(void) {
#if defined(_WIN32)
    return strdup(DEFAULT_PROJECT_PATH);
#else
    struct passwd* pw = getpwuid(getuid());
    if (!pw) return strdup("/tmp/HarmonyBox/Projects"); // Fallback
    
    size_t len = strlen(pw->pw_dir) + strlen(DEFAULT_PROJECT_PATH_PREFIX) + 2;
    char* path = malloc(len);
    if (path) {
        snprintf(path, len, "%s%s", pw->pw_dir, DEFAULT_PROJECT_PATH_PREFIX);
    }
    return path;
#endif
}

void EnsureDirectoryExists(const char* path) {
    if (!path) return;
    mkdir(path, 0777);
}

FileBrowser InitFileBrowser(const char* directory) {
    FileBrowser browser;
    
    strncpy(browser.currentDirectory, directory ? directory : ".", 511);
    browser.currentDirectory[511] = '\0';
    browser.files = LoadDirectoryFiles(browser.currentDirectory);
    browser.selectedFile = -1;
    browser.scrollPosition = (Vector2){0};
    
    return browser;
}

void UnloadFileBrowser(FileBrowser* browser) {
    if (browser) {
        UnloadDirectoryFiles(browser->files);
    }
}

void SaveProject(AppState* app) {
    // Placeholder for saving project functionality
    if (app && app->projectPath[0] != '\0') {
        TraceLog(LOG_INFO, "Saving project to: %s", app->projectPath);
        app->projectModified = false;
    }
}

void LoadProject(AppState* app, const char* path) {
    // Placeholder for loading project functionality
    if (app && path) {
        TraceLog(LOG_INFO, "Loading project from: %s", path);
        strncpy(app->projectPath, path, sizeof(app->projectPath) - 1);
        app->projectModified = false;
    }
}
