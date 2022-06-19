@echo off

if "%1"=="_chk_" shift

rem --- Konfiguracja ocen.bat ---
rem W przypadku opracowania wlasnego zestawu testow nalezy zmienic
rem zawartosc zmiennej TESTS a testy zabpiowac do kataloguw IN\, OUT\
rem nazwy testow powinny analogiczne do przykladowych

if "%1"=="PNSearch" goto PNSearch
if "%1"=="NMKSolver" goto PNSearch



set T=%1
goto end

:PNSearch
set I=PNSearch
set T=1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 BONUS1 BONUS2 BONUS3 BONUS4 BONUS5 BONUS6 BONUS7 BONUS8 BONUS9 BONUS10
set C=bin\cmp.exe
goto end


rem --- Koniec konfiguracji

:end

if "%C%"=="" goto def_chk
goto new_chk
:def_chk
set C=bin\cmp.exe
:new_chk

:real_end
