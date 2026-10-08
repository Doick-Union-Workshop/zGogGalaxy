#pragma once

#include <GalaxyApi.h>
#include <memory>
#include <cstdint>

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

		// Callbacks
		void OnAuthSuccess() override;
		void OnAuthFailure(galaxy::api::IAuthListener::FailureReason reason) override;
		void OnAuthLost() override;
	};

	extern std::unique_ptr<GalaxyClient> galaxyClient;
}
