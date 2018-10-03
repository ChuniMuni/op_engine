#include "stdafx.h"
#include "LanguagesManager.h"


ENGINE_API CLanguagesManager* langManager = nullptr;

CLanguagesManager::CLanguagesManager()
{
	currentLangIndex = static_cast<u32>(-1);
	if (!pSettings->section_exist("languages"))
	{
		Msg("! ERROR Cannot find required section [%s]", "languages");
		FATAL("Invalid required configuration! See log for detail.");
	}
	if (!pSettings->line_exist("languages", "default"))
	{
		Msg("! ERROR Cannot find required default language[%s]", "default");
		FATAL("Invalid required configuration! See log for detail.");
	}
	defaultLanguageName = pSettings->r_string("languages", "default");
	for (int idx = 0; idx<static_cast<int>(pSettings->line_count("languages")); idx++)
	{
		LPCSTR langId, textId;
		if (pSettings->r_line("languages", idx, &langId, &textId))
		{
			if (xr_strcmp(langId, "default") == 0)
			{
				continue;
			}
			langTokens.push_back(xr_token());
			xr_token* last = &langTokens.back();
			last->name = xr_strdup(langId);
			last->id = langTokens.size() - 1;
		}
	}
	xr_vector<xr_token>::iterator defLangIter;
	if (currentLangIndex == static_cast<u32>(-1) && (defLangIter = std::find_if(langTokens.begin(), langTokens.end(), [&](xr_token token)
	{
		return xr_strcmp(token.name, defaultLanguageName) == 0;
	})) != langTokens.end())
	{
		currentLangIndex = std::distance(langTokens.begin(), defLangIter);
	}
	else
	{
		Msg("! ERROR Cannot find configured default language[%s]!", defaultLanguageName);
		FATAL("Invalid required configuration! See log for detail.");
	}
}


CLanguagesManager::~CLanguagesManager()
{
}

LPCSTR CLanguagesManager::GetCurrentLangName()
{
	//winsor: all check passed from constuctor
	return langTokens[currentLangIndex].name;
}

u32 CLanguagesManager::GetCurrentLangIndex()
{
	//winsor: all check passed from constuctor
	return currentLangIndex;
}

xr_string CLanguagesManager::SetupSoundFile(LPCSTR base)
{
	string512 ffn;
	sprintf_s(ffn, "%s\\%s", GetCurrentLangName(),base);
	if (FS.exist("$game_sounds$", ffn))
		return ffn;
	return base;
}

xr_string CLanguagesManager::SetupSoundFileExt(LPCSTR base, LPCSTR ext)
{
	string512 ffn;
	string_path	fn;
	sprintf_s(ffn, "%s\\%s", GetCurrentLangName(), base);
	if (FS.exist(fn,"$game_sounds$", ffn,ext))
		return ffn;
	return base;
}
