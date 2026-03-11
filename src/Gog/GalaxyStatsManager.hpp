#pragma once

#include <GalaxyApi.h>
#include <cstdint>
#include <memory>

namespace GOG
{
	class GalaxyStatsManager : public galaxy::api::GlobalUserStatsAndAchievementsRetrieveListener,
		galaxy::api::GlobalStatsAndAchievementsStoreListener,
		galaxy::api::GlobalAchievementChangeListener
	{
	public:
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
		inline static Utils::Logger* logger = Utils::CreateLogger("zGogGalaxy::GalaxyStatsManager");

		// Callbacks
		void OnUserStatsAndAchievementsRetrieveSuccess(galaxy::api::GalaxyID userID) override;
		void OnUserStatsAndAchievementsRetrieveFailure(
			galaxy::api::GalaxyID userID,
			galaxy::api::IUserStatsAndAchievementsRetrieveListener::FailureReason failureReason) override;
		void OnUserStatsAndAchievementsStoreSuccess() override;
		void OnUserStatsAndAchievementsStoreFailure(
			galaxy::api::IStatsAndAchievementsStoreListener::FailureReason failureReason) override;
		void OnAchievementUnlocked(const char* name) override;
	};

	inline std::unique_ptr<GalaxyStatsManager> galaxyStatsManager;

	GalaxyStatsManager::GalaxyStatsManager()
	{
	}

	GalaxyStatsManager::~GalaxyStatsManager()
	{
	}

	bool GalaxyStatsManager::IsUserStatsReady() const noexcept
	{
		return galaxy::api::Stats() != nullptr;
	}

	// Achievements
	void GalaxyStatsManager::QueryAchievements()
	{
		if (!IsUserStatsReady()) {
			return;
		}

		galaxy::api::Stats()->RequestUserStatsAndAchievements(galaxy::api::User()->GetGalaxyID(), this);

		if (const auto err = galaxy::api::GetError()) {
			logger->Error("Failed to query achievements: {0}", err->GetMsg());
		}
	}

	void GalaxyStatsManager::StoreAchievements()
	{
		if (!IsUserStatsReady()) {
			return;
		}

		galaxy::api::Stats()->StoreStatsAndAchievements(this);

		if (const auto err = galaxy::api::GetError()) {
			logger->Error("Failed to store achievements: {0}", err->GetMsg());
		}
	}

	void GalaxyStatsManager::SetAchievement(const char* achievementId)
	{
		if (!IsUserStatsReady()) {
			return;
		}

		galaxy::api::Stats()->SetAchievement(achievementId);
		galaxy::api::Stats()->StoreStatsAndAchievements(this);

		if (const auto err = galaxy::api::GetError()) {
			logger->Error("Failed to set achievement {0}: {1}", achievementId, err->GetMsg());
		}
	}

	void GalaxyStatsManager::ClearAchievement(const char* achievementId)
	{
		if (!IsUserStatsReady()) {
			return;
		}

		galaxy::api::Stats()->ClearAchievement(achievementId);
		galaxy::api::Stats()->StoreStatsAndAchievements(this);

		if (const auto err = galaxy::api::GetError()) {
			logger->Error("Failed to clear achievement {0}: {1}", achievementId, err->GetMsg());
		}
	}

	void GalaxyStatsManager::ResetAchievements()
	{
		if (!IsUserStatsReady()) {
			return;
		}

		galaxy::api::Stats()->ResetStatsAndAchievements(this);

		if (const auto err = galaxy::api::GetError()) {
			logger->Error("Failed to reset achievements: {0}", err->GetMsg());
		}
	}

	// Callbacks
	void GalaxyStatsManager::OnUserStatsAndAchievementsRetrieveSuccess(galaxy::api::GalaxyID userID)
	{
		logger->Info("Stats and achievements for user {0} retrieved", userID.GetRealID());
	}

	void GalaxyStatsManager::OnUserStatsAndAchievementsRetrieveFailure(
		galaxy::api::GalaxyID userID,
		galaxy::api::IUserStatsAndAchievementsRetrieveListener::FailureReason failureReason)
	{
		logger->Error(
			"Failed to retrieve stats and achievements for user {0}, reason: {1}",
			userID.GetRealID(),
			failureReason);
	}

	void GalaxyStatsManager::OnUserStatsAndAchievementsStoreSuccess()
	{
		logger->Info("User stats and achievements stored");
	}

	void GalaxyStatsManager::OnUserStatsAndAchievementsStoreFailure(
		galaxy::api::IStatsAndAchievementsStoreListener::FailureReason failureReason)
	{
		logger->Error("Failed to store user stats and achievements, reason: {0}", failureReason);
	}

	void GalaxyStatsManager::OnAchievementUnlocked(const char* name)
	{
		logger->Info("Achievement unlocked: {0}", name);
	}
}
