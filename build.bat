set compiler_flags=-MTd -nologo -EHa- -Gm- -GR- -Od -Oi -W4 -wd4530 -wd4005 -wd4189 -wd4505 -wd4996 -wd4100 -wd4201 -FC -Zi
set linker_flags=-opt:ref -subsystem:windows
pushd ..\build
cl %compiler_flags% -Fmgoblinlord.map ..\GoblinLord\src\win32_goblinlord.cpp -Fd /link -incremental:no -opt:ref %linker_flags%
popd