#include "emoticonssettings.h"
#include <libqutim/libqutim_global.h>
#include "modulemanagerimpl.h"
#include "emoticonsselector.h"
#include <libqutim/settingslayer.h>
#include <libqutim/icon.h>

namespace Core
{
	static CoreModuleHelper<EmoticonsSettings, StartupModule> emoticons_settings_static(
			QT_TRANSLATE_NOOP("Plugin", "Emoticons settings"),
			QT_TRANSLATE_NOOP("Plugin", "Default settings widget set for emoticons")
			);

	EmoticonsSettings::EmoticonsSettings()
	{
		GeneralSettingsItem<EmoticonsSelector> *item = new GeneralSettingsItem<EmoticonsSelector>(Settings::Appearance, Icon("emoticon"), QT_TRANSLATE_NOOP("Settings","Emoticons"));
		Settings::registerItem(item);
	}

}