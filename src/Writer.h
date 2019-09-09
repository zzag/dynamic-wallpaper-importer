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

#include "Wallpaper.h"

#include <QDir>
#include <QString>

#include <functional>
#include <memory>

class Q_DECL_EXPORT Writer {
public:
    /**
     * Sets the preferred image file extension.
     */
    void setFormat(const QString& format);

    /**
     * Sets the preferred id of the wallpaper.
     */
    void setId(const QString& id);

    /**
     * Sets the preferred human-readable name of the wallpaper.
     */
    void setName(const QString& name);

    /**
     * Sets the dynamic wallpaper to be written to the disk.
     */
    void setWallpaper(std::shared_ptr<Wallpaper> wallpaper);

    /**
     * Writes the dynamic wallpaper to the disk.
     */
    void write(const QString& targetPath = QString());

private:
    void forEachImage(std::function<void(const Wallpaper::Image&, int)> callback) const;

    QString fileName(const QString& baseName) const;
    QImage solarNoonImage() const;
    QImage timedNoonImage() const;
    QImage solarMidnightImage() const;
    QImage timedMidnightImage() const;

    void writeImages() const;
    void writeMetaData() const;
    void writePreview() const;

    QDir m_packageRoot;
    QString m_format;
    QString m_id;
    QString m_name;
    std::shared_ptr<Wallpaper> m_wallpaper;
};
