include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Xml
)

add_library(dynamicwallpaperimportercommon SHARED
    Importer.cc
    Loader.cc
    Wallpaper.cc
    Writer.cc
)

target_link_libraries(dynamicwallpaperimportercommon
    Qt5::Core
    Qt5::Gui
)

add_executable(dynamic-wallpaper-importer
    main.cc
)

target_link_libraries(dynamic-wallpaper-importer
    Qt5::Core
    Qt5::Gui

    dynamicwallpaperimportercommon
)

add_subdirectory(importers)

install(TARGETS dynamicwallpaperimportercommon ${INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)
install(TARGETS dynamic-wallpaper-importer ${INSTALL_TARGETS_DEFAULT_ARGS})
