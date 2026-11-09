#ifndef APP_H
#define APP_H

#include "window.h"

/* THERE CAN ONLY BE ONE APP OPEN AT A TIME */
class App {
    struct M{
        bool is_open = true;
        Window* PCWin;
        Window* GMWin;
    } m;
    explicit App(M m) : m(std::move(m)) {}
public:
    static std::optional<App> create();

    App(const App& other);
    App& operator=(const App& other);

    App(App&& other) noexcept;
    App& operator=(App&& other) noexcept;

    void launch();
    bool shouldClose();

    ~App();
};

#endif
