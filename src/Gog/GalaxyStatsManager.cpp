#include "GalaxyStatsManager.hpp"
#include <Utils/zDUtils/Logger.h>

namespace GOG
{
	GalaxyStatsManager* GalaxyStatsManager::singleton = nullptr;

	GalaxyStatsManager::GalaxyStatsManager()
	{
		singleton = this;
	}

	GalaxyStatsManager* GalaxyStatsManager::GetSingleton()
	{
		if (singleton == nullptr)
			singleton = new GalaxyStatsManager();
		return singleton;
	}

	GalaxyStatsManager::~GalaxyStatsManager()
	{
		singleton = nullptr;
	}

	bool GalaxyStatsManager::IsUserStatsReady() const noexcept
	{
		return galaxy::api::Stats() != nullptr;
	}

	// Achievements
	void GalaxyStatsManager::QueryAchievements()
	{
		if (!IsUserStatsReady()) return;

		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyStatsManager::QueryAchievements");

		galaxy::api::Stats()->RequestUserStatsAndAchievements(galaxy::api::User()->GetGalaxyID(), this);

		if (const auto err = galaxy::api::GetError())
			log->Error("Failed to query achievements: {0}", err->GetMsg());
	}

	void GalaxyStatsManager::StoreAchievements()
	{
		if (!IsUserStatsReady()) return;

		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyStatsManager::StoreAchievements");

		galaxy::api::Stats()->StoreStatsAndAchievements(this);

		if (const auto err = galaxy::api::GetError())
			log->Error("Failed to store achievements: {0}", err->GetMsg());
	}

	void GalaxyStatsManager::SetAchievement(const char* achievementId)
	{
		if (!IsUserStatsReady()) return;

		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyStatsManager::SetAchievement");

		galaxy::api::Stats()->SetAchievement(achievementId);
		galaxy::api::Stats()->StoreStatsAndAchievements(this);

		if (const auto err = galaxy::api::GetError())
			log->Error("Failed to set achievement {0}: {1}", achievementId, err->GetMsg());
	}

	void GalaxyStatsManager::ClearAchievement(const char* achievementId)
	{
		if (!IsUserStatsReady()) return;

		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyStatsManager::ClearAchievement");

		galaxy::api::Stats()->ClearAchievement(achievementId);
		galaxy::api::Stats()->StoreStatsAndAchievements(this);

		if (const auto err = galaxy::api::GetError())
			log->Error("Failed to clear achievement {0}: {1}", achievementId, err->GetMsg());
	}

	void GalaxyStatsManager::ResetAchievements()
	{
		if (!IsUserStatsReady()) return;

		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyStatsManager::ResetAchievements");

		galaxy::api::Stats()->ResetStatsAndAchievements(this);

		if (const auto err = galaxy::api::GetError())
			log->Error("Failed to reset achievements: {1}", err->GetMsg());
	}

	// Callbacks
	void GalaxyStatsManager::OnUserStatsAndAchievementsRetrieveSuccess(galaxy::api::GalaxyID userID)
	{
		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyStatsManager::OnUserStatsAndAchievementsRetrieveSuccess");
		log->Info("Stats and achievements for user {0} retrieved", userID.GetRealID());
	}

	void GalaxyStatsManager::OnUserStatsAndAchievementsRetrieveFailure(galaxy::api::GalaxyID userID, galaxy::api::IUserStatsAndAchievementsRetrieveListener::FailureReason failureReason)
	{
		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyStatsManager::OnUserStatsAndAchievementsRetrieveFailure");
		log->Error("Failed to retrieve stats and achievements for user {0}, reason: {1}", userID.GetRealID(), failureReason);
	}

	void GalaxyStatsManager::OnUserStatsAndAchievementsStoreSuccess()
	{
		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyStatsManager::OnUserStatsAndAchievementsStoreSuccess");
		log->Info("User stats and achievements stored");
	}

	void GalaxyStatsManager::OnUserStatsAndAchievementsStoreFailure(galaxy::api::IStatsAndAchievementsStoreListener::FailureReason failureReason)
	{
		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyStatsManager::OnUserStatsAndAchievementsStoreFailure");
		log->Error("Failed to store user stats and achievements, reason: {0}", failureReason);
	}

	void GalaxyStatsManager::OnAchievementUnlocked(const char* name)
	{
		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyStatsManager::OnAchievementUnlocked");
		log->Info("Achievement unlocked: {0}", name);
	}
}