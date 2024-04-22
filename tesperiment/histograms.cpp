#include <imgui.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <TotoGL/Window.hpp>
#include <algorithm>
#include <math/random/random.hpp>
#include <vector>

constexpr int N = 1'000'000;
constexpr int RESOLUTION = 20;
constexpr int HISTOGRAM_HEIGHT = 160;

template <typename Type>
struct Histogram {
    std::vector<int> values;
    Type min;
    Type max;
    [[nodiscard]] int height() const {
        return *std::max_element(values.begin(), values.end());
    }
    [[nodiscard]] int sum() const {
        return std::accumulate(values.begin(), values.end(), 0);
    }
    std::vector<float> normalized() const {
        std::vector<float> normalized_values(values.size());
        for (int i = 0; i < values.size(); i++) {
            normalized_values[i] = static_cast<float>(values[i]) / sum();
        }
        return normalized_values;
    }
};

template <class RNG>
Histogram<float> testDistributionReal(RNG& rng) {
    std::vector<float> values(N);
    for (int i = 0; i < N; i++) {
        values[i] = rng();
    }
    float min = *std::min_element(values.begin(), values.end());
    float max = *std::max_element(values.begin(), values.end());
    float step = (max - min) / RESOLUTION;
    std::vector<int> histogram(RESOLUTION);
    for (int i = 0; i < N; i++) {
        int index = static_cast<int>((values[i] - min) / step);
        histogram[index]++;
    }
    return { histogram, min, max };
}

template <class RNG>
Histogram<int> testDistributionInt(RNG& rng) {
    std::vector<int> values(N);
    for (int i = 0; i < N; i++) {
        values[i] = rng();
    }
    int min = *std::min_element(values.begin(), values.end());
    int max = *std::max_element(values.begin(), values.end());
    int res = max - min + 1;
    std::vector<int> histogram(res);
    for (int i = 0; i < N; i++) {
        histogram[values[i] - min]++;
    }
    return { histogram, min, max };
}

enum Test {
    OPTION_A,
    OPTION_B,
    OPTION_C,
    OPTION_D,
};
template <class RNG>
Histogram<Test> testDistributionEnum(RNG& rng) {
    std::vector<Test> values(N);
    for (int i = 0; i < N; i++) {
        values[i] = rng();
    }
    std::vector<int> histogram(4);
    for (int i = 0; i < N; i++) {
        histogram[static_cast<int>(values[i])]++;
    }
    return { histogram, OPTION_A, OPTION_D };
}

int histograms() {
    Random::Uniform<float> uniform_dist(0, 1);
    Random::Exponential<float> exponential_dist(1);
    Random::Poisson<int> poisson_dist(3);
    Random::Binomial<int> binomial_dist(.9, RESOLUTION);
    Random::Normal<float> normal_dist;
    Random::Enumerated<Test> enum_dist({ { OPTION_A, .5 }, { OPTION_B, .3 }, { OPTION_C, .1 }, { OPTION_D, .2 } });

    TotoGL::Window window(800, 600, "Random distributions");

    std::string distribution = "None";
    std::vector<float> histogram(RESOLUTION);
    float min = 0;
    float max = 1;

    window.on(TotoGL::InputEventName::KEY, [&](TotoGL::InputEvent event) {
        if (event.action == GLFW_PRESS) {
            switch (event.button) {
            case GLFW_KEY_1: {
                distribution = "Uniform";
                auto hist = testDistributionReal(uniform_dist);
                histogram = hist.normalized();
                min = hist.min;
                max = hist.max;
            } break;
            case GLFW_KEY_2: {
                distribution = "Exponential";
                auto hist = testDistributionReal(exponential_dist);
                histogram = hist.normalized();
                min = hist.min;
                max = hist.max;
            } break;
            case GLFW_KEY_3: {
                distribution = "Poisson";
                auto hist = testDistributionInt(poisson_dist);
                histogram = hist.normalized();
                min = static_cast<float>(hist.min);
                max = static_cast<float>(hist.max);
            } break;
            case GLFW_KEY_4: {
                distribution = "Binomial";
                auto hist = testDistributionInt(binomial_dist);
                histogram = hist.normalized();
                min = static_cast<float>(hist.min);
                max = static_cast<float>(hist.max);
            } break;
            case GLFW_KEY_5: {
                distribution = "Normal";
                auto hist = testDistributionReal(normal_dist);
                histogram = hist.normalized();
                min = hist.min;
                max = hist.max;
            } break;
            case GLFW_KEY_6: {
                distribution = "Enum";
                auto hist = testDistributionEnum(enum_dist);
                histogram = hist.normalized();
                min = hist.min;
                max = hist.max;
            } break;
            default:
                break;
            }
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
            ImGui::Begin("Random distributions");
            ImGui::PlotHistogram("##", histogram.data(), static_cast<int>(histogram.size()), 0, nullptr, 0, 1, ImVec2(750, 500));
            ImGui::Text("Distribution: %s", distribution.c_str());
            ImGui::Text("Min: %f", min);
            ImGui::Text("Max: %f", max);
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        });
    }

    return 0;
}