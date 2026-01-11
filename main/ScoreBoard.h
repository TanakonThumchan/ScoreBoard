#include <stdint.h>

enum team_t{
    TEAM1 = 1,
    TEAM2 = 2
};

class ScoreBoard {
    private:
    uint8_t teamOneScore;
    uint8_t teamOneFoul;
    uint8_t teamTwoScore;
    uint8_t teamTwoFoul;

    public:
    ScoreBoard();

    uint8_t addScore(team_t team);
    uint8_t removeScore(team_t team);
    uint8_t addFoul(team_t team);
    uint8_t removeFoul(team_t team);
};