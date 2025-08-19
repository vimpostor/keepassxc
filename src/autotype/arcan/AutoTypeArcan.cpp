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

#include "AutoTypeArcan.h"
#include <qpa/qplatformnativeinterface.h>

AutoTypePlatformArcan::AutoTypePlatformArcan()
{
}

bool AutoTypePlatformArcan::isAvailable()
{
    const auto interface = QApplication::platformNativeInterface();
    if (!interface) {
        return false;
    }
    m_cont = static_cast<arcan_shmif_cont*>(interface->nativeResourceForIntegration("arcan_shmif_cont"));
    return m_cont;
}

QStringList AutoTypePlatformArcan::windowTitles()
{
    return {activeWindowTitle()};
}

WId AutoTypePlatformArcan::activeWindow()
{
    return 1;
}

QString AutoTypePlatformArcan::activeWindowTitle()
{
    return m_windowtitle;
}

bool AutoTypePlatformArcan::raiseWindow(WId)
{
    // no need to raise the window: Imagine being only able to type in windows that are active
    return true;
}

AutoTypeExecutor* AutoTypePlatformArcan::createExecutor()
{
    return new AutoTypeExecutorArcan(this);
}

AutoTypeAction::Result AutoTypePlatformArcan::sendKey(const AutoTypeKey* action)
{
    const QByteArray c = QString(action->character).toLocal8Bit();
    const auto* data = c.data();
    arcan_event ev{};
    ev.ext.kind = EVENT_EXTERNAL_MESSAGE;
    ev.ext.message.multipart = false;
    auto len = strlen(data);
    memcpy(ev.ext.message.data, data, len);
    if (!len && action->character.unicode() == 0 && action->key == Qt::Key_Tab) {
        ev.ext.message.data[0] = '\t';
        len++;
    }
    ev.ext.message.data[len] = '\0';
    const auto res = arcan_shmif_enqueue(m_cont, &ev);
    return res < 0 ? AutoTypeAction::Result::Failed("arcan_shmif_enqueue: " + QString::number(res))
                   : AutoTypeAction::Result::Ok();
}

AutoTypeExecutorArcan::AutoTypeExecutorArcan(AutoTypePlatformArcan* platform)
    : m_platform(platform)
{
}

AutoTypeAction::Result AutoTypeExecutorArcan::execBegin(const AutoTypeBegin* action)
{
    Q_UNUSED(action);
    return AutoTypeAction::Result::Ok();
}

AutoTypeAction::Result AutoTypeExecutorArcan::execType(const AutoTypeKey* action)
{
    // It sucks that AutoTypeExecutor only has execBegin() and no execEnd(),
    // otherwise we could collect the entire passphrase and pass it along in one go.
    // We could patch the upstream classes to send an execEnd(),
    // but that would make future rebasing torturous.
    return m_platform->sendKey(action);
}

AutoTypeAction::Result AutoTypeExecutorArcan::execClearField(const AutoTypeClearField* action)
{
    Q_UNUSED(action);
    execType(new AutoTypeKey(Qt::Key_Home));
    execType(new AutoTypeKey(Qt::Key_End, Qt::ShiftModifier));
    execType(new AutoTypeKey(Qt::Key_Backspace));
    return AutoTypeAction::Result::Ok();
}
