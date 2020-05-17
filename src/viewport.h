#pragma once
#define DEPTH 2
#define WIDTH 640
#define HEIGHT 400

struct ViewInfo {
    struct View view;

    struct ViewPort viewPort;
    struct BitMap bitMap;

    struct ColorMap* colorMap;
    struct ViewExtra* viewExtra;
    struct ViewPortExtra* viewPortExtra;  
};

void CreateView(struct ViewInfo* viewInfo);
void FreeView(struct ViewInfo* viewInfo);