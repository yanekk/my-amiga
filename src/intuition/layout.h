#include <proto/gadtools.h>
struct Layout 
{
    struct NewGadget newGadget;
    struct Gadget* glist;
    struct Gadget* lastGadget;
};

typedef enum GAD_TYPE 
{
    GAD_BUTTON,
    GAD_SLIDER,
    GAD_TEXTBOX,
    GAD_CHECKBOX,
    GAD_CYCLE,
    GAD_SCROLLER,
    GAD_LIST,
    GAD_PALETTE,
    GAD_LABEL,
    GAD_FILE_REQUESTER,
    GAD_FONT_REQUESTER,
    GAD_VIEWPORT,
} GAD_TYPE;

#define GADGET_COUNT 9
#define GADGET_HEIGHT 12
#define PADDING 13
#define GADGET_WIDTH 110
#define WINDOW_WIDTH 275
#define WINDOW_HEIGHT 200

struct Layout* CreateLayout();
void AddGadgetToLayout(struct Layout* layout, UBYTE kind, GAD_TYPE gadgetId, char* label, ULONG flags, Tag tag1, ...);
void FreeLayout(struct Layout* layout);
void OpenFileRequester();
void OpenFontRequester();