//
//  SoundManger.h
//  KnightRPS
//
//  Created by jiaolong on 15/6/30.
//
//

#ifndef __KnightRPS__SoundManger__
#define __KnightRPS__SoundManger__

#include <stdio.h>
#include "cocos2d.h"

class SoundManger
{
public:
    
    static SoundManger *getInstance();
    
    bool SoundInit();
    
    bool isStopMusic();
    
    void playMusic(const char* pszFilePath , bool isTrue);
    
    void playSound(const char* pszFilePath);
    
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    
    int playSound(const char *pszFilePath, bool bLoop,
                  float pitch, float pan, float gain);
    
    void setMusicStatus(const char *pszFilePath,bool status);

    void setSoundStatus(bool status);
    
    bool getMusicStatus() { return _isMusic;}
    
    bool getSoundStatus() {return _isSound;}
    
    std::string getCurrMusic() {return _currMusic;}
    
	void setCurrMusic(std::string music){ _currMusic = music; }
		
    bool isMusicOn();
protected:
    SoundManger();
    
private:
    
    static SoundManger *_pInstance;
    
    bool _isMusic;
    bool _isSound;
    
    std::string _currMusic;
};
#endif /* defined(__KnightRPS__SoundManger__) */
