#include <GalaxyApi.h>

namespace GOTHIC_NAMESPACE
{
	static void Gog_QueryAchievements()
	{
		GOG::GalaxyStatsManager::GetSingleton()->QueryAchievements();
	}

    static void Gog_StoreAchievements()
    {
		GOG::GalaxyStatsManager::GetSingleton()->StoreAchievements();
    }

	static void Gog_UnlockAchievement(const zSTRING& t_achievementName)
	{
        if (t_achievementName.IsEmpty()) return;
		
		const char* name = t_achievementName.ToChar();
		GOG::GalaxyStatsManager::GetSingleton()->SetAchievement(name);
	}

	static void Gog_ClearAchievement(const zSTRING& t_achievementName)
	{
		if (t_achievementName.IsEmpty()) return;

		const char* name = t_achievementName.ToChar();
		GOG::GalaxyStatsManager::GetSingleton()->ClearAchievement(name);
	}
}
