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

#include "vkontakte_allmessageslistjob.h"

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "vkontakte_messageslistjob.h"
#include "digikam_debug.h"

namespace Vkontakte
{

class Q_DECL_HIDDEN AllMessagesListJob::Private
{
public:

    QString               accessToken;         ///< Vkontakte Access token
    int                   out;
    int                   previewLength;
    int                   filters;
    int                   timeOffset;

    int                   totalCount[2];
    QList<MessageInfoPtr> list;
};

AllMessagesListJob::AllMessagesListJob(const QString& accessToken,
                                       int out, int previewLength,
                                       int filters, int timeOffset)
    : KJobWithSubjobs(),
      d(new Private)
{
    d->accessToken   = accessToken;
    d->out           = out;
    d->previewLength = previewLength;
    d->filters       = filters;
    d->timeOffset    = timeOffset;
    d->totalCount[0] = -1; // for incoming messages
    d->totalCount[1] = -1; // for outgoing messages
}

AllMessagesListJob::~AllMessagesListJob()
{
    delete d;
}

void AllMessagesListJob::startNewJob(int offset, int count, int out)
{
    Q_ASSERT(out == 0 || out == 1);

    MessagesListJob* const job = new MessagesListJob(d->accessToken, out,
                                                     offset, count,
                                                     d->previewLength, d->filters, d->timeOffset);
    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(jobFinished(KJob*)));

    m_jobs.append(job);
    job->start();
}

void AllMessagesListJob::start()
{
    // out=-1 means to retrieve both incoming and outgoing messages

    if (d->out == 0 || d->out == -1) // incoming
    {
        startNewJob(0, 100, 0);
    }

    if (d->out == 1 || d->out == -1) // outgoing
    {
        startNewJob(0, 100, 1);
    }
}

void AllMessagesListJob::jobFinished(KJob *kjob)
{
    MessagesListJob* const job = dynamic_cast<MessagesListJob*>(kjob);
    Q_ASSERT(job);

    if (!job)
    {
        return;
    }

    m_jobs.removeAll(job);

    if (job->error())
    {
        setError(job->error());
        setErrorText(job->errorText());
        qCWarning(DIGIKAM_WEBSERVICES_LOG) << "Job error: " << job->errorString();

        return;
    }

    d->list.append(job->list());

    int out = job->out(); // incoming or outgoing
    Q_ASSERT(out == 0 || out == 1);

    // If this was the first job, start all others

    if      (d->totalCount[out] == -1)
    {
        d->totalCount[out] = job->totalCount();

        for (int offset = 100 ; offset < d->totalCount[out] ; offset += 100)
        {
            startNewJob(offset, qMin(100, d->totalCount[out] - offset), out);
        }
    }
    else if (d->totalCount[out] != job->totalCount())
    {
        // TODO: some new messages might have been added, what should we do then?
        doKill();
        setError(KJob::UserDefinedError + 2);

        if (out == 1)
        {
            setErrorText(i18n("The number of outgoing messages has changed between requests."));
        }
        else // out == 0
        {
            setErrorText(i18n("The number of incoming messages has changed between requests."));
        }

        qCWarning(DIGIKAM_WEBSERVICES_LOG) << "Job error: " << job->errorString();
        emitResult();

        return;
    }

    // All jobs have finished

    if (m_jobs.size() == 0)
    {
        std::sort(d->list.begin(), d->list.end()); // sort by message ID (which should be equivalent to sorting by date)
        emitResult();
    }
}

QList<MessageInfoPtr> AllMessagesListJob::list() const
{
    return d->list;
}

int AllMessagesListJob::count() const
{
    return d->list.size();
}

} // namespace Vkontakte
