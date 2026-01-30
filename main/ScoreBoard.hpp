#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

enum team_t
{
    TEAM1 = 1,
    TEAM2 = 2
};

struct GameState
{
    uint8_t teamOneScore;
    uint8_t teamOneFoul;

    uint8_t teamTwoScore;
    uint8_t teamTwoFoul;
};

class ScoreBoard
{
private:
    GameState _state;
    SemaphoreHandle_t _mutex;
    
    static ScoreBoard _instance;
    bool _isInitialized;
    ScoreBoard();

public:
    static ScoreBoard &getInstance();

    ScoreBoard(const ScoreBoard&) = delete;
    ScoreBoard& operator=(const ScoreBoard&) = delete;

    bool init();

    void resetGame();
    uint8_t addScore(team_t team);
    uint8_t removeScore(team_t team);
    uint8_t addFoul(team_t team);
    uint8_t removeFoul(team_t team);

    GameState getGameState();
};