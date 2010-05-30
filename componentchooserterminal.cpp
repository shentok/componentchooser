/***************************************************************************
                          componentchooser.cpp  -  description
                             -------------------
    copyright            : (C) 2002 by Joseph Wenninger
    email                : jowenn@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License verstion 2 as    *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/

#include "componentchooserterminal.h"
#include "componentchooserterminal.moc"

#include <kglobalsettings.h>
#include <kconfiggroup.h>
#include <KServiceTypeTrader>


CfgTerminalEmulator::CfgTerminalEmulator(QWidget *parent)
    : QWidget(parent), Ui::TerminalEmulatorConfig_UI(), CfgPlugin()
{
	setupUi(this);
    connect(kcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(configChanged()));
}

CfgTerminalEmulator::~CfgTerminalEmulator() {
}

void CfgTerminalEmulator::configChanged()
{
	emit changed(true);
}

void CfgTerminalEmulator::defaults()
{
	load(0);
}


void CfgTerminalEmulator::load(KConfig *) {
    m_terminalEmulators.clear();
    kcombobox->clear();

    foreach(KService::Ptr browser, KServiceTypeTrader::self()->query("Application", "'TerminalEmulator' in Categories"))
    {
        m_terminalEmulators << browser;
        kcombobox->addItem(KIcon(browser->icon()), browser->name());
    }

    KConfigGroup config(KSharedConfig::openConfig("kdeglobals"), QLatin1String("General") );
    QString exec = config.readPathEntry( QLatin1String("TerminalApplication"), "konsole" );
    if (!exec.isEmpty())
    {
        KService::Ptr browserService = KService::serviceByStorageId( exec );
        if (browserService)
        {
            int i = 0;
            foreach(KService::Ptr browser, m_terminalEmulators)
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

void CfgTerminalEmulator::save(KConfig *)
{
    if (m_terminalEmulators.count() > kcombobox->currentIndex() && kcombobox->currentIndex() >= 0)
    {
        const QString storageId = m_terminalEmulators.at(kcombobox->currentIndex())->storageId();

        KConfigGroup config(KSharedConfig::openConfig("kdeglobals"), QLatin1String("General") );
        config.writePathEntry( QLatin1String("TerminalApplication"), storageId); // KConfig::Normal|KConfig::Global
        config.sync();
    }

    KGlobalSettings::self()->emitChange(KGlobalSettings::SettingsChanged);

    emit changed(false);
}
// vim: sw=4 ts=4 noet
