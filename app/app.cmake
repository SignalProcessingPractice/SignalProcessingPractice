#
# Qt6 を検索
#
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia)

message(STATUS "QT_HOST_PATH = ${QT_HOST_PATH}")
message(STATUS "QT_HOST_PATH_CMAKE_DIR = ${QT_HOST_PATH_CMAKE_DIR}")
message(STATUS "QT6_DIR = ${Qt6_DIR}")

#
# Qt プロジェクトのセットアップ
#
qt_standard_project_setup()

#
# ソースファイルの収集
#
file(GLOB SRC_FILES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/*.h
    ${CMAKE_CURRENT_LIST_DIR}/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/common/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/common/*.h
    ${CMAKE_CURRENT_LIST_DIR}/common/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/model/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/model/*.h
    ${CMAKE_CURRENT_LIST_DIR}/model/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/presenter/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/presenter/*.h
    ${CMAKE_CURRENT_LIST_DIR}/presenter/*.hpp
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

target_include_directories(APP
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(APP
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Multimedia
        SIGNAL_PROCESSING_PRACTICE_LIB
)

install(TARGETS APP
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(WIN32)
    set_target_properties(APP PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

if(WIN32)
    qt_generate_deploy_app_script(
        TARGET APP
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )

    install(SCRIPT ${deploy_script})
endif()
