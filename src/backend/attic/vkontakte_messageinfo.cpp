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

#include "vkontakte_messageinfo.h"
#include "vkontakte_util.h"

// Qt includes

#include <QRegularExpression>

namespace Vkontakte
{

class Q_DECL_HIDDEN MessageInfo::Private
{
public:

    QString date;
    int     uid;
    int     mid;
    QString title;
    QString body;
    int     readState;
    int     out;
    QString chatId;
    QString chatActive;
};

MessageInfo::MessageInfo()
    : d(new Private)
{
    d->uid       = -1;
    d->mid       = -1;
    d->readState = 0;
    d->out       = 0;
}

MessageInfo::~MessageInfo()
{
    delete d;
}

void MessageInfo::setDateString(const QString& createdDate)
{
    d->date = createdDate;
}

QString MessageInfo::dateString() const
{
    return d->date;
}

QDateTime MessageInfo::date() const
{
    return unixTimeToQDateTime(d->date);
}

void MessageInfo::setUid(int uid)
{
    d->uid = uid;
}

int MessageInfo::uid() const
{
    return d->uid;
}

void MessageInfo::setMid(int mid)
{
    d->mid = mid;
}

int MessageInfo::mid() const
{
    return d->mid;
}

void MessageInfo::setTitle(const QString& title)
{
    // vk.com puts "..." into the title when the subject is not specified
    QRegularExpression rx(QRegularExpression::anchoredPattern("(Re(\\(\\d+\\))?: )?( ?)\\.\\.\\.( ?)"));
    if (!rx.match(title).hasMatch())
    {
        d->title = title;
    }
}

QString MessageInfo::title() const
{
    return d->title;
}

QString MessageInfo::coreTitle() const
{
    QString res = d->title;
    res.remove(QRegularExpression("^Re(\\(\\d+\\))?: "));

    return res;
}

void MessageInfo::setBody(const QString& body)
{
    d->body = body;
    d->body.remove('\n');
    d->body.replace("<br>", "\n");
    d->body.replace("&lt;", "<");
    d->body.replace("&gt;", ">");
    d->body.replace("&quot;", "\"");
    d->body.replace("&amp;", "&");
}

QString MessageInfo::body() const
{
    return d->body;
}

void MessageInfo::setReadState(int readState)
{
    d->readState = readState;
}

int MessageInfo::readState() const
{
    return d->readState;
}

void MessageInfo::setOut(int out)
{
    d->out = out;
}

int MessageInfo::out() const
{
    return d->out;
}

void MessageInfo::setChatId(const QString& chatId)
{
    d->chatId = chatId;
}

QString MessageInfo::chatId() const
{
    return d->chatId;
}

void MessageInfo::setChatActive(const QString& chatActive)
{
    d->chatActive = chatActive;
}

QString MessageInfo::chatActive() const
{
    return d->chatActive;
}

QString MessageInfo::remoteId() const
{
    return QString::fromUtf8("priv_mid%1").arg(mid(), 6, 10, QLatin1Char('0'));
}

MessageInfoPtr::MessageInfoPtr(MessageInfo* const ptr)
    : QSharedPointer<MessageInfo>(ptr)
{
}

bool MessageInfoPtr::operator<(const MessageInfoPtr& o) const
{
    return ((*this)->mid() < o->mid());
}

} // namespace Vkontakte
