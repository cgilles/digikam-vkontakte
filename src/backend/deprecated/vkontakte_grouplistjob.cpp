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

#include "vkontakte_grouplistjob.h"

// Qt includes

#include <qjson/qobjecthelper.h>

namespace Vkontakte
{

class Q_DECL_HIDDEN GroupListJob::Private
{
public:

    QList<GroupInfoPtr> list;
    bool                extended;
};

GroupListJob::GroupListJob(const QString& accessToken, int uid, bool extended)
    : VkontakteJob(accessToken, "groups.get"),
      d(new Private)
{
    d->extended = extended;

    if (uid != -1)
    {
        addQueryItem("uid", QString::number(uid));
    }

    if (extended)
    {
        addQueryItem("extended", "1");
    }
}

GroupListJob::~GroupListJob()
{
    delete d;
}

QList<GroupInfoPtr> GroupListJob::list() const
{
    return d->list;
}

GroupInfoPtr GroupListJob::handleSingleData(const QVariant& data)
{
     GroupInfoPtr info = GroupInfoPtr(new GroupInfo());
     QJson::QObjectHelper::qvariant2qobject(data.toMap(), info.data());

     return info;
}

void GroupListJob::handleData(const QVariant& data)
{
    if (d->extended)
    {
        QVariantList dataList = data.toList();
        dataList.pop_front(); // total count (unused)

        foreach (const QVariant& item, dataList)
        {
            d->list.append(handleSingleData(item));
        }
    }
    else
    {
        // TODO: test with both extended={true, false}

        foreach (const QVariant& item, data.toList())
        {
            GroupInfoPtr group = GroupInfoPtr(new GroupInfo());
            group->setGid(item.toInt());
            d->list.append(group);
        }
    }
}

} // namespace Vkontakte
