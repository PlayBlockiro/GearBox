#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM "Windows"
    #define IS_WINDOWS 1
    #include <windows.h>
#else
    #define PLATFORM "Linux"
    #define IS_WINDOWS 0
    #include <unistd.h>
#endif


void check_for_updates();
int has_steam();
int has_portal();
void launch_portal();
void inject_source_commands();
void run_menu();

int main() {
    printf("Running on: %s\n", PLATFORM);
    
    check_for_updates();

    if (!has_steam()) {
        printf("[!] Steam not found. Please install Steam first.\n");
        return 1;
    }

    if (!has_portal()) {
        printf("[!] Portal 1 is not installed.\n");
        return 1;
    }

    launch_portal();

    printf("[*] Waiting for Portal 1 to launch...\n");
#if IS_WINDOWS
    Sleep(5000); 
#else
    sleep(5);
#endif

    inject_source_commands();
    run_menu();

    return 0;
}

void check_for_updates() {
    char input[10];
    printf("Do you want to check for updates? (y/n): ");
    fgets(input, sizeof(input), stdin);
    if (input[0] == 'y' || input[0] == 'Y') {
        printf("[*] Checking for updates... (not yet implemented)\n");
        // Placeholder for real update logic later
    }
}

int has_steam() {
#if IS_WINDOWS
    FILE *fp = _popen("where steam", "r");
    if (!fp) return 0;
#else
    FILE *fp = popen("which steam || flatpak info com.valvesoftware.Steam", "r");
    if (!fp) return 0;
#endif

    char buffer[256];
    int found = 0;
    while (fgets(buffer, sizeof(buffer), fp)) {
        if (strstr(buffer, "steam")) {
            found = 1;
            break;
        }
    }

#if IS_WINDOWS
    _pclose(fp);
#else
    pclose(fp);
#endif

    return found;
}

int has_portal() {
#if IS_WINDOWS
    FILE *f = fopen("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Portal\\portal.exe", "r");
    if (f) { fclose(f); return 1; }
#else
    char fullpath[512];
    const char *home = getenv("HOME");

    // Native Steam path
    snprintf(fullpath, sizeof(fullpath), "%s/.steam/steam/steamapps/common/Portal/hl2.sh", home);
    FILE *f = fopen(fullpath, "r");
    if (f) { fclose(f); return 1; }

    // Flatpak path
    snprintf(fullpath, sizeof(fullpath), "%s/.var/app/com.valvesoftware.Steam/.steam/steam/steamapps/common/Portal/hl2.sh", home);
    f = fopen(fullpath, "r");
    if (f) { fclose(f); return 1; }
#endif
    return 0;
}

void launch_portal() {
    printf("[*] Launching Portal 1...\n");

#if IS_WINDOWS
    system("start steam://rungameid/400");
#else
    int status = system("steam steam://rungameid/400");
    if (status != 0) {
        printf("[*] Trying Flatpak Steam...\n");
        system("flatpak run com.valvesoftware.Steam steam://rungameid/400");
    }
#endif
}

void inject_source_commands() {
    printf("[*] Writing Source Engine commands to autoexec.cfg...\n");

#if IS_WINDOWS
    const char *cfg_path = "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Portal\\portal\\cfg\\autoexec.cfg";
#else
    const char *home = getenv("HOME");
    char cfg_path[512];

    snprintf(cfg_path, sizeof(cfg_path), "%s/.steam/steam/steamapps/common/Portal/portal/cfg/autoexec.cfg", home);
    FILE *test = fopen(cfg_path, "r");
    if (!test) {
        snprintf(cfg_path, sizeof(cfg_path), "%s/.var/app/com.valvesoftware.Steam/.steam/steam/steamapps/common/Portal/portal/cfg/autoexec.cfg", home);
    } else {
        fclose(test);
    }
#endif

    FILE *cfg = fopen(cfg_path, "w");
    if (!cfg) {
        printf("[!] Failed to write autoexec.cfg. Try running as admin or check permissions.\n");
        return;
    }

    fprintf(cfg, "sv_cheats 1\n");
    fprintf(cfg, "god\n");
    fprintf(cfg, "stopsound\n");
    fprintf(cfg, "startupmovies_skip 1\n");
    fclose(cfg);

    printf("[+] Commands written to: %s\n", cfg_path);
}

void run_menu() {
    printf("[*] Compiling and launching menu components...\n");

#if IS_WINDOWS
    system("gcc menu.c -o menu.exe && start menu.exe");
    system("g++ menuadva.cpp -o menuadva.exe && start menuadva.exe");
#else
    system("gcc menu.c -o menu && ./menu &");
    system("g++ menuadva.cpp -o menuadva && ./menuadva &");
#endif
}
