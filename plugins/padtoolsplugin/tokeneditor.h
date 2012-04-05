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
 *  Main Developers : Eric Maeker <eric.maeker@gmail.com>,                *
 *                    Guillaume Denry <guillaume.denry@gmail.com>          *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef PADTOOLS_TOKENEDITOR_H
#define PADTOOLS_TOKENEDITOR_H

#include <QDialog>
#include <QModelIndex>

namespace PadTools {
class TokenModel;

namespace Ui {
class TokenEditor;
}

class TokenEditor : public QDialog
{
    Q_OBJECT
    
public:
    explicit TokenEditor(QWidget *parent = 0);
    ~TokenEditor();
    
    void setTokenModel(TokenModel *model);
    void setCurrentIndex(const QModelIndex &index);

    void setTokenName(const QString &name);

    void setConditionnalHtml(const QString &before, const QString &after);
    void setConditionnalPlainText(const QString &before, const QString &after);

    QString toRawSourceHtml() const;

protected:
    void done(int result);

private:
    Ui::TokenEditor *ui;
};

} // namespace PadTools

#endif // PADTOOLS_TOKENEDITOR_H
