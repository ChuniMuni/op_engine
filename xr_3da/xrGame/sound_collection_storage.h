////////////////////////////////////////////////////////////////////////////
//	Module 		: sound_collection_storage.h
//	Created 	: 13.10.2005
//  Modified 	: 13.10.2005
//	Author		: Dmitriy Iassenev
//	Description : sound collection storage
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "sound_player.h"

class CSoundCollectionStorage {
public:
	typedef CSoundPlayer::CSoundCollectionParams					CSoundCollectionParams;
	typedef CSoundPlayer::CSoundCollection							CSoundCollection;
	typedef std::pair<CSoundCollectionParams,CSoundCollection*>		SOUND_COLLECTION_PAIR;
	typedef xr_hashmap<CSoundCollectionParams, CSoundCollection*>	OBJECTS;

private:
	OBJECTS								m_objects;

public:
	virtual								~CSoundCollectionStorage	();
	CSoundCollection*					object(const CSoundCollectionParams &params);
};

extern CSoundCollectionStorage			*g_sound_collection_storage;

IC			CSoundCollectionStorage		&sound_collection_storage	();

#include "sound_collection_storage_inline.h"