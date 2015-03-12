#ifndef LEAPMUSIC_MENU
#define LEAPMUSIC_MENU

#include "../include/Leap.h"
#include "Entry.hpp"

class Menu{
 public:
	void openOrUpdateMenu(Leap::Vector handPosition);
	void close();
	void addEntry();
	void addEntry(Entry e);
	void removeEntry(Entry e);
	std::vector<Entry> entries;
	bool isOpen = false;
	bool justClosed = false;

 private:
	const float MENU_SIZE = 60;
	const bool DEBUG = false;
	int nrOfEntries = 0;
	Leap::Vector menuCenter;
};

#endif
