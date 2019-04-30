for /f "tokens=* eol=." %%a in ('dir /ad /b') do (  
cd %%a
"..\..\tools\7z.exe" a -tzip "..\..\data\skins\%%a.sskn" "*" -r -mmt=on
cd ..
)
