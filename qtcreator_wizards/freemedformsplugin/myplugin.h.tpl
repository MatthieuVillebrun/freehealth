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
 *   Main Developer: %Author% <%AuthorEmail%>                  *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef %PluginName:u%_IPLUGIN_%CppHeaderSuffix:u%
#define %PluginName:u%_IPLUGIN_%CppHeaderSuffix:u%

#include <extensionsystem/iplugin.h>

namespace %PluginName% {
namespace Internal {

class %PluginName%Plugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.freemedforms.FreeMedForms.%PluginName%Plugin" FILE "%PluginName%.json")

public:
    %PluginName%Plugin();
    ~%PluginName%Plugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private Q_SLOTS:
    void postCoreInitialization();
    void coreAboutToClose();

// Declare plugin unit tests here
#ifdef WITH_TESTS
private Q_SLOTS:
    void initTestCase();
    void test_your_plugin_unit_test(); // see file tests/test_%PluginName%.cpp
    void cleanTestCase();
#endif

};

} // namespace Internal
} // namespace %PluginName%

#endif // %PluginName:u%_IPLUGIN_%CppHeaderSuffix:u%
