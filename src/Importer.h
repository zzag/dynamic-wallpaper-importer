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

#pragma once

#include <QMimeType>
#include <QObject>

#include <memory>

class Wallpaper;

class Q_DECL_EXPORT Importer : public QObject {
    Q_OBJECT

public:
    explicit Importer(QObject* parent = nullptr);
    ~Importer() override;

    /**
     * Attempts to load a dynamic wallpaper with the given @p fileName.
     *
     * This method will return @c null, if the importer failed to load the wallpaper.
     */
    virtual std::unique_ptr<Wallpaper> load(const QString& fileName) const = 0;

private:
    Q_DISABLE_COPY(Importer)
};

Q_DECLARE_INTERFACE(Importer, "com.github.zzag.wallpaper.Importer")
