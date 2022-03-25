/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : Unit tests for VKontakte web service
 *
 * Copyright (C) 2011-2015 by Alexander Potashev <aspotashev at gmail dot com>
 * Copyright (C) 2011-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_VKONTAKTE_VKTESTBASE_H
#define DIGIKAM_VKONTAKTE_VKTESTBASE_H

// Qt includes

#include <QObject>
#include <QVector>

// Local includes

#include "vkontakte_apppermissions.h"

namespace Vkontakte
{
    class VkApi;
}

class VkTestBase : public QObject
{
    Q_OBJECT

public:

    explicit VkTestBase(QObject* const parent = nullptr);
    ~VkTestBase() override;

protected:

    void authenticate(Vkontakte::AppPermissions::Value permissions);
    QString accessToken()   const;

private:

    QString getSavedToken() const;

private:

    Vkontakte::VkApi* m_vkapi;
};

#endif // DIGIKAM_VKONTAKTE_VKTESTBASE_H
