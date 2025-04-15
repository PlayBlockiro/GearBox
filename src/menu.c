#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <time.h>

#if defined(_WIN32)
    #include <windows.h>
    #include <direct.h>  // For _mkdir
    #define DEFAULT_PROJECT_PATH "C:/ProgramFiles/BlockiroLLC/GearBox/Projects"
    #define DIR_SEPARATOR "\\"
    #define mkdir(path, mode) _mkdir(path)
#else
    #define DEFAULT_PROJECT_PATH_PREFIX "/.GearBox/Projects"
    #define DIR_SEPARATOR "/"
#endif

#define MAX_INPUT_LEN 256
#define NUM_ITEMS 5
#define BASE_FONT_SIZE 20
#define HIGHLIGHTED_FONT_SIZE 30
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MAX_DIRECTORY_FILES 1024

typedef enum { SCREEN_MAIN_MENU, SCREEN_NEW_PROJECT, SCREEN_FILE_BROWSER } AppScreen;

typedef struct {
    const char *text;
    Rectangle bounds;
    bool isHovered;
    float fontSize;
} MenuItem;

// Simple text input structure
typedef struct {
    char text[MAX_INPUT_LEN];
    int cursorPosition;
    bool editMode;
    Rectangle bounds;
} TextInput;

// Simple file browser structure
typedef struct {
    char currentDirectory[512];
    FilePathList files;
    int selectedFile;
    Vector2 scrollPosition;
} FileBrowser;

char *GetDefaultProjectPath(void) {
#if defined(_WIN32)
    return strdup(DEFAULT_PROJECT_PATH);
#else
    struct passwd *pw = getpwuid(getuid());
    size_t len = strlen(pw->pw_dir) + strlen(DEFAULT_PROJECT_PATH_PREFIX) + 2;
    char *path = malloc(len);
    snprintf(path, len, "%s%s", pw->pw_dir, DEFAULT_PROJECT_PATH_PREFIX);
    return path;
#endif
}

void EnsureDirectoryExists(const char *path) {
    mkdir(path, 0777);
}

// Initialize file browser with a starting directory
FileBrowser InitFileBrowser(const char *directory) {
    FileBrowser browser;
    
    strncpy(browser.currentDirectory, directory, 511);
    browser.currentDirectory[511] = '\0';
    browser.files = LoadDirectoryFiles(browser.currentDirectory);
    browser.selectedFile = -1;
    browser.scrollPosition = (Vector2){0};
    
    return browser;
}

// Unload file browser resources
void UnloadFileBrowser(FileBrowser *browser) {
    UnloadDirectoryFiles(browser->files);
}

// Custom GUI functions
bool Button(Rectangle bounds, const char *text) {
    Vector2 mousePoint = GetMousePosition();
    bool mouseHover = CheckCollisionPointRec(mousePoint, bounds);
    bool pressed = false;
    
    // Draw button
    Color buttonColor = mouseHover ? (Color){81, 113, 144, 255} : (Color){59, 91, 118, 255};
    DrawRectangleRec(bounds, buttonColor);
    DrawRectangleLinesEx(bounds, 1, mouseHover ? WHITE : LIGHTGRAY);
    
    // Draw text centered
    int textWidth = MeasureText(text, 20);
    DrawText(text, bounds.x + (bounds.width - textWidth)/2, bounds.y + (bounds.height - 20)/2, 20, WHITE);
    
    // Check click
    if (mouseHover && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) pressed = true;
    
    return pressed;
}

void UpdateTextInput(TextInput *input) {
    Vector2 mousePoint = GetMousePosition();
    bool mouseHover = CheckCollisionPointRec(mousePoint, input->bounds);
    
    // Toggle edit mode on click
    if (mouseHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        input->editMode = true;
    } else if (!mouseHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        input->editMode = false;
    }
    
    // Handle text input when in edit mode
    if (input->editMode) {
        // Get character pressed (unicode)
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (strlen(input->text) < MAX_INPUT_LEN - 1)) {
                // Insert character at cursor position
                memmove(input->text + input->cursorPosition + 1, 
                        input->text + input->cursorPosition, 
                        strlen(input->text) - input->cursorPosition);
                input->text[input->cursorPosition] = (char)key;
                input->cursorPosition++;
            }
            key = GetCharPressed();
        }
        
        // Handle special keys
        if (IsKeyPressed(KEY_BACKSPACE) && input->cursorPosition > 0) {
            memmove(input->text + input->cursorPosition - 1, 
                    input->text + input->cursorPosition, 
                    strlen(input->text) - input->cursorPosition + 1);
            input->cursorPosition--;
        }
        
        if (IsKeyPressed(KEY_DELETE) && input->cursorPosition < strlen(input->text)) {
            memmove(input->text + input->cursorPosition, 
                    input->text + input->cursorPosition + 1, 
                    strlen(input->text) - input->cursorPosition);
        }
        
        if (IsKeyPressed(KEY_LEFT) && input->cursorPosition > 0) input->cursorPosition--;
        if (IsKeyPressed(KEY_RIGHT) && input->cursorPosition < strlen(input->text)) input->cursorPosition++;
        if (IsKeyPressed(KEY_HOME)) input->cursorPosition = 0;
        if (IsKeyPressed(KEY_END)) input->cursorPosition = strlen(input->text);
    }
    
    // Draw text input
    Color backgroundColor = input->editMode ? (Color){70, 70, 70, 255} : (Color){60, 60, 60, 255};
    DrawRectangleRec(input->bounds, backgroundColor);
    DrawRectangleLinesEx(input->bounds, 1, input->editMode ? WHITE : GRAY);
    
    // Draw text with padding
    DrawText(input->text, input->bounds.x + 5, input->bounds.y + (input->bounds.height - 20)/2, 20, WHITE);
    
    // Draw cursor when in edit mode
    if (input->editMode) {
        // Calculate cursor position
        int cursorX = input->bounds.x + 5 + MeasureText(TextSubtext(input->text, 0, input->cursorPosition), 20);
        DrawRectangle(cursorX, input->bounds.y + 5, 2, input->bounds.height - 10, WHITE);
    }
}

int main(void) {
    // Allow window resizing
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GearBox");
    SetTargetFPS(60);

    // Terraria‑style subtitle
    const char *randomSubtitles[] = {
        "Powered by duct tape", "Built in MS Paint", "No refunds",
        "Made with bugs!", "100% more spaghetti", "Now with raytracing*",
        "*Not really", "Intern Edition"
    };
    int numSubs = sizeof(randomSubtitles)/sizeof(randomSubtitles[0]);
    srand((unsigned)time(NULL));
    char title[128];
    snprintf(title, 128, "GearBox - %s", randomSubtitles[rand()%numSubs]);
    SetWindowTitle(title);

    AppScreen screen = SCREEN_MAIN_MENU;
    MenuItem items[NUM_ITEMS] = {
        {"New Project", {50,100,200,40},false,BASE_FONT_SIZE},
        {"Open Project",{50,160,200,40},false,BASE_FONT_SIZE},
        {"Marketplace",{50,220,200,40},false,BASE_FONT_SIZE},
        {"Settings",    {50,280,200,40},false,BASE_FONT_SIZE},
        {"Exit",        {50,340,200,40},false,BASE_FONT_SIZE}
    };
    int sel = 0;

    // New Project fields
    TextInput projectNameInput = {
        .text = "",
        .cursorPosition = 0,
        .editMode = false,
        .bounds = (Rectangle){50, 125, 400, 30}
    };
    
    TextInput projectPathInput = {
        .cursorPosition = 0,
        .editMode = false,
        .bounds = (Rectangle){50, 195, 400, 30}
    };
    
    // Initialize path with default
    char *defaultPath = GetDefaultProjectPath();
    strcpy(projectPathInput.text, defaultPath);
    free(defaultPath);
    
    // File browser
    FileBrowser fileBrowser;
    bool fileBrowserInitialized = false;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        if (screen == SCREEN_MAIN_MENU) {
            Vector2 m = GetMousePosition();
            if (IsKeyPressed(KEY_DOWN)) sel = (sel+1)%NUM_ITEMS;
            if (IsKeyPressed(KEY_UP))   sel = (sel-1+NUM_ITEMS)%NUM_ITEMS;

            for (int i=0; i<NUM_ITEMS; i++) {
                bool hov = CheckCollisionPointRec(m, items[i].bounds);
                bool seli = (i==sel);
                items[i].isHovered = hov;
                float tgt = (hov||seli)?HIGHLIGHTED_FONT_SIZE:BASE_FONT_SIZE;
                items[i].fontSize += (tgt - items[i].fontSize)*0.2f;
                if (hov) sel = i;
            }

            DrawText("GearBox :3", 50,30,30,RAYWHITE);
            for (int i=0; i<NUM_ITEMS; i++) {
                DrawText(items[i].text,
                         items[i].bounds.x,
                         items[i].bounds.y,
                         (int)items[i].fontSize,
                         (i==sel)?RED:RAYWHITE);
            }

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                CheckCollisionPointRec(GetMousePosition(), items[0].bounds)) {
                screen = SCREEN_NEW_PROJECT;
            }
            
            // Exit when clicking Exit
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                CheckCollisionPointRec(GetMousePosition(), items[4].bounds)) {
                CloseWindow();
                return 0;
            }

            int h = GetScreenHeight();
            DrawText("© Blockiro LLC and GearBox contributors, under the MPL 2.0",
                     50, h-45, 15, GRAY);
            DrawText("Not affiliated or endorsed with Valve corporation.",
                     50, h-25, 15, GRAY);

        } else if (screen == SCREEN_NEW_PROJECT) {
            DrawText("New Project", 50,30,30,RAYWHITE);

            int y0 = 100;
            DrawText("Project Name", 50, y0, 20, GRAY);
            UpdateTextInput(&projectNameInput);

            DrawText("Project Destination", 50, y0+70, 20, GRAY);
            UpdateTextInput(&projectPathInput);
            
            if (Button((Rectangle){460, y0+95, 100, 30}, "Browse")) {
                if (fileBrowserInitialized) {
                    UnloadFileBrowser(&fileBrowser);
                }
                fileBrowser = InitFileBrowser(projectPathInput.text);
                fileBrowserInitialized = true;
                screen = SCREEN_FILE_BROWSER;
            }

            int h = GetScreenHeight();
            if (Button((Rectangle){50, h-60, 150, 30}, "Create Project")) {
                if (strlen(projectNameInput.text) > 0) {
                    char fullPath[512];
                    snprintf(fullPath, 512, "%s%s%s", projectPathInput.text, DIR_SEPARATOR, projectNameInput.text);
                    EnsureDirectoryExists(projectPathInput.text);
                    EnsureDirectoryExists(fullPath);
                    TraceLog(LOG_INFO, "Created project at: %s", fullPath);
                    screen = SCREEN_MAIN_MENU;
                }
            }
            if (Button((Rectangle){220, h-60, 150, 30}, "Back")) {
                screen = SCREEN_MAIN_MENU;
            }
        } else if (screen == SCREEN_FILE_BROWSER) {
            int w = GetScreenWidth();
            int h = GetScreenHeight();
            
            // Draw a darker semi-transparent overlay
            DrawRectangle(0, 0, w, h, (Color){0, 0, 0, 200});
            
            // Draw file browser panel
            Rectangle panel = (Rectangle){w/2 - 300, h/2 - 200, 600, 400};
            DrawRectangleRec(panel, (Color){40, 40, 40, 255});
            DrawRectangleLinesEx(panel, 2, RAYWHITE);
            
            // Draw current directory
            DrawText("Select Directory", panel.x + 10, panel.y + 10, 20, RAYWHITE);
            DrawText(fileBrowser.currentDirectory, panel.x + 10, panel.y + 40, 16, LIGHTGRAY);
            
            // Parent directory button
            if (Button((Rectangle){panel.x + panel.width - 100, panel.y + 10, 90, 30}, "Parent Dir")) {
                // Get parent directory
                char *lastSlash = strrchr(fileBrowser.currentDirectory, '/');
                if (lastSlash != NULL) {
                    *lastSlash = '\0';
                    // If we're at root, make sure we have at least "/"
                    if (strlen(fileBrowser.currentDirectory) == 0) {
                        strcpy(fileBrowser.currentDirectory, "/");
                    }
                    UnloadDirectoryFiles(fileBrowser.files);
                    fileBrowser.files = LoadDirectoryFiles(fileBrowser.currentDirectory);
                    fileBrowser.selectedFile = -1;
                }
            }
            
            // Draw file list
            Rectangle listView = (Rectangle){panel.x + 10, panel.y + 70, panel.width - 20, panel.height - 120};
            DrawRectangleRec(listView, (Color){20, 20, 20, 255});
            
            // Calculate visible items and scroll
            int itemHeight = 25;
            int visibleItems = (int)(listView.height / itemHeight);
            int totalItems = fileBrowser.files.count;
            
            // Handle scrolling
            float scrollMax = (totalItems - visibleItems) * itemHeight;
            if (scrollMax < 0) scrollMax = 0;
            if (GetMouseWheelMove() != 0 && CheckCollisionPointRec(GetMousePosition(), listView)) {
                fileBrowser.scrollPosition.y -= GetMouseWheelMove() * 30;
            }
            if (fileBrowser.scrollPosition.y < 0) fileBrowser.scrollPosition.y = 0;
            if (fileBrowser.scrollPosition.y > scrollMax) fileBrowser.scrollPosition.y = scrollMax;
            
            // Draw files
            for (int i = 0; i < fileBrowser.files.count; i++) {
                float itemY = listView.y + i * itemHeight - fileBrowser.scrollPosition.y;
                
                // Skip items outside of view
                if (itemY + itemHeight < listView.y || itemY > listView.y + listView.height) continue;
                
                const char *filename = GetFileName(fileBrowser.files.paths[i]);
                bool isDir = DirectoryExists(fileBrowser.files.paths[i]);
                Color itemColor = isDir ? SKYBLUE : RAYWHITE;
                
                Rectangle itemRect = (Rectangle){listView.x, itemY, listView.width, itemHeight};
                bool isSelected = (i == fileBrowser.selectedFile);
                
                // Draw selection highlight
                if (isSelected) {
                    DrawRectangleRec(itemRect, (Color){60, 60, 60, 255});
                }
                
                // Draw filename
                char displayName[256];
                if (isDir) {
                    snprintf(displayName, 256, "[DIR] %s", filename);
                } else {
                    snprintf(displayName, 256, "%s", filename);
                }
                
                DrawText(displayName, itemRect.x + 5, itemRect.y + 5, 16, itemColor);
                
                // Handle item selection
                if (CheckCollisionPointRec(GetMousePosition(), itemRect)) {
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        fileBrowser.selectedFile = i;
                        
                        // Double click to enter directory
                        static float lastClickTime = 0;
                        float currentTime = GetTime();
                        if (currentTime - lastClickTime < 0.5 && isDir) {
                            // Enter directory
                            char newPath[512];
                            snprintf(newPath, 512, "%s/%s", fileBrowser.currentDirectory, filename);
                            strcpy(fileBrowser.currentDirectory, newPath);
                            UnloadDirectoryFiles(fileBrowser.files);
                            fileBrowser.files = LoadDirectoryFiles(fileBrowser.currentDirectory);
                            fileBrowser.selectedFile = -1;
                            fileBrowser.scrollPosition.y = 0;
                        }
                        lastClickTime = currentTime;
                    }
                }
            }
            
            // Draw scrollbar if needed
            if (totalItems > visibleItems) {
                float scrollbarHeight = listView.height * (visibleItems / (float)totalItems);
                float scrollbarY = listView.y + (fileBrowser.scrollPosition.y / scrollMax) * (listView.height - scrollbarHeight);
                DrawRectangle(listView.x + listView.width - 10, scrollbarY, 8, scrollbarHeight, GRAY);
            }
            
            // OK and Cancel buttons
            if (Button((Rectangle){panel.x + panel.width - 220, panel.y + panel.height - 40, 100, 30}, "Select")) {
                // If directory is selected or we're in directory mode
                if (fileBrowser.selectedFile >= 0 && DirectoryExists(fileBrowser.files.paths[fileBrowser.selectedFile])) {
                    // Use the selected directory
                    strcpy(projectPathInput.text, fileBrowser.files.paths[fileBrowser.selectedFile]);
                } else {
                    // Use current directory
                    strcpy(projectPathInput.text, fileBrowser.currentDirectory);
                }
                
                UnloadFileBrowser(&fileBrowser);
                fileBrowserInitialized = false;
                screen = SCREEN_NEW_PROJECT;
            }
            
            if (Button((Rectangle){panel.x + panel.width - 110, panel.y + panel.height - 40, 100, 30}, "Cancel")) {
                UnloadFileBrowser(&fileBrowser);
                fileBrowserInitialized = false;
                screen = SCREEN_NEW_PROJECT;
            }
        }

        EndDrawing();
    }

    // Clean up resources
    if (fileBrowserInitialized) {
        UnloadFileBrowser(&fileBrowser);
    }
    
    CloseWindow();
    return 0;
}
