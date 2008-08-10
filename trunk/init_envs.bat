@echo off
rem run this before running builds

set SDKFOLDER=C:\Symbian\9.1\S60_3rd
set CWFolder=C:\Programme\Carbide
set PATH=%PATH%;%CWFolder%\plugins\com.nokia.carbide.cpp.support_1.0.0\Symbian_Tools\Command_Line_Tools
set MWCSym2Includes=%CWFolder%\plugins\com.nokia.carbide.cpp.support_1.0.0\Symbian_Support\MSL;%CWFolder%\plugins\com.nokia.carbide.cpp.support_1.0.0\Symbian_Support\MSL\MSL_C\MSL_Common\Include;%CWFolder%\plugins\com.nokia.carbide.cpp.support_1.0.0\Symbian_Support\MSL\MSL_C\MSL_Win32\Include;%CWFolder%\plugins\com.nokia.carbide.cpp.support_1.0.0\Symbian_Support\MSL\MSL_C\MSL_X86;%CWFolder%\plugins\com.nokia.carbide.cpp.support_1.0.0\Symbian_Support\MSL\MSL_C++\MSL_Common\Include;%CWFolder%\plugins\com.nokia.carbide.cpp.support_1.0.0\Symbian_Support\MSL\MSL_Extras\MSL_Common\Include;%CWFolder%\plugins\com.nokia.carbide.cpp.support_1.0.0\Symbian_Support\MSL\MSL_Extras\MSL_Win32\Include;%CWFolder%\plugins\com.nokia.carbide.cpp.support_1.0.0\Symbian_Support\Win32-x86 Support\Headers\Win32 SDK
set MWSym2Libraries=%CWFolder%\plugins\com.nokia.carbide.cpp.support_1.0.0\Symbian_Support\Win32-x86 Support\Libraries\Win32 SDK;%CWFolder%\plugins\com.nokia.carbide.cpp.support_1.0.0\Symbian_Support\Runtime\Runtime_x86\Runtime_Win32\Libs
set MWSym2LibraryFiles=MSL_ALL_MSE_Symbian_D.lib;gdi32.lib;user32.lib;kernel32.lib;
