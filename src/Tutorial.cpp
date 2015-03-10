#include <cmath>

#include "Tutorial.hpp"
#include "LeapListener.hpp"
#include "../include/Leap.h"

using namespace Leap;
Tutorial::Tutorial(LeapListener& listener) : listener(listener){

}

void Tutorial::play(){

	if(isTutorialStillPlaying){
	
		if(playSound){
			
			if(runOnce){
				stepPlaying();
			}
			if (listener.playing){
				recordingSound = true;
				playSound = false;
				runOnce = true;
				//Graphics::hideSign();
			}
		}

		else if(recordingSound){
			if(runOnce){
				stepRecording();
			}
			if(listener.recording){
				menuOpenTT = true;
				recordingSound = false;
				runOnce = true;
				//Graphics::hideSign();
			}
		}	

		else if(menuOpenTT){
				if(runOnce){
					stepOpenMenu();
				}
				if(listener.menuOpen){
					turnTutorialOnOrOff();
					menuOpenTT = false;
					runOnce = true;
					//Graphics::hideSign();
			}
		}
	}
}

void Tutorial::turnTutorialOnOrOff(){
	isTutorialStillPlaying = !isTutorialStillPlaying;
}

void Tutorial::stepPlaying(){
	std::cout << "Hi, this is Leap Music. To play a sound, shut your hand with all fingers except the forefinger." << std::endl;
	//Graqhics::showSine("./src/graphics/textures/paly.tga");
	/*
	* Kod för att köra bild och ljud för detta event.
	*/
	runOnce = false;
	return;
}

void Tutorial::stepRecording(){
	std::cout << "Good, if you want to record the sound shut all fingers of the hand, even the forefinger." << std::endl;
	/*
	* Kod för att köra bild och ljud för detta event.
	*/
	runOnce = false;
	return;

}

void Tutorial::stepOpenMenu(){
	std::cout << "Open menu by turning the hand 180 degrees to the right or left." << std::endl; 
	/*
	* Kod för att köra bild och ljud för detta event.
	*/
	runOnce = false;
	return;
}

void Tutorial::stepSelectItem(){
	std::cout << "Select the menu item by move your hand to the item you want to select." << std::endl; 
	/*
	* Kod för att köra bild och ljud för detta event.
	*/
	runOnce = false;
	return;
}