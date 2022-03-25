/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-11-15
 * Description : a tool to export images to VKontakte web service
 *
 * Copyright (C) 2011-2015 by Alexander Potashev <aspotashev at gmail dot com>
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009      by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2010      by Roman Tsisyk <roman at tsisyk dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_VK_WINDOW_H
#define DIGIKAM_VK_WINDOW_H

// Local includes

#include "wstooldialog.h"
#include "dprogresswdg.h"
#include "dinfointerface.h"
#include "ditemslist.h"

class KJob;

using namespace Digikam;

namespace DigikamGenericVKontaktePlugin
{

class VKWindow : public WSToolDialog
{
    Q_OBJECT

public:

    explicit VKWindow(DInfoInterface* const iface,
                      QWidget* const parent,
                      bool import = false);
    ~VKWindow()                             override;

    /**
     * Use this method to (re-)activate the dialog after it has been created
     * to display it. This also loads the currently selected images.
     */
    void startReactivation();

Q_SIGNALS:

    void signalUpdateBusyStatus(bool busy);

private Q_SLOTS:

    // requesting photo information
    void slotPhotoUploadDone(KJob* kjob);

    void slotStartTransfer();

    void slotFinished();

    void slotUpdateBusyStatus(bool busy);
    void slotUpdateBusyStatusReady();

    void slotAuthenticated();
    void slotAuthCleared();
    void slotUpdateHeaderLabel();

private:

    void initAccountBox();

    void readSettings();
    void writeSettings();

    void reset();

    void handleVkError(KJob* kjob);

    void closeEvent(QCloseEvent* event)     override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericVKontaktePlugin

#endif // DIGIKAM_VK_WINDOW_H
