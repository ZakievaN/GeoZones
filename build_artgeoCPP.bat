REM REM Задаем компанию и название продукта

REM АртГео
set COMPANYNAME="ООО Нефтегазовый НИЦ МГУ им. М.В. Ломоносова, ООО Актуальные технологии"
set PRODUCT="АртГео" 

set SLN="GeoZones\GeoZones.vcxproj"

set OUTPUTPATHFILE="E:\Projects\Build\Artgeo\GeoZones\GeoZones.dll"

REM Сохраняю предыдущую версию сборки

	FOR /F "delims=. tokens=1-3" %%a IN ('DATE /T') DO (
	SET CUR_DAY=%%a
	SET CUR_MONTH=%%b
	SET CUR_YEAR=%%c)
	SET ADD_STR=%CUR_DAY%%CUR_MONTH%%CUR_YEAR%
	RENAME %OUTPUTPATHFILE% "GeoZones%ADD_STR%.dll"


set OUTPUTPATH="E:\Projects\Build\Artgeo\GeoZones"
call "c:\Program Files (x86)\MSBuild\14.0\Bin\MsBuild.exe" %SLN% /t:Build /p:Configuration=Release /p:Platform=x64 /p:OutDir=%OUTPUTPATH%


call del %OUTPUTPATH%\*.pdb
call del %OUTPUTPATH%\*.exp
call del %OUTPUTPATH%\*.iobj
call del %OUTPUTPATH%\*.ipdb
call del %OUTPUTPATH%\*.lib


set FILEDESCR=/s desc "Геометризация областей насыщения"
set COMPINFO=/s company %COMPANYNAME% /s (c) "Copyright (c) 2017 "%COMPANYNAME%
set PRODINFO=/s product %PRODUCT%

verpatch /va %OUTPUTPATHFILE% %FILEDESCR% %COMPINFO% %PRODINFO%