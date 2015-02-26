#include <cmath>
#include <algorithm>
#include <mutex>

#include "../include/Leap.h"
#include "LeapListener.hpp"
#include "Matte.hpp"
#include "Menu.hpp"
#include "Sound.hpp"

using namespace Leap;

LeapListener::LeapListener(const LeapListener& from):
	tone(from.tone),
	menuOpen(from.menuOpen),
	playing(from.playing),
	recording(from.recording) {

}

LeapListener& LeapListener::operator=(const LeapListener& rhs) {
	if (this != &rhs){
		tone = rhs.tone;
		menuOpen = rhs.menuOpen;
		playing = rhs.playing;
		recording = rhs.recording;
	}
	return *this;
}

void LeapListener::onInit(const Controller&) {
	std::cout << "Initialized" << std::endl;
}

void LeapListener::onConnect(const Controller&) {
	std::cout << "Connected" << std::endl;
}

void LeapListener::onDisconnect(const Controller&) {
	// Note: not dispatched when running in a debugger.
	std::cout << "Disconnected" << std::endl;
}

void LeapListener::onExit(const Controller&) {
	std::cout << "Exited" << std::endl;
}

void LeapListener::onFrame(const Controller& controller) {
	// Get the most recent frame and report some basic information
	const Frame frame = controller.frame();
	if (DEBUG) {
		std::cout << "Frame id: " << frame.id()
						<< ", timestamp: " << frame.timestamp()
						<< ", hands: " << frame.hands().count()
						<< ", extended fingers: " << frame.fingers().extended().count()
						<< ", tools: " << frame.tools().count()
						<< ", gestures: " << frame.gestures().count() << std::endl;
	}

	HandList hands = frame.hands();
	for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
		// Get the first hand
		const Hand hand = *hl;
		std::string handType = hand.isLeft() ? "Left hand" : "Right hand";
		if (DEBUG) {
		std::cout << std::string(2, ' ') << handType << ", id: " << hand.id()
							<< ", palm position: " << hand.palmPosition() << std::endl;
		}

		// Get the hand's normal vector and direction
		const Vector normal = hand.palmNormal();
		const Vector direction = hand.direction();
		{
			std::lock_guard<std::mutex> lock(palmPositionLock);
			palmPosition = hand.palmPosition();
		}
		{
			std::lock_guard<std::mutex> lock(frequencyLock);
			frequency = Matte::linearToDb(hand.palmPosition().y); 
			std::cout << frequency << std::endl;
		}
		if (DEBUG) {
		// Calculate the hand's pitch, roll, and yaw angles
		std::cout << std::string(2, ' ') <<	 "pitch: " << direction.pitch() * RAD_TO_DEG << " degrees, "
							<< "roll: " << normal.roll() * RAD_TO_DEG << " degrees, "
							<< "yaw: " << direction.yaw() * RAD_TO_DEG << " degrees" << std::endl;
		}

		// Get the Arm bone
		Arm arm = hand.arm();
		if (DEBUG) {
			std::cout << std::string(2, ' ') <<	 "Arm direction: " << arm.direction()
								<< " wrist position: " << arm.wristPosition()
								<< " elbow position: " << arm.elbowPosition() << std::endl;
		}

		bool fingersClosed[] = {true, false, false, false, false};
		// Get fingers
		const FingerList fingers = hand.fingers();
		// The coordinates of the top of the thumb and index fingers.
		Vector indexPoint, thumbPoint;
		for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl) {
			const Finger finger = *fl;
			if (DEBUG){
				std::cout << std::string(4, ' ') <<	 fingerNames[finger.type()]
									<< " finger, id: " << finger.id()
									<< ", length: " << finger.length()
									<< "mm, width: " << finger.width() << std::endl;
			}

			// Get finger bones
			for (int b = 0; b < 4; ++b) {
				Bone::Type boneType = static_cast<Bone::Type>(b);
				Bone bone = finger.bone(boneType);
				if (DEBUG) {
					std::cout << std::string(6, ' ') <<	 boneNames[boneType]
										<< " bone, start: " << bone.prevJoint()
										<< ", end: " << bone.nextJoint()
										<< ", direction: " << bone.direction() << std::endl;
				}
			}
			// Figure out some specific hand positions.
			if(finger.type() != Finger::TYPE_THUMB){
				Vector metacarpalDirection = finger.bone(Bone::TYPE_METACARPAL).direction();
				Vector intermediateDirection = finger.bone(Bone::TYPE_INTERMEDIATE).direction();
				if(! Matte::fuzzyAngleEquals(metacarpalDirection, intermediateDirection, 2)){
					if (DEBUG) {
						std::cout << "Finger closed. ";
					}
					fingersClosed[finger.type()] = true;
				}
				if(finger.type() == Finger::TYPE_INDEX)
					indexPoint = finger.bone(Bone::TYPE_DISTAL).nextJoint();
			}
			else
				thumbPoint = finger.bone(Bone::TYPE_DISTAL).nextJoint();
		}

		// Check if all the positions in the array are set to true
		if (std::all_of(std::begin(fingersClosed),
										std::end(fingersClosed),
										[](bool b){
											return b;
										})) {
			std::cout << "Recording.       ";
			std::lock_guard<std::mutex> lock(recordPlayingLock);
			recording = true;
			playing = true;
		}
		else if (fingersClosed[Finger::TYPE_PINKY] && fingersClosed[Finger::TYPE_RING]) {
			std::cout << "Playing.       ";
			std::lock_guard<std::mutex> lock(recordPlayingLock);
			playing = true;
			recording = false;
		}
		else {
			std::cout << "The hand is open.      ";
			std::lock_guard<std::mutex> lock(recordPlayingLock);
			recording = false;
			playing = false;
		}

		// Is the hard turned upside down?
		if (abs(hand.palmNormal().roll() * RAD_TO_DEG) > 120){
			std::cout << "Menu open. " << std::endl;
			std::lock_guard<std::mutex> lock(menuLock);
			menuOpen = true;
		}
		else {
			std::cout << "Menu Closed. " << std::endl;
			std::lock_guard<std::mutex> lock(menuLock);
			menuOpen = false;
		}

		if( Matte::fuzzyEquals(indexPoint, thumbPoint, 5))
			if (DEBUG) {
				std::cout << "Index finger and thumb touching. ";
			}
	}

	// Get tools
	const ToolList tools = frame.tools();
	for (ToolList::const_iterator tl = tools.begin(); tl != tools.end(); ++tl) {
		const Tool tool = *tl;
		std::cout << std::string(2, ' ') <<	 "Tool, id: " << tool.id()
							<< ", position: " << tool.tipPosition()
							<< ", direction: " << tool.direction() << std::endl;
	}
}

void LeapListener::onFocusGained(const Controller&) {
	std::cout << "Focus Gained" << std::endl;
}

void LeapListener::onFocusLost(const Controller&) {
	std::cout << "Focus Lost" << std::endl;
}

void LeapListener::onDeviceChange(const Controller& controller) {
	std::cout << "Device Changed" << std::endl;
	const DeviceList devices = controller.devices();

	for (int i = 0; i < devices.count(); ++i) {
		std::cout << "id: " << devices[i].toString() << std::endl;
		std::cout << "	isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
	}
}

void LeapListener::onServiceConnect(const Controller&) {
	std::cout << "Service Connected" << std::endl;
}

void LeapListener::onServiceDisconnect(const Controller&) {
	std::cout << "Service Disconnected" << std::endl;
}


bool LeapListener::getMenuOpen() {
	std::lock_guard<std::mutex> lock(menuLock);
	return menuOpen;
}


double LeapListener::getTone() {
	std::lock_guard<std::mutex> lock(toneLock);
	return tone;
}


bool LeapListener::isRecording() {
	std::lock_guard<std::mutex> lock(recordPlayingLock);
	return recording;
}


bool LeapListener::isPlaying() {
	std::lock_guard<std::mutex> lock(recordPlayingLock);
	return playing;
}

float LeapListener::getFrequency() {
	std::lock_guard<std::mutex> lock(frequencyLock);
	return frequency;
}

Vector LeapListener::getPalmPosition(){
	std::lock_guard<std::mutex> lock(palmPositionLock);
	return palmPosition;
}

