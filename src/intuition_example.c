#include <stdio.h>

#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/exec_protos.h>

#include "intuition_example.h"
#include "layout.h"

void RunIntuitionExample() {
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
                        //HaveFunWithGraphics();
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
}