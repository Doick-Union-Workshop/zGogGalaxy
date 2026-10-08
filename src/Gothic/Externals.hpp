#pragma once

namespace GOTHIC_NAMESPACE
{
	void Gog_UnlockAchievement(const zSTRING& t_achievementName);
	void Gog_ClearAchievement(const zSTRING& t_achievementName);
	void Gog_QueryAchievements();
	void Gog_StoreAchievements();
	void Gog_ResetAchievements();
}