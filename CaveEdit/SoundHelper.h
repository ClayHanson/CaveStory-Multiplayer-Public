#pragma once
namespace SoundHelper
{
	enum SoundType
	{
		// Uses pSoundFileName
		ST_WAVE_RESOURCE,
		
		// Uses pSoundFileName
		ST_WAVE_FILE,

		// Uses iSoundId
		ST_ORGANYA,

		// Uses iSoundId
		ST_PXTONE
	};

	enum PlayMode
	{
		PM_STOP_THEN_PLAY,
		PM_PLAY,
		PM_STOP,

		// Does not do anything with ST_ORGANYA, as organya music is designed to always loop, regardless.
		PM_LOOP
	};

	class AdvancedSoundObject
	{
	public: // Linking
		static AdvancedSoundObject* first;
		AdvancedSoundObject* next;
		AdvancedSoundObject* prev;

	protected: // Protected stuff
		struct
		{
			HWND hMCI;
		} Handles;

	public: // Variables
		struct
		{
			bool bPlaying;
			bool bPaused;
			char* pFileName;
		} State;

	public: // C++ management
		AdvancedSoundObject();
		~AdvancedSoundObject();

	public: // File management methods
		BOOL SetFile(const char* pFile);
		void Close();

	public: // Sound methods
		void SetVolume(unsigned char iVolume);
		void Play(BOOL bLoop = FALSE);
		void Stop();
		void Resume();
		void Pause();
		void Rewind();

	public: // Static methods
		static BOOL PlayFile(const char* pFile, BOOL bLoop = FALSE);
		static BOOL StopFile(const char* pFile);
	};

	void PlaySoundObject(SoundType iType, PlayMode iMode, const char* pSoundFileName = nullptr, int iSoundId = -1);
};