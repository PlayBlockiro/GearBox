#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    const char *text;
    Vector2 position;
    Vector2 size;
    bool hovered;
    bool clicked;
} Button;

typedef struct {
    Vector2 position;
    float radius;
    Texture2D icon;
    bool hovered;
    bool clicked;
} IconButton;

typedef struct {
    Rectangle bounds;
    float value;
    bool dragging;
} Slider;

bool ButtonLogic(Button *btn) {
    Vector2 mouse = GetMousePosition();
    btn->hovered = CheckCollisionPointRec(mouse, (Rectangle){btn->position.x, btn->position.y, btn->size.x, btn->size.y});
    btn->clicked = btn->hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    return btn->clicked;
}

void DrawButton(Button *btn) {
    Color color = btn->hovered ? DARKGRAY : GRAY;
    DrawRectangleV(btn->position, btn->size, color);
    int textWidth = MeasureText(btn->text, 20);
    Vector2 textPos = {
        btn->position.x + btn->size.x / 2 - textWidth / 2,
        btn->position.y + btn->size.y / 2 - 10
    };
    DrawText(btn->text, textPos.x, textPos.y, 20, WHITE);
}

bool IconButtonLogic(IconButton *btn) {
    Vector2 mouse = GetMousePosition();
    float dist = Vector2Distance(mouse, btn->position);
    btn->hovered = dist <= btn->radius;
    btn->clicked = btn->hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    return btn->clicked;
}

void DrawIconButton(IconButton *btn) {
    Color color = btn->hovered ? LIGHTGRAY : GRAY;
    DrawCircleV(btn->position, btn->radius, color);
    DrawTexturePro(btn->icon,
        (Rectangle){0, 0, (float)btn->icon.width, (float)btn->icon.height},
        (Rectangle){btn->position.x - btn->radius / 1.5f, btn->position.y - btn->radius / 1.5f, btn->radius * 1.5f, btn->radius * 1.5f},
        (Vector2){0, 0}, 0.0f, WHITE);
}

float SliderLogic(Slider *sld) {
    Vector2 mouse = GetMousePosition();
    Rectangle knob = {
        sld->bounds.x + sld->value * sld->bounds.width - 5,
        sld->bounds.y - 5,
        10,
        sld->bounds.height + 10
    };

    if (CheckCollisionPointRec(mouse, knob) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        sld->dragging = true;
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        sld->dragging = false;
    }
    if (sld->dragging) {
        float newValue = (mouse.x - sld->bounds.x) / sld->bounds.width;
        sld->value = Clamp(newValue, 0.0f, 1.0f);
    }
    return sld->value;
}

void DrawSlider(Slider *sld) {
    DrawRectangleRec(sld->bounds, DARKGRAY);
    float knobX = sld->bounds.x + sld->value * sld->bounds.width;
    DrawRectangle(knobX - 5, sld->bounds.y - 5, 10, sld->bounds.height + 10, RAYWHITE);
}

