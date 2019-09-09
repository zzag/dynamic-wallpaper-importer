find_package(libheif REQUIRED)
find_package(libplist REQUIRED)

add_library(heic MODULE
    HeicImporter.cc
)

target_link_libraries(heic
    Qt5::Core
    Qt5::Xml

    libheif::libheif
    libplist::libplist

    dynamicwallpaperimportercommon
)

install(TARGETS heic DESTINATION ${PLUGIN_INSTALL_DIR}/dynamic-wallpaper/importers/)
