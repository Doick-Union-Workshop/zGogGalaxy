#pragma once

#include <GalaxyApi.h>
#include <cstdint>
#include <memory>

namespace GOG
{
	class GalaxyClient : public galaxy::api::GlobalAuthListener
	{
	public:
		enum GogInitStatus {
			IN_PROGRESS = 1,
			PRODUCT_NOT_OWNED = 2,
			NOT_INSTALLED = 3,
			OFFLINE = 4,
			OK = 5
		};

		GalaxyClient();
		~GalaxyClient();

		[[nodiscard]] uint64_t GetGalaxyID() const;
		[[nodiscard]] bool IsSignedIn() const;
		[[nodiscard]] bool IsLoggedOut() const;

		int Init(const char* clientId, const char* clientSecret, bool online = true);
		void ProcessData();
		void Shutdown();
		void SignIn(bool online = false);
		void SignOut();

	private:
		int GogStatus = GogInitStatus::NOT_INSTALLED;
		inline static Utils::Logger* logger = Utils::CreateLogger("zGogGalaxy::GalaxyClient");

		// Callbacks
		void OnAuthSuccess() override;
		void OnAuthFailure(galaxy::api::IAuthListener::FailureReason reason) override;
		void OnAuthLost() override;
	};

	inline std::unique_ptr<GalaxyClient> galaxyClient;

	GalaxyClient::GalaxyClient()
	{
		GogStatus = GogInitStatus::NOT_INSTALLED;
	}

	GalaxyClient::~GalaxyClient()
	{
		Shutdown();
	}

	int GalaxyClient::Init(const char* clientId, const char* clientSecret, bool online)
	{
		logger->Info("Initializing GOG Galaxy...");
		galaxy::api::Init({ clientId, clientSecret });

		if (const auto err = galaxy::api::GetError())
		{
			logger->Error("GOG Galaxy not installed: {0}", err->GetMsg());
			GogStatus = GogInitStatus::NOT_INSTALLED;
		}
		else
		{
			GogStatus = GogInitStatus::IN_PROGRESS;
			SignIn(online);
		}

		return GogStatus;
	}

	void GalaxyClient::ProcessData()
	{
		if (GogStatus == GogInitStatus::NOT_INSTALLED) {
			return;
		}
		galaxy::api::ProcessData();
	}

	void GalaxyClient::Shutdown()
	{
		if (GogStatus < GogInitStatus::OFFLINE) {
			return;
		}
		galaxy::api::Shutdown();
	}

	// User
	uint64_t GalaxyClient::GetGalaxyID() const
	{
		if (GogStatus < GogInitStatus::OFFLINE) {
			return 0;
		}
		galaxy::api::GalaxyID galaxyID = galaxy::api::User()->GetGalaxyID();
		return galaxyID.GetRealID();
	}

	void GalaxyClient::SignIn(bool online)
	{
		if (GogStatus == GogInitStatus::NOT_INSTALLED) {
			return;
		}

		logger->Info("Signing in...");
		galaxy::api::User()->SignInGalaxy(online, 15U, this);

		if (const auto err = galaxy::api::GetError())
		{
			logger->Error("Signing in error: {0}", err->GetMsg());
			GogStatus = GogInitStatus::PRODUCT_NOT_OWNED;
		}
		else if (online) {
			GogStatus = GogInitStatus::IN_PROGRESS;
		}
		else
		{
			GogStatus = GogInitStatus::OFFLINE;
			logger->Info("Signed in offline");
		}
	}

	void GalaxyClient::SignOut()
	{
		if (GogStatus < GogInitStatus::OFFLINE) {
			return;
		}
		galaxy::api::User()->SignOut();
	}

	bool GalaxyClient::IsSignedIn() const
	{
		if (GogStatus < GogInitStatus::OFFLINE) {
			return false;
		}
		return galaxy::api::User()->SignedIn();
	}

	bool GalaxyClient::IsLoggedOut() const
	{
		if (GogStatus < GogInitStatus::OFFLINE) {
			return true;
		}
		return !galaxy::api::User()->IsLoggedOn();
	}

	// Callbacks
	void GalaxyClient::OnAuthSuccess()
	{
		GogStatus = GogInitStatus::OK;
		logger->Info("GOG Galaxy successfully initialized");
		GOG::galaxyStatsManager->QueryAchievements();
	}

	void GalaxyClient::OnAuthFailure(galaxy::api::IAuthListener::FailureReason reason)
	{
		switch (reason)
		{
		case galaxy::api::IAuthListener::FailureReason::FAILURE_REASON_GALAXY_NOT_INITIALIZED:
			logger->Error("GOG Galaxy not initialized, call Init() first!");
			[[fallthrough]];
		case galaxy::api::IAuthListener::FailureReason::FAILURE_REASON_UNDEFINED:
		case galaxy::api::IAuthListener::FailureReason::FAILURE_REASON_GALAXY_SERVICE_NOT_AVAILABLE:
			GogStatus = GogInitStatus::NOT_INSTALLED;
			break;
		case galaxy::api::IAuthListener::FailureReason::FAILURE_REASON_GALAXY_SERVICE_NOT_SIGNED_IN:
		case galaxy::api::IAuthListener::FailureReason::FAILURE_REASON_CONNECTION_FAILURE:
		case galaxy::api::IAuthListener::FailureReason::FAILURE_REASON_INVALID_CREDENTIALS:
		case galaxy::api::IAuthListener::FailureReason::FAILURE_REASON_EXTERNAL_SERVICE_FAILURE:
			GogStatus = GogInitStatus::OFFLINE;
			break;
		case galaxy::api::IAuthListener::FailureReason::FAILURE_REASON_NO_LICENSE:
			GogStatus = GogInitStatus::PRODUCT_NOT_OWNED;
			break;
		}
	}

	void GalaxyClient::OnAuthLost()
	{
		galaxy::api::User()->SignOut();
		GogStatus = GogInitStatus::OFFLINE;
		logger->Error("GOG Galaxy not initialized, call Init() first!");
	}
}
