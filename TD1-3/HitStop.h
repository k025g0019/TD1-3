#pragma once
#pragma once

class HitStop {
public:
    static HitStop& Instance() {
        static HitStop instance;
        return instance;
    }

    void Start(int frames) {
        timer = frames;
    }

    bool IsActive() {
        if (timer > 0) {
            return true;
        }
        else {

            return false;
        }
    }

    void Update() {
        if (timer > 0) timer--;
    }

private:
    HitStop() : timer(0) {}
    int timer;
};
