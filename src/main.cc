/*
 * Copyright (C) 2019 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <QGuiApplication>

#include <QCommandLineOption>
#include <QCommandLineParser>

#include "Loader.h"
#include "Wallpaper.h"
#include "Writer.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    QCoreApplication::setApplicationName("dynamic-wallpaper-importer");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Dynamic wallpaper importer");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption formatOption(QStringLiteral("format"),
        QCoreApplication::translate("format", "Preferred image format."),
        QCoreApplication::translate("format", "png|jpg"),
        QStringLiteral("png"));
    parser.addOption(formatOption);

    QCommandLineOption sourceOption(QStringLiteral("source"),
        QCoreApplication::translate("main", "Path to the source dynamic wallpaper."),
        QCoreApplication::translate("main", "file"));
    parser.addOption(sourceOption);

    QCommandLineOption idOption(QStringLiteral("id"),
        QCoreApplication::translate("main", "Preferred id of the wallpaper."),
        QCoreApplication::translate("main", "id"));
    parser.addOption(idOption);

    QCommandLineOption labelOption(QStringLiteral("label"),
        QCoreApplication::translate("main", "Preferred name of the wallpaper."),
        QCoreApplication::translate("main", "label"));
    parser.addOption(labelOption);

    QCommandLineOption targetOption(QStringLiteral("target"),
        QCoreApplication::translate("target", "Directory where wallpaper will be stored."),
        QCoreApplication::translate("target", "directory"));
    parser.addOption(targetOption);

    parser.process(app);

    if (!parser.isSet(sourceOption) || !parser.isSet(idOption) || !parser.isSet(labelOption))
        parser.showHelp(-1);

    Loader loader;
    std::shared_ptr<Wallpaper> wallpaper = loader.load(parser.value(sourceOption));
    if (!wallpaper)
        return -1;

    Writer writer;
    writer.setWallpaper(wallpaper);
    writer.setFormat(parser.value(formatOption));
    writer.setId(parser.value(idOption));
    writer.setName(parser.value(labelOption));
    writer.write(parser.value(targetOption));

    return 0;
}
