#pragma once

#include <TotoGL/Window.hpp>
#include <functional>

void initImGui(TotoGL::Window& window);
void renderImGui(std::function<void(void)>&& f);

bool isImGuiFocused();
