#include <thread>
#include <chrono>
#include <cstdlib>

#include "LeapListener.hpp"
#include "Consumer.hpp"
#include "Mixer.hpp"
#include "Tutorial.hpp"
#include "graphics/Graphics.hpp"

Consumer::Consumer(LeapListener& listen, Sound& sound, Tutorial& tutorial) :
	listener(listen), sound(sound), tutorial(tutorial) {
		Entry entry1("TechnoViking: 'All heil das'." , [](void){
				std::cout << "TechnoViking: 'All heil das'. \n"; });
		menu.addEntry(entry1);

		Entry entry2("Delete last recorded track.", [&](void){
				sound.getMixer().deleteLastTrack();
				std::cout << "Deleted the last recorded track \n";
			});

		menu.addEntry(entry2);

		Entry entry3("Add common time.", [&](void){
				sound.getMixer().changeBeatPlaying();
				std::cout << "Turned on/off beat track.\n";
			});
		menu.addEntry(entry3);

		Entry entry4("Turn playback on/off", [&](void){
				sound.getMixer().changePlayBack();
				std::cout << "Turned on/off playback.\n";
			});
		menu.addEntry(entry4);

		//Entry entry5("Im fukking done!", [](void){exit(0);});
		//menu.addEntry(entry5);
}

void Consumer::startConsumeLoop() {
	while (true) {
		if(tutorial.isTutorialStillPlaying){
			tutorial.play();
		}

		palmPosition = listener.getPalmPosition();

		Graphics::handX = palmPosition.x;
		Graphics::handY = palmPosition.y;
		sound.getMixer().setVolume((palmPosition.x + 300) / 600);
		sound.getMixer().setToneFromC0(5.9 + palmPosition.y / 8.2);


		if (listener.menuOpen) {
			menu.openOrUpdateMenu(palmPosition);
			Graphics::openMenu();
		} else {
			menu.close();
			Graphics::closeMenu();
		}


		// If the user has switched to or from playing a note, tell that
		// to the subsystems. Only tell them on switches.
		if (playingLastFrame != listener.playing && !listener.menuOpen){
			playingLastFrame = !playingLastFrame;
			sound.getMixer().playing = playingLastFrame;
			Graphics::playing = playingLastFrame;
		}

		if(recordingLastFrame != listener.recording && !listener.menuOpen){
			recordingLastFrame = !recordingLastFrame;
			sound.getMixer().startOrStopRecording(recordingLastFrame);
			Graphics::recording = recordingLastFrame;
		}

		std::this_thread::sleep_for (std::chrono::milliseconds(1));
	}
}

void Consumer::threadEntry(LeapListener& listener, Sound& sound, Tutorial& tutorial) {
	Consumer* consumerPointer = new Consumer(listener, sound, tutorial);
	return consumerPointer->startConsumeLoop();
}
