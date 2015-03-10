#ifndef LEAPMUSIC_CONSUMER
#define LEAPMUSIC_CONSUMER

#include "LeapListener.hpp"
#include "Menu.hpp"
#include "Sound.hpp"

class Consumer{
 public:
	/**
	 * @brief Consumer is a consumer of data from LeapListener.
	 *
	 * @param listen A reference to the instance of LeapListener
	 * from which data is to be consumed.
	 * @param sound A reference to the Sound system used for output.
	 *
	 * @return Consumer A new instance of the Consumer class.
	 */
	Consumer(LeapListener& listen, Sound& sound);

	/**
	 * @brief Starts the loop which consumes data from the
	 * LeapListener specified in the constructor.
	 */
	void startConsumeLoop();
	static void threadEntry(LeapListener& listener, Sound& sound);

 private:
	LeapListener& listener;
	Sound& sound;
	bool playingLastFrame = false;
	bool recordingLastFrame = false;
	Menu menu;
	Leap::Vector palmPosition;
};

#endif
