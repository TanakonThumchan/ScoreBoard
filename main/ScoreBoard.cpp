#include "ScoreBoard.hpp"

ScoreBoard::ScoreBoard() : _state{0, 0, 0, 0}, _mutex(nullptr), _isInitialized(false) {}

ScoreBoard ScoreBoard::_instance;

ScoreBoard &ScoreBoard::getInstance()
{
    return _instance;
}

bool ScoreBoard::init()
{
    if (_isInitialized)
    {
        return true;
    }

    _mutex = xSemaphoreCreateMutex();

    if (_mutex == nullptr)
    {
        return false;
    }

    resetGame();

    _isInitialized = true;
    return true;
}

void ScoreBoard::resetGame()
{
    if (_isInitialized && xSemaphoreTake(_mutex, portMAX_DELAY))
    {
        _state = {0, 0, 0, 0};
        xSemaphoreGive(_mutex);
    }
}

GameState ScoreBoard::getGameState()
{
    GameState currentState;
    if (_isInitialized && xSemaphoreTake(_mutex, portMAX_DELAY))
    {
        currentState = _state;
        xSemaphoreGive(_mutex);
    }
    return currentState;
}

uint8_t ScoreBoard::addScore(team_t team)
{
    uint8_t newScore = 0;
    if (_isInitialized && xSemaphoreTake(_mutex, portMAX_DELAY))
    {
        if (team == TEAM1)
        {
            newScore = ++_state.teamOneScore;
        }
        else
        {
            newScore = ++_state.teamTwoScore;
        }
        xSemaphoreGive(_mutex);
    }
    return newScore;
}

uint8_t ScoreBoard::removeScore(team_t team)
{
    uint8_t newScore = 0;
    if (_isInitialized && xSemaphoreTake(_mutex, portMAX_DELAY))
    {
        if (team == TEAM1)
        {
            newScore = --_state.teamOneScore;
        }
        else
        {
            newScore = --_state.teamTwoScore;
        }
        xSemaphoreGive(_mutex);
    }
    return newScore;
}

uint8_t ScoreBoard::addFoul(team_t team)
{
    uint8_t newFoul = 0;
    if (_isInitialized && xSemaphoreTake(_mutex, portMAX_DELAY))
    {
        if (team == TEAM1)
        {
            newFoul = ++_state.teamOneFoul;
        }
        else
        {
            newFoul = ++_state.teamTwoFoul;
        }
        xSemaphoreGive(_mutex);
    }
    return newFoul;
}

uint8_t ScoreBoard::removeFoul(team_t team)
{
    uint8_t newFoul = 0;
    if (_isInitialized && xSemaphoreTake(_mutex, portMAX_DELAY))
    {
        if (team == TEAM1)
        {
            newFoul = --_state.teamOneFoul;
        }
        else
        {
            newFoul = --_state.teamTwoFoul;
        }
        xSemaphoreGive(_mutex);
    }
    return newFoul;
}
