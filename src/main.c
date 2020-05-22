#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/graphics_protos.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>

#include <proto/exec.h>
#include <clib/exec_protos.h>
#include "layout.h"
#include "screen.h"
#include "drawing.h"
#include "animtools.h"

#define BLACK 0x000
#define RED 0xf00
#define GREEN 0x0f0
#define BLUE 0x00f

#define COLOR_COUNT 32
#define AREA_SIZE 200

#define GADGETID(x) (((struct Gadget *)(x->IAddress))->GadgetID)

struct GfxBase *GfxBase = NULL;
extern struct Custom custom;

static void DrawBoxes() 
{
    GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 0);

    struct View* oldView = GfxBase->ActiView;

    struct ViewInfo* viewInfo = AllocVec(sizeof(struct ViewInfo) + COLOR_COUNT * sizeof(LONG), MEMF_CLEAR);

    viewInfo->color_count = COLOR_COUNT;
    viewInfo->colortable[0] = BLACK;
    viewInfo->colortable[1] = RED;
    viewInfo->colortable[2] = GREEN;
    viewInfo->colortable[3] = BLUE;


    CreateView(viewInfo);

    /*static struct Box boxes[3] = {
        {.x = 25, .y = 25, .width = 75, .height = 75, .color = 1 },
        {.x = 50, .y = 50, .width = 75, .height = 75, .color = 2 },
        {.x = 75, .y = 75, .width = 75, .height = 75, .color = 3 },
    };
    
    for (int box = 0; box <= 2; box++) 
        DrawBox(viewInfo, boxes[box]);*/

    ULONG rassize = RASSIZE(WIDTH, HEIGHT);
    UBYTE *tmpbuf = AllocVec (rassize, MEMF_CHIP|MEMF_CLEAR);
    
    struct RastPort* rp = &viewInfo->rastPort;

    struct TmpRas tmpRas = { 0 };
    InitTmpRas(&tmpRas, tmpbuf, rassize);
    rp->TmpRas = &tmpRas;
    
    struct AreaInfo areaInfo = { 0 };
    WORD* areaBuffer = AllocVec(AREA_SIZE * sizeof(WORD), MEMF_CLEAR);
    InitArea(&areaInfo, areaBuffer, (AREA_SIZE * 2) / 5);
    rp->AreaInfo = &areaInfo;

    setupGelSys(rp, (BYTE)0xfc);
    
    UWORD myImage[] = {
        0xFFFF, 0xFFFF,
        0x3C3C, 0x300C,
        0x0c30, 0x0FF0,
        0x0240, 0x03C0,
        0x0180, 0x0180,
        0, 0
    };
    
    UWORD* myImageChip = AllocVec(sizeof(myImage), MEMF_CHIP);
    CopyMem(myImage, myImageChip, sizeof(myImage));

    WORD mySpriteColours[]= { 0x0000, 0x00f0, 0x0f00 };

    struct VSprite myVSprite =
    {
    NULL, NULL, NULL, NULL, 0, 0, VSPRITE, 0, 0, 5, 1, 2, 0, 0,
    (WORD*)myImageChip, 0, 0, (WORD*)mySpriteColours, NULL, 0x3, 0, 0
    };
    myVSprite.Width = 1;
    myVSprite.Height = 5;
    myVSprite.Depth = 2;
    myVSprite.X = 30;
    myVSprite.Y = 30;
    AddVSprite(&myVSprite, rp);
    SortGList(rp);
    DrawGList(rp, &viewInfo->viewPort);
    MrgCop(&viewInfo->view);
    LoadView(&viewInfo->view);
    WaitTOF();

    //WritePixel(rp, 150, 150);
    /*USHORT areaPattern[3][8] =
    {
        {
            0x0000, 0x0000,
            0xffff, 0xffff,
            0x0000, 0x0000,
            0xffff, 0xffff
        },
        {
            0x0000, 0x0000,
            0x0000, 0x0000,
            0xffff, 0xffff,
            0xffff, 0xffff
        },
        {
            0xff00, 0xff00,
            0xff00, 0xff00,
            0xff00, 0xff00,
            0xff00, 0xff00
        }
    };
    
    SetAfPt(rp, (UWORD*)&areaPattern, -3);
    
    SetAPen(rp, -1);
    SetBPen(rp, 0);
    SetDrMd(rp, JAM2);*/
    /*SetAPen(rp, 2);
    AreaMove(rp, 0, 0);
    AreaDraw(rp, 40, 0);
    AreaDraw(rp, 40, 40);
    AreaDraw(rp, 0, 40);
    AreaEnd(rp);*/

    /*const UBYTE areaPattern[3][8] =
    {
        {
            0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff
        },
        {
            0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff
        },
        {
            0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff
        }
    };
    
    BltPattern(rp, (UBYTE*)&areaPattern, 0, 0, 40, 40, 8);*/

    //BltTemplate()

    for(int i = 0; i < 100; i++) {
        myVSprite.X += 2;
        SortGList(rp);
        DrawGList(rp, &viewInfo->viewPort);
        MrgCop(&viewInfo->view);
        WaitTOF();
    }

   /* SetAPen(rp, 2);
    SetDrMd(rp, JAM2);
    DrawEllipse(rp, 100, 100, 20, 50);

    SetAPen(rp, 3);
    DrawCircle(rp, 150, 150, 20);*/

    //SetDrPt(&viewInfo->rastPort, 0xAAAA);
    //Move(&viewInfo->rastPort, 100, 100);
    //Draw(&viewInfo->rastPort, 120, 130);

    /*SHORT linearray[] = {
        10, 10,
        60, 15,
        15, 15,
        30, 10,
        10, 10
    };
    */
    //PolyDraw(&viewInfo->rastPort, 4, linearray);
    
    /*SetAfPt(rp, NULL, 0);
    Flood(rp, 1, 100, 100);
    //SetRast(&viewInfo->rastPort, 0);
    FreeVec(areaBuffer);
    FreeVec(tmpbuf);*/
    Delay(1 * TICKS_PER_SECOND);
    
    RemVSprite(&myVSprite);
    //freeVSprite(&myVSprite);

    cleanupGelSys(rp->GelsInfo, rp);
    FreeView(viewInfo);

    LoadView(oldView);

    CloseLibrary((struct Library *)GfxBase);
}

int main(void) 
{
    DrawBoxes();

    return EXIT_SUCCESS;

    struct Layout* layout = CreateLayout();
    AddGadgetToLayout(layout, BUTTON_KIND, GAD_BUTTON, "Button", PLACETEXT_IN, TAG_END);
    AddGadgetToLayout(layout, SLIDER_KIND, GAD_SLIDER, "Button", PLACETEXT_IN, TAG_END);
    AddGadgetToLayout(layout, STRING_KIND, GAD_TEXTBOX, "Text box", PLACETEXT_ABOVE, TAG_END);
    AddGadgetToLayout(layout, CHECKBOX_KIND, GAD_CHECKBOX, "Checkbox", PLACETEXT_RIGHT, TAG_END);

    char *labels[4] = {"A", "B", "C", NULL};
    AddGadgetToLayout(layout, CYCLE_KIND, GAD_CYCLE, "Cycle", PLACETEXT_ABOVE, 
        GTCY_Labels, labels, 
        GTCY_Active, 1, 
        TAG_END);
        
    AddGadgetToLayout(layout, SCROLLER_KIND, GAD_SCROLLER, "Scroller", PLACETEXT_ABOVE, 
        GTSC_Top, 0,
        GTSC_Total, 100,
        GTSC_Visible, 10,
        TAG_END);

    struct List list;
    NewList(&list);

    struct Node node_1 = { .ln_Name = "One" };
    AddTail(&list, &node_1);

    struct Node node_2 = { .ln_Name = "Two" };
    AddTail(&list, &node_2);
    
    struct Node node_3 = { .ln_Name = "Three" };
    AddTail(&list, &node_3); 

    layout->newGadget.ng_Height = GADGET_HEIGHT * 3;

    AddGadgetToLayout(layout, LISTVIEW_KIND, GAD_LIST, "Scroller", PLACETEXT_ABOVE, 
        GTLV_Labels, &list,
        GTLV_ShowSelected, 0,
        TAG_END);

    AddGadgetToLayout(layout, PALETTE_KIND, GAD_PALETTE, "Palette", PLACETEXT_ABOVE,  
        GTPA_Depth, 2,
        TAG_END);
        
    layout->newGadget.ng_Height = GADGET_HEIGHT;
    //AddGadgetToLayout(layout, TEXT_KIND, GAD_LABEL, "Label", PLACETEXT_ABOVE, TAG_END);
    AddGadgetToLayout(layout, BUTTON_KIND, GAD_FILE_REQUESTER, "File Request", PLACETEXT_IN, TAG_END);
    AddGadgetToLayout(layout, BUTTON_KIND, GAD_FONT_REQUESTER, "Font Request", PLACETEXT_IN, TAG_END);
    AddGadgetToLayout(layout, BUTTON_KIND, GAD_VIEWPORT, "Open ViewPort", PLACETEXT_IN, TAG_END);

    struct Window *window = OpenWindowTags(NULL,
        WA_Width, WINDOW_WIDTH,
        WA_Height, WINDOW_HEIGHT,
        WA_Gadgets, layout->glist,
        WA_CloseGadget, TRUE,
        WA_SizeGadget, TRUE,
        WA_DepthGadget, TRUE,
        WA_DragBar, TRUE,
        WA_IDCMP, IDCMP_CLOSEWINDOW | BUTTONIDCMP | SLIDERIDCMP | TEXTIDCMP | CHECKBOXIDCMP | CYCLEIDCMP | SCROLLERIDCMP | LISTVIEWIDCMP,
        WA_Title, "Pozdrowienia z Gdanska",
        WA_Activate, TRUE,
        TAG_DONE);
    GT_RefreshWindow(window, NULL);

    struct IntuiMessage *imsg;
    BOOL is_running = TRUE;
    while(is_running) {
        while((imsg = GT_GetIMsg(window->UserPort))) {
            if(imsg->Class == IDCMP_CLOSEWINDOW)
                is_running = FALSE;

            if(imsg->Class == IDCMP_GADGETUP) {
                switch(GADGETID(imsg)) {
                    case GAD_LIST:
                        printf("List: %d\n", imsg->Code);
                        break;
                    case GAD_BUTTON:
                        printf("Button\n");
                        break;
                    case GAD_CHECKBOX:
                        printf("Checkbox: %d\n", imsg->Code);
                        break;
                    case GAD_CYCLE:
                        printf("Cycle: %d\n", imsg->Code);
                        break;
                    case GAD_SLIDER:
                        printf("Slider: %d\n", imsg->Code);
                        break;
                    case GAD_PALETTE:
                        printf("Palette: %d\n", imsg->Code);
                        break;
                    case GAD_FILE_REQUESTER:
                        OpenFileRequester();
                        break;
                    case GAD_FONT_REQUESTER:
                        OpenFontRequester();
                        break;
                    case GAD_VIEWPORT:
                        //openViewPort();
                        break;
                }
            }
            if(imsg->Class == IDCMP_MOUSEMOVE) {
                switch(GADGETID(imsg)) {
                    case GAD_SLIDER:
                        printf("Slider: %d\n", imsg->Code);
                        break;
                    case GAD_SCROLLER:
                        printf("Scroller: %d\n", imsg->Code);
                        break;
                }
            }
            GT_ReplyIMsg(imsg); 
        }
    }

    FreeLayout(layout);
    CloseWindow(window);
    return EXIT_SUCCESS;
}
