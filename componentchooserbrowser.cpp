/***************************************************************************
                          componentchooserbrowser.cpp
                             -------------------
    copyright            : (C) 2002 by Joseph Wenninger
    email                : jowenn@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2 as     *
 *   published by the Free Software Foundationi                            *
 *                                                                         *
 ***************************************************************************/

#include "componentchooserbrowser.h"
#include <kopenwithdialog.h>
#include <kglobalsettings.h>
#include <kconfiggroup.h>
#include <KServiceTypeTrader>



CfgBrowser::CfgBrowser(QWidget *parent)
    : QWidget(parent), Ui::BrowserConfig_UI(),CfgPlugin()
{
    setupUi(this);
    connect(kcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(configChanged()));
}

CfgBrowser::~CfgBrowser() {
}

void CfgBrowser::configChanged()
{
    emit changed(true);
}

void CfgBrowser::defaults()
{
    load(0);
}


void CfgBrowser::load(KConfig *)
{
    m_browsers.clear();
    kcombobox->clear();

    foreach(KService::Ptr browser, KServiceTypeTrader::self()->query("Application", "'WebBrowser' in Categories"))
    {
        m_browsers << browser;
        kcombobox->addItem(KIcon(browser->icon()), browser->name());
    }

    KConfigGroup config(KSharedConfig::openConfig("kdeglobals"), QLatin1String("General") );
    QString exec = config.readPathEntry( QLatin1String("BrowserApplication"), QString("") );
    if (!exec.isEmpty())
    {
        KService::Ptr browserService = KService::serviceByStorageId( exec );
        if (browserService)
        {
            int i = 0;
            foreach(KService::Ptr browser, m_browsers)
            {
                if (browser->storageId() == browserService->storageId())
                {
                    kcombobox->setCurrentIndex(i);
                }
                i++;
            }
        }
    }

    emit changed(false);
}

void CfgBrowser::save(KConfig *)
{
    QString exec;
    if (m_browsers.count() > kcombobox->currentIndex())
    {
        exec = m_browsers.at(kcombobox->currentIndex())->storageId();
    }

    KConfigGroup config(KSharedConfig::openConfig("kdeglobals"), QLatin1String("General") );
    config.writePathEntry( QLatin1String("BrowserApplication"), exec); // KConfig::Normal|KConfig::Global
    config.sync();

    KGlobalSettings::self()->emitChange(KGlobalSettings::SettingsChanged);

    emit changed(false);
}
