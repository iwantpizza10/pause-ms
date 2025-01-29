#include <Geode/Geode.hpp>
#include <chrono>
#include <string>

using namespace geode::prelude;

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>

std::chrono::steady_clock::time_point deathtime = std::chrono::steady_clock::now();

auto trickery = Mod::get()->getSettingValue<bool>("trickery");
auto trickerythreshold = Mod::get()->getSettingValue<int64_t>("trickery-threshold");

$execute {
	listenForSettingChanges("trickery", [](bool value) {
		trickery = value;
	});

	listenForSettingChanges("trickery-threshold", [](int64_t value) {
		trickerythreshold = value;
	});
};

class $modify(BetterPlayerObject, PlayerObject) {
	void playerDestroyed(bool p0) {
		deathtime = std::chrono::steady_clock::now();

		PlayerObject::playerDestroyed(p0);
	}
};

class $modify(BetterPlayLayer, PlayLayer) {
	void pauseGame(bool p0) {
		auto pausetime = std::chrono::steady_clock::now();

		std::chrono::duration<double> diff = pausetime - deathtime;
		float elapsed = diff.count() * 1000;

		if (elapsed <= 1000) {
			int rounded = std::round(elapsed);
			auto strversion = std::to_string(rounded);

			auto sprite = CCSprite::createWithSpriteFrameName("GJ_pauseBtn_001.png");
			Notification::create("Paused in " + strversion + "ms", sprite, 0.50)->show();

			if (trickery == true && elapsed <= trickerythreshold) {
				PlayLayer::pauseGame(p0);

				FMODAudioEngine::get()->stopAllEffects(); // prevent death sound from playing when in pause screen
				FMODAudioEngine::get()->playEffect("achievement_01.ogg");
			} else {
				PlayLayer::pauseGame(p0);
			}
		} else {
			PlayLayer::pauseGame(p0);
		}
	}
};
