/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : a tool to export images to VKontakte web service
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

#ifndef DIGIKAM_VK_AUTH_WIDGET_H
#define DIGIKAM_VK_AUTH_WIDGET_H

// Qt includes

#include <QWidget>
#include <QString>
#include <QGroupBox>

class KJob;

namespace Vkontakte
{
    class VkApi;
}

namespace DigikamGenericVKontaktePlugin
{

class VKAuthWidget : public QGroupBox
{
    Q_OBJECT

public:

    explicit VKAuthWidget(QWidget* const parent, Vkontakte::VkApi* const vkapi);
    ~VKAuthWidget() override;

    QString albumsURL() const;

Q_SIGNALS:

    void signalAuthCleared();
    void signalUpdateAuthInfo();

public Q_SLOTS:

    void slotStartAuthentication(bool forceLogout);

private Q_SLOTS:

    void slotChangeUserClicked();
    void slotUpdateAuthInfo();
    void slotStartGetUserInfo();
    void slotGetUserInfoDone(KJob* kjob);

private:

    void handleVkError(KJob* kjob);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericVKontaktePlugin

#endif // DIGIKAM_VK_AUTH_WIDGET_H
