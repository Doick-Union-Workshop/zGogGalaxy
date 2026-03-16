namespace GOTHIC_NAMESPACE
{
	void Gog_UnlockAchievement(const zSTRING& t_achievementName)
	{
		GOG::galaxyStatsManager->SetAchievement(t_achievementName.ToChar());
	}

	void Gog_ClearAchievement(const zSTRING& t_achievementName)
	{
		GOG::galaxyStatsManager->ClearAchievement(t_achievementName.ToChar());
	}

	void Gog_QueryAchievements()
	{
		GOG::galaxyStatsManager->QueryAchievements();
	}

	void Gog_StoreAchievements()
	{
		GOG::galaxyStatsManager->StoreAchievements();
	}

	void Gog_ResetAchievements()
	{
		GOG::galaxyStatsManager->ResetAchievements();
	}
}
