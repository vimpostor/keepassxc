/*
 *  Copyright (C) 2017 KeePassXC Team <team@keepassxc.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 or (at your option)
 *  version 3 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KEEPASSX_AUTOTYPEARCAN_H
#define KEEPASSX_AUTOTYPEARCAN_H

#include <QApplication>
#include <QSet>
#include <QWidget>
#include <QtPlugin>
extern "C" {
#include <arcan_shmif.h>
}

#include "autotype/AutoTypePlatformPlugin.h"

class AutoTypePlatformArcan : public QObject, public AutoTypePlatformInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.keepassx.AutoTypePlatformArcan")
    Q_INTERFACES(AutoTypePlatformInterface)
public:
    AutoTypePlatformArcan();
    bool isAvailable() override;
    QStringList windowTitles() override;
    WId activeWindow() override;
    QString activeWindowTitle() override;
    bool raiseWindow(WId) override;
    AutoTypeExecutor* createExecutor() override;
    AutoTypeAction::Result sendKey(const AutoTypeKey* action);

private:
    QString m_windowtitle;
    arcan_shmif_cont* m_cont = nullptr;
};

class AutoTypeExecutorArcan : public AutoTypeExecutor
{
public:
    explicit AutoTypeExecutorArcan(AutoTypePlatformArcan* platform);
    AutoTypeAction::Result execBegin(const AutoTypeBegin* action) override;
    AutoTypeAction::Result execType(const AutoTypeKey* action) override;
    AutoTypeAction::Result execClearField(const AutoTypeClearField* action) override;

private:
    AutoTypePlatformArcan* const m_platform;
};

#endif // KEEPASSX_AUTOTYPEARCAN_H
