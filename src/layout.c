#include "layout.h"
#include <clib/gadtools_protos.h>
#include <clib/intuition_protos.h>
#include <clib/exec_protos.h>
#include <stdarg.h>
#include <stdio.h>
#define MAX_TAGS 32

static struct TextAttr Topaz80 = { "topaz.font", 8, 0, 0, };

struct Layout* CreateLayout() 
{
    struct Layout* layout = AllocVec(sizeof(struct Layout), MEMF_CLEAR);
    struct Screen* screen = LockPubScreen(NULL);
    struct VisualInfo* visualInfo = GetVisualInfo(screen, TAG_END);

    layout->newGadget.ng_TextAttr   = &Topaz80;
    layout->newGadget.ng_VisualInfo = visualInfo;

    layout->newGadget.ng_LeftEdge = PADDING;
    layout->newGadget.ng_TopEdge = PADDING * 2;
    layout->newGadget.ng_Width      = GADGET_WIDTH;
    layout->newGadget.ng_Height     = GADGET_HEIGHT;
    layout->lastGadget = CreateContext(&layout->glist);
    return layout;
}

void AddGadgetToLayout(struct Layout* layout, UBYTE kind, GAD_TYPE gadgetId, char* label, ULONG flags, Tag tag1, ...) 
{
    layout->newGadget.ng_GadgetText = (UBYTE*)label;
    layout->newGadget.ng_Flags = flags;
    layout->newGadget.ng_GadgetID = gadgetId;
    struct TagItem *taglist = AllocVec(MAX_TAGS * sizeof(struct TagItem), MEMF_CLEAR);

    size_t tagCount = 0;
    if(tag1 != 0) {
        va_list ap;
        va_start(ap, tag1);
        Tag tagName = tag1;
        ULONG tagValue = va_arg(ap, Tag);
        while(tagName) {
            taglist[tagCount++] = (struct TagItem) {.ti_Tag = tagName, .ti_Data = tagValue };
            tagName = va_arg(ap, Tag);
            if(tagName == 0)
                break;
            tagValue = va_arg(ap, Tag);
        }
        va_end(ap);
    }

    layout->lastGadget = CreateGadgetA(kind, layout->lastGadget, &layout->newGadget, taglist);
    if(layout->newGadget.ng_LeftEdge == PADDING) {
        layout->newGadget.ng_LeftEdge += GADGET_WIDTH + PADDING;
    } else {
        layout->newGadget.ng_LeftEdge = PADDING;
        layout->newGadget.ng_TopEdge += layout->newGadget.ng_Height + PADDING;
    }
    FreeVec(taglist);
}

void FreeLayout(struct Layout* layout) 
{
    FreeGadgets(layout->glist);
    FreeVisualInfo(layout->newGadget.ng_VisualInfo);
    FreeVec(layout);
}