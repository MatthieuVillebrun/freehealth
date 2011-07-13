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
#include "defautuserviewerpages.h"
#include "usermodel.h"
#include "userpassworddialog.h"

#include <printerplugin/printer.h>
#include <printerplugin/textdocumentextra.h>

#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <QDataWidgetMapper>

#include "ui_userviewer_identity.h"
#include "ui_userviewer_contact.h"
#include "ui_userviewer_medicalrights.h"
#include "ui_userviewer_papers.h"
#include "ui_userviewer_professional.h"

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace {
}  // End anonymous namespace

DefaultUserIdentityWidget::DefaultUserIdentityWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserViewer_IdentityUI),
    m_Mapper(0),
    m_Model(0)
{
    ui->setupUi(this);
    ui->languageCombo->setDisplayMode(Views::LanguageComboBox::AvailableTranslations);
    ui->titleCombo->addItems(titles());
    ui->genderCombo->addItems(genders());
}

DefaultUserIdentityWidget::~DefaultUserIdentityWidget()
{
    delete ui;
}

void DefaultUserIdentityWidget::setUserModel(UserModel *model)
{
    // prepare the mapper
    if (!m_Mapper) {
        m_Mapper = new QDataWidgetMapper(this);
    }
    m_Mapper->setModel(model);
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_Mapper->addMapping(ui->uuidLineEdit, Core::IUser::Uuid);
    m_Mapper->addMapping(ui->titleCombo, Core::IUser::TitleIndex, "currentIndex");
    m_Mapper->addMapping(ui->genderCombo, Core::IUser::GenderIndex, "currentIndex");
    m_Mapper->addMapping(ui->nameLineEdit, Core::IUser::Name);
    m_Mapper->addMapping(ui->loginLineEdit, Core::IUser::ClearLogin);
    m_Mapper->addMapping(ui->secNameLineEdit, Core::IUser::SecondName);
    m_Mapper->addMapping(ui->firstnameLineEdit, Core::IUser::Firstname);
    m_Mapper->addMapping(ui->lastLoginDateTimeEdit, Core::IUser::LastLogin);
    m_Mapper->addMapping(ui->languageCombo, Core::IUser::LocaleCodedLanguage, "currentLanguage");
}

void DefaultUserIdentityWidget::setUserIndex(const int index)
{
    if (m_Mapper)
        m_Mapper->setCurrentIndex(index);
}

void DefaultUserIdentityWidget::clear()
{
//    m_Mapper->model()->revert();
}

bool DefaultUserIdentityWidget::submit()
{
    return m_Mapper->submit();
}

void DefaultUserIdentityWidget::on_but_changePassword_clicked()
{
    int row = m_Mapper->currentIndex();
    UserPasswordDialog d(m_Model->index(row, Core::IUser::Password).data().toString(), this);
    if (d.exec() == QDialog::Accepted){
        if (!d.canGetNewPassword())
            return;
        QModelIndex idx = m_Model->index(row, Core::IUser::Password);
        m_Model->setData(idx, d.cryptedPassword());
    }
}

void DefaultUserIdentityWidget::on_but_viewHistory_clicked()
{
    int row = m_Mapper->currentIndex();
    Utils::informativeMessageBox(tr("Login history."),
                                     tr("User %1\nLast connection : %2")
                           .arg(m_Model->index(row,Core::IUser::Name).data().toString())
                           .arg(m_Model->index(row,Core::IUser::LastLogin).data().toDateTime().toString()),
                            m_Model->index(row,Core::IUser::LoginHistory).data().toString(),
                            qApp->applicationName());
}


DefaultUserIdentityPage::DefaultUserIdentityPage(QObject *parent) :
    IUserViewerPage(parent),
    m_Model(0)
{
    setObjectName("DefaultUserIdentityPage");
}

DefaultUserIdentityPage::~DefaultUserIdentityPage()
{}

QString DefaultUserIdentityPage::id() const
{
    return objectName();
}

QString DefaultUserIdentityPage::name() const
{
    return tkTr(Trans::Constants::IDENTITY_TEXT);
}

QString DefaultUserIdentityPage::category() const
{
    return tkTr(Trans::Constants::IDENTITY_TEXT);
}

QString DefaultUserIdentityPage::title() const
{
    return tr("User identity");
}

QWidget *DefaultUserIdentityPage::createPage(QWidget *parent)
{
    if (!m_Widget)
        m_Widget = new DefaultUserIdentityWidget(parent);
    return m_Widget;
}

void DefaultUserIdentityPage::setUserModel(UserModel *model)
{
    m_Model = model;
    if (m_Widget)
        m_Widget->setUserModel(model);
}

void DefaultUserIdentityPage::setUserIndex(const int index)
{
    if (m_Model && m_Widget)
        m_Widget->setUserIndex(index);
}

bool DefaultUserIdentityPage::clear()
{
    if (m_Widget)
        m_Widget->clear();
    return true;
}

bool DefaultUserIdentityPage::submit()
{
    if (m_Widget)
        return m_Widget->submit();
    return false;
}


//////////////////////////////////////////////////////////////////////////////////////////
DefaultUserContactWidget::DefaultUserContactWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserViewer_ContactUI),
    m_Mapper(0),
    m_Model(0)
{
    ui->setupUi(this);
}

DefaultUserContactWidget::~DefaultUserContactWidget()
{
    delete ui;
}

void DefaultUserContactWidget::setUserModel(UserModel *model)
{
    // prepare the mapper
    if (!m_Mapper) {
        m_Mapper = new QDataWidgetMapper(this);
    }
    m_Mapper->setModel(model);
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_Mapper->addMapping(ui->adressTextEdit, Core::IUser::Adress, "plainText");
    m_Mapper->addMapping(ui->countryLineEdit, Core::IUser::Country);
    m_Mapper->addMapping(ui->zipcodeLineEdit, Core::IUser::Zipcode);
    m_Mapper->addMapping(ui->cityLineEdit, Core::IUser::City);
    m_Mapper->addMapping(ui->tel1LineEdit, Core::IUser::Tel1);
    m_Mapper->addMapping(ui->tel2LineEdit, Core::IUser::Tel2);
    m_Mapper->addMapping(ui->tel3LineEdit, Core::IUser::Tel3);
    m_Mapper->addMapping(ui->faxLineEdit, Core::IUser::Fax);
    m_Mapper->addMapping(ui->mailLineEdit, Core::IUser::Mail);
}

void DefaultUserContactWidget::setUserIndex(const int index)
{
    if (m_Mapper)
        m_Mapper->setCurrentIndex(index);
}

void DefaultUserContactWidget::clear()
{
//    m_Mapper->model()->revert();
}

bool DefaultUserContactWidget::submit()
{
    return m_Mapper->submit();
}


DefaultUserContactPage::DefaultUserContactPage(QObject *parent) :
    IUserViewerPage(parent),
    m_Model(0)
{
    setObjectName("DefaultUserContactPage");
}

DefaultUserContactPage::~DefaultUserContactPage()
{}

QString DefaultUserContactPage::id() const
{
    return objectName();
}

QString DefaultUserContactPage::name() const
{
    return tr("Contact");
}

QString DefaultUserContactPage::category() const
{
    return tkTr(Trans::Constants::IDENTITY_TEXT);
}

QString DefaultUserContactPage::title() const
{
    return tr("Contact");
}

QWidget *DefaultUserContactPage::createPage(QWidget *parent)
{
    if (!m_Widget)
        m_Widget = new DefaultUserContactWidget(parent);
    return m_Widget;
}

void DefaultUserContactPage::setUserModel(UserModel *model)
{
    m_Model = model;
    if (m_Widget)
        m_Widget->setUserModel(model);
}

void DefaultUserContactPage::setUserIndex(const int index)
{
    if (m_Model && m_Widget)
        m_Widget->setUserIndex(index);
}

bool DefaultUserContactPage::clear()
{
    if (m_Widget)
        m_Widget->clear();
    return true;
}

bool DefaultUserContactPage::submit()
{
    if (m_Widget)
        return m_Widget->submit();
    return false;
}


//////////////////////////////////////////////////////////////////////////////////////////
DefaultUserProfessionalWidget::DefaultUserProfessionalWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserViewer_ProfessionalUI),
    m_Mapper(0),
    m_Model(0)
{
    ui->setupUi(this);
    QStringListModel *m1 = new QStringListModel(ui->specialtyListView);
    ui->specialtyListView->setModel(m1);
    QStringListModel *m2 = new QStringListModel(ui->qualificationsListView);
    ui->qualificationsListView->setModel(m2);
    QStringListModel *m3 = new QStringListModel(ui->practIdsListView);
    ui->practIdsListView->setModel(m3);
}

DefaultUserProfessionalWidget::~DefaultUserProfessionalWidget()
{
    delete ui;
}

void DefaultUserProfessionalWidget::setUserModel(UserModel *model)
{
    // prepare the mapper
    if (!m_Mapper) {
        m_Mapper = new QDataWidgetMapper(this);
    }
    m_Mapper->setModel(model);
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    m_Mapper->addMapping(ui->specialtyListView, Core::IUser::Specialities, "stringList");
    m_Mapper->addMapping(ui->qualificationsListView, Core::IUser::Qualifications, "stringList");
    m_Mapper->addMapping(ui->practIdsListView, Core::IUser::PractitionerId, "stringList");
}

void DefaultUserProfessionalWidget::setUserIndex(const int index)
{
    if (m_Mapper)
        m_Mapper->setCurrentIndex(index);
}

void DefaultUserProfessionalWidget::clear()
{
//    m_Mapper->model()->revert();
}

bool DefaultUserProfessionalWidget::submit()
{
    return m_Mapper->submit();
}



DefaultUserProfessionalPage::DefaultUserProfessionalPage(QObject *parent) :
    IUserViewerPage(parent),
    m_Model(0)
{
    setObjectName("DefaultUserProfessionalPage");
}

DefaultUserProfessionalPage::~DefaultUserProfessionalPage()
{}

QString DefaultUserProfessionalPage::id() const
{
    return objectName();
}

QString DefaultUserProfessionalPage::name() const
{
    return tr("Professional identifiants");
}

QString DefaultUserProfessionalPage::category() const
{
    return tkTr(Trans::Constants::IDENTITY_TEXT);
}

QString DefaultUserProfessionalPage::title() const
{
    return tr("Professional identifiants");
}

QWidget *DefaultUserProfessionalPage::createPage(QWidget *parent)
{
    if (!m_Widget)
        m_Widget = new DefaultUserProfessionalWidget(parent);
    return m_Widget;
}

void DefaultUserProfessionalPage::setUserModel(UserModel *model)
{
    m_Model = model;
    if (m_Widget)
        m_Widget->setUserModel(model);
}

void DefaultUserProfessionalPage::setUserIndex(const int index)
{
    if (m_Model && m_Widget)
        m_Widget->setUserIndex(index);
}

bool DefaultUserProfessionalPage::clear()
{
    if (m_Widget)
        m_Widget->clear();
    return true;
}

bool DefaultUserProfessionalPage::submit()
{
    if (m_Widget)
        return m_Widget->submit();
    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
DefaultUserRightsWidget::DefaultUserRightsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserViewer_RightsUI),
    m_Mapper(0),
    m_Model(0)
{
    ui->setupUi(this);
}

DefaultUserRightsWidget::~DefaultUserRightsWidget()
{
    delete ui;
}

void DefaultUserRightsWidget::setUserModel(UserModel *model)
{
    // prepare the mapper
    if (!m_Mapper) {
        m_Mapper = new QDataWidgetMapper(this);
    }
    m_Mapper->setModel(model);
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_Mapper->addMapping(ui->userManagerRightsListWidget, Core::IUser::ManagerRights, "rights");
    m_Mapper->addMapping(ui->drugsRightsListWidget, Core::IUser::DrugsRights, "rights");
    m_Mapper->addMapping(ui->medicalRightsListWidget, Core::IUser::MedicalRights, "rights");
    m_Mapper->addMapping(ui->paramedicalRightsWidget, Core::IUser::ParamedicalRights, "rights");
    m_Mapper->addMapping(ui->administrativeRightsWidget, Core::IUser::AdministrativeRights, "rights");
}

void DefaultUserRightsWidget::setUserIndex(const int index)
{
    if (m_Mapper)
        m_Mapper->setCurrentIndex(index);
}

void DefaultUserRightsWidget::clear()
{
//    m_Mapper->model()->revert();
}

bool DefaultUserRightsWidget::submit()
{
    return m_Mapper->submit();
}


DefaultUserRightsPage::DefaultUserRightsPage(QObject *parent) :
    IUserViewerPage(parent),
    m_Model(0)
{
    setObjectName("DefaultUserRightsPage");
}

DefaultUserRightsPage::~DefaultUserRightsPage()
{}

QString DefaultUserRightsPage::id() const
{
    return objectName();
}

QString DefaultUserRightsPage::name() const
{
    return tkTr(Trans::Constants::RIGHTS);
}

QString DefaultUserRightsPage::category() const
{
    return tkTr(Trans::Constants::RIGHTS);
}

QString DefaultUserRightsPage::title() const
{
    return tkTr(Trans::Constants::USER_RIGHTS);
}

QWidget *DefaultUserRightsPage::createPage(QWidget *parent)
{
    if (!m_Widget)
        m_Widget = new DefaultUserRightsWidget(parent);
    return m_Widget;
}

void DefaultUserRightsPage::setUserModel(UserModel *model)
{
    m_Model = model;
    if (m_Widget)
        m_Widget->setUserModel(model);
}

void DefaultUserRightsPage::setUserIndex(const int index)
{
    if (m_Model && m_Widget)
        m_Widget->setUserIndex(index);
}

bool DefaultUserRightsPage::clear()
{
    if (m_Widget)
        m_Widget->clear();
    return true;
}

bool DefaultUserRightsPage::submit()
{
    if (m_Widget)
        return m_Widget->submit();
    return false;
}


//////////////////////////////////////////////////////////////////////////////////////////
DefaultUserPapersWidget::DefaultUserPapersWidget(const int type, QWidget *parent) :
    QWidget(parent),
    m_Mapper(0),
    m_Model(0),
    m_type(type),
    m_row(-1)
{
    preview = Print::Printer::previewer(this);
}

DefaultUserPapersWidget::~DefaultUserPapersWidget()
{
}

void DefaultUserPapersWidget::setUserModel(UserModel *model)
{
    m_Model = model;
}

void DefaultUserPapersWidget::setUserIndex(const int index)
{
    if (m_Model) {
        m_row = index;
        switch (m_type) {
        case DefaultUserPapersPage::GenericPaper:
            preview->setHeader(m_Model->paper(m_row, Core::IUser::GenericHeader));
            preview->setFooter(m_Model->paper(m_row, Core::IUser::GenericFooter));
            preview->setWatermark(m_Model->paper(m_row, Core::IUser::GenericWatermark));
            break;

        case DefaultUserPapersPage::AdministrativePaper:
            preview->setHeader(m_Model->paper(m_row, Core::IUser::AdministrativeHeader));
            preview->setFooter(m_Model->paper(m_row, Core::IUser::AdministrativeFooter));
            preview->setWatermark(m_Model->paper(m_row, Core::IUser::AdministrativeWatermark));
            break;

        case DefaultUserPapersPage::PrescriptionPaper:
            preview->setHeader(m_Model->paper(m_row, Core::IUser::PrescriptionHeader));
            preview->setFooter(m_Model->paper(m_row, Core::IUser::PrescriptionFooter));
            preview->setWatermark(m_Model->paper(m_row, Core::IUser::PrescriptionWatermark));
            break;
        }
    }
}

void DefaultUserPapersWidget::clear()
{
//    m_Mapper->model()->revert();
}

bool DefaultUserPapersWidget::submit()
{
    if (m_Model) {
        Print::TextDocumentExtra *text = 0;
        QString uid = m_Model->index(m_row, Core::IUser::Uuid).data().toString();
        switch (m_type) {
        case DefaultUserPapersPage::GenericPaper:
            text = new Print::TextDocumentExtra();
            preview->headerToPointer(text);
            m_Model->setPaper(uid, Core::IUser::GenericHeader, text);
            text = new Print::TextDocumentExtra();
            preview->footerToPointer(text);
            m_Model->setPaper(uid, Core::IUser::GenericFooter, text);
            text = new Print::TextDocumentExtra();
            preview->watermarkToPointer(text);
            m_Model->setPaper(uid, Core::IUser::GenericWatermark, text);
            break;

        case DefaultUserPapersPage::AdministrativePaper:
            text = new Print::TextDocumentExtra();
            preview->headerToPointer(text);
            m_Model->setPaper(uid, Core::IUser::AdministrativeHeader, text);
            text = new Print::TextDocumentExtra();
            preview->footerToPointer(text);
            m_Model->setPaper(uid, Core::IUser::AdministrativeFooter, text);
            text = new Print::TextDocumentExtra();
            preview->watermarkToPointer(text);
            m_Model->setPaper(uid, Core::IUser::AdministrativeWatermark, text);
            break;

        case DefaultUserPapersPage::PrescriptionPaper:
            text = new Print::TextDocumentExtra();
            preview->headerToPointer(text);
            m_Model->setPaper(uid, Core::IUser::PrescriptionHeader, text);
            text = new Print::TextDocumentExtra();
            preview->footerToPointer(text);
            m_Model->setPaper(uid, Core::IUser::PrescriptionFooter, text);
            text = new Print::TextDocumentExtra();
            preview->watermarkToPointer(text);
            m_Model->setPaper(uid, Core::IUser::PrescriptionWatermark, text);
            break;
        }
    }
    return true;
}


DefaultUserPapersPage::DefaultUserPapersPage(const PaperType type, QObject *parent) :
    IUserViewerPage(parent),
    m_Model(0),
    m_type(type)
{
    setObjectName("DefaultUserPapersPage");
}

DefaultUserPapersPage::~DefaultUserPapersPage()
{}

QString DefaultUserPapersPage::id() const
{
    return objectName();
}

QString DefaultUserPapersPage::name() const
{
    switch (m_type) {
    case GenericPaper: return tr("Generic papers");
    case AdministrativePaper: return tr("Administrative papers");
    case PrescriptionPaper:return tr("Prescription papers");
    }
    return QString();
}

QString DefaultUserPapersPage::category() const
{
    return tr("Documents");
}

QString DefaultUserPapersPage::title() const
{
    return name();
}

QWidget *DefaultUserPapersPage::createPage(QWidget *parent)
{
    if (!m_Widget)
        m_Widget = new DefaultUserPapersWidget(m_type, parent);
    return m_Widget;
}

void DefaultUserPapersPage::setUserModel(UserModel *model)
{
    m_Model = model;
    if (m_Widget)
        m_Widget->setUserModel(model);
}

void DefaultUserPapersPage::setUserIndex(const int index)
{
    if (m_Model && m_Widget)
        m_Widget->setUserIndex(index);
}

bool DefaultUserPapersPage::clear()
{
    if (m_Widget)
        m_Widget->clear();
    return true;
}

bool DefaultUserPapersPage::submit()
{
    if (m_Widget)
        return m_Widget->submit();
    return false;
}


