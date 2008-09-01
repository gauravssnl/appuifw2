/*
* ============================================================================
* Name : AknEditStateIndicator.h
*
*
* Description:
*
*
* Version:
*
* Copyright © 2002 Nokia Corporation.
* This material, including documentation and any related
* computer programs, is protected by copyright controlled by
* Nokia Corporation. All rights are reserved. Copying,
* including reproducing, storing, adapting or translating, any
* or all of this material requires the prior written consent of
* Nokia Corporation. This material also contains confidential
* information which may not be disclosed to others without the
* prior written consent of Nokia Corporation.
*/

#ifndef AKNEDITSTATEINDICATOR_H
#define AKNEDITSTATEINDICATOR_H

#include <coemop.h>

class CAknIndicatorContainer;

enum TAknEditingState
{
EStateNone,
ET9Upper,
ET9Lower,
ET9Shifted,
ENumeric,
EMultitapUpper,
EMultitapLower,
EMultitapShifted,

};

class MAknEditingStateIndicator
{
public:
DECLARE_TYPE_ID(0x101F4108);

virtual void SetState(TAknEditingState aState) = 0;
virtual CAknIndicatorContainer* IndicatorContainer() = 0;
};


#endif

