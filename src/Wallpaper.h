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

#include <QImage>
#include <QString>

#include <memory>

/**
 * The Wallpaper class represents a dynamic wallpaper.
 *
 * As its name suggests, a dynamic wallpaper updates itself throughout the day.
 */
class Q_DECL_EXPORT Wallpaper {
public:
    /**
     * This enum type is used to specify the type of the dynamic wallpaper.
     *
     * The type of the dynamic wallpaper specifies what kind of metadata is provided
     * along each image. The metadata will be used by the wallpaper plugin to determine
     * image(s) that reflect the user's light situation most accurately.
     */
    enum Type {
        /**
         * The type of the dynamic wallpaper is unknown to the loader.
         *
         * Most likely, the appropriate importer plugin haven't been found.
         */
        Unknown,
        /**
         * A solar dynamic wallpaper uses the position of the Sun provided along each
         * image to determine what image(s) reflect the user's light situation most
         * accurately.
         *
         * Beware that the dynamic wallpaper may not work if the user lives close to
         * the North or the South pole.
         */
        Solar,
        /**
         * A timed dynamic wallpaper uses the current time to determine what images
         * reflect the user's light situation most accurately. The dynamic wallpaper
         * must provide a real number along each image, which is computed as follows
         *
         * @code
         * time = the number of seconds since the start of the day / 86400
         * @endcode
         */
        Timed,
    };

    struct Image {
        // The filename of the image.
        QImage data;

        // The azimuth angle of the Sun, in degrees.
        qreal azimuth;

        // The elevation angle of the Sun, in degrees.
        qreal elevation;

        // The time value, between 0 and 1.
        qreal time;
    };

    Wallpaper();
    Wallpaper(Type type, const QVector<Image>& images);

    /**
     * Returns the type of the dynamic wallpaper.
     */
    Type type() const;

    /**
     * Returns a list of all images stored in the wallpaper.
     */
    QVector<Image> images() const;

private:
    QVector<Image> m_images;
    Type m_type = Unknown;
};
