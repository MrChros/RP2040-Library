@echo off
setlocal enabledelayedexpansion

echo.
echo //////////////////////////////////////
echo // TTF Font to Byte Array Generator //
echo // by Dr. Chros on Nov 14th, 2021   //
echo //////////////////////////////////////
echo.

:: List of Fonts
set font_name[0]=Consolas Bold
set font_name[1]=Consolas Bold Italic
set font_name[2]=Consolas Italic
set font_name[3]=Consolas

set font_name[4]=Lucida Console

set font_name[5]=Courier New Bold
set font_name[6]=Courier New Bold Italic
set font_name[7]=Courier New Italic
set font_name[8]=Courier New

set font_name[9]=DejaVu Sans Mono
set font_name[10]=DejaVu Sans Mono Bold
set font_name[11]=DejaVu Sans Mono Bold Oblique

set font_name[12]=Victor Mono Regular
set font_name[13]=Victor Mono Bold
set font_name[14]=Victor Mono Oblique
set font_name[15]=Victor Mono Bold Oblique

:: Different font sizes to be generated
set font_size[0]=5
set font_size[1]=6
set font_size[2]=7
set font_size[3]=8
set font_size[4]=9
set font_size[5]=10
set font_size[6]=12
set font_size[7]=14
set font_size[8]=16
set font_size[9]=24
set font_size[10]=28
set font_size[11]=32


:: Finally loop through both arrays and call the python script to generate the fonts
for /l %%n in (0,1,15) do ( 
	echo -----------------------
	echo !font_name[%%n]! 
	echo -----------------------
	for /l %%m in (0,1,11) do ( 
		call python ./Font_to_ByteArray.py -s !font_size[%%m]! --font "!font_name[%%n]!"
	) 
	echo.
)

echo All done...
pause