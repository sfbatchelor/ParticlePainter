#pragma once
#include "stdafx.h"
#include <map>

#ifndef SYNC_EXE_LOCATION
#define SYNC_EXE_LOCATION "editor.exe"
#endif
#ifndef SYNC_EXE
#define SYNC_EXE "editor.exe"
#endif

extern "C"
{
#include "base\sync.h"
#include "base\device.h"
}
#include "utils\ProcessUtils.h"
#include "ofLog.h"


//! Namespace that encapsulates sync interface to connect to the
//! server and send/receive data to/from it (has to be static data as callback pointers are void*)
// SYNC_PLAYER macro defines when the program reads from a .track file instead of connecting with rocket
namespace SyncTracker
{

	struct AdvancedSyncData 
		//! struct that is passed to the C rocket api, and then through to the SLOT callback functions, can be used to 
		//! define advanced data fields
	{
	};
	static std::map<std::string, const sync_track*> s_syncedTracks;
	static std::map<const sync_track*, double> s_syncedValues;

#ifndef SYNC_PLAYER
	static sync_cb s_callbackStruct;
#endif

	static bool s_playing = false;
	static int s_frame = 0;
	static bool s_dirtyFrame = true;
	static sync_device* s_device;
	static AdvancedSyncData s_advancedSyncData;


	//! callback from server when sync is paused
	static void pauseSlot(void *d, int flag)
	{
		s_playing = false;
		//LOG("SYNC SLOT CALLED: pause");
	}

	//! callback from server when sync is paused and the rows are being scrubbed
	static void setRowSlot(void *d, int row)
	{
		s_frame = row;
		s_dirtyFrame = true;
		//LOG("SYNC SLOT CALLED: set row [", row, "]");
	}

	//! callback returning to server when app is playing 
	static int isPlayingSlot(void *d)
	{
		return s_playing;
	}

	//! File prefix is the naem of the track file the sync tracker will export 
	static void createDevice(const char * filePrefix)
	{
		s_device = sync_create_device(filePrefix);

#ifndef SYNC_PLAYER
		s_callbackStruct = {
			SyncTracker::pauseSlot,
			SyncTracker::setRowSlot,
			SyncTracker::isPlayingSlot
		};
#endif

	}

	//! Connect to host/port, returns if connection failed.
	static bool connect(const char* host = "localhost", unsigned short port = SYNC_DEFAULT_PORT)
	{
#ifndef SYNC_PLAYER
		return (bool)sync_tcp_connect(s_device, host, port);
#else 
		return 0;
#endif
	}

	//! Update Sever/Client connection and give the current frame to the sync tracker, 
	//! if the tracker is playing this value will be used.
	static bool update(int clientFrame)
	{
		if (s_dirtyFrame)
		{
			s_dirtyFrame = false;
		}
		else 
			s_frame = clientFrame;


#ifndef SYNC_PLAYER
		if (sync_update(s_device, floor(s_frame), &s_callbackStruct, &s_advancedSyncData) == -1)
		{
			char args[1]  ;
			//ProcessUtils::startProcessIfNotRunning(SYNC_EXE_LOCATION, SYNC_EXE,args);
			return false;
		}
#endif			
		for (auto it = s_syncedTracks.begin(); it != s_syncedTracks.end(); it++)
		{
			s_syncedValues[it->second] = sync_get_val(it->second, s_frame);
		}
		return true;

	}
	static void killDevice()
	{
		s_syncedTracks.clear();
		s_syncedValues.clear();
		if (s_device != NULL)
		{
			ofLogNotice() << "Killing Sync Connection Device";
			sync_destroy_device(s_device);
		}
	}

	static bool isDeviceAlive()
	{
		if (s_device == NULL)
			return false;
		return true;
	}

	static void saveTracks()
	{
#ifndef SYNC_PLAYER
		if (sync_save_tracks(s_device) == -1)
		{
			ofLogError() << "Failed to save synced tracks";
		}
		else
		{
			ofLogNotice() << "Saved synced tracks";
		}
#endif
	}

	static void saveAndDisconnect()
	{
		saveTracks();
		killDevice();

	}

	static void play()
	{
		s_playing = true;
	}

	static void pause()
	{
		s_playing = false;
	}


	//! set a value to instance and receive on the sync server
	static void addSyncValue(const char * name)
	{
		const sync_track* track;

		track = sync_get_track(s_device, name);
		s_syncedTracks[name] = track;
		double value = sync_get_val(track, s_frame);
		//LOG("Gotten  value [", value, "] from track: [", name, "]");
		s_syncedValues[track] = value;
	}

	//! get a value from the sync server
	static double getSyncValue(const char * name)
	{
		const sync_track* track;
		if (s_syncedTracks.find(name) == s_syncedTracks.end())
		{
			ofLogError() << "Failed to get Synced Value[" << name << "] : track not found";
			return 0.0;
		}
		else
		{
			track = s_syncedTracks[name];
		}
		if (s_syncedValues.find(track) == s_syncedValues.end())
		{
			ofLogError() << "Failed to get Synced Value[" << name << "] :has a track but no value!";
			return 0.0;
		}
		else
		{
			return s_syncedValues[track];
		}
	}

	//!  retreive the current frame from the tracker, 
	//! use this when paused and scrubbing the tracker
	static int getFrame()
	{
		return s_frame;
	}

	static bool isPlaying()
	{
		return s_playing;
	}


};