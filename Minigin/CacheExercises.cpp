#include "CacheExercises.h"

#include <chrono>
#include <vector>

#include <imgui.h>
#include <imgui_plot.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

struct TestResults
{
	TestResults(int requiredSamples = 10)
		: hasBeenExecuted { false }
		, requiredSamples{ requiredSamples }
		, samples{}
		, steps{}
	{ }

	bool hasBeenExecuted{ false };
	int requiredSamples{ 10 };

	std::vector<float> samples{};
	std::vector<float> steps{};
};

template <typename T>
void ThrashTheCache(TestResults& results)
{
	constexpr int numValues{ 10'000'000 };
	std::vector<T> arr(numValues, T{});

	results.samples.clear();

	for (int stepSize = 1; stepSize <= 1024; stepSize *= 2)
	{
		long long totalTime = 0;

		for (int sample = 0; sample < results.requiredSamples; ++sample)
		{
			const auto start = std::chrono::steady_clock::now();

			for (int i = 0; i < arr.size(); i += stepSize)
			{
				arr[i] *= 2;
			}

			const auto end = std::chrono::steady_clock::now();

			if (sample > 0 && sample < results.requiredSamples - 1)
			{
				totalTime += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			}
		}

		results.samples.push_back((float)totalTime / results.requiredSamples);
		results.steps.push_back((float)stepSize);
	}
}

void AddResultsPlot(const TestResults& results)
{
	ImGui::PlotConfig conf{};

	conf.values.xs = results.steps.data();
	conf.values.count = (int)results.steps.size();
	conf.values.ys = results.samples.data();
	conf.values.ys_count = (int)results.samples.size();

	conf.frame_size = ImVec2(200, 100);

	conf.grid_y.show = true;
	conf.grid_y.size = 3000;

	conf.tooltip.show = true;
	conf.tooltip.format = "x=%.2f, y=%.2f";

	conf.scale.min = 0.0f;
	conf.scale.max = 30000.0f;

	ImGui::Plot("Cache Thrash", conf);
}

void dae::ImGuiTestExercise1()
{
	static TestResults results{ 10 };

	ImGui::Begin("Exercise 1", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	if (ImGui::Button("Thrash the cache"))
	{
		ThrashTheCache<int>(results);
		results.hasBeenExecuted = true;
	}

	ImGui::InputInt("Samples", &results.requiredSamples, 1, 10);

	if (results.hasBeenExecuted)
	{
		AddResultsPlot(results);
	}

	ImGui::End();
}

void dae::ImGuiTestExercise2()
{

	
}
