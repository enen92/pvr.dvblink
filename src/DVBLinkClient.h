/*
 *      Copyright (C) 2005-2012 Team XBMC
 *      http://xbmc.org
 
 *      Copyright (C) 2012 Palle Ehmsen(Barcode Madness)
 *      http://www.barcodemadness.com
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301  USA
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#pragma once

#include "p8-platform/os.h"
#include "p8-platform/threads/threads.h"
#include "p8-platform/threads/mutex.h"
#include "p8-platform/util/util.h"
#include "libdvblinkremote/dvblinkremote.h"
#include "HttpPostClient.h"
#include "TimeShiftBuffer.h"
#include "xbmc_pvr_types.h"
#include "libXBMC_addon.h"
#include "libXBMC_pvr.h"
#include "libKODI_guilib.h"
#include "dvblink_connection.h"
#include "client.h"
#include <map>

#define DVBLINK_BUILD_IN_RECORDER_SOURCE_ID   "8F94B459-EFC0-4D91-9B29-EC3D72E92677"
#define DVBLINK_RECODINGS_BY_DATA_ID   "F6F08949-2A07-4074-9E9D-423D877270BB"
#define DVBLINK_RECODINGS_BY_SERIES_ID   "0E03FEB8-BD8F-46e7-B3EF-34F6890FB458"

typedef std::map<std::string, std::string> recording_id_to_url_map_t;

/* timer type ids */
#define TIMER_ONCE_MANUAL			(PVR_TIMER_TYPE_NONE + 1)
#define TIMER_ONCE_EPG				(PVR_TIMER_TYPE_NONE + 2)
#define TIMER_ONCE_MANUAL_CHILD		(PVR_TIMER_TYPE_NONE + 3)
#define TIMER_ONCE_EPG_CHILD		(PVR_TIMER_TYPE_NONE + 4)
#define TIMER_ONCE_KEYWORD_CHILD	(PVR_TIMER_TYPE_NONE + 5)
#define TIMER_REPEATING_MANUAL		(PVR_TIMER_TYPE_NONE + 6)
#define TIMER_REPEATING_EPG			(PVR_TIMER_TYPE_NONE + 7)
#define TIMER_REPEATING_KEYWORD		(PVR_TIMER_TYPE_NONE + 8)

enum dvblink_client_rec_num_e
{
  dcrn_keep_all = 0,
  dcrn_keep_1 = 1,
  dcrn_keep_2 = 2,
  dcrn_keep_3 = 3,
  dcrn_keep_4 = 4,
  dcrn_keep_5 = 5,
  dcrn_keep_6 = 6,
  dcrn_keep_7 = 7,
  dcrn_keep_10 = 10
};

enum dvblink_client_rec_showtype_e
{
  dcrs_record_all = 0, dcrs_record_new_only = 1
};

/* dvblink update interval values */
const int UPDATE_INTERVAL_60_SEC   = 60;
const int UPDATE_INTERVAL_120_SEC  = 120;
const int UPDATE_INTERVAL_180_SEC  = 180;
const int UPDATE_INTERVAL_240_SEC  = 240;
const int UPDATE_INTERVAL_300_SEC  = 300;
const int UPDATE_INTERVAL_360_SEC  = 360;
const int UPDATE_INTERVAL_420_SEC  = 420;
const int UPDATE_INTERVAL_480_SEC  = 480;
const int UPDATE_INTERVAL_540_SEC  = 540;
const int UPDATE_INTERVAL_600_SEC  = 600;
const int UPDATE_INTERVAL_1200_SEC = 1200;
const int UPDATE_INTERVAL_1800_SEC = 1800;
const int UPDATE_INTERVAL_2400_SEC = 2400;
const int UPDATE_INTERVAL_3000_SEC = 3000;
const int UPDATE_INTERVAL_3600_SEC = 3600;

struct schedule_desc
{
  schedule_desc(unsigned int idx, int type, int margin_before, int margin_after)
  {
    schedule_kodi_idx = idx;
    schedule_kodi_type = type;
    schedule_margin_before = margin_before;
    schedule_margin_after = margin_after;
  }

  schedule_desc() :
    schedule_kodi_idx(PVR_TIMER_NO_CLIENT_INDEX), schedule_kodi_type(PVR_TIMER_TYPE_NONE), schedule_margin_before(0), schedule_margin_after(0)
  {
  }

  unsigned int schedule_kodi_idx;
  int schedule_kodi_type;
  int schedule_margin_before;
  int schedule_margin_after;
};

struct dvblink_server_caps
{
  dvblink_server_caps() :
    setting_margins_supported_(false),
    favorites_supported_(false),
    transcoding_supported_(false),
    transcoding_recordings_supported_(false),
    recordings_supported_(false),
    timeshifting_supported_(false),
    device_management_supported_(false),
    timeshift_commands_supported_(false),
    resume_supported_(false),
    start_any_time_supported_(false)
  {}

  std::string server_version_;
  std::string server_build_;
  bool setting_margins_supported_;
  bool favorites_supported_;
  bool transcoding_supported_;
  bool transcoding_recordings_supported_;
  bool recordings_supported_;
  bool timeshifting_supported_;
  bool device_management_supported_;
  bool timeshift_commands_supported_;
  bool resume_supported_;
  bool start_any_time_supported_;
};

class DVBLinkClient: public P8PLATFORM::CThread
{
public:
  DVBLinkClient(ADDON::CHelper_libXBMC_addon* xbmc, CHelper_libXBMC_pvr* pvr, CHelper_libKODI_guilib* gui,
      std::string clientname, std::string hostname, long port, bool showinfomsg, std::string username,
      std::string password, bool add_episode_to_rec_title, bool group_recordings_by_series, bool no_group_single_rec, int default_update_interval, int default_rec_show_type);
  ~DVBLinkClient(void);
  const char *GetBackendVersion();
  void GetAddonCapabilities(PVR_ADDON_CAPABILITIES* pCapabilities);
  int GetChannelsAmount();
  PVR_ERROR GetChannels(ADDON_HANDLE handle, bool bRadio);
  PVR_ERROR GetEPGForChannel(ADDON_HANDLE handle, int iChannelUid, time_t iStart, time_t iEnd);
  int GetRecordingsAmount();
  PVR_ERROR GetRecordings(ADDON_HANDLE handle);
  PVR_ERROR DeleteRecording(const PVR_RECORDING& recording);
  PVR_ERROR GetTimerTypes(PVR_TIMER_TYPE types[], int *size);
  int GetTimersAmount();
  PVR_ERROR GetTimers(ADDON_HANDLE handle);
  PVR_ERROR AddTimer(const PVR_TIMER &timer);
  PVR_ERROR DeleteTimer(const PVR_TIMER &timer);
  PVR_ERROR UpdateTimer(const PVR_TIMER &timer);
  int GetChannelGroupsAmount(void);
  PVR_ERROR GetChannelGroups(ADDON_HANDLE handle, bool bRadio);
  PVR_ERROR GetChannelGroupMembers(ADDON_HANDLE handle, const PVR_CHANNEL_GROUP &group);
  bool GetStatus();
  bool OpenLiveStream(const PVR_CHANNEL &channel, bool use_timeshift, bool use_transcoder, int width, int height,
      int bitrate, const std::string& audiotrack);
  void StopStreaming();
  int GetCurrentChannelId();
  void GetDriveSpace(long long *iTotal, long long *iUsed);
  long long SeekLiveStream(long long iPosition, int iWhence);
  PVR_ERROR GetStreamTimes(PVR_STREAM_TIMES* stream_times);
  long long LengthLiveStream(void);
  bool IsLive();
  int ReadLiveStream(unsigned char *pBuffer, unsigned int iBufferSize);
  bool GetRecordingURL(const char* recording_id, std::string& url, bool use_transcoder, int width,
    int height, int bitrate, std::string audiotrack);
  int GetRecordingLastPlayedPosition(const PVR_RECORDING &recording);
  PVR_ERROR SetRecordingLastPlayedPosition(const PVR_RECORDING &recording, int position);

private:
  bool DoEPGSearch(dvblinkremote::EpgSearchResult& epgSearchResult, const std::string& channelId, const long startTime,
      const long endTime, const std::string & programId = "");
  void SetEPGGenre(dvblinkremote::ItemMetadata& metadata, int& genre_type, int& genre_subtype);
  std::string GetBuildInRecorderObjectID();
  std::string GetRecordedTVByDateObjectID(const std::string& buildInRecoderObjectID);
  int GetInternalUniqueIdFromChannelId(const std::string& channelId);
  virtual void * Process(void);
  bool get_dvblink_program_id(std::string& channelId, int start_time, std::string& dvblink_program_id);
  int GetSchedules(ADDON_HANDLE handle, const dvblinkremote::RecordingList& recordings);
  void get_server_caps();

  std::string make_timer_hash(const std::string& timer_id, const std::string& schedule_id);
  bool parse_timer_hash(const char* timer_hash, std::string& timer_id, std::string& schedule_id);
  unsigned int get_kodi_timer_idx_from_dvblink(const std::string& id);
  bool is_valid_ch_idx(int ch_idx);
  void add_schedule_desc(const std::string& id, const schedule_desc& sd);
  bool get_schedule_desc(const std::string& id, schedule_desc& sd);

  typedef std::map<int, dvblinkremote::Channel* > dvblink_channel_map_t;

  bool m_connected;
  dvblink_channel_map_t m_channels;
  int m_currentChannelId;
  long m_timerCount;
  long m_recordingCount;
  P8PLATFORM::CMutex m_mutex;
  P8PLATFORM::CMutex live_mutex_;
  CHelper_libXBMC_pvr *PVR;
  ADDON::CHelper_libXBMC_addon *XBMC;
  CHelper_libKODI_guilib *GUI;
  server_connection_properties connection_props_;
  LiveStreamerBase* m_live_streamer;
  bool m_add_episode_to_rec_title;
  bool m_group_recordings_by_series;
  bool m_showinfomsg;
  bool m_updating;
  bool m_update_timers_now;
  bool m_update_timers_repeat;
  bool m_update_recordings;
  std::string m_recordingsid;
  std::string m_recordingsid_by_date;
  std::string m_recordingsid_by_series;
  recording_id_to_url_map_t m_recording_id_to_url_map;
  dvblink_server_caps server_caps_;
  dvblinkremote::ChannelFavorites channel_favorites_;
  std::map<std::string, int> inverse_channel_map_;
  bool no_group_single_rec_;
  time_t default_update_interval_sec_;
  int default_rec_show_type_;
  std::map<std::string, schedule_desc> schedule_map_;
  std::map<std::string, unsigned int> timer_idx_map_;
  unsigned int timer_idx_seed_;
};

/*!
 * @brief PVR macros
 */
#define PVR_STRCPY(dest, source) do { strncpy(dest, source, sizeof(dest)-1); dest[sizeof(dest)-1] = '\0'; } while(0)
#define PVR_STRCLR(dest) memset(dest, 0, sizeof(dest))
#define PVR_INT2STR(dest, source) sprintf(dest, "%d", source)
#define PVR_STR2INT(dest, source) dest = atoi(source)
