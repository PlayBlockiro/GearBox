#include "timeline.h"
#include "ui_components.h"

void DrawTimeline(AppState* app) {
    if (!app) return;
    
    // Timeline is positioned at the bottom of the screen, above the Assets panel
    Rectangle timelineBounds = {
        0, 
        app->panels[PANEL_ASSETS].bounds.y - 180, 
        GetScreenWidth(), 
        180
    };
    
    // Draw timeline background
    DrawRectangleRec(timelineBounds, COLOR_TIMELINE_BG);
    
    // Draw timeline header
    DrawRectangle(0, timelineBounds.y, GetScreenWidth(), 25, COLOR_PANEL_HEADER);
    DrawText("Timeline", 10, timelineBounds.y + 5, 18, COLOR_TEXT);
    
    // Timeline controls (Patterns button)
    if (Button((Rectangle){GetScreenWidth() - 100, timelineBounds.y + 3, 90, 20}, 
              "Patterns", app->showPatternEditor)) {
        app->showPatternEditor = !app->showPatternEditor;
        app->panels[PANEL_PATTERN_EDITOR].visible = app->showPatternEditor;
    }
    
    // Mixer button
    if (Button((Rectangle){GetScreenWidth() - 200, timelineBounds.y + 3, 90, 20}, 
              "Mixer", app->showMixer)) {
        app->showMixer = !app->showMixer;
        app->panels[PANEL_MIXER].visible = app->showMixer;
    }
    
    // Draw Timeline Track Area
    int trackHeight = 130; // Space for tracks
    int trackY = timelineBounds.y + 30; // Space below the header

    // Draw track background and timeline guide
    for (int i = 0; i < app->numTracks; i++) {
        DrawRectangle(0, trackY + i * (trackHeight + 5), GetScreenWidth(), trackHeight, COLOR_TRACK_BG);
        DrawLine(0, trackY + i * (trackHeight + 5), GetScreenWidth(), trackY + i * (trackHeight + 5), COLOR_TRACK_BORDER);
    }

    // Draw timeline control markers
    float timelinePosition = app->timelinePosition;
    DrawLine(timelinePosition, timelineBounds.y + 25, timelinePosition, timelineBounds.y + 25 + trackHeight, COLOR_TIMELINE_CURSOR);
    
    // Optionally, you can draw a playback marker here if you need to visualize the timeline position
    
    // Draw track labels (Optional)
    for (int i = 0; i < app->numTracks; i++) {
        DrawText(TextFormat("Track %d", i + 1), 10, trackY + i * (trackHeight + 5) + 5, 14, COLOR_TEXT);
    }
}

