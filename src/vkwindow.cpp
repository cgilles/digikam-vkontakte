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

#include "vkwindow.h"

// Qt includes

#include <QFileInfo>
#include <QCheckBox>
#include <QGroupBox>
#include <QCloseEvent>
#include <QButtonGroup>
#include <QLabel>
#include <QSpinBox>
#include <QRadioButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QPushButton>
#include <QLineEdit>
#include <QMessageBox>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

// libkvkontakte includes

#include "vkontakte_uploadphotosjob.h"
#include "vkontakte_vkapi.h"

// Local includes

#include "digikam_version.h"
#include "dprogresswdg.h"
#include "vkalbumchooser.h"
#include "vkauthwidget.h"

namespace DigikamGenericVKontaktePlugin
{

class Q_DECL_HIDDEN VKWindow::Private
{
public:

    explicit Private()
      : import        (false),
        mainWidget    (nullptr),
        settingsBox   (nullptr),
        headerLabel   (nullptr),
        accountBox    (nullptr),
        albumsBox     (nullptr),
        imgList       (nullptr),
        uploadWidget  (nullptr),
        iface         (nullptr),
        progressBar   (nullptr),
        vkapi         (nullptr),
        albumToSelect (-1)
    {
    }

    bool              import;

    /// User interface
    QWidget*          mainWidget;
    QWidget*          settingsBox;
    QLabel*           headerLabel;

    /// accounts
    VKAuthWidget*     accountBox;

    // album selection
    VKAlbumChooser*   albumsBox;

    DItemsList*       imgList;
    QWidget*          uploadWidget;
    DInfoInterface*   iface;
    DProgressWdg*     progressBar;

    /// Pointers to running jobs
    QList<KJob*>      jobs;

    Vkontakte::VkApi* vkapi;

    int               albumToSelect;

    QString           appId;
};

VKWindow::VKWindow(DInfoInterface* const iface,
                   QWidget* const parent,
                   bool import)
    : WSToolDialog(parent, QLatin1String("VKontakte Dialog")),
      d           (new Private)
{
    d->iface = iface;
    d->vkapi = new Vkontakte::VkApi(this);

    // read settings from file

    readSettings();

    connect(this, SIGNAL(finished(int)),
            this, SLOT(slotFinished()));

    d->import                     = import;
    d->mainWidget                 = new QWidget(this);
    QHBoxLayout* const mainLayout = new QHBoxLayout(d->mainWidget);
    d->imgList                    = new DItemsList(this);
    d->imgList->setObjectName(QLatin1String("WebService ImagesList"));
    d->imgList->setControlButtonsPlacement(DItemsList::ControlButtonsBelow);
    d->imgList->setAllowRAW(false); // TODO: implement conversion
    d->imgList->setIface(d->iface);
    d->imgList->loadImagesFromCurrentSelection();
    d->imgList->listView()->setWhatsThis(i18n("This is the list of images "
                                              "to upload to your VKontakte album."));

    d->settingsBox                       = new QWidget(this);
    QVBoxLayout* const settingsBoxLayout = new QVBoxLayout(d->settingsBox);

    d->headerLabel                       = new QLabel(d->settingsBox);
    d->headerLabel->setWhatsThis(i18n("This is a clickable link to open the "
                                      "VKontakte service in a web browser."));
    d->headerLabel->setOpenExternalLinks(true);
    d->headerLabel->setFocusPolicy(Qt::NoFocus);

    // Account box

    initAccountBox();

    // Album box

    d->albumsBox = new VKAlbumChooser(d->settingsBox, d->vkapi);
    d->albumsBox->selectAlbum(d->albumToSelect);

    // ------------------------------------------------------------------------

    QGroupBox* const uploadBox         = new QGroupBox(i18n("Destination"), d->settingsBox);
    uploadBox->setWhatsThis(i18n("This is the location where VKontakte images "
                                 "will be downloaded."));
    QVBoxLayout* const uploadBoxLayout = new QVBoxLayout(uploadBox);
    d->uploadWidget                    = d->iface->uploadWidget(uploadBox);
    uploadBoxLayout->addWidget(d->uploadWidget);

    // ------------------------------------------------------------------------

#if 0
    QGroupBox* const optionsBox = new QGroupBox(i18n("Options"), d->settingsBox);
    optionsBox->setWhatsThis(i18n("These are options that will be applied to images before upload."));
#endif
/*
    store state in rc file

    d->checkKeepOriginal = new QCheckBox(i18n("Save in high resolution"), d->settingsBox);

    QVBoxLayout* const optionsBoxLayout = new QVBoxLayout(optionsBox);
    optionsBoxLayout->addWidget(d->checkKeepOriginal);
*/
    d->progressBar = new DProgressWdg(d->settingsBox);
    d->progressBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    d->progressBar->hide();

    // Layouts

    settingsBoxLayout->addWidget(d->headerLabel);
    settingsBoxLayout->addWidget(d->accountBox);
    settingsBoxLayout->addWidget(d->albumsBox);
    settingsBoxLayout->addWidget(uploadBox);
/*
     settingsBoxLayout->addWidget(optionsBox);
*/
    settingsBoxLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    settingsBoxLayout->addWidget(d->progressBar);

    mainLayout->addWidget(d->imgList);
    mainLayout->addWidget(d->settingsBox);

    setMainWidget(d->mainWidget);
    setModal(false);

    if (!d->import)
    {
        setWindowTitle(i18nc("@title:window", "Export to VKontakte Web Service"));

        startButton()->setText(i18n("Start Upload"));
        startButton()->setToolTip(i18n("Start upload to VKontakte service"));

        setMinimumSize(700, 520);
        uploadBox->hide();
    }
    else
    {
        // TODO: import support

        d->imgList->hide();
/*
         optionsBox->hide();
*/
    }

    // UI slots

    connect(startButton(), SIGNAL(clicked(bool)),
            this, SLOT(slotStartTransfer()));

    // for startReactivation()

    connect(d->vkapi, SIGNAL(authenticated()),
            this, SLOT(show()));

    // Dialog update slots

    connect(this, SIGNAL(signalUpdateBusyStatus(bool)),
            this, SLOT(slotUpdateBusyStatus(bool)));

    // TBD: busy status handling needs improvement

    connect(d->vkapi, SIGNAL(authenticated()),
            this, SLOT(slotUpdateBusyStatusReady()));

    slotUpdateBusyStatus(true);
    startReactivation();
}

VKWindow::~VKWindow()
{
    reset();
    delete d;
}

//---------------------------------------------------------------------------

void VKWindow::initAccountBox()
{
    d->accountBox = new VKAuthWidget(d->settingsBox, d->vkapi);

    connect(d->vkapi, SIGNAL(authenticated()),
            this, SLOT(slotAuthenticated()));

    connect(d->accountBox, SIGNAL(signalAuthCleared()),
            this, SLOT(slotAuthCleared()));

    connect(d->accountBox, SIGNAL(signalUpdateAuthInfo()),
            this, SLOT(slotUpdateHeaderLabel()));
}

void VKWindow::startReactivation()
{
    d->imgList->loadImagesFromCurrentSelection();

    reset();

     // show() will be called after that

    d->accountBox->slotStartAuthentication(false);
}

void VKWindow::reset()
{
    emit signalUpdateBusyStatus(false);
}

void VKWindow::slotUpdateBusyStatus(bool busy)
{
    if (d->albumsBox)
    {
        d->albumsBox->setEnabled(!busy && d->vkapi->isAuthenticated());
    }

    if (!busy)
    {
        setCursor(Qt::ArrowCursor);
        startButton()->setEnabled(d->vkapi->isAuthenticated());
        setRejectButtonMode(QDialogButtonBox::Close);
    }
    else
    {
        setCursor(Qt::WaitCursor);
        startButton()->setEnabled(false);
        setRejectButtonMode(QDialogButtonBox::Cancel);
    }
}

void VKWindow::slotUpdateBusyStatusReady()
{
    slotUpdateBusyStatus(false);
}

//---------------------------------------------------------------------------

void VKWindow::readSettings()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup grp        = config->group("VKontakte Settings");

    d->appId                = grp.readEntry("VkAppId", "2446321");
    d->albumToSelect        = grp.readEntry("SelectedAlbumId", -1);
    d->vkapi->setAppId(d->appId);
    d->vkapi->setRequiredPermissions(Vkontakte::AppPermissions::Photos);
    d->vkapi->setInitialAccessToken(grp.readEntry("AccessToken", ""));
}

void VKWindow::writeSettings()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup grp        = config->group("VKontakte Settings");

    grp.writeEntry("VkAppId", d->appId);

    if (!d->vkapi->accessToken().isEmpty())
    {
        grp.writeEntry("AccessToken", d->vkapi->accessToken());
    }

    int aid = 0;

    if (!d->albumsBox->getCurrentAlbumId(aid))
    {
        grp.deleteEntry("SelectedAlbumId");
    }
    else
    {
        grp.writeEntry("SelectedAlbumId", aid);
    }
}

//---------------------------------------------------------------------------

void VKWindow::closeEvent(QCloseEvent* event)
{
    if (!event)
    {
        return;
    }

    slotFinished();
    event->accept();
}

void VKWindow::slotFinished()
{
    writeSettings();
    reset();
}

void VKWindow::slotAuthenticated()
{
    if (d->albumsBox)
    {
        d->albumsBox->setEnabled(true);
    }
}

void VKWindow::slotAuthCleared()
{
    if (d->albumsBox)
    {
        d->albumsBox->setEnabled(false);
        d->albumsBox->clearList();
    }
}

void VKWindow::slotUpdateHeaderLabel()
{
    d->headerLabel->setText(QString::fromLatin1("<b><h2><a href=\"%1\">"
                                                "<font color=\"black\">%2</font></a></h2></b>")
                           .arg(d->accountBox->albumsURL()).arg(i18n("VKontakte")));
}

//---------------------------------------------------------------------------

void VKWindow::handleVkError(KJob* kjob)
{
    if (kjob)
    {
        QMessageBox::critical(this, i18nc("@title:window", "Request to VKontakte failed"),
                              kjob->errorText());
    }
}

//---------------------------------------------------------------------------

void VKWindow::slotStartTransfer()
{
    int aid = 0;

    if (!d->albumsBox->getCurrentAlbumId(aid))
    {
        // TODO: offer the user to create an album if there are no albums yet

        QMessageBox::information(this, QString(), i18n("Please select album first."));
        return;
    }

    // TODO: import support

    if (!d->import)
    {
        emit signalUpdateBusyStatus(true);

        QStringList files;

        foreach (const QUrl& url, d->imgList->imageUrls(true))
        {
            files.append(url.toLocalFile());
        }

        Vkontakte::UploadPhotosJob* const job
            = new Vkontakte::UploadPhotosJob(d->vkapi->accessToken(),
                                             files,
                                             false /*d->checkKeepOriginal->isChecked()*/,
                                             aid);

        connect(job, SIGNAL(result(KJob*)),
                this, SLOT(slotPhotoUploadDone(KJob*)));

        connect(job, SIGNAL(progress(int)),
                d->progressBar, SLOT(setValue(int)));

        d->jobs.append(job);
        job->start();
    }

    d->progressBar->show();
    d->progressBar->progressScheduled(i18n("Vkontakte Export"), false, true);
    d->progressBar->progressThumbnailChanged(
        QIcon::fromTheme(QLatin1String("preferences-web-browser-shortcuts")).pixmap(22, 22));
}

void VKWindow::slotPhotoUploadDone(KJob* kjob)
{
    Vkontakte::UploadPhotosJob* const job = dynamic_cast<Vkontakte::UploadPhotosJob*>(kjob);

    if (!job)
    {
        return;
    }

    d->jobs.removeAll(job);

    if (job->error())
    {
        handleVkError(job);
    }

    d->progressBar->hide();
    d->progressBar->progressCompleted();

    emit signalUpdateBusyStatus(false);
}

} // namespace DigikamGenericVKontaktePlugin
