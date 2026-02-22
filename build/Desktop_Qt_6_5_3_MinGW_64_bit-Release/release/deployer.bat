@echo off
chcp 65001 > nul
echo ============================================
echo 惠电宿舍电费充值管理系统 - 自动封装脚本
echo ============================================
echo.

:: ============================================
:: 配置部分 - 根据你的实际情况修改
:: ============================================

:: 你的Qt安装路径（64位MinGW）
set QT_PATH=G:\QT_file\6.5.3\mingw_64

:: 项目名称
set PROJECT_NAME=GraduatingDesice

:: 编译输出路径
set BUILD_RELEASE_DIR=G:\qttto\GraduatingDesice\build\Desktop_Qt_6_5_3_MinGW_64_bit-Release\release

:: 发布目录
set DEPLOY_DIR=%cd%\惠电系统_发布版_%date:~0,4%%date:~5,2%%date:~8,2%

:: ============================================
:: 开始封装
:: ============================================

echo 1. 检查必要的文件和目录...

:: 检查Qt路径
if not exist "%QT_PATH%" (
    echo 错误: Qt路径不存在 - %QT_PATH%
    echo 请确认你的Qt安装位置，并修改脚本中的QT_PATH变量
    echo.
    echo 常见Qt安装位置：
    echo   C:\Qt\6.5.3\mingw_64
    echo   D:\Qt\6.5.3\mingw_64
    pause
    exit /b 1
)

echo   找到Qt路径: %QT_PATH%

:: 检查可执行文件
set EXE_FILE=%BUILD_RELEASE_DIR%\%PROJECT_NAME%.exe
if not exist "%EXE_FILE%" (
    echo 错误: 找不到可执行文件
    echo 路径: %EXE_FILE%
    echo.
    echo 可能的原因：
    echo 1. 没有编译Release版本
    echo 2. 路径名有拼写错误
    echo 3. 构建目录名称不同
    echo.
    echo 请检查以下路径是否存在：
    echo   %BUILD_RELEASE_DIR%
    echo.
    echo 如果路径不存在，请：
    echo 1. 在Qt Creator中编译Release版本
    echo 2. 检查实际构建目录的名称
    pause
    exit /b 1
)

echo   找到可执行文件: %EXE_FILE%
echo   文件大小: 
for %%F in ("%EXE_FILE%") do echo     %%~zF 字节

echo.
echo 2. 创建发布目录...
if exist "%DEPLOY_DIR%" (
    echo   删除旧的发布目录...
    rmdir /s /q "%DEPLOY_DIR%"
)
mkdir "%DEPLOY_DIR%"
echo   创建目录: %DEPLOY_DIR%

echo.
echo 3. 复制可执行文件...
copy "%EXE_FILE%" "%DEPLOY_DIR%\"
echo   复制完成

echo.
echo 4. 使用windeployqt自动收集依赖...
cd "%DEPLOY_DIR%"
echo   当前目录: %cd%
echo   运行windeployqt...

"%QT_PATH%\bin\windeployqt.exe" --verbose 1 %PROJECT_NAME%.exe
if errorlevel 1 (
    echo   警告: windeployqt运行可能有问题
    echo   继续封装...
)

echo.
echo 5. 检查并复制必要的插件...

:: 检查并创建插件目录结构
if not exist plugins mkdir plugins
if not exist platforms mkdir platforms
if not exist sqldrivers mkdir sqldrivers
if not exist styles mkdir styles

:: 复制平台插件
if exist "%QT_PATH%\plugins\platforms\qwindows.dll" (
    copy "%QT_PATH%\plugins\platforms\qwindows.dll" platforms\
    echo   ✓ 已复制: platforms/qwindows.dll
) else (
    echo   ✗ 未找到: platforms/qwindows.dll
)

:: 复制SQL驱动插件
set /a sql_count=0
for /f "delims=" %%f in ('dir /b "%QT_PATH%\plugins\sqldrivers\*.dll" 2^>nul') do (
    copy "%QT_PATH%\plugins\sqldrivers\%%f" sqldrivers\
    set /a sql_count+=1
)
echo   ✓ 已复制 %sql_count% 个SQL驱动插件

:: 复制样式插件
if exist "%QT_PATH%\plugins\styles\qwindowsvistastyle.dll" (
    copy "%QT_PATH%\plugins\styles\qwindowsvistastyle.dll" styles\
    echo   ✓ 已复制: styles/qwindowsvistastyle.dll
) else (
    echo   ✗ 未找到样式插件
)

echo.
echo 6. 检查并复制运行时库...
echo   检查MinGW运行时库...

:: 检查并复制MinGW运行时库
set MINGW_DLLS=libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll
set /a copied_dlls=0

for %%d in (%MINGW_DLLS%) do (
    if exist "%QT_PATH%\bin\%%d" (
        copy "%QT_PATH%\bin\%%d" .
        echo   ✓ 已复制: %%d
        set /a copied_dlls+=1
    ) else (
        echo   ✗ 未找到: %%d
    )
)

if %copied_dlls% == 0 (
    echo   ! 警告: 未找到MinGW运行时库
    echo   ! 可能是使用了MSVC版本的Qt
)

:: 检查并复制其他可能的运行时库
if exist "%QT_PATH%\bin\libgomp-1.dll" (
    copy "%QT_PATH%\bin\libgomp-1.dll" .
    echo   ✓ 已复制: libgomp-1.dll
)

echo.
echo 7. 复制资源文件...

:: 复制图标目录
if exist "G:\qttto\GraduatingDesice\icon" (
    xcopy /s /e /y "G:\qttto\GraduatingDesice\icon" icon\
    echo   ✓ 已复制: icon目录
)

:: 复制样式表
if exist "G:\qttto\GraduatingDesice\styles.qss" (
    copy "G:\qttto\GraduatingDesice\styles.qss" .
    echo   ✓ 已复制: styles.qss
)

echo.
echo 8. 创建启动脚本...
(
echo @echo off
echo chcp 65001 ^> nul
echo echo ========================================
echo echo     惠电宿舍电费充值管理系统
echo echo ========================================
echo echo.
echo 
echo rem 获取脚本所在目录
echo set "APP_PATH=%%~dp0"
echo 
echo rem 启动程序
echo echo 正在启动系统...
echo start "" "%%APP_PATH%%%PROJECT_NAME%.exe" %%*
echo 
echo rem 等待程序启动
echo timeout /t 1 /nobreak ^> nul
) > "启动惠电系统.bat"

:: 创建卸载脚本
(
echo @echo off
echo chcp 65001 ^> nul
echo echo ========================================
echo echo     卸载惠电宿舍电费充值管理系统
echo echo ========================================
echo echo.
echo echo 警告：这将删除系统及其所有文件！
echo echo.
echo set /p confirm=确定要卸载吗？(输入Y确认): 
echo if /i "%%confirm%%"=="Y" (
echo     echo 正在删除文件...
echo     timeout /t 2 /nobreak ^> nul
echo     rmdir /s /q "%%~dp0"
echo     echo 卸载完成！
echo ) else (
echo     echo 取消卸载。
echo )
echo echo.
echo pause
) > "卸载.bat"

echo   ✓ 创建: 启动惠电系统.bat
echo   ✓ 创建: 卸载.bat

echo.
echo 9. 创建说明文档...
(
echo 惠电宿舍电费充值管理系统 使用说明
echo ====================================
echo.
echo 版本信息
echo --------
echo 程序名称: 惠电宿舍电费充值管理系统
echo Qt版本: 6.5.3
echo 编译器: MinGW 64-bit
echo 构建日期: %date:~0,4%-%date:~5,2%-%date:~8,2%
echo.
echo 快速开始
echo --------
echo 1. 双击"启动惠电系统.bat"运行程序
echo 2. 使用测试账号登录：
echo    - 学生账号: 2021001 / 密码: 123456
echo    - 管理员账号: admin / 密码: admin123
echo.
echo 功能说明
echo --------
echo 学生端：
echo   - 查询电费余额
echo   - 电费充值
echo   - 查看充值记录
echo   - 查看用电记录
echo   - 网页查询电费
echo.
echo 管理员端：
echo   - 学生信息管理
echo   - 宿舍信息管理
echo   - 充值记录管理
echo   - 电费扣费
echo   - 数据统计
echo.
echo 注意事项
echo --------
echo 1. 数据库文件会自动创建在程序目录
echo 2. 首次运行会自动初始化示例数据
echo 3. 如无法运行，请检查VC++运行库
echo.
) > README.txt
echo   ✓ 创建: README.txt

echo.
echo ============================================
echo 封装完成！
echo ============================================
echo.
echo 发布目录: %DEPLOY_DIR%
echo.
echo 目录内容:
dir /b "%DEPLOY_DIR%"
echo.
echo 请按以下步骤测试:
echo 1. 打开目录: %DEPLOY_DIR%
echo 2. 双击"启动惠电系统.bat"
echo 3. 测试所有功能是否正常
echo.
echo 重要提示:
echo 如果程序无法运行，请检查以下文件是否存在:
echo   - %PROJECT_NAME%.exe
echo   - Qt6Core.dll
echo   - Qt6Gui.dll
echo   - Qt6Widgets.dll
echo   - Qt6Sql.dll
echo   - Qt6Charts.dll
echo   - platforms\qwindows.dll
echo   - sqldrivers\qsqlite.dll
echo   - styles.qss
echo   - icon\目录
echo.
echo 如果缺少文件，请:
echo 1. 检查Qt路径是否正确
echo 2. 重新运行windeployqt
echo.
pause