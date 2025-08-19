find_package(PkgConfig REQUIRED)
pkg_check_modules(arcan-shmif REQUIRED IMPORTED_TARGET arcan-shmif)

set(autotype_ARCAN_SOURCES AutoTypeArcan.cpp)

find_package(Qt6 CONFIG REQUIRED COMPONENTS GuiPrivate)
add_library(keepassxc-autotype-arcan MODULE ${autotype_ARCAN_SOURCES})
target_link_libraries(keepassxc-autotype-arcan keepassxc_gui Qt::Core Qt::Widgets Qt::Gui Qt::GuiPrivate PkgConfig::arcan-shmif)
install(TARGETS keepassxc-autotype-arcan
        BUNDLE DESTINATION . COMPONENT Runtime
        LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR} COMPONENT Runtime)
