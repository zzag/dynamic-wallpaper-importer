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

#include "Loader.h"
#include "Importer.h"
#include "Wallpaper.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QLibrary>
#include <QPluginLoader>

static QStringList discoverCandidates()
{
    QStringList candidates;

    const QStringList libraryPaths = QCoreApplication::libraryPaths();
    for (const QString& path : libraryPaths) {
        const QDir pluginDir(path + QLatin1String("/dynamic-wallpaper/importers/"));
        if (!pluginDir.exists())
            continue;
        const QStringList entries = pluginDir.entryList(QDir::Files | QDir::NoDotAndDotDot);
        for (const QString& entry : entries)
            candidates << pluginDir.absoluteFilePath(entry);
    }

    return candidates;
}

static QVector<Importer*> discoverImporters()
{
    QVector<Importer*> importers;

    const QStringList candidates = discoverCandidates();
    for (const QString& candidate : candidates) {
        if (!QLibrary::isLibrary(candidate))
            continue;
        QPluginLoader loader(candidate);
        if (Importer* importer = qobject_cast<Importer*>(loader.instance()))
            importers << importer;
    }

    return importers;
}

Loader::Loader(QObject* parent)
    : QObject(parent)
    , m_importers(discoverImporters())
{
}

Loader::~Loader()
{
}

std::unique_ptr<Wallpaper> Loader::load(const QString& fileName) const
{
    if (m_importers.isEmpty())
        qWarning() << "No importer plugins have been found";

    for (Importer* importer : m_importers) {
        std::unique_ptr<Wallpaper> wallpaper = importer->load(fileName);
        if (wallpaper)
            return wallpaper;
    }

    return nullptr;
}
