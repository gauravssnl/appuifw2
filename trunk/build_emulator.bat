@echo off

if not exist %SDKFOLDER% goto:nosdk goto nosdk

pushd .
cd symbian\src\
bldmake bldfiles
call abld reallyclean winscw udeb
call abld build winscw udeb
popd

copy python\resource\appuifw2.py %SDKFOLDER%\Epoc32\winscw\c\resource
copy %SDKFOLDER%\Epoc32\release\winscw\udeb\_appuifw2.pyd %SDKFOLDER%\Epoc32\winscw\c\sys\bin\_appuifw2.pyd

goto exit


:nosdk
echo run init_envs.bat first!

:exit
