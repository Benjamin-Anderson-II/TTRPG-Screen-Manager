#pragma once

#include "window.h"
#include <memory>

/* THERE CAN ONLY BE ONE APP OPEN AT A TIME */
class App {
    struct M{
        bool is_open = true;
        std::unique_ptr<Window> PCWin;
        std::unique_ptr<Window> GMWin;
    } m;
    explicit App(M m) : m(std::move(m)) {}
public:
    static std::optional<App> create();

    App(App&& other) noexcept;
    App& operator=(App&& other) noexcept;

    void launch();

    ~App();
};
