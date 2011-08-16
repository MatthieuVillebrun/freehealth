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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef CORE_APPCONFIGWIZARD_H
#define CORE_APPCONFIGWIZARD_H

#include <QObject>
#include <QWidget>
#include <QWizardPage>
#include <QWizard>

QT_BEGIN_NAMESPACE
class QLabel;
class QComboBox;
QT_END_NAMESPACE

/**
 * \file appconfigwizard.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 11 May 2011
*/

namespace Utils {
    class LanguageComboBox;
}

namespace Core {
class ServerPreferencesWidget;

class AppConfigWizard : public QWizard
{
    Q_OBJECT
public:
    AppConfigWizard(QWidget *parent = 0);

protected Q_SLOTS:
    void done(int r);

protected:
    void resizeEvent(QResizeEvent *event);
};


class CoreConfigPage: public QWizardPage
{
    Q_OBJECT
public:
    CoreConfigPage(QWidget *parent = 0);

    bool validatePage();
    int nextId() const;

private:
    void changeEvent(QEvent *e);
    void retranslate();

private:
    QLabel *langLabel, *typeLabel;
    Utils::LanguageComboBox *combo;
    QComboBox *installCombo;
};

class ServerConfigPage: public QWizardPage
{
    Q_OBJECT
public:
    ServerConfigPage(QWidget *parent = 0);

    bool validatePage();
    int nextId() const;

private:
    void changeEvent(QEvent *e);
    void retranslate();

private:
    Core::ServerPreferencesWidget *serverWidget;
};

class ClientConfigPage: public QWizardPage
{
    Q_OBJECT
public:
    ClientConfigPage(QWidget *parent = 0);

    bool validatePage();
    int nextId() const;

private:
    void changeEvent(QEvent *e);
    void retranslate();

private:
    Core::ServerPreferencesWidget *serverWidget;
};

class EndConfigPage: public QWizardPage
{
    Q_OBJECT
public:
    EndConfigPage(QWidget *parent = 0);

public:
    void initializePage();

private Q_SLOTS:
    void comboDbActivated(int);
    void comboVirtualActivated(int);

    void showEvent(QShowEvent *event);
};


}  // End namespace Core


#endif // CORE_APPCONFIGWIZARD_H
