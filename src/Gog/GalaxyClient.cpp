#include "GalaxyClient.hpp"
#include "GalaxyStatsManager.hpp"
#include <Utils/zDUtils/Logger.h>

namespace GOG
{
	GalaxyClient* GalaxyClient::singleton = NULL;

	GalaxyClient::GalaxyClient()
	{
		GogStatus = GOG_INIT_NOT_INSTALLED;
		singleton = this;
	}

	GalaxyClient* GalaxyClient::GetSingleton()
	{
		if (singleton == NULL)
		{
			singleton = new GalaxyClient();
		}

		return singleton;
	}

	GalaxyClient::~GalaxyClient()
	{
		galaxy::api::Shutdown();
		singleton = NULL;
	}

	int GalaxyClient::Init(const char* clientId, const char* clientSecret, bool online)
	{
		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyClient::Init");

		log->Info("Initializing GOG Galaxy...");
		galaxy::api::Init({ clientId, clientSecret });

		auto err = galaxy::api::GetError();
		if (err)
		{
			log->Error("GOG Galaxy not installed: {0}", err->GetMsg());
			GogStatus = GOG_INIT_NOT_INSTALLED;
		}
		else
		{
			GogStatus = GOG_INIT_IN_PROGRESS;
			SignIn(online);
		}

		return GogStatus;
	}

	void GalaxyClient::ProcessData()
	{
		if (GogStatus == GOG_INIT_NOT_INSTALLED) return;
		galaxy::api::ProcessData();
	}

	void GalaxyClient::Shutdown()
	{
		if (GogStatus < GOG_INIT_OFFLINE) return;
		galaxy::api::Shutdown();
	}

	// User
	uint64_t GalaxyClient::GetGalaxyID()
	{
		if (GogStatus < GOG_INIT_OFFLINE) return 0;
		galaxy::api::GalaxyID galaxyID = galaxy::api::User()->GetGalaxyID();
		return galaxyID.GetRealID();
	}

	void GalaxyClient::SignIn(bool online)
	{
		if (GogStatus == GOG_INIT_NOT_INSTALLED) return;

		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyClient::SignIn");

		log->Info("Signing in...");
		galaxy::api::User()->SignInGalaxy(online, 15U, this);

		auto err = galaxy::api::GetError();
		if (err)
		{
			log->Error("Signing in error: {0}", err->GetMsg());
			GogStatus = GOG_INIT_PRODUCT_NOT_OWNED;
		}
		else if (online) {
			GogStatus = GOG_INIT_IN_PROGRESS;
		}
		else
		{
			GogStatus = GOG_INIT_OFFLINE;
			log->Info("Signed in offline");
		}
	}

	void GalaxyClient::SignOut()
	{
		if (GogStatus < GOG_INIT_OFFLINE) return;
		galaxy::api::User()->SignOut();
	}

	bool GalaxyClient::IsSignedIn()
	{
		if (GogStatus < GOG_INIT_OFFLINE)
			return false;
		return galaxy::api::User()->SignedIn();
	}

	bool GalaxyClient::IsLoggedOut() {
		if (GogStatus < GOG_INIT_OFFLINE)
			return true;
		return !galaxy::api::User()->IsLoggedOn();
	}

	// Callbacks
	void GalaxyClient::OnAuthSuccess()
	{
		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyClient::OnAuthSuccess");
		GogStatus = GOG_INIT_OK;
		log->Info("GOG Galaxy successfully initialized");
		GalaxyStatsManager::GetSingleton()->QueryAchievements();
	}

	void GalaxyClient::OnAuthFailure(galaxy::api::IAuthListener::FailureReason reason)
	{
		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyClient::OnAuthFailure");

		switch (reason)
		{
			case galaxy::api::IAuthListener::FailureReason::FAILURE_REASON_GALAXY_NOT_INITIALIZED:
				log->Warning("GOG Galaxy not initialized, call Init() first!");

			case galaxy::api::IAuthListener::FailureReason::FAILURE_REASON_UNDEFINED:
			case galaxy::api::IAuthListener::FailureReason::FAILURE_REASON_GALAXY_SERVICE_NOT_AVAILABLE:
				GogStatus = GOG_INIT_NOT_INSTALLED;
				break;
			case galaxy::api::IAuthListener::FailureReason::FAILURE_REASON_GALAXY_SERVICE_NOT_SIGNED_IN:
			case galaxy::api::IAuthListener::FailureReason::FAILURE_REASON_CONNECTION_FAILURE:
			case galaxy::api::IAuthListener::FailureReason::FAILURE_REASON_INVALID_CREDENTIALS:
			case galaxy::api::IAuthListener::FailureReason::FAILURE_REASON_EXTERNAL_SERVICE_FAILURE:
				GogStatus = GOG_INIT_OFFLINE;
				break;

			case galaxy::api::IAuthListener::FailureReason::FAILURE_REASON_NO_LICENSE:
				GogStatus = GOG_INIT_PRODUCT_NOT_OWNED;
				break;
		}
	}

	void GalaxyClient::OnAuthLost()
	{
		static Utils::Logger* log = Utils::CreateLogger("zGogGalaxy::GalaxyClient::OnAuthLost");
		galaxy::api::User()->SignOut();
		GogStatus = GOG_INIT_OFFLINE;
		log->Error("GOG Galaxy not initialized, call Init() first!");
	}
}