class Module {
public:
    Module(int sleepTime, void (*function)(void));
    int sleepTime;
    void (*loopFunction)(void);
private:
    void startLoop();
};
