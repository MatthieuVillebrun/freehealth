/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Authors:                                                               *
 *       Eric MAEKER <eric.maeker@gmail.com>                               *
 *                                                                         *
 ***************************************************************************/
#pragma once

#include <coreplugin/modemanager/imode.h>
#include <coreplugin/icorelistener.h>
#include <pmhplugin/pmhcontextualwidget.h>

#include <QObject>
#include <QHash>
#include <QString>
#include <QStyledItemDelegate>

QT_BEGIN_NAMESPACE
class QModelIndex;
class QAbstractButton;
class QPushButton;
class QToolBar;
class QSortFilterProxyModel;
QT_END_NAMESPACE

/**
 * \file pmhmode.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.2
 * \date 05 May 2012
*/
namespace Form {
class EpisodeModel;
}
namespace PMH {
namespace Internal {
class PmhModeWidget;
namespace Ui {
class PmhModeWidget;
}

class PmhModeWidgetCoreListener : public Core::ICoreListener
{
    Q_OBJECT
public:
    PmhModeWidgetCoreListener(PmhModeWidget *parent);
    ~PmhModeWidgetCoreListener();
    bool coreAboutToClose();
    QString errorMessage() const {return _errorMsg;}

private:
    PmhModeWidget *_pmhModeWidget;
    QString _errorMsg;
};

class PmhModeWidget : public PmhContextualWidget
{
    Q_OBJECT
    friend class PMH::Internal::PmhModeWidgetCoreListener;

public:
    PmhModeWidget(QWidget *parent = 0);
    ~PmhModeWidget();

    int currentSelectedCategory() const;
    bool enableAction(WidgetAction action) const;
    QString currentFormLabel() const;

public Q_SLOTS:
    bool clear();

protected Q_SLOTS:
    bool createEpisode();
    bool validateCurrentEpisode();
    bool renewEpisode();
    bool saveCurrentEpisode();
    bool removeCurrentEpisode();
    bool takeScreenshotOfCurrentEpisode();
    bool printFormOrEpisode();

protected:
    bool isDirty() const;

private Q_SLOTS:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void onButtonClicked(QAbstractButton *button);
    void createCategory();
    void removeItem();
    void onCurrentPatientChanged();
    void createPmh();
    void pmhModelRowsInserted(const QModelIndex &parent, int start, int end);
    void episodeChanged(const QModelIndex &current, const QModelIndex &previous);
    void refreshPmhSynthesis();

private:
    bool saveCurrentEditingEpisode();
    void createEpisodeToolbar();
    void checkCurrentEpisodeViewVisibility();
    void selectAndActivateFirstEpisode();
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    void changeEvent(QEvent *e);
//    bool eventFilter(QObject *o, QEvent *e);

private:
    Ui::PmhModeWidget *ui;
    QToolBar *m_ToolBar;
    QPushButton *m_EditButton;
    Form::EpisodeModel *m_currentEpisodeModel;
    QSortFilterProxyModel *m_proxyModel;
    QToolBar *m_episodeToolBar;
    QModelIndex m_currentFormIndex;
};

class PmhMode : public Core::IMode
{
    Q_OBJECT
public:
    explicit PmhMode(QObject *parent = 0);
    ~PmhMode();

    QString name() const;

private Q_SLOTS:
    void onCurrentPatientChanged();

private:
    QWidget *m_Widget;
    bool m_inPluginManager;
};

}  // End namespace Internal
}  // End namespace PMH

