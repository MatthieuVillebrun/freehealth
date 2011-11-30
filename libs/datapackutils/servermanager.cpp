/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developpers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
//#include "servermanager_p.h"
#include "servermanager.h"

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <QNetworkAccessManager>
#include <QNetworkConfigurationManager>
#include <QNetworkRequest>
#include <QDir>

#include <QDebug>

using namespace DataPack;
using namespace Trans::ConstantTranslations;

namespace {

const char * const TAG_ROOT = "ServerManager";
const char * const TAG_SERVER = "Server";


}  // End namespace Anonymous


ServerManager::ServerManager(QObject *parent) :
    IServerManager(parent),
    m_NetworkAccessManager(new QNetworkAccessManager(this))
{
    setObjectName("ServerManager");
//    if (!QDir(filesCachePath).exists()) {
//        LOG_ERROR(tkTr(Trans::Constants::PATH_1_DOESNOT_EXISTS).arg(filesCachePath));
//    }
//    m_d->filesCachePath = QDir::cleanPath(filesCachePath);
}

ServerManager::~ServerManager()
{
    delete m_NetworkAccessManager;
    // Servers are delete by the QObject inheritance
}

bool ServerManager::setGlobalConfiguration(const QString &xmlContent, QString *errorMsg = 0)
{
}

QString ServerManager::xmlConfiguration() const
{}

void ServerManager::connectServer(const Server &server, const ServerIdentification &ident)
{
    // TODO
    Q_UNUSED(server);
    Q_UNUSED(ident);
    if (server.isLocalPath()) {
//        server.setConnected(true);
        return;
    }
}

ServerDescription ServerManager::downloadServerDescription(const Server &server)
{
    // TODO
    if (server.isLocalPath()) {

    }
    ServerDescription desc;
    return desc;
}

QList<PackDescription> ServerManager::downloadPackDescription(const Server &server, const Pack &pack)
{
    // TODO
    Q_UNUSED(server);
    Q_UNUSED(pack);
    QList<PackDescription> list;
    return list;
}

Pack ServerManager::downloadAndUnzipPack(const Server &server, const Pack &pack)
{
    // TODO
    Q_UNUSED(server);
    Q_UNUSED(pack);
    return pack;
}

bool ServerManager::installDataPack(const Server &server, const Pack &pack)
{
    // TODO
    Q_UNUSED(server);
    Q_UNUSED(pack);
    return false;
}

QString ServerManager::cachePath() const
{
    return filesCachePath;
}

bool ServerManager::addServer(const QString &url)
{
    // check if a server already exists with the same URL
    foreach (Server *child, m_Servers)
        if (child->url() == url)
            return false;

    Server *server = new Server(url, this);
    m_Servers.append(server);
    return true;
}

Server *ServerManager::getServerAt(int index) const
{
    if (index < m_Servers.count() && index >= 0)
        return m_Servers.at(index);
    return 0;
}

int ServerManager::getServerIndex(const QString &url) const
{
    for (int i = 0; i < m_Servers.count(); i++)
        if (m_Servers.at(i)->url() == url)
            return i;
    return -1;
}

void ServerManager::removeServerAt(int index)
{
    delete m_Servers.at(index);
    m_Servers.remove(index);
}

void ServerManager::connectAndUpdate(int index)
{
    if (index < m_Servers.count() && index >= 0)
        m_Servers.at(index)->connectAndUpdate();
}
