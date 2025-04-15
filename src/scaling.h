#ifndef SCALING_H
#define SCALING_H

#include "raylib.h"

typedef struct {
    float baseWidth;
    float baseHeight;
    float scaleX;
    float scaleY;
    float scale;
} UIScaler;

static UIScaler uiScaler = { 1280.0f, 720.0f, 1.0f, 1.0f, 1.0f };

static void UpdateUIScale(int screenWidth, int screenHeight) {
    uiScaler.scaleX = screenWidth / uiScaler.baseWidth;
    uiScaler.scaleY = screenHeight / uiScaler.baseHeight;
    uiScaler.scale = fminf(uiScaler.scaleX, uiScaler.scaleY);
}

static float ScaleF(float value) {
    return value * uiScaler.scale;
}

static Vector2 ScaleV(Vector2 value) {
    return (Vector2){ value.x * uiScaler.scale, value.y * uiScaler.scale };
}

static Rectangle ScaleR(Rectangle value) {
    return (Rectangle){
        value.x * uiScaler.scale,
        value.y * uiScaler.scale,
        value.width * uiScaler.scale,
        value.height * uiScaler.scale
    };
}

#endif // SCALING_H
