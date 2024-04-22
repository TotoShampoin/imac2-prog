#include "TotoGL/Primitives/Color.hpp"
#include "TotoGL/Window.hpp"
#include <GLFW/glfw3.h>
#include <cstddef>
#include <imgui.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <Eigen/Eigen>
#include <TotoGL/TotoGL.hpp>
#include <math/markov.hpp>
#include <math/random/uniform.hpp>

#include <vector>

int markov() {
    auto window = TotoGL::Window(800, 600, "Markov Chain");

    // Markov chain
    // auto rng = Random::Uniform<float>(0, 1);
    auto probabilities = Eigen::Matrix<float, 3, 3>();
    probabilities << 0.1, 0.5, 0.4,
        0.3, 0.2, 0.5,
        0.6, 0.4, 0.0;
    auto values = std::vector<TotoGL::ColorRGB>({
        TotoGL::ColorRGB(1, 1, 0),
        TotoGL::ColorRGB(1, 0, 1),
        TotoGL::ColorRGB(0, 1, 1),
    });
    // auto current = size_t { 0 };

    auto chain = MarkovChain<TotoGL::ColorRGB, 3>(values, probabilities);

    auto total = size_t { 0 };
    std::vector<int> histogram(3);

    window.on(TotoGL::InputEventName::KEY, [&](const TotoGL::InputEvent& event) {
        if (event.action != GLFW_RELEASE && event.button == GLFW_KEY_SPACE) {
            chain.next();
            histogram[chain.currentIndex()]++;
            total++;
        }
    });

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui_ImplGlfw_InitForOpenGL(window.glfwWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 460");

    while (!window.shouldClose()) {
        window.draw([&] {
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::SetNextWindowSize(ImVec2(800, 600));
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::Begin("Random distributions", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

            auto next_proba = chain.nextProbabilities();
            ImGui::ColorEdit3("Color 0", &chain.values()[0].r);
            ImGui::SameLine();
            ImGui::Text("p = %.2f", next_proba[0]);
            ImGui::ColorEdit3("Color 1", &chain.values()[1].r);
            ImGui::SameLine();
            ImGui::Text("p = %.2f", next_proba[1]);
            ImGui::ColorEdit3("Color 2", &chain.values()[2].r);
            ImGui::SameLine();
            ImGui::Text("p = %.2f", next_proba[2]);

            ImGui::ColorEdit3("Current", &chain.current().r);

            std::vector<float> data(3);
            for (size_t i = 0; i < 3; i++) {
                data[i] = static_cast<float>(histogram[i]) / total;
            }
            ImGui::PlotHistogram("##", data.data(), static_cast<int>(data.size()), 0, nullptr, 0, 1, ImVec2(750, 450));

            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        });
    }

    return 0;
}