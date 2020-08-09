#pragma once
#include <proto/graphics.h>

#define DEPTH 3
#define WIDTH 320
#define HEIGHT 200

struct ViewInfo {
    struct View view;

    struct ViewPort viewPort;
    struct BitMap bitMap;

    struct ColorMap* colorMap;
    struct ViewExtra* viewExtra;
    struct ViewPortExtra* viewPortExtra;  
    struct RastPort rastPort;

    int color_count;
    UWORD colortable[];
};

struct ViewInfo * CreateView();
void FreeView(struct ViewInfo* viewInfo);
void RefreshScreen(struct ViewInfo* viewInfo);