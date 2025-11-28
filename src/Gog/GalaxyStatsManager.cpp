#include "GalaxyStatsManager.hpp"
#include <Utils/zDUtils/Logger.h>

namespace GOG
{
	GalaxyStatsManager* GalaxyStatsManager::singleton = NULL;

	GalaxyStatsManager::GalaxyStatsManager()
	{
		singleton = this;
	}

	GalaxyStatsManager* GalaxyStatsManager::GetSingleton()
	{
		if (singleton == NULL)
		{
			singleton = new GalaxyStatsManager();
		}

		return singleton;
	}

	GalaxyStatsManager::~GalaxyStatsManager()
	{
		singleton = NULL;
	}

	bool GalaxyStatsManager::IsUserStatsReady()
	{
		return galaxy::api::Stats() != NULL;
	}

	// Achievements
	void GalaxyStatsManager::QueryAchievements()
	{
		if (!IsUserStatsReady()) return;

		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyStatsManager::QueryAchievements");

		galaxy::api::Stats()->RequestUserStatsAndAchievements(galaxy::api::User()->GetGalaxyID(), this);

		auto err = galaxy::api::GetError();
		if (err)
			log->Error("Failed to query achievements: {1}", err->GetMsg());
	}

	void GalaxyStatsManager::StoreAchievements()
	{
		if (!IsUserStatsReady()) return;

		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyStatsManager::StoreAchievements");

		galaxy::api::Stats()->StoreStatsAndAchievements(this);

		auto err = galaxy::api::GetError();
		if (err)
			log->Error("Failed to store achievements: {1}", err->GetMsg());
	}

	void GalaxyStatsManager::SetAchievement(const char* achievementId)
	{
		if (!IsUserStatsReady()) return;

		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyStatsManager::SetAchievement");

		galaxy::api::Stats()->RequestUserStatsAndAchievements(galaxy::api::User()->GetGalaxyID(), this);
		galaxy::api::Stats()->SetAchievement(achievementId);
		galaxy::api::Stats()->StoreStatsAndAchievements(this);

		auto err = galaxy::api::GetError();
		if (err)
			log->Error("Failed to set achievement {0}: {1}", achievementId, err->GetMsg());
	}

	void GalaxyStatsManager::ClearAchievement(const char* achievementId)
	{
		if (!IsUserStatsReady()) return;

		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyStatsManager::ClearAchievement");

		galaxy::api::Stats()->ClearAchievement(achievementId);
		galaxy::api::Stats()->StoreStatsAndAchievements(this);

		auto err = galaxy::api::GetError();
		if (err)
			log->Error("Failed to clear achievement {0}: {1}", achievementId, err->GetMsg());
	}

	// Callbacks
	void GalaxyStatsManager::OnUserStatsAndAchievementsRetrieveSuccess(galaxy::api::GalaxyID userID)
	{
		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyStatsManager::OnUserStatsAndAchievementsRetrieveSuccess");
		log->Info("User {0} stats and achievements retrieved", userID.GetRealID());
	}

	void GalaxyStatsManager::OnUserStatsAndAchievementsRetrieveFailure(galaxy::api::GalaxyID userID, galaxy::api::IUserStatsAndAchievementsRetrieveListener::FailureReason failureReason)
	{
		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyStatsManager::OnUserStatsAndAchievementsRetrieveFailure");
		log->Error("User {0} stats and achievements could not be retrieved, for reason: {1}", userID.GetRealID(), failureReason);
	}

	void GalaxyStatsManager::OnUserStatsAndAchievementsStoreSuccess()
	{
		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyStatsManager::OnUserStatsAndAchievementsStoreSuccess");
		log->Info("User stats and achievements stored");
	}

	void GalaxyStatsManager::OnUserStatsAndAchievementsStoreFailure(galaxy::api::IStatsAndAchievementsStoreListener::FailureReason failureReason)
	{
		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyStatsManager::OnUserStatsAndAchievementsStoreFailure");
		log->Error("User stats and achievements could not be stored, for reason: {0}", failureReason);
	}

	void GalaxyStatsManager::OnAchievementUnlocked(const char* name)
	{
		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyStatsManager::OnAchievementUnlocked");
		log->Info("Achievement {0} unlocked", name);
	}
}