/*
 * Copyright (C) 2020-2023 Christoph Oelckers
 *
 * This file is part of Raze
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "ns.h"	// Must come before everything else!
#ifdef OASIS_STAR_API
#include "raze_ogengine_integration.h"
#endif

#include "build.h"
#include "v_draw.h"
#include "statistics.h"
#include "gstrings.h"
#include "gamestate.h"
#include "raze_sound.h"
#include "d_net.h"
#include "screenjob.h"
#include "blood.h"

BEGIN_BLD_NS


void GameInterface::LevelCompleted(MapRecord* map, int skill)
{
	// Save the player state before taking down anything.
	for (int i = connecthead; i >= 0; i = connectpoint2[i])
	{
		if (getPlayer(i)->GetActor())
		{
			gPlayerTemp[i].CopyFromPlayer(getPlayer(i));
			gHealthTemp[i] = getPlayer(i)->GetActor()->xspr.health;
		}
	}

	EndLevel();
	Mus_Stop();

	SummaryInfo info{};
	Level.fillSummary(info);

	info.endofgame = map == nullptr;

	ShowIntermission(currentLevel, map, &info, [=](bool)
		{
			soundEngine->StopAllChannels();
			gameaction = map ? ga_nextlevel : ga_creditsmenu;
		});
}

bool AllowedKillType(DBloodActor* actor)
{
    if (!actor || actor->spr.statnum != kStatDude)
        return false;

    auto type = actor->GetType();
    return type != kDudeBat && type != kDudeRat && type != kDudeInnocent && type != kDudeBurningInnocent;
}

void AddKill(DBloodActor* killer, DBloodActor* killed)
{
	if (AllowedKillType(killed))
	{
		int playernum = killer->IsPlayerActor() ? killer->GetType() - kDudePlayer1 : -1;
		Level.addKill(playernum, 1);
#ifdef OASIS_STAR_API
		if (playernum >= 0)
		{
			char dudeName[32];
			snprintf(dudeName, sizeof(dudeName), "BloodDude%d", (int)killed->GetType());
			Raze_STAR_OnKillName(dudeName);
		}
#endif
	}
}

END_BLD_NS
