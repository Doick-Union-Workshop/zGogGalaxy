#include "GalaxyClient.hpp"
#include "GalaxyStatsManager.hpp"
#include <Utils/zDUtils/Logger.h>

namespace GOG
{
	GalaxyClient* GalaxyClient::singleton = nullptr;

	GalaxyClient::GalaxyClient()
	{
		GogStatus = GogInitStatus::NOT_INSTALLED;
		singleton = this;
	}

	GalaxyClient* GalaxyClient::GetSingleton()
	{
		if (singleton == nullptr)
			singleton = new GalaxyClient();
		return singleton;
	}

	GalaxyClient::~GalaxyClient()
	{
		galaxy::api::Shutdown();
		singleton = nullptr;
	}

	int GalaxyClient::Init(const char* clientId, const char* clientSecret, bool online)
	{
		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyClient::Init");

		log->Info("Initializing GOG Galaxy...");
		galaxy::api::Init({ clientId, clientSecret });

		if (const auto err = galaxy::api::GetError())
		{
			log->Error("GOG Galaxy not installed: {0}", err->GetMsg());
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
		if (GogStatus == GogInitStatus::NOT_INSTALLED) return;
		galaxy::api::ProcessData();
	}

	void GalaxyClient::Shutdown()
	{
		if (GogStatus < GogInitStatus::OFFLINE) return;
		galaxy::api::Shutdown();
	}

	// User
	uint64_t GalaxyClient::GetGalaxyID() const
	{
		if (GogStatus < GogInitStatus::OFFLINE) return 0;
		galaxy::api::GalaxyID galaxyID = galaxy::api::User()->GetGalaxyID();
		return galaxyID.GetRealID();
	}

	void GalaxyClient::SignIn(bool online)
	{
		if (GogStatus == GogInitStatus::NOT_INSTALLED) return;

		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyClient::SignIn");

		log->Info("Signing in...");
		galaxy::api::User()->SignInGalaxy(online, 15U, this);

		if (const auto err = galaxy::api::GetError())
		{
			log->Error("Signing in error: {0}", err->GetMsg());
			GogStatus = GogInitStatus::PRODUCT_NOT_OWNED;
		}
		else if (online) {
			GogStatus = GogInitStatus::IN_PROGRESS;
		}
		else
		{
			GogStatus = GogInitStatus::OFFLINE;
			log->Info("Signed in offline");
		}
	}

	void GalaxyClient::SignOut()
	{
		if (GogStatus < GogInitStatus::OFFLINE) return;
		galaxy::api::User()->SignOut();
	}

	bool GalaxyClient::IsSignedIn() const
	{
		if (GogStatus < GogInitStatus::OFFLINE)
			return false;
		return galaxy::api::User()->SignedIn();
	}

	bool GalaxyClient::IsLoggedOut() const
	{
		if (GogStatus < GogInitStatus::OFFLINE)
			return true;
		return !galaxy::api::User()->IsLoggedOn();
	}

	// Callbacks
	void GalaxyClient::OnAuthSuccess()
	{
		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyClient::OnAuthSuccess");
		GogStatus = GogInitStatus::OK;
		log->Info("GOG Galaxy successfully initialized");
		GalaxyStatsManager::GetSingleton()->QueryAchievements();
	}

	void GalaxyClient::OnAuthFailure(galaxy::api::IAuthListener::FailureReason reason)
	{
		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyClient::OnAuthFailure");

		switch (reason)
		{
			case galaxy::api::IAuthListener::FailureReason::FAILURE_REASON_GALAXY_NOT_INITIALIZED:
				log->Error("GOG Galaxy not initialized, call Init() first!");
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
		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyClient::OnAuthLost");
		galaxy::api::User()->SignOut();
		GogStatus = GogInitStatus::OFFLINE;
		log->Error("GOG Galaxy not initialized, call Init() first!");
	}
}