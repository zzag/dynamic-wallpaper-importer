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

#include "Writer.h"
#include "Wallpaper.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>

void Writer::setFormat(const QString& format)
{
    m_format = format;
}

void Writer::setId(const QString& id)
{
    m_id = id;
}

void Writer::setName(const QString& name)
{
    m_name = name;
}

void Writer::setWallpaper(std::shared_ptr<Wallpaper> wallpaper)
{
    m_wallpaper = wallpaper;
}

void Writer::write(const QString& targetPath)
{
    if (!m_wallpaper)
        return;

    QDir targetDirectory;
    if (targetPath.isEmpty())
        targetDirectory.setPath(QDir::currentPath());
    else
        targetDirectory.setPath(targetPath);

    m_packageRoot.setPath(targetDirectory.filePath(m_id));
    if (!m_packageRoot.exists())
        m_packageRoot.mkpath(QStringLiteral("."));

    writeImages();
    writePreview();
    writeMetaData();
}

QString Writer::fileName(const QString& baseName) const
{
    return baseName + QLatin1Char('.') + m_format;
}

void Writer::forEachImage(std::function<void(const Wallpaper::Image&, int)> callback) const
{
    const int imageCount = m_wallpaper->images().count();
    for (int i = 0; i < imageCount; ++i) {
        const Wallpaper::Image& image = m_wallpaper->images().at(i);
        callback(image, i);
    }
}

void Writer::writeImages() const
{
    const QDir imagesRoot = m_packageRoot.path() + QStringLiteral("/contents/images/");
    if (!imagesRoot.exists())
        imagesRoot.mkpath(QStringLiteral("."));

    forEachImage([=](const Wallpaper::Image& image, int index) {
        const QString baseName = QString::number(index);
        image.data.save(imagesRoot.filePath(fileName(baseName)));
    });
}

void Writer::writeMetaData() const
{
    QJsonDocument document;
    QJsonArray metaDataArray;

    forEachImage([&](const Wallpaper::Image& image, int index) {
        QJsonObject imageObject;
        switch (m_wallpaper->type()) {
        case Wallpaper::Solar:
            imageObject[QLatin1String("Azimuth")] = image.azimuth;
            imageObject[QLatin1String("Elevation")] = image.elevation;
            break;
        case Wallpaper::Timed:
            imageObject[QLatin1String("Time")] = image.time;
            break;
        default:
            Q_UNREACHABLE();
            break;
        }
        imageObject[QLatin1String("FileName")] = fileName(QString::number(index));
        metaDataArray.append(imageObject);
    });

    QJsonObject wallpaperObject;
    if (m_wallpaper->type() == Wallpaper::Solar)
        wallpaperObject[QLatin1String("Type")] = QLatin1String("solar");
    if (m_wallpaper->type() == Wallpaper::Timed)
        wallpaperObject[QLatin1String("Type")] = QLatin1String("timed");
    wallpaperObject[QLatin1String("Preview")] = fileName(QStringLiteral("preview"));
    wallpaperObject[QLatin1String("MetaData")] = metaDataArray;

    QJsonObject pluginObject;
    pluginObject[QLatin1String("Id")] = m_id;
    pluginObject[QLatin1String("Name")] = m_name;

    QJsonObject root;
    root[QLatin1String("KPlugin")] = pluginObject;
    root[QLatin1String("Wallpaper")] = wallpaperObject;
    document.setObject(root);

    QFile file(m_packageRoot.path() + QLatin1String("/metadata.json"));
    if (!file.open(QIODevice::WriteOnly))
        return;

    file.write(document.toJson(QJsonDocument::Indented));
}

QImage Writer::solarNoonImage() const
{
    QImage noonImage;

    qreal highestElevation = -90;
    forEachImage([&](const Wallpaper::Image& image, int) {
        if (image.elevation < highestElevation)
            return;
        noonImage = image.data;
        highestElevation = image.elevation;
    });

    return noonImage;
}

QImage Writer::timedNoonImage() const
{
    QImage noonImage;

    qreal bestScore = 1;
    forEachImage([&](const Wallpaper::Image& image, int) {
        const qreal score = std::abs(image.time - 0.5);
        if (bestScore < score)
            return;
        noonImage = image.data;
        bestScore = score;
    });

    return noonImage;
}

QImage Writer::solarMidnightImage() const
{
    QImage midnightImage;

    qreal lowestElevation = 90;
    forEachImage([&](const Wallpaper::Image& image, int) {
        if (lowestElevation < image.elevation)
            return;
        midnightImage = image.data;
        lowestElevation = image.elevation;
    });

    return midnightImage;
}

QImage Writer::timedMidnightImage() const
{
    QImage midnightImage;

    qreal bestScore = 1;
    forEachImage([&](const Wallpaper::Image& image, int) {
        const qreal score = std::min(image.time, 1 - image.time);
        if (bestScore < score)
            return;
        midnightImage = image.data;
        bestScore = score;
    });

    return midnightImage;
}

void Writer::writePreview() const
{
    const QImage midnightImage = m_wallpaper->type() == Wallpaper::Solar ? solarMidnightImage() : timedMidnightImage();
    const QImage noonImage = m_wallpaper->type() == Wallpaper::Solar ? solarNoonImage() : timedNoonImage();

    const QSize previewSize = midnightImage.size().expandedTo(noonImage.size());
    QImage previewImage(previewSize, QImage::Format_RGB888);

    const QRect targetLeftHalfRect(0, 0, previewImage.width() / 2, previewImage.height());
    const QRect targetRightHalfRect(previewImage.width() / 2, 0, previewImage.width() / 2, previewImage.height());
    const QRect sourceLeftHalfRect(0, 0, midnightImage.width() / 2, midnightImage.height());
    const QRect sourceRightHalfRect(noonImage.width() / 2, 0, noonImage.width() / 2, noonImage.height());

    QPainter painter(&previewImage);
    painter.drawImage(targetLeftHalfRect, midnightImage, sourceLeftHalfRect);
    painter.drawImage(targetRightHalfRect, noonImage, sourceRightHalfRect);
    painter.end();

    previewImage.save(m_packageRoot.path() + QLatin1String("/contents/images/") + fileName(QStringLiteral("preview")));
}
