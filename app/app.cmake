#
# Qt6 を検索
#
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia)

#
# Qt の自動コード生成を有効化 (MOC / UIC / RCC)
#
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#
# ソースファイルの収集
#
file(GLOB SRC_FILES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/*.h
    ${CMAKE_CURRENT_LIST_DIR}/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/view/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/*.h
    ${CMAKE_CURRENT_LIST_DIR}/view/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/view/*.ui
    ${CMAKE_CURRENT_LIST_DIR}/view/*.qrc
)

#
# 実行ファイルのターゲット定義
#
qt_add_executable(APP
    ${SRC_FILES}
)

target_link_libraries(APP
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Multimedia
        SIGNAL_PROCESSING_PRACTICE_LIB
)

#
# Windows: GUI サブシステムに設定し, windeployqt スクリプトを生成
#
if(WIN32)
    set_target_properties(APP PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    qt_generate_deploy_app_script(
        TARGET APP
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
endif()
