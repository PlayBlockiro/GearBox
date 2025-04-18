#ifndef APP_STATE_H
#define APP_STATE_H

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// Constants
#define MAX_INPUT_LEN 256
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define MAX_TIMELINE_TRACKS 16
#define MAX_ASSETS 128
#define MAX_COMPONENTS 16
#define MAX_RECENT_PROJECTS 10
#define MAX_PATTERNS 64
#define MAX_NOTES 256

// UI Colors
#define COLOR_BG             (Color){30, 30, 30, 255}      // Dark background
#define COLOR_PANEL_BG       (Color){40, 40, 40, 255}      // Panel background
#define COLOR_PANEL_HEADER   (Color){60, 60, 60, 255}      // Panel header
#define COLOR_ACCENT         (Color){0, 120, 210, 255}     // Unity-like blue
#define COLOR_ACCENT_ALT     (Color){255, 128, 0, 255}     // FL Studio orange
#define COLOR_TEXT           (Color){230, 230, 230, 255}   // Light text
#define COLOR_TEXT_DIM       (Color){180, 180, 180, 255}   // Dimmed text
#define COLOR_GRID           (Color){50, 50, 50, 128}      // Grid lines
#define COLOR_TIMELINE_BG    (Color){25, 25, 25, 255}      // Timeline background
#define COLOR_SELECTION      (Color){100, 150, 230, 100}   // Selection highlight

// Enums
typedef enum {
    SCREEN_SPLASH,
    SCREEN_START,
    SCREEN_EDITOR
} AppScreen;

typedef enum {
    TOOL_SELECT,
    TOOL_MOVE,
    TOOL_PEN,
    TOOL_ERASER,
    TOOL_SCALE
} EditorTool;

typedef enum {
    PANEL_ASSETS,
    PANEL_INSPECTOR,
    PANEL_HIERARCHY,
    PANEL_PATTERN_EDITOR,
    PANEL_MIXER,
    PANEL_COUNT
} PanelType;

typedef enum {
    ELEMENT_TYPE_AUDIO,
    ELEMENT_TYPE_OBJECT,
    ELEMENT_TYPE_EFFECT,
    ELEMENT_TYPE_EVENT
} ElementType;

// Structures
typedef struct {
    char name[64];
    Color color;
    bool muted;
    bool solo;
    float volume;
    float pan;
    Rectangle bounds;
    bool selected;
} Track;

typedef struct {
    char name[64];
    ElementType type;
    int id;
    Rectangle bounds;
    Color color;
    bool selected;
    int trackIndex;
    float startTime;
    float duration;
} TimelineElement;

typedef struct {
    char name[64];
    char type[32];
    int id;
    Texture2D thumbnail;
} Asset;

typedef struct {
    char name[64];
    char path[256];
    time_t lastModified;
} RecentProject;

typedef struct {
    char name[64];
    int id;
    bool folded;
    int notes[MAX_NOTES];
    int noteCount;
    Color color;
} Pattern;

typedef struct {
    float zoom;
    float scrollX;
    float scrollY;
    Rectangle viewBounds;
    int selectedTrack;
    Vector2 cursor;
    Vector2 origin;
    float bpm;
    float timeSignatureNumerator;
    float timeSignatureDenominator;
    float snapDivision;
    bool showGrid;
} TimelineState;

typedef struct {
    char name[64];
    int id;
    Rectangle bounds;
    bool visible;
    bool collapsed;
    Vector2 scrollPosition;
    float minWidth;
    float minHeight;
} Panel;

typedef struct {
    char name[64];
    float value;
    float min;
    float max;
    bool isActive;
    char dataType[32];
} ComponentProperty;

typedef struct {
    char name[64];
    ComponentProperty properties[MAX_COMPONENTS];
    int propertyCount;
    bool folded;
} Component;

typedef struct {
    char name[64];
    Component components[MAX_COMPONENTS];
    int componentCount;
    bool selected;
    bool folded;
} GameObject;

typedef struct {
    Rectangle bounds;
    char text[MAX_INPUT_LEN];
    int cursorPosition;
    bool editMode;
    bool active;
} TextInput;

// File browser structure
typedef struct {
    char currentDirectory[512];
    FilePathList files;
    int selectedFile;
    Vector2 scrollPosition;
} FileBrowser;

// Global state
typedef struct {
    AppScreen currentScreen;
    EditorTool currentTool;
    
    // Project data
    char projectName[64];
    char projectPath[256];
    bool projectModified;
    
    // UI state
    Panel panels[PANEL_COUNT];
    bool isDraggingPanel;
    int draggingPanelIndex;
    Vector2 dragOffset;
    bool showMixer;
    bool showPatternEditor;
    int selectedAssetIndex;
    
    // Timeline data
    TimelineState timeline;
    Track tracks[MAX_TIMELINE_TRACKS];
    int trackCount;
    TimelineElement elements[MAX_ASSETS];
    int elementCount;
    int selectedElementIndex;
    
    // Asset data
    Asset assets[MAX_ASSETS];
    int assetCount;
    
    // Recent projects
    RecentProject recentProjects[MAX_RECENT_PROJECTS];
    int recentProjectCount;
    
    // Pattern data
    Pattern patterns[MAX_PATTERNS];
    int patternCount;
    int selectedPatternIndex;
    
    // Game objects
    GameObject gameObjects[MAX_ASSETS];
    int gameObjectCount;
    int selectedGameObjectIndex;
    
    // Current frame time
    float deltaTime;
    
    // Transport controls
    bool isPlaying;
    bool isRecording;
    float playheadPosition;
    
    // Other UI state
    bool showContextMenu;
    Vector2 contextMenuPosition;
    int contextMenuSelectedItem;
    
    // Mouse state
    Vector2 mousePosition;
    Vector2 prevMousePosition;
    bool isMouseDragging;
    
    // View state
    float sceneZoom;
    Vector2 sceneScrollPosition;
    
    // Splash screen
    float splashTimer;
} AppState;

// Core initialization function
void InitApp(AppState *app);

#endif // APP_STATE_H
