#ifndef TIMELINE_H
#define TIMELINE_H

#include "app_state.h"

// Timeline functions
void DrawTimeline(AppState* app);
void UpdateTimeline(AppState* app);
void CreateTrack(AppState* app, const char* name, Color color);
void CreateTimelineElement(AppState* app, const char* name, ElementType type, int trackIndex, float startTime, float duration);

#endif // TIMELINE_H
