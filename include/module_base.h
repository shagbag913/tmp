#include <thread>

class Module {
    public:
        Module(void (*loopFunction)());
        void startLoop();
        void stopLoop();

    private:
        std::thread loopThread;
        void (*loopFunction)();
};
