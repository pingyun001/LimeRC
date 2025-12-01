@echo off
setlocal enabledelayedexpansion

REM 获取脚本所在目录路径
set "target_dir=%~dp0"

REM 切换到脚本所在目录
cd /d "%target_dir%"

REM 遍历当前目录及所有子目录中的.map文件
for /r . %%F in (*.map) do (
    REM 获取文件所在目录路径
    set "file_dir=%%~dpF"
    
    REM 检查文件是否不在目标目录（避免重复复制）
    if /i not "!file_dir!"=="%target_dir%" (
        REM 复制文件到目标目录（静默覆盖）
        copy /y "%%F" "%target_dir%" >nul
    )
)

echo copy map Finish