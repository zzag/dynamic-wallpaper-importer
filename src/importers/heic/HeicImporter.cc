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

#include "HeicImporter.h"
#include "Wallpaper.h"

#include <QDomDocument>
#include <QLoggingCategory>
#include <QMimeDatabase>

#include <libheif/heif.h>
#include <plist/plist.h>

Q_LOGGING_CATEGORY(heic, "heic")

struct HeifContextDeleter {
    static void cleanup(heif_context* context)
    {
        heif_context_free(context);
    }
};

HeicImporter::HeicImporter(QObject* parent)
    : Importer(parent)
{
}

HeicImporter::~HeicImporter()
{
}

static QVector<Wallpaper::Image> discoverImages(heif_context* context)
{
    QVector<Wallpaper::Image> images;

    const int imageCount = heif_context_get_number_of_top_level_images(context);
    QVector<heif_item_id> imageIds(imageCount);
    heif_context_get_list_of_top_level_image_IDs(context, imageIds.data(), imageCount);

    for (const heif_item_id& id : imageIds) {
        heif_image_handle* handle = nullptr;
        heif_error error = heif_context_get_image_handle(context, id, &handle);
        if (error.code != heif_error_Ok)
            continue;

        heif_image* image = nullptr;
        error = heif_decode_image(handle, &image, heif_colorspace_RGB, heif_chroma_interleaved_24bit, nullptr);
        if (error.code != heif_error_Ok)
            continue;

        int bytesPerLine = 0;
        const uint8_t* data = heif_image_get_plane_readonly(image, heif_channel_interleaved, &bytesPerLine);
        if (!data)
            continue;

        const int width = heif_image_handle_get_width(handle);
        const int height = heif_image_handle_get_height(handle);

        Wallpaper::Image wallpaperImage;
        wallpaperImage.data = QImage(data, width, height, bytesPerLine, QImage::Format_RGB888).copy();
        images << wallpaperImage;
    }

    return images;
}

static QByteArray discoverMetaData(heif_context* context)
{
    heif_image_handle* handle = nullptr;
    heif_context_get_primary_image_handle(context, &handle);

    const int metaDataCount = heif_image_handle_get_number_of_metadata_blocks(handle, nullptr);
    if (metaDataCount != 1)
        return QByteArray();

    heif_item_id metaDataId = 0;
    heif_image_handle_get_list_of_metadata_block_IDs(handle, nullptr, &metaDataId, 1);

    QByteArray metaDataBlock(heif_image_handle_get_metadata_size(handle, metaDataId), 0);
    heif_image_handle_get_metadata(handle, metaDataId, metaDataBlock.data());

    QDomDocument metaDataDocument(metaDataBlock);
    metaDataDocument.setContent(metaDataBlock);
    if (metaDataDocument.isNull())
        return QByteArray();

    const QDomNodeList nodes = metaDataDocument.elementsByTagName(QStringLiteral("rdf:Description"));
    if (nodes.count() != 1)
        return QByteArray();

    const QDomElement descriptionElement = nodes.at(0).toElement();

    QString rawMetaData;

    if (descriptionElement.hasAttribute(QStringLiteral("apple_desktop:solar")))
        rawMetaData = descriptionElement.attribute(QStringLiteral("apple_desktop:solar"));
    if (descriptionElement.hasAttribute(QStringLiteral("apple_desktop:h24")))
        rawMetaData = descriptionElement.attribute(QStringLiteral("apple_desktop:h24"));

    return QByteArray::fromBase64(rawMetaData.toUtf8());
}

static Wallpaper::Type wallpaperTypeFromMetaData(const QByteArray& metaData)
{
    Wallpaper::Type type = Wallpaper::Unknown;

    const QVector<QPair<QByteArray, Wallpaper::Type>> types {
        { QByteArrayLiteral("si"), Wallpaper::Solar },
        { QByteArrayLiteral("ti"), Wallpaper::Timed },
    };

    plist_t plist;
    plist_from_memory(metaData.data(), metaData.size(), &plist);

    for (const QPair<QByteArray, Wallpaper::Type>& pair : types) {
        if (!plist_dict_get_item(plist, pair.first))
            continue;
        type = pair.second;
        break;
    }

    plist_free(plist);

    return type;
}

static bool associateSolarMetaData(const QByteArray& metaData, QVector<Wallpaper::Image>& images)
{
    plist_t plist;
    plist_from_memory(metaData.data(), metaData.size(), &plist);

    plist_t root = plist_dict_get_item(plist, "si");

    const int itemCount = plist_array_get_size(root);
    for (int i = 0; i < itemCount; ++i) {
        plist_t node = plist_array_get_item(root, i);

        qreal azimuth;
        qreal elevation;
        uint64_t imageIndex;

        plist_get_uint_val(plist_dict_get_item(node, "i"), &imageIndex);
        plist_get_real_val(plist_dict_get_item(node, "z"), &azimuth);
        plist_get_real_val(plist_dict_get_item(node, "a"), &elevation);

        images[imageIndex].azimuth = azimuth;
        images[imageIndex].elevation = elevation;
    }

    plist_free(plist);

    return true;
}

static bool associateTimedMetaData(const QByteArray& metaData, QVector<Wallpaper::Image>& images)
{
    plist_t plist;
    plist_from_memory(metaData.data(), metaData.size(), &plist);

    plist_t root = plist_dict_get_item(plist, "ti");

    const int itemCount = plist_array_get_size(root);
    for (int i = 0; i < itemCount; ++i) {
        plist_t node = plist_array_get_item(root, i);

        qreal time;
        uint64_t imageIndex;

        plist_get_uint_val(plist_dict_get_item(node, "i"), &imageIndex);
        plist_get_real_val(plist_dict_get_item(node, "t"), &time);

        images[imageIndex].time = time;
    }

    plist_free(plist);

    return true;
}

static bool isHeicFile(const QString& fileName)
{
    const QMimeDatabase database;

    const QMimeType heicMimeType = database.mimeTypeForName(QStringLiteral("image/heif"));
    const QMimeType fileMimeType = database.mimeTypeForFile(fileName);
    if (fileMimeType == heicMimeType)
        return true;

    if (fileName.endsWith(QStringLiteral(".heic")) || fileName.endsWith(QStringLiteral(".heif")))
        return true;

    return false;
}

std::unique_ptr<Wallpaper> HeicImporter::load(const QString& fileName) const
{
    if (!isHeicFile(fileName))
        return nullptr;

    QScopedPointer<heif_context, HeifContextDeleter> context(heif_context_alloc());

    heif_error error = heif_context_read_from_file(context.data(), fileName.toUtf8(), nullptr);
    if (error.code != heif_error_Ok) {
        qCWarning(heic, "Could not load %s: %s", fileName.toUtf8().constData(), error.message);
        return nullptr;
    }

    const QByteArray metaData = discoverMetaData(context.data());
    if (metaData.isEmpty()) {
        qCWarning(heic, "Could not find wallpaper metadata");
        return nullptr;
    }

    const Wallpaper::Type type = wallpaperTypeFromMetaData(metaData);
    if (type == Wallpaper::Type::Unknown) {
        qCWarning(heic, "Unknown wallpaper type");
        return nullptr;
    }

    QVector<Wallpaper::Image> images = discoverImages(context.data());
    if (images.isEmpty()) {
        qCWarning(heic, "Dynamic wallpaper does not have any images");
        return nullptr;
    }

    switch (type) {
    case Wallpaper::Type::Solar:
        if (!associateSolarMetaData(metaData, images))
            return nullptr;
        break;
    case Wallpaper::Type::Timed:
        if (!associateTimedMetaData(metaData, images))
            return nullptr;
        break;
    case Wallpaper::Type::Unknown:
        Q_UNREACHABLE();
        break;
    }

    return std::make_unique<Wallpaper>(type, images);
}
