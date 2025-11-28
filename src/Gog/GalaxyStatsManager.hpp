#pragma once

#include <GalaxyApi.h>
#include <cstdint>

namespace GOG
{
	class GalaxyStatsManager : public galaxy::api::GlobalUserStatsAndAchievementsRetrieveListener,
		galaxy::api::GlobalStatsAndAchievementsStoreListener,
		galaxy::api::GlobalAchievementChangeListener
	{
	protected:
		static GalaxyStatsManager* singleton;

	public:
		static GalaxyStatsManager* GetSingleton();
		GalaxyStatsManager();
		~GalaxyStatsManager();

		[[nodiscard]] bool IsUserStatsReady() const noexcept;

		// Achievements
		void QueryAchievements();
		void StoreAchievements();
		void ClearAchievement(const char* achievementId);
		void SetAchievement(const char* achievementId);
		void ResetAchievements();

	private:
		// Callbacks
		void OnUserStatsAndAchievementsRetrieveSuccess(galaxy::api::GalaxyID userID) override;
		void OnUserStatsAndAchievementsRetrieveFailure(galaxy::api::GalaxyID userID, galaxy::api::IUserStatsAndAchievementsRetrieveListener::FailureReason failureReason) override;
		void OnUserStatsAndAchievementsStoreSuccess() override;
		void OnUserStatsAndAchievementsStoreFailure(galaxy::api::IStatsAndAchievementsStoreListener::FailureReason failureReason) override;
		void OnAchievementUnlocked(const char* name) override;
	};
}
