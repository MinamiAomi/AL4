#pragma once

#include <memory>

class Player;
class PlayerState;

class PlayerStateManager {
public:
    PlayerStateManager(Player& player) : player(player) {}
    void Update();

    template<class T>
    void ChangeState() {
        static_assert(std::is_base_of_v<PlayerState, T>, "Not inherited.");
        standbyState_ = std::make_unique<T>(*this);
    }

    Player& player;

private:
    std::unique_ptr<PlayerState> activeState_;
    std::unique_ptr<PlayerState> standbyState_;
};

class PlayerState {
public:
    PlayerState(PlayerStateManager& manager) : manager_(manager) {}
    virtual ~PlayerState() {}
    virtual void Initialize() = 0;
    virtual void Update() = 0;

protected:
    PlayerStateManager& manager_;
};

class PlayerRoot :
    public PlayerState {
public:
    using PlayerState::PlayerState;
    void Initialize() override;
    void Update() override;

private:


};

class PlayerAttack :
    public PlayerState {
public:
    using PlayerState::PlayerState;
    void Initialize() override;
    void Update() override;

private:
};