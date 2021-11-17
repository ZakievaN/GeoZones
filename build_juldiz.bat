REM REM Задаем компанию и название продукта

REM Жулдыз, не задаем явно название продукта, возьмется из параметров по
REM умолчанию
set COMPANYNAME="ТОО Алстронтелеком"

REM Общие параметры
set MAJOR=1
set MINOR=5
set YEAR=2017

set SLN="GeoZonesWrap\GeoZonesWrap.csproj"

set OUTPUTPATH="E:\Projects\Build\Juldiz\GeoZones"
call "c:\Program Files (x86)\MSBuild\14.0\Bin\MsBuild.exe" %SLN% /t:Build /p:Configuration=Release /p:CompanyName=%COMPANYNAME% /p:Major=%MAJOR% /p:Minor=%MINOR% /p:Year=%YEAR% /p:OutputPath=%OUTPUTPATH% /p:DebugSymbols=false /p:DebugType=None
