//
//  SoundManger.cpp
//  KnightRPS
//
//  Created by jiaolong on 15/6/30.
//
//

#include "AW_SoundManger.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

using namespace CocosDenshion;

SoundManger *SoundManger::_pInstance = nullptr;

SoundManger *SoundManger::getInstance()
{
    if (_pInstance == nullptr)
    {
        _pInstance = new SoundManger();
        
        _pInstance->SoundInit();
    }
    
    return _pInstance;
}

SoundManger::SoundManger()
:

_isSound(true),
_isMusic(true)

{
    
}

void SoundManger::stopMusic()
{
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}

void SoundManger::pauseMusic()
{
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

void SoundManger::resumeMusic()
{
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}

bool SoundManger::isStopMusic()
{
  return ! SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying();
}

bool SoundManger::SoundInit()
{
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic("Sound/bgm0.mp3");
	SimpleAudioEngine::getInstance()->preloadBackgroundMusic("Sound/bgm1.mp3");
    //////////////////
    
    SimpleAudioEngine::getInstance()->preloadEffect("Sound/click.wav");
    //SimpleAudioEngine::getInstance()->preloadEffect("Sound/countTime.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("Sound/scrollPage.wav");
    SimpleAudioEngine::getInstance()->preloadEffect("Sound/winCoin.wav");
    
    return true;
}

void SoundManger::playMusic(const char* pszFilePath , bool isTrue)
{
    if (_isMusic)
    {
        _currMusic = pszFilePath;
        
        SimpleAudioEngine::getInstance()->playBackgroundMusic(pszFilePath, isTrue);
    }
}

void SoundManger::playSound(const char* pszFilePath)
{
    if (_isSound)
    {
        SimpleAudioEngine::getInstance()->playEffect(pszFilePath);
    }
}

int SoundManger::playSound(const char *pszFilePath, bool bLoop,
                           float pitch = 0, float pan = 0, float gain = 0)
{
    if (_isSound)
    {
        return SimpleAudioEngine::getInstance()->playEffect(pszFilePath,bLoop,pitch,pan,gain);
    }
    
    return 0;
}

bool SoundManger::isMusicOn()
{
    return SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying();
}

void SoundManger::setMusicStatus(const char *pszFilePath,bool status)
{
    _isMusic = status;
    
    if (status)
    {
		SimpleAudioEngine::getInstance()->playBackgroundMusic(_currMusic.c_str(), false);
    }
    else
    {
        SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    }
}

void SoundManger::setSoundStatus(bool status)
{
    _isSound = status;
    
    if (status)
    {
        SimpleAudioEngine::getInstance()->resumeAllEffects();
    }
    else
    {
        SimpleAudioEngine::getInstance()->pauseAllEffects();
    }
}

