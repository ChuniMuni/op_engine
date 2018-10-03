#pragma once

class ENGINE_API CLanguagesManager
{
private:
	LPCSTR defaultLanguageName;
public:
	xr_vector<xr_token> langTokens;
	u32 currentLangIndex;
	CLanguagesManager();
	~CLanguagesManager();
	LPCSTR GetCurrentLangName();
	u32 GetCurrentLangIndex();
	xr_string SetupSoundFile(LPCSTR base);
	xr_string SetupSoundFileExt(LPCSTR base,LPCSTR ext);
};

extern ENGINE_API	CLanguagesManager*	langManager;
