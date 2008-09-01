/*
 ============================================================================
 Name		: appuifw2DllMain.cpp 
 Author	  : Arkadiusz Wahlig
 Copyright   : Copyright Arkadiusz Wahlig.
 Description : appuifw2DllMain.cpp - main DLL source
 ============================================================================
 */

//  Include Files  

#include <e32std.h>		 // GLDEF_C

//  Global Functions

//  Exported Functions

#ifndef EKA2 // for EKA1 only
EXPORT_C TInt E32Dll(TDllReason /*aReason*/)
// Called when the DLL is loaded and unloaded. Note: have to define
// epoccalldllentrypoints in MMP file to get this called in THUMB.

	{
	return KErrNone;
	}
#endif

