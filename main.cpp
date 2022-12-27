#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <limits>
#include <random>
#include <string>
#include <codecvt>
#include <cstring>
#include <process.h>
#include <windows.h>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "cJSON.h"
#include "math.h"
#include "keyboard.h"

// ----- Defines ----- //
#define VERSION 100
#define PARSE      cJSON_Parse
#define GETITEM    cJSON_GetObjectItem
#define GETARRSIZE cJSON_GetArraySize
#define GETARRITEM cJSON_GetArrayItem
#define DEBUG

// ----- Struct & Class ----- //
struct music {
    sf::SoundBuffer soundBuffer;
    std::wstring name;
};

struct Text {
    std::wstring str;
    sf::Color color;
    sf::Vector2f pos;
    int size;
};


bool used[100] = { false };

sf::ContextSettings videoSettings(24, 8, 4, 4, 6);
class Message {
public:
    Message(sf::Texture* _texture, std::string _info, sf::Vector2f _scale, std::vector<Text> _text);
    Message(const Message& ms);
    Message& operator = (const Message& ms);
    sf::RenderWindow* getWindow();
    void startDisplay();
    void updAndRender(int y_pos);
    bool displayFinish();
    void setText(std::vector<Text> _text);
    void reset(std::vector<Text> _text);

    bool start = false;
    bool finish = false;

protected:

    sf::RenderWindow window;
    sf::Texture* texture;
    sf::Sprite sprite;
    sf::Vector2f scale;
    std::vector<sf::Text> text;

    HWND hWnd;

    clock_t startTime;

    SimpleEaseIn pop;
    SimpleEaseIn quit;
};

// ----- Global Variables ----- //
DWORD forever = 0xffffffff;

struct {
    std::string fontName = "MinecraftRegular.ttf";
    std::string bg_info_imageName;
    sf::IntRect bg_info_rect;
    sf::Vector2f bg_info_scale;

    std::string bg_warn_imageName;
    sf::IntRect bg_warn_rect;
    sf::Vector2f bg_warn_scale;

    bool show_warning = true;
    std::string mode = "loop";
} settings;
sf::Font font;

std::wstring now;
bool loaded = false;
bool shouldQuit = false;
int volume = 20;
bool volumeChanged = false;
bool musicChanged = false;

// ----- Functions ----- //
inline std::wstring to_wide_string(const std::string& input);
inline int get();
void setHWND(HWND hWnd);

// ----- Debug ----- //
#ifndef DEBUG
#pragma comment( linker, "/subsystem:windows /entry:mainCRTStartup" )
#endif // !DEBUG


int main() {
    memset(used, 0, sizeof used);

    srand(time(0));

    std::thread t_sound([] {
        std::vector<music> soundBuffers;
        
        {
            std::ifstream ifstream("./src/list.json");
            std::string data((std::istreambuf_iterator<char>(ifstream)),
                             std::istreambuf_iterator<char>());

            cJSON* root = PARSE(data.c_str());
            cJSON* item;

            item = GETITEM(root, "version");

            item = GETITEM(root, "show_warning");
            if (item) {
                settings.show_warning = item->valueint;
            }

            item = GETITEM(root, "mode");
            if (item) {
                settings.mode = std::string(item->valuestring);
            }

            item = GETITEM(root, "font");
            if (item) {
                settings.fontName = std::string(item->valuestring);
            }


            item = GETITEM(root, "background_info");
            if (item) {
                cJSON* imageName = GETITEM(item, "image");
                cJSON* arrayRect = GETITEM(item, "rect");
                cJSON* arrayScale = GETITEM(item, "scale");
                int rectSize = cJSON_GetArraySize(arrayRect);
                int scaleSize = cJSON_GetArraySize(arrayScale);
                if (!imageName) {
                    exit(EXIT_FAILURE);
                }
                if (rectSize != 4) {
                }
                if (scaleSize != 2) {
                }
                settings.bg_info_imageName = std::string(imageName->valuestring);
                settings.bg_info_rect = {

                    GETARRITEM(arrayRect, 0)->valueint,
                    GETARRITEM(arrayRect, 1)->valueint,
                    GETARRITEM(arrayRect, 2)->valueint,
                    GETARRITEM(arrayRect, 3)->valueint,
                };
                settings.bg_info_scale = {
                    (float)GETARRITEM(arrayScale, 0)->valuedouble,
                    (float)GETARRITEM(arrayScale, 1)->valuedouble
                };

            }
            item = GETITEM(root, "background_warn");
            if (item) {
                cJSON* imageName = GETITEM(item, "image");
                cJSON* arrayRect = GETITEM(item, "rect");
                cJSON* arrayScale = GETITEM(item, "scale");
                int rectSize = cJSON_GetArraySize(arrayRect);
                int scaleSize = cJSON_GetArraySize(arrayScale);
                if (!imageName) {

                    exit(EXIT_FAILURE);
                }
                if (rectSize != 4) {
                }
                if (scaleSize != 2) {
                }
                settings.bg_warn_imageName = std::string(imageName->valuestring);
                settings.bg_warn_rect = {
                    GETARRITEM(arrayRect, 0)->valueint,
                    GETARRITEM(arrayRect, 1)->valueint,
                    GETARRITEM(arrayRect, 2)->valueint,
                    GETARRITEM(arrayRect, 3)->valueint,
                };
                settings.bg_warn_scale = {
                    (float)GETARRITEM(arrayScale, 0)->valuedouble,
                    (float)GETARRITEM(arrayScale, 1)->valuedouble
                };

            }
            music music;
            item = GETITEM(root, "music");
            if (item) {
                int arraySize = cJSON_GetArraySize(item);
                for (int i = 0; i < arraySize; i++) {
                    cJSON* arrayItem = GETARRITEM(item, i);
                    if (arrayItem == nullptr) continue;

                    cJSON* file = GETITEM(arrayItem, "file");
                    cJSON* name = GETITEM(arrayItem, "name");

                    music.soundBuffer.loadFromFile("./src/music/" + std::string(file->valuestring));
                    music.name = to_wide_string(std::string(name->valuestring));
                    soundBuffers.push_back(music);
                }
            }
        }

        loaded = true;

        sf::Sound sound;
        sound.setVolume(10);

        std::default_random_engine e;
        e.seed(get());
            std::uniform_int_distribution<size_t> u(0, soundBuffers.size() - 1);
        size_t num = -1;
        while (true) {
            
            if (settings.mode == "random") num = u(e);
            else if (settings.mode == "loop") num = (num + 1) % soundBuffers.size();
            sound.setBuffer(soundBuffers[num].soundBuffer);
            now = soundBuffers[num].name;
            musicChanged = true;
            sound.play();
            while (sound.getStatus() == sf::Sound::Playing) {
                if (volumeChanged) {
                    sound.setVolume(volume);
                    volumeChanged = false;
                }
                if (shouldQuit) {
                    sound.stop();
                    shouldQuit = false;
                    return;
                }
            }
        }

    });
    t_sound.detach();

    std::wstring cache = L"";

    while (!loaded);

    font.loadFromFile("./src/" + settings.fontName);
    sf::Text playingNow;
    playingNow.setFont(font);
    playingNow.setCharacterSize(25);
    playingNow.setString(L"Playing Now");
    playingNow.setPosition(15, 15);
    sf::Text name;
    name.setFont(font);
    name.setCharacterSize(25);
    name.setFillColor(sf::Color::Yellow);
    name.setString(cache);
    name.setPosition(15, 50);

    sf::Texture bg_info;
    bg_info.loadFromFile("./src/" + settings.bg_info_imageName, settings.bg_info_rect);
    sf::Texture bg_warn;
    bg_warn.loadFromFile("./src/" + settings.bg_warn_imageName, settings.bg_warn_rect);

    Message playing(
        &bg_info, "", settings.bg_info_scale,
        {});
    Message warning(
        &bg_warn, "", settings.bg_warn_scale,
        { { L"制作者: Cg1340", sf::Color::Yellow, {55, 15}, 26 },
          { L"更多详情请见 cg1340.github.io", sf::Color::Yellow, {55, 50}, 26 } });
    warning.startDisplay();


    while (true) {
        if (musicChanged) {
            playing.reset({ { L"现在正在播放", sf::Color::Yellow, {55, 15}, 26 },
                            { now, sf::Color::White, {55, 50}, 26 } });
            musicChanged = false;
        }

        if (!playing.finish && playing.start) {
            playing.updAndRender(0);
        }
        if (!warning.finish && warning.start && settings.show_warning) {
            warning.updAndRender(1);
        }

        if (keyDown(0x31)) {
            shouldQuit = true;
            while (shouldQuit);
            return 0;
        }
        if (keyDown(0x32)) {
            volume = (volume - 5 >= 0 ? volume - 5 : 0);
            volumeChanged = true;
            if (settings.show_warning)
                warning.reset({ {L"音量已调整到 " + std::to_wstring(volume), sf::Color::Yellow, {55, 15}, 26}});
        }
        if (keyDown(0x33)) {
            volume = (volume + 5 <= 100 ? volume + 5 : 100);
            volumeChanged = true;
            if (settings.show_warning)
                warning.reset({ {L"音量已调整到 " + std::to_wstring(volume), sf::Color::Yellow, {55, 15}, 26} });
        }
    }
    return EXIT_SUCCESS;
}

inline std::wstring to_wide_string(const std::string& input) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(input);
}

inline int get() {
    srand(time(nullptr));
    return rand() + rand() * rand() - rand() & rand() * rand();
}

void setHWND(HWND hWnd) {
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    LONG style = GetWindowLong(hWnd, GWL_EXSTYLE);
    style = style | WS_EX_LAYERED | WS_EX_TOOLWINDOW;
    SetWindowLong(hWnd, GWL_EXSTYLE, style);
    SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
}

Message::Message(sf::Texture* _texture, std::string _info, sf::Vector2f _scale, std::vector<Text> _text) {
    this->texture = _texture;
    this->sprite.setTexture(*texture);
    this->sprite.setScale(_scale);

    this->scale = _scale;

    this->window.create({ (unsigned)((*texture).getSize().x * _scale.x),
        (unsigned)((*texture).getSize().y * _scale.y) },
        L"MusicPlayer",
        sf::Style::None,
        videoSettings);
    this->window.setPosition({ 0, 0 });
    this->hWnd = this->window.getSystemHandle();

    for (auto item : _text) {
        sf::Text t;
        t.setString(item.str);
        t.setFont(font);
        t.setColor(item.color);
        t.setCharacterSize(item.size);
        t.setPosition(item.pos);
        this->text.push_back(t);
    }
}

Message::Message(const Message& ms) {
    this->texture = ms.texture;
    this->sprite = sf::Sprite(ms.sprite);
    this->scale = ms.scale;
    this->text = ms.text;

    this->start = ms.start;
    this->finish = ms.finish;
    this->startTime = ms.startTime;
    this->pop = SimpleEaseIn(ms.pop);
    this->quit = SimpleEaseIn(ms.quit);

    this->window.create({ (unsigned)((*texture).getSize().x * scale.x),
        (unsigned)((*texture).getSize().y * scale.y) },
        L"MusicPlayer",
        sf::Style::None,
        videoSettings);

    this->hWnd = this->window.getSystemHandle();
    
}

Message& Message::operator=(const Message& ms) {
    Message m(ms);
    
    return m;
}

sf::RenderWindow* Message::getWindow() {
    return &this->window;
}

void Message::startDisplay() {
    if (start) return;
    this->start = true;
    this->pop.setValue(500, -((double)this->window.getSize().x), 0);
    this->quit.setValue(500, 0, -((double)this->window.getSize().x));
    this->window.setVisible(true);
    this->pop.start();
    this->startTime = clock();
}

void Message::updAndRender(int y_pos) {
    if (finish) return;
    if (clock() - this->startTime < 5500) {
        this->window.setPosition({ (int)this->pop.get(), (int)(y_pos * this->texture->getSize().y * this->scale.y) });
    }
    else if (clock() - this->startTime >= 5500 && clock() - this->startTime <= 6100) {
        this->quit.start();
        this->window.setPosition({ (int)this->quit.get(), (int)(y_pos * this->texture->getSize().y * this->scale.y) });
    }
    else {
        this->finish = true;
    }

    sf::Event event{};
    while (this->window.pollEvent(event));

    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    LONG style = GetWindowLong(hWnd, GWL_EXSTYLE);
    style = style | WS_EX_LAYERED | WS_EX_TOOLWINDOW;
    SetWindowLong(hWnd, GWL_EXSTYLE, style);
    SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

    this->window.clear();
    this->window.draw(this->sprite);
    for (auto item : this->text) {
        this->window.draw(item);
    }
    this->window.display();
}

bool Message::displayFinish() {
    return this->finish;
}

void Message::setText(std::vector<Text> _text) {
    this->text.clear();
    for (auto item : _text) {
        sf::Text t;
        t.setString(item.str);
        t.setFont(font);
        t.setColor(item.color);
        t.setCharacterSize(item.size);
        t.setPosition(item.pos);
        this->text.push_back(t);
    }
}

void Message::reset(std::vector<Text> _text) {
    this->setText(_text);
    if (clock() - this->startTime > 500 && clock() - this->startTime <= 5500) {
        this->startTime = clock() - 500;
    }
    else if (clock() - this->startTime > 5500) {
        this->start = false;
        this->finish = false;
        this->pop.clear();
        this->quit.clear();
        this->startDisplay();
    }
}
