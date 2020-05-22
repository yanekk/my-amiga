/* animtools.h */
#ifndef GELTOOLS_H
#define GELTOOLS_H

/*
** These data structures are used by the functions in animtools.c to
** allow for an easier interface to the animation system.
*/

/* Data structure to hold information for a new VSprite.                */
typedef struct newVSprite {
        WORD           *nvs_Image;      /* image data for the vsprite   */
        WORD           *nvs_ColorSet;   /* color array for the vsprite  */
        SHORT           nvs_WordWidth;  /* width in words               */
        SHORT           nvs_LineHeight; /* height in lines              */
        SHORT           nvs_ImageDepth; /* depth of the image           */
        SHORT           nvs_X;          /* initial x position           */
        SHORT           nvs_Y;          /* initial y position           */
        SHORT           nvs_Flags;      /* vsprite flags                */
        USHORT          nvs_HitMask;    /* Hit mask.                    */
        USHORT          nvs_MeMask;     /* Me mask.                     */
        } NEWVSPRITE;

/* Data structure to hold information for a new Bob.                */
typedef struct newBob {
        WORD       *nb_Image;       /* image data for the bob       */
        SHORT       nb_WordWidth;   /* width in words               */
        SHORT       nb_LineHeight;  /* height in lines              */
        SHORT       nb_ImageDepth;  /* depth of the image           */
        SHORT       nb_PlanePick;   /* planes that get image data   */
        SHORT       nb_PlaneOnOff;  /* unused planes to turn on     */
        SHORT       nb_BFlags;      /* bob flags                    */
        SHORT       nb_DBuf;        /* 1=double buf, 0=not          */
        SHORT       nb_RasDepth;    /* depth of the raster          */
        SHORT       nb_X;           /* initial x position           */
        SHORT       nb_Y;           /* initial y position           */
        USHORT      nb_HitMask;     /* Hit mask.                    */
        USHORT      nb_MeMask;      /* Me mask.                     */
        } NEWBOB ;

/* Data structure to hold information for a new animation component.       */
typedef struct newAnimComp {
        WORD  (*nac_Routine)(); /* routine called when Comp is displayed.   */
        SHORT   nac_Xt;         /* initial delta offset position.           */
        SHORT   nac_Yt;         /* initial delta offset position.           */
        SHORT   nac_Time;       /* Initial Timer value.                     */
        SHORT   nac_CFlags;     /* Flags for the Component.                 */
        } NEWANIMCOMP;

/* Data structure to hold information for a new animation sequence.         */
typedef struct newAnimSeq {
        struct AnimOb  *nas_HeadOb; /* common Head of Object.               */
        WORD   *nas_Images;         /* array of Comp image data             */
        SHORT  *nas_Xt;             /* arrays of initial offsets.           */
        SHORT  *nas_Yt;             /* arrays of initial offsets.           */
        SHORT  *nas_Times;          /* array of Initial Timer value.        */
        WORD (**nas_Routines)();    /* Array of fns called when comp drawn  */
        SHORT   nas_CFlags;         /* Flags for the Component.             */
        SHORT   nas_Count;          /* Num Comps in seq (= arrays size)     */
        SHORT   nas_SingleImage;    /* one (or count) images.               */
        } NEWANIMSEQ;

#define INTUITIONNAME "intuition.library" /* intuitionbase.h does not define a library name. */

#include "animtools_proto.h"              /* Include Prototyping. */
#endif


/* animtools_proto.h */
