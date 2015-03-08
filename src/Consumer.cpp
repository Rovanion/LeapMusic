#include <thread>
#include <chrono>
#include <cstdlib>

#include "LeapListener.hpp"
#include "Consumer.hpp"
#include "Mixer.hpp"
#include "graphics/Graphics.hpp"

Consumer::Consumer(LeapListener& listen, Sound& sound) :
	listener(listen), sound(sound), playingNote(false), recording(false) {
		Entry entry1("TechnoViking: 'All heil das'." , [](void){std::cout << "TechnoViking: 'All heil das'." << std::endl; });
		menu.addEntry(entry1);

		Entry entry2("DEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEL", [&](void){ sound.getMixer().deleteLastTrack(); });
		menu.addEntry(entry2);

		Entry entry3("Add common time.", [&](void){ sound.getMixer().changeBeatPlaying(); });
		menu.addEntry(entry3);

		Entry entry4("Turn playback on/off", [&](void){ sound.getMixer().changePlayBack(); });
		menu.addEntry(entry4);

		//Entry entry5("Im fukking done!", [](void){exit(0);});
		//menu.addEntry(entry5);
}

void Consumer::startConsumeLoop() {
	while (true) {
		palmPosition = listener.getPalmPosition();

		Graphics::handX = palmPosition.x;
		Graphics::handY = palmPosition.y;

		menuOpen = listener.getMenuOpen();
		if(menuOpen){
			menu.openOrUpdateMenu(palmPosition);
		} else{
			menu.close();
		}

		// If the user has switched to or from playing a note, tell that
		// to the sound system.
		if (playingNote != listener.isPlaying()){
			playingNote = !playingNote;
			sound.getMixer().playing = playingNote;
		}

		if(recording != listener.isRecording()){
			recording = !recording;
			sound.getMixer().startOrStopRecording(recording);
		}


		currentTone = listener.getFrequency();
		//Implement the mixing part hear. Save the old sine and add it to the other includeing the curentTone.

		sound.getMixer().setFrequency(currentTone);
		std::this_thread::sleep_for (std::chrono::milliseconds(1));
	}
}

void Consumer::threadEntry(LeapListener& listener, Sound& sound) {
	Consumer* consumerPointer = new Consumer(listener, sound);
	return consumerPointer->startConsumeLoop();
}
