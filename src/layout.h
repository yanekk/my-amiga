#include <clib/gadtools_protos.h>
struct Layout 
{
    struct NewGadget newGadget;
    struct Gadget* glist;
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
    GAD_LABEL
} GAD_TYPE;

#define GADGET_COUNT 9
#define GADGET_HEIGHT 12
#define PADDING 13
#define GADGET_WIDTH 100
#define WINDOW_WIDTH 275
#define WINDOW_HEIGHT 200

struct Gadget* CreateLayout(struct Layout* layout);
void AdjustNewGadget(struct Layout* layout, char* label, GAD_TYPE gadgetId, ULONG flags) ;
void FreeLayout(struct Layout* layout);
void AddNextGadgetToLayout(struct Layout* layout);