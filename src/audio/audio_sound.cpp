#pragma once
#include "audio/audio_sound.hpp"
#include <iostream>

std::shared_ptr<sf::Sound> Audio::sound_loader::load(const sf::SoundBuffer &buffer) const
{

    sf::Sound *sound = new sf::Sound;
    sound->setBuffer(buffer);
    return std::shared_ptr<sf::Sound>(sound);
}
std::shared_ptr<sf::SoundBuffer> Audio::sound_buffer_loader::load(const std::string &path) const
{
    sf::SoundBuffer *buffer = new sf::SoundBuffer;
    if (!buffer->loadFromFile(path))
    {
        std::cout << "failed to load audio at path: "
                  << path.c_str() << std::endl;
    }
    std::cout << "duration: "
              << buffer->getDuration().asSeconds() << std::endl;
    return std::shared_ptr<sf::SoundBuffer>(buffer);
}
