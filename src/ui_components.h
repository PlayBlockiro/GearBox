#ifndef UI_COMPONENTS_H
#define UI_COMPONENTS_H

#include "app_state.h"

// UI Component declarations
bool Button(Rectangle bounds, const char* text, bool isActive);
bool IconButton(Rectangle bounds, const char* icon, const char* tooltip, bool isActive);
void Slider(Rectangle bounds, float* value, float min, float max, const char* label);
void UpdateTextInput(TextInput* input);
bool Dropdown(Rectangle bounds, const char* label, const char** options, int optionCount, int* selectedIndex);
void DrawPropertyEditor(AppState* app, ComponentProperty* property, Rectangle bounds);
void DrawTabBar(Rectangle bounds, const char** tabNames, int tabCount, int* selectedTab);
void DrawWaveform(Rectangle bounds, Color color);
void DrawContextMenu(AppState* app);

#endif // UI_COMPONENTS_H
