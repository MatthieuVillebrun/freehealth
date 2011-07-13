/***************************************************************************
 * The FreeMedForms project is a set of free, open source medical         *
 * applications.                                                          *
 * (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 * All rights reserved.                                                   *
 *                                                                        *
 * This program is free software: you can redistribute it and/or modify   *
 * it under the terms of the GNU General Public License as published by   *
 * the Free Software Foundation, either version 3 of the License, or      *
 * (at your option) any later version.                                    *
 *                                                                        *
 * This program is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 * GNU General Public License for more details.                           *
 *                                                                        *
 * You should have received a copy of the GNU General Public License      *
 * along with this program (COPYING.FREEMEDFORMS file).                   *
 * If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                        *
 *      NAME <MAIL@Address>                                                *
 *      NAME <MAIL@Address>                                                *
 ***************************************************************************/
/**
  \class UserPlugin::UserWizard
  \brief Wizard for user creation.
  You can tell tkWiz to create the user itself or use a defined user. createUser() define the
  creation mode. If you set it to false, inform the row of the model to use with setModelRow(). By default,
  UserWizard create itself a new user.\n
  Usage :
  \code
    UserWizard wiz;
    wiz.createUser(true);   // will create the user (optionnal)
    wiz.show();
  \endcode

  \todo If row is defined --> populate all wizard pages with users values.
  \ingroup usertoolkit widget_usertoolkit usermanager
*/

#include "userwizard.h"

#include <usermanagerplugin/userdata.h>
#include <usermanagerplugin/database/userbase.h>

#include <texteditorplugin/texteditor.h>

#include <listviewplugin/stringlistview.h>
#include <listviewplugin/stringlistmodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/translators.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_icons.h>

#include <usermanagerplugin/usermodel.h>
#include <usermanagerplugin/widgets/userrightswidget.h>

#include <listviewplugin/languagecombobox.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/widgets/lineeditechoswitcher.h>

#include <translationutils/constanttranslations.h>

#include <QGridLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QCoreApplication>
#include <QEvent>
#include <QPushButton>
#include <QList>
#include <QCheckBox>
#include <QGroupBox>

#include "ui_userwizardcontactwidget.h"

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline UserPlugin::UserModel *userModel() { return UserModel::instance(); }
static inline UserPlugin::Internal::UserBase *userBase() { return UserPlugin::Internal::UserBase::instance(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline QString bundlePath()  { return settings()->path(Core::ISettings::BundleResourcesPath); }

static inline QString defaultPaper(const QString &profession, const QString &paper, const QString &paperType = QString::null)
{
    QString lang = QLocale().name().left(2);
    QString fileName;
    if (paperType.isEmpty()) {
        fileName = QString(bundlePath() + "/profiles/%1/default/user_%2_%3.xml").arg(profession).arg(paper).arg(lang);
    } else {
        fileName = QString(bundlePath() + "/profiles/%1/default/user_%2_%3_%4.xml").arg(profession).arg(paper).arg(paperType).arg(lang);
    }
    if (QFileInfo(fileName).exists()) {
        return Utils::readTextFile(fileName);
    }
    lang = Trans::Constants::ALL_LANGUAGE;
    if (paperType.isEmpty()) {
        fileName = QString(bundlePath() + "/profiles/%1/default/user_%2_%3.xml").arg(profession).arg(paper).arg(lang);
    } else {
        fileName = QString(bundlePath() + "/profiles/%1/default/user_%2_%3_%4.xml").arg(profession).arg(paper).arg(paperType).arg(lang);
    }
    if (QFileInfo(fileName).exists()) {
        return Utils::readTextFile(fileName);
    }
    if (!paperType.isEmpty()) {
        fileName = QString(bundlePath() + "/profiles/%1/default/user_%2_%3.xml").arg(profession).arg(paper).arg(lang);
        if (QFileInfo(fileName).exists()) {
            return Utils::readTextFile(fileName);
        }
    }
    return QString();
}

static inline QString defaultHeader(const QString &profession)
{
    return defaultPaper(profession, "header");
}

static inline QString defaultFooter(const QString &profession)
{
    return defaultPaper(profession, "footer");
}

// paperType == "prescription" "administrative" "generic"
static inline QString defaultWatermark(const QString &profession, const QString &paperType = QString::null)
{
    return defaultPaper(profession, "watermark", paperType);
}

QHash<int, QString> UserWizard::m_Papers;
QHash<int, int> UserWizard::m_Rights;

UserWizard::UserWizard(QWidget *parent) :
        QWizard(parent),
        m_User(new UserData),
        m_Row(-1),
        m_Saved(false),
        m_CreateUser(true)
{
    setPage(IdentityAndLoginPage, new UserIdentityAndLoginPage(this));
    setPage(ContactPage, new UserContactPage(this));
    setPage(ProfilPage, new UserProfilPage(this));
    setPage(RightsPage, new UserRightsPage(this));
    setPage(SpecialiesQualificationsPage, new UserSpecialiesQualificationsPage(this));

    setWindowTitle(tr("User Creator Wizard"));
    QList<QWizard::WizardButton> layout;
    layout << QWizard::CancelButton << QWizard::Stretch << QWizard::BackButton
            << QWizard::NextButton << QWizard::FinishButton;
    setButtonLayout(layout);
    //    setAttribute(Qt::WA_DeleteOnClose);
}

UserWizard::~UserWizard()
{
    if (m_User) {
        delete m_User;
        m_User = 0;
    }
}

void UserWizard::done(int r)
{
    validateCurrentPage();

    // No user -> rejected & close
    if (!m_User) {
        QDialog::done(QDialog::Rejected);
        return;
    }

    // dialog result == Rejected -> ask for a confirmation
    if (r == QDialog::Rejected) {
        m_Saved = false;
        bool yes = Utils::yesNoMessageBox(tr("WARNING ! You don't save this user."),
                               tr("If you continue changes will be lost.\n"
                                  "Do you really want to close this dialog ?"),
                               "", tr("Data losing"));
        if (yes) {
            QDialog::done(r);
        }
        return;
    }

    // Dialog is Accepted here, User not saved -> save it
    if (true) {

        // Feed userData with the wizard values
        m_User->setValidity(1);
        m_User->setLogin64(Utils::loginForSQL(field("Login").toString()));
        m_User->setClearPassword(field("Password").toString());
        m_User->setCryptedPassword(Utils::cryptPassword(field("Password").toString()));
        m_User->setName(field("Name"));
        m_User->setSecondName(field("SecondName"));
        m_User->setFirstname(field("Firstname"));
        m_User->setTitle(field("Title"));
        m_User->setGender(field("Gender"));
        m_User->setAddress(field("Address"));
        m_User->setZipcode(field("Zipcode"));
        m_User->setCity(field("City"));
        m_User->setCountry(field("Country"));
        m_User->setLanguage(field("Language"));
        m_User->setTel1(field("Tel1"));
        m_User->setTel2(field("Tel2"));
        m_User->setTel3(field("Tel3"));
        m_User->setFax(field("Fax"));
        m_User->setMail(field("Mail"));
        m_User->setSpecialty(field("Specialities").toStringList());
        m_User->setQualification(field("Qualifications").toStringList());
        m_User->setPractitionerIdentifiant(field("Identifiants").toStringList());

        m_User->setRights(Constants::USER_ROLE_USERMANAGER, Core::IUser::UserRights(m_Rights.value(Core::IUser::ManagerRights)));
        m_User->setRights(Constants::USER_ROLE_MEDICAL, Core::IUser::UserRights(m_Rights.value(Core::IUser::MedicalRights)));
        m_User->setRights(Constants::USER_ROLE_DOSAGES, Core::IUser::UserRights(m_Rights.value(Core::IUser::DrugsRights)));
        m_User->setRights(Constants::USER_ROLE_PARAMEDICAL, Core::IUser::UserRights(m_Rights.value(Core::IUser::ParamedicalRights)));
        m_User->setRights(Constants::USER_ROLE_ADMINISTRATIVE, Core::IUser::UserRights(m_Rights.value(Core::IUser::AdministrativeRights)));

//        m_User->setGenericHeader(m_Papers.value(Core::IUser::GenericHeader));
//        m_User->setGenericFooter(m_Papers.value(Core::IUser::GenericFooter));
//        m_User->setGenericWatermark(m_Papers.value(Core::IUser::GenericWatermark));

//        m_User->setAdminHeader(m_Papers.value(Core::IUser::AdministrativeHeader));
//        m_User->setAdminFooter(m_Papers.value(Core::IUser::AdministrativeFooter));
//        m_User->setAdminWatermark(m_Papers.value(Core::IUser::AdministrativeWatermark));

//        m_User->setPrescriptionHeader(m_Papers.value(Core::IUser::PrescriptionHeader));
//        m_User->setPrescriptionFooter(m_Papers.value(Core::IUser::PrescriptionFooter));
//        m_User->setPrescriptionWatermark(m_Papers.value(Core::IUser::PrescriptionWatermark));

        m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(m_Papers.value(Core::IUser::GenericHeader)), Core::IUser::GenericHeader);
        m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(m_Papers.value(Core::IUser::GenericFooter)), Core::IUser::GenericFooter);
        m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(m_Papers.value(Core::IUser::GenericWatermark)), Core::IUser::GenericWatermark);

        m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(m_Papers.value(Core::IUser::AdministrativeHeader)), Core::IUser::AdministrativeHeader);
        m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(m_Papers.value(Core::IUser::AdministrativeFooter)), Core::IUser::AdministrativeFooter);
        m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(m_Papers.value(Core::IUser::AdministrativeWatermark)), Core::IUser::AdministrativeWatermark);

        m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(m_Papers.value(Core::IUser::PrescriptionHeader)), Core::IUser::PrescriptionHeader);
        m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(m_Papers.value(Core::IUser::PrescriptionFooter)), Core::IUser::PrescriptionFooter);
        m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(m_Papers.value(Core::IUser::PrescriptionWatermark)), Core::IUser::PrescriptionWatermark);


        if (m_CreateUser) {
            // Create user in database
            if (!userBase()->createUser(m_User)) {
                Utils::warningMessageBox(tr("An error occured during database access."),
                                         tr("Logged errors saved. Please refer to the %1 to manage this error.")
                                         .arg(Utils::Log::saveLog()),
                                         "", tr("Error during database access"));
                QDialog::done(QDialog::Rejected);
            } else {
                Utils::informativeMessageBox(tr("User correctly saved into database."),
                                             tr("The user was correctly created and saved into database."),
                                             "", tr("User correctly saved into database."));
                // Reset the usermodel
                userModel()->refresh();
                m_Saved = true;
                QDialog::done(r);
            }

//            m_Row = userModel()->rowCount();
//            if (!userModel()->insertRows(m_Row, 1)) {
//            }
        }

#ifdef DEBUG
        // warn user
        m_User->warn();
#endif

//        if (userModel()->submitRow(m_Row)) {
//        } else {
//            userModel()->removeRows(m_Row, 1);
//            Utils::warningMessageBox(tr("An error occured during database access."),
//                                         tr("Logged errors saved. Please refer to the %1 to manage this error.")
//                                         .arg(Utils::Log::saveLog()),
//                                         "", tr("Error during database access"));
//            QDialog::done(r);
//            m_Saved = false;
//        }
    }
}

QString UserWizard::createdUuid() const
{
    if (m_User) {
        return m_User->uuid();
    }
    return QString();
}

UserIdentityAndLoginPage::UserIdentityAndLoginPage(QWidget *parent) :
        QWizardPage(parent), langLbl(0)
{
    identGroup = new QGroupBox(this);
    logGroup = new QGroupBox(this);

    langLbl = new QLabel(tr("Language"), this);
    lblTitle = new QLabel(tr("Title"), this);
    lblName = new QLabel(tr("Name"), this);
    lblFirstName = new QLabel(tr("Firstname"), this);
    lblSecondName = new QLabel(tr("Second Name"), this);
    lblGender = new QLabel(tr("Gender"), this);

    Views::LanguageComboBox *cbLanguage = new Views::LanguageComboBox(this);
    cbLanguage->setDisplayMode(Views::LanguageComboBox::AvailableTranslations);
    cbLanguage->setCurrentLanguage(QLocale().language());

    cbTitle = new QComboBox(this);
    leName = new QLineEdit(this);
    leFirstName = new QLineEdit(this);
    leSecondName = new QLineEdit(this);
    cbGender = new QComboBox(this);

    connect(cbLanguage, SIGNAL(currentLanguageChanged(QLocale::Language)), Core::Translators::instance(), SLOT(changeLanguage(QLocale::Language)));

    QFormLayout *identLayout = new QFormLayout;
    identLayout->addRow(langLbl, cbLanguage);
    identLayout->addRow(lblTitle, cbTitle);
    identLayout->addRow(lblName, leName);
    identLayout->addRow(lblFirstName, leFirstName);
    identLayout->addRow(lblSecondName, leSecondName);
    identLayout->addRow(lblGender, cbGender);
    identGroup->setLayout(identLayout);

    registerField("Language", cbLanguage , "currentIndex");
    registerField("Name", leName, "text");
    registerField("Firstname", leFirstName, "text");
    registerField("SecondName", leSecondName, "text");
    registerField("Title", cbTitle, "currentIndex");
    registerField("Gender", cbGender, "currentIndex");

    lblL = new QLabel(tr("Login"), this);
    lblP = new QLabel(tr("Password"), this);
    lblCP = new QLabel(tr("Confirm Password"), this);

    leLogin = new Utils::LineEditEchoSwitcher(this);
    lePassword = new Utils::LineEditEchoSwitcher(this);
    lePasswordConfirm = new Utils::LineEditEchoSwitcher(this);
    leLogin->toogleEchoMode();
    lePassword->toogleEchoMode();
    lePasswordConfirm->toogleEchoMode();
    leLogin->setIcon(theme()->icon(Core::Constants::ICONEYES));
    lePassword->setIcon(theme()->icon(Core::Constants::ICONEYES));
    lePasswordConfirm->setIcon(theme()->icon(Core::Constants::ICONEYES));

    registerField("Login", leLogin, "text");
    registerField("Password", lePassword, "text");
    registerField("ConfirmPassword", lePasswordConfirm, "text");

    QFormLayout *loglayout = new QFormLayout;
    loglayout->addRow(lblL, leLogin);
    loglayout->addRow(lblP, lePassword);
    loglayout->addRow(lblCP, lePasswordConfirm);
    logGroup->setLayout(loglayout);

    QGridLayout *lay = new QGridLayout(this);
    setLayout(lay);
    lay->addWidget(identGroup);
    lay->addWidget(logGroup);

    retranslate();
}

void UserIdentityAndLoginPage::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
        retranslate();
}

void UserIdentityAndLoginPage::retranslate()
{
    setTitle(tr("Create a new user."));
    setSubTitle(tr("Enter your identity."));
    if (langLbl) {
        langLbl->setText(tr("Language"));
        lblTitle->setText(tr("Title"));
        lblName->setText(tr("Name"));
        lblFirstName->setText(tr("Firstname"));
        lblSecondName->setText(tr("Second Name"));
        lblGender->setText(tr("Gender"));
        cbTitle->addItems(titles());
        cbGender->addItems(genders());
        lblL->setText(tr("Login"));
        lblP->setText(tr("Password"));
        lblCP->setText(tr("Confirm Password"));
        identGroup->setTitle(tr("Identity"));
        logGroup->setTitle(tr("Database connection"));
    }
}

bool UserIdentityAndLoginPage::validatePage()
{
    if (field("Name").toString().isEmpty() || field("Firstname").toString().isEmpty()) {
        Utils::warningMessageBox(tr("Forbidden anonymous user."),
                                 tr("All users must have at least a name and a firstname.\n"
                                    "You can not poursue with an anonymous user."), "",
                                 tr("Forbidden anonymous user."));
        return false;
    }
    if (field("Password").toString() != field("ConfirmPassword")) {
        Utils::warningMessageBox(tr("Password confirmation error."),
                                 tr("You must correctly confirm your password to go throw this page."),
                                 "", tr("Wrong password"));
        return false;
    }
    if (field("Login").toString().isEmpty()) {
        Utils::warningMessageBox(tr("Login error."),
                                 tr("You must specify a valid login. Empty login is forbidden."),
                                 "", tr("Wrong login"));
        return false;
    }
    if (field("Login").toString().size() <= 3) {
        Utils::warningMessageBox(tr("Login error."),
                                 tr("You must specify a valid login. Login must be more than 3 chars."),
                                 "", tr("Wrong login"));
        return false;
    }
    // log/pass already used ?
    if (userModel()->isCorrectLogin(field("Login").toString(),
                                    field("Password").toString())) {
        Utils::warningMessageBox(tr("Login and password already used"),
                                 tr("The users' database already contains the same login/password couple.\n"
                                    "You must specify a different login/password."),
                                 "", tr("Login/Password already used"));
        return false;
    }
    return true;
}


UserContactPage::UserContactPage(QWidget *parent) :
        QWizardPage(parent), ui(new Ui::UserWizardContactWidget)
{
    setTitle(tr("Please enter your complete address."));
    setSubTitle(tr("This represents your professional address."));

    ui->setupUi(this);
    ui->country->setText(QLocale::countryToString(QLocale().country()));
    ui->tabWidget->setCurrentIndex(0);

    registerField("Address", ui->address , "plainText");
    registerField("City", ui->city , "text");
    registerField("Zipcode", ui->zipcode , "text");
    registerField("Country", ui->country , "text");

    registerField("Tel1", ui->tel1, "text");
    registerField("Tel2", ui->tel2, "text");
    registerField("Tel3", ui->tel3, "text");
    registerField("Fax", ui->fax, "text");
    registerField("Mail", ui->mail, "text");
}

UserContactPage::~UserContactPage()
{
    delete ui;
}


UserProfilPage::UserProfilPage(QWidget *parent) :
        QWizardPage(parent)
{
    setTitle(tr("Select a profil"));
    setSubTitle(tr("FreeMedForms allows you to create users using predefined profils. Select your profil and options."));

//    registerField("isMedical", new QWidget(this));
//    registerField("UserManager", new QWidget(this));
//    registerField("DrugsManager", new QWidget(this));
//    registerField("Medical", new QWidget(this));
//    registerField("Paramedical", new QWidget(this));
//    registerField("Administrative", new QWidget(this));

    Views::StringListModel *model = new Views::StringListModel(this);
    model->setStringList(QStringList() << tkTr(Trans::Constants::DOCTOR) << tr("Software administrator"));
    model->setCheckable(true);
    model->setReadOnly(true);
    view = new Views::StringListView(this);
    view->setModel(model);
    view->setActions(0);

    box = new QCheckBox(tr("Define all rights"), this);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(view, 0, 0);
    layout->addWidget(box, 1, 0);
}

bool UserProfilPage::validatePage()
{
    UserWizard::setUserRights(Core::IUser::ManagerRights, Core::IUser::NoRights);
    UserWizard::setUserRights(Core::IUser::DrugsRights, Core::IUser::NoRights);
    UserWizard::setUserRights(Core::IUser::MedicalRights, Core::IUser::NoRights);
    UserWizard::setUserRights(Core::IUser::ParamedicalRights, Core::IUser::NoRights);
    UserWizard::setUserRights(Core::IUser::AdministrativeRights, Core::IUser::NoRights);
    next = UserWizard::SpecialiesQualificationsPage;
    QStringList result = view->getCheckedStringList().toStringList();
    if (result.contains(tkTr(Trans::Constants::DOCTOR))) {
        UserWizard::setUserRights(Core::IUser::ManagerRights, Core::IUser::NoRights);
        UserWizard::setUserRights(Core::IUser::DrugsRights, Core::IUser::AllRights);
        UserWizard::setUserRights(Core::IUser::MedicalRights, Core::IUser::AllRights);
        UserWizard::setUserRights(Core::IUser::ParamedicalRights, int(Core::IUser::ReadAll | Core::IUser::Print));
        UserWizard::setUserRights(Core::IUser::AdministrativeRights, Core::IUser::NoRights);
        next = UserWizard::SpecialiesQualificationsPage;
        // create default papers
        UserWizard::setUserPaper(Core::IUser::GenericHeader, defaultPaper("medicals", "header"));
        UserWizard::setUserPaper(Core::IUser::GenericFooter, defaultPaper("medicals", "footer"));
        UserWizard::setUserPaper(Core::IUser::GenericWatermark, defaultPaper("medicals", "watermark"));

        UserWizard::setUserPaper(Core::IUser::PrescriptionHeader, defaultPaper("medicals", "header", "prescriptions"));
        UserWizard::setUserPaper(Core::IUser::PrescriptionFooter, defaultPaper("medicals", "footer", "prescriptions"));
        UserWizard::setUserPaper(Core::IUser::PrescriptionWatermark, defaultPaper("medicals", "watermark", "prescriptions"));

        UserWizard::setUserPaper(Core::IUser::AdministrativeHeader, defaultPaper("medicals", "header"));
        UserWizard::setUserPaper(Core::IUser::AdministrativeFooter, defaultPaper("medicals", "footer"));
        UserWizard::setUserPaper(Core::IUser::AdministrativeWatermark, defaultPaper("medicals", "watermark"));
    }
    if (result.contains(tr("Software administrator"))) {
        UserWizard::setUserRights(Core::IUser::ManagerRights, Core::IUser::AllRights);
    }

    if (box->isChecked()) {
        next = UserWizard::RightsPage;
        return true;
    }
    return true;
}

UserSpecialiesQualificationsPage::UserSpecialiesQualificationsPage(QWidget *parent)
        : QWizardPage(parent)
{
    setTitle(tr("Define user's specialties and qualifications."));
    setSubTitle(tr("Use the context menu to add, remove, move up or down items."));
    QTabWidget *tab = new QTabWidget(this);

    QStringListModel *modelspe = new QStringListModel(this);
    Views::StringListView *speView = new Views::StringListView(this);
    speView->setModel(modelspe);
    QStringListModel *modelqual = new QStringListModel(this);
    Views::StringListView *quaView = new Views::StringListView(this);
    quaView->setModel(modelqual);
    QStringListModel *modelids = new QStringListModel(this);
    Views::StringListView *idsView = new Views::StringListView(this);
    idsView->setModel(modelids);

    tab->addTab(speView, tr("Specialities"));
    tab->addTab(quaView, tr("Qualifications"));
    tab->addTab(idsView, tr("Identifiants"));

    registerField("Specialities", speView, "stringList");
    registerField("Qualifications", quaView, "stringList");
    registerField("Identifiants", idsView, "stringList");

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(tab, 0, 1);
    setLayout(layout);
}

UserRightsPage::UserRightsPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Define user's rights."));
    setSubTitle(tr("Role by role, define the user's rights."));

    QTabWidget *tab = new QTabWidget(this);

    um = new Internal::UserRightsWidget(this);
    drugs = new Internal::UserRightsWidget(this);
    med = new Internal::UserRightsWidget(this);
    paramed = new Internal::UserRightsWidget(this);
    administ = new Internal::UserRightsWidget(this);

    tab->addTab(um, tr("Users"));
    tab->addTab(drugs, tr("Drugs"));
    tab->addTab(med, tr("Medicals"));
    tab->addTab(paramed, tr("Paramedicals"));
    tab->addTab(administ, tr("Administrative"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(tab, 0, 0);
    setLayout(layout);
}

void UserRightsPage::initializePage()
{
    um->setRights(UserWizard::userRights(Core::IUser::ManagerRights));
    drugs->setRights(UserWizard::userRights(Core::IUser::DrugsRights));
    med->setRights(UserWizard::userRights(Core::IUser::MedicalRights));
    paramed->setRights(UserWizard::userRights(Core::IUser::ParamedicalRights));
    administ->setRights(UserWizard::userRights(Core::IUser::AdministrativeRights));

}

bool UserRightsPage::validatePage()
{
    UserWizard::setUserRights(Core::IUser::ManagerRights, um->getRights());
    UserWizard::setUserRights(Core::IUser::DrugsRights, drugs->getRights());
    UserWizard::setUserRights(Core::IUser::MedicalRights, med->getRights());
    UserWizard::setUserRights(Core::IUser::ParamedicalRights, paramed->getRights());
    UserWizard::setUserRights(Core::IUser::AdministrativeRights, administ->getRights());
    return true;
}

