#pragma once

#include <GalaxyApi.h>
#include <cstdint>

namespace GOG
{
	class GalaxyClient : public galaxy::api::GlobalAuthListener {

	protected:
		static GalaxyClient* singleton;

	public:
		enum {
			GOG_INIT_IN_PROGRESS = 1,
			GOG_INIT_PRODUCT_NOT_OWNED = 2,
			GOG_INIT_NOT_INSTALLED = 3,
			GOG_INIT_OFFLINE = 4,
			GOG_INIT_OK = 5
		};

		static GalaxyClient* GetSingleton();
		GalaxyClient();
		~GalaxyClient();

		// Initialization
		int Init(const char* clientId, const char* clientSecret, bool online = true);
		void ProcessData();
		void Shutdown();

		// User
		uint64_t GetGalaxyID();
		void SignIn(bool online = false);
		void SignOut();
		bool IsSignedIn();
		bool IsLoggedOut();

	private:
		int GogStatus = GOG_INIT_NOT_INSTALLED;

		// Callbacks
		void OnAuthSuccess() override;
		void OnAuthFailure(galaxy::api::IAuthListener::FailureReason reason) override;
		void OnAuthLost() override;
	};
}
