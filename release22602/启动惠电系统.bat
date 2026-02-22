@echo off
chcp 65001 > nul
echo ========================================
echo     惠电宿舍电费充值管理系统
echo ========================================
echo.
ECHO is off.
rem 获取脚本所在目录
set "APP_PATH=%~dp0"
ECHO is off.
rem 启动程序
echo 正在启动系统...
start "" "%APP_PATH%GraduatingDesice.exe" %*
ECHO is off.
rem 等待程序启动
timeout /t 1 /nobreak > nul
