REM REM Задаем компанию и название продукта

REM АртГео
set COMPANYNAME="ООО Нефтегазовый НИЦ МГУ им. М.В. Ломоносова, ООО Актуальные технологии"
set PRODUCT="АртГео" 

REM Общие параметры
set MAJOR=1
set MINOR=5
set YEAR=2017

set SLN="GeoZonesWrap\GeoZonesWrap.csproj"

set OUTPUTPATH="E:\Projects\Build\Artgeo\GeoZones"
call "c:\Program Files (x86)\MSBuild\14.0\Bin\MsBuild.exe" %SLN% /t:Build /p:Configuration=Release /p:Product=%PRODUCT% /p:CompanyName=%COMPANYNAME% /p:Major=%MAJOR% /p:Minor=%MINOR% /p:Year=%YEAR% /p:OutputPath=%OUTPUTPATH% /p:DebugSymbols=false /p:DebugType=None


