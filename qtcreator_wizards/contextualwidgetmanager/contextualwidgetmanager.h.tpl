/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers:                                                       *
 *       %Author% <%AuthorEmail%>                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef %PluginName:u%_INTERNAL_%PluginName:u%CONTEXTUALWIDGETMANAGER_H
#define %PluginName:u%_INTERNAL_%PluginName:u%CONTEXTUALWIDGETMANAGER_H

#include <coreplugin/contextmanager/icontext.h>
#include <QObject>
#include <QPointer>

/**
 * \file %PluginName:l%contextualwidget.h
 * \author %Author%
 * \version 0.8.0
 * \date
*/

namespace Core {
class IContext;
}

namespace %PluginName% {
namespace Internal {
class %PluginName%ActionHandler;
class %PluginName%ContextualWidget;

class %PluginName%ActionHandler : public QObject
{
    Q_OBJECT
public:
    %PluginName%ActionHandler(QObject *parent = 0);
    virtual ~%PluginName%ActionHandler() {}

    void setCurrentView(%PluginName%ContextualWidget *view);

private Q_SLOTS:
    void clear();
    void showDatabaseInformation();

private Q_SLOTS:
    void updateActions();

protected:
    QAction *aClear;
    QAction *aShowDatabaseInformation;

    QPointer<%PluginName%ContextualWidget> m_CurrentView;
};

class %PluginName%ContextualWidgetManager : public %PluginName%ActionHandler
{
    Q_OBJECT

public:
    explicit %PluginName%ContextualWidgetManager(QObject *parent = 0);
    ~%PluginName%ContextualWidgetManager();

    %PluginName%ContextualWidget *currentView() const;

private Q_SLOTS:
    void updateContext(Core::IContext *object, const Core::Context &additionalContexts);
};

} // namespace Internal
} // namespace %PluginName%

#endif // %PluginName:u%_INTERNAL_%PluginName:u%CONTEXTUALWIDGETMANAGER_H
