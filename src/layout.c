#include "layout.h"
#include <clib/gadtools_protos.h>
#include <clib/intuition_protos.h>

static struct TextAttr Topaz80 = { "topaz.font", 8, 0, 0, };

struct Gadget* CreateLayout(struct Layout* layout) 
{
    struct Screen* screen = LockPubScreen(NULL);
    struct VisualInfo* visualInfo = GetVisualInfo(screen, TAG_END);

    layout->newGadget.ng_TextAttr   = &Topaz80;
    layout->newGadget.ng_VisualInfo = visualInfo;

    layout->newGadget.ng_LeftEdge = PADDING;
    layout->newGadget.ng_TopEdge = PADDING * 2;
    layout->newGadget.ng_Width      = GADGET_WIDTH;
    layout->newGadget.ng_Height     = GADGET_HEIGHT;
    return CreateContext(&layout->glist);
}

void AdjustNewGadget(struct Layout* layout, char* label, GAD_TYPE gadgetId, ULONG flags) {
    layout->newGadget.ng_GadgetText = (UBYTE*)label;
    layout->newGadget.ng_Flags = flags;
    layout->newGadget.ng_GadgetID   = gadgetId;
}

void FreeLayout(struct Layout* layout) 
{
    FreeGadgets(layout->glist);
    FreeVisualInfo(layout->newGadget.ng_VisualInfo);
}

void AddNextGadgetToLayout(struct Layout* layout) 
{
    if(layout->newGadget.ng_LeftEdge == PADDING) {
        layout->newGadget.ng_LeftEdge += GADGET_WIDTH + PADDING;
    } else {
        layout->newGadget.ng_LeftEdge = PADDING;
        layout->newGadget.ng_TopEdge += layout->newGadget.ng_Height + PADDING;
    }
}