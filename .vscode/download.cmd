set HEXFILE=%1
set "HEXFILE=%HEXFILE:\=/%"

REM 用绝对路径表示 openocd.exe
REM 用相对路径表示 wch-interface.cfg
D:\application\code\MounRiver\MounRiver_Studio2\resources\app\resources\win32\components\WCH\OpenOCD\OpenOCD\bin\openocd.exe ^
    -f .\tools\wch-interface.cfg ^
    -c init -c halt ^
    -c "flash erase_sector wch_riscv 0 last" ^
    -c "program %HEXFILE%" ^
    -c wlink_reset_resume -c exit