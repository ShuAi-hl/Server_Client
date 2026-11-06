cmake_minimum_required(VERSION 3.21)
project(reactor_client_demo_cpp LANGUAGES CXX)

# 注意：Qt 的 moc 无法在含非 ASCII 字符的路径下创建输出文件，
# 源码位于中文目录时请把构建目录放在纯英文路径，例如：
#   cmake -S . -B C:/Users/<you>/.cache/reactor_client_build -G Ninja ...

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 三方库根目录（可用 -DFFMPEG_ROOT=... 覆盖）
set(FFMPEG_ROOT "C:/ffmpeg/ffmpeg-7.1.1" CACHE PATH "FFmpeg 根目录")
set(OPENCV_ROOT "C:/opencv452"          CACHE PATH "OpenCV 根目录")

find_package(Qt6 REQUIRED COMPONENTS Widgets Network Multimedia)
qt_standard_project_setup()

# MVC 分层：
#   model      —— 数据、状态、信令与媒体业务逻辑（不依赖任何界面类）
#   media      —— 编解码与 RTP 封装等底层媒体基础设施
#   controller —— 控制器：编排 model 与 view，承载全部交互逻辑
#   view       —— 纯界面：只负责展示与用户输入转发
add_executable(reactor_client
    src/main.cpp

    # ---- Controller ----
    src/controller/AppController.cpp

    # ---- Model ----
    src/model/AppModel.cpp
    src/model/SignalingClient.cpp
    src/model/MediaEngine.cpp

    # ---- Media 基础设施 ----
    src/media/RtpPacket.cpp
    src/media/VideoCodec.cpp
    src/media/AudioCodec.cpp

    # ---- View ----
    src/view/MainWindow.cpp
    src/view/LoginDialog.cpp
    src/view/VideoWidget.cpp
)

# Windows 下链接为 GUI 子系统（-mwindows），避免双击时弹出 cmd 控制台窗口。
# 注意：开启后程序没有控制台输出，调试阶段的 qDebug/printf 需用调试器查看。
if (WIN32)
    set_target_properties(reactor_client PROPERTIES WIN32_EXECUTABLE ON)
endif()

target_include_directories(reactor_client PRIVATE
    src
    src/model
    src/media
    src/controller
    src/view
    "${FFMPEG_ROOT}/include"
    "${OPENCV_ROOT}/include"
)

target_link_directories(reactor_client PRIVATE
    "${FFMPEG_ROOT}/lib"
    "${OPENCV_ROOT}/x64/mingw/lib"
)

target_link_libraries(reactor_client PRIVATE
    Qt6::Widgets Qt6::Network Qt6::Multimedia
    avcodec avformat avutil swscale swresample
    opencv_core452 opencv_imgproc452 opencv_imgcodecs452 opencv_videoio452
)

# 运行时依赖部署（可选）：cmake --build . --target deploy
add_custom_target(deploy
    COMMAND Qt6::windeployqt --no-translations "$<TARGET_FILE:reactor_client>"
    DEPENDS reactor_client
    COMMENT "复制 Qt 运行时依赖到输出目录"
)
