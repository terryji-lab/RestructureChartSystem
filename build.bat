@echo off
set SRC=E:\ChartSystem\src
set MINGW=E:\mingw64\bin\g++.exe

echo Building ChartSystem...

%MINGW% -std=c++14 -g -I %SRC% ^
  %SRC%\main.cpp ^
  %SRC%\app\Themes.cpp ^
  %SRC%\app\AppHelpers.cpp ^
  %SRC%\app\ExportHelpers.cpp ^
  %SRC%\chart\Chart.cpp ^
  %SRC%\chart\BarChart.cpp ^
  %SRC%\chart\LineChart.cpp ^
  %SRC%\chart\PieChart.cpp ^
  %SRC%\chart\AreaChart.cpp ^
  %SRC%\chart\ChartCoordinates.cpp ^
  %SRC%\chart\ChartGridRenderer.cpp ^
  %SRC%\ui\Button.cpp ^
  %SRC%\ui\Card.cpp ^
  %SRC%\ui\PopupCard.cpp ^
  %SRC%\ui\TextInput.cpp ^
  %SRC%\ui\Pages.cpp ^
  %SRC%\ui\MainPage.cpp ^
  %SRC%\ui\ChartPage.cpp ^
  %SRC%\ui\ExportPathDialog.cpp ^
  %SRC%\utils\FileDataReader.cpp ^
  %SRC%\utils\FilePathUtils.cpp ^
  %SRC%\utils\ImageExporter.cpp ^
  %SRC%\utils\StringUtils.cpp ^
  %SRC%\utils\ClipboardUtils.cpp ^
  -leasyx -lgdi32 -lole32 -loleaut32 -luuid -lwinmm -lmsimg32 -lgdiplus ^
  -o E:\ChartSystem\main.exe

if %ERRORLEVEL% EQU 0 (
    echo Build succeeded!
) else (
    echo Build FAILED.
)
