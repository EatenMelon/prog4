#include "CacheExercises.h"

#include <chrono>
#include <vector>

#include <imgui.h>
#include <imgui_plot.h>

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

struct Transform
{
	float matrix[16] =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
};

class GameObject3D
{
public:
	Transform transform{};
	int ID{};
};

class GameObject3DAlt
{
public:
	Transform* pTransform{};
	int ID{};
};

template <typename T>
void ThrashTheCache(TestResults& results)
{
	constexpr int numValues{ 10'000'000 };
	std::vector<T> arr(numValues, T{});

	results.samples.clear();
	results.steps.clear();

	for (int stepSize = 1; stepSize <= 1024; stepSize *= 2)
	{
		long long totalTime = 0;

		for (int sample = 0; sample < results.requiredSamples; ++sample)
		{
			const auto start = std::chrono::steady_clock::now();

			for (int i = 0; i < arr.size(); i += stepSize)
			{
				if constexpr (requires(T x) { x.ID; })
				{
					arr[i].ID *= 2;
				}
				else
				{
					arr[i] *= 2;
				}
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

void AddResultsPlot
(
	const TestResults& results,
	const std::string& name,
	const ImColor& color = ImColor(255, 0, 0, 255),
	float maxScale = 30000.f
)
{
	ImGui::PlotConfig conf{};

	conf.values.xs = results.steps.data();
	conf.values.count = (int)results.steps.size();
	conf.values.ys = results.samples.data();
	conf.values.ys_count = (int)results.samples.size();

	conf.values.color = color;

	conf.frame_size = ImVec2(200, 100);

	conf.grid_y.show = true;
	conf.grid_y.size = maxScale / 10;

	conf.tooltip.show = true;
	conf.tooltip.format = "x=%.2f, y=%.2f";

	conf.scale.min = 0.f;
	conf.scale.max = maxScale;

	conf.line_thickness = 2.f;

	ImGui::Plot(name.c_str(), conf);
}

void dae::ImGuiTestExercise1()
{
	ImGui::Begin("Exercise 1", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	static TestResults results{ 10 };

	ImGui::InputInt("Samples", &results.requiredSamples, 1, 10);

	if (ImGui::Button("Thrash the cache"))
	{
		ThrashTheCache<int>(results);
		results.hasBeenExecuted = true;
	}

	if (results.hasBeenExecuted)
	{
		AddResultsPlot(results, "int test", ImColor(255, 128, 0, 255), 3000.f);
	}

	ImGui::End();
}

void dae::ImGuiTestExercise2()
{
	ImGui::Begin("Exercise 2", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	static TestResults results3D{ 10 };
	static TestResults results3DAlt{ 10 };

	constexpr ImColor color3DResult{ 0, 255, 0, 255 };
	constexpr ImColor color3DAltResult{ 0, 255, 255, 255 };

	ImGui::InputInt("Samples", &results3D.requiredSamples, 1, 10);

	results3DAlt.requiredSamples = results3D.requiredSamples;

	if (ImGui::Button("Thrash the cache with Gameobject3D"))
	{
		ThrashTheCache<GameObject3D>(results3D);
		results3D.hasBeenExecuted = true;
	}

	if (results3D.hasBeenExecuted)
	{
		AddResultsPlot(results3D, "Gameobject3D test", color3DResult);
	}

	if (ImGui::Button("Thrash the cache with Gameobject3DAlt"))
	{
		ThrashTheCache<GameObject3DAlt>(results3DAlt);
		results3DAlt.hasBeenExecuted = true;
	}

	if (results3DAlt.hasBeenExecuted)
	{
		AddResultsPlot(results3DAlt, "Gameobject3DAlt test", color3DAltResult);
	}

	if (results3D.hasBeenExecuted && results3DAlt.hasBeenExecuted)
	{
		ImGui::Text("Combined:");

		ImGui::PlotConfig conf{};

		std::vector<const float*> data{};
		data.push_back(results3D.samples.data());
		data.push_back(results3DAlt.samples.data());

		conf.values.xs = results3D.steps.data();
		conf.values.count = (int)results3D.steps.size();
		conf.values.ys_list = data.data();
		conf.values.ys_count = (int)data.size();

		static ImU32 colors[2] = { color3DResult, color3DAltResult };

		conf.values.colors = colors;

		conf.frame_size = ImVec2(200, 100);

		conf.grid_y.show = true;
		conf.grid_y.size = 3000;

		conf.tooltip.show = true;
		conf.tooltip.format = "x=%.2f, y=%.2f";

		conf.scale.min = 0.0f;
		conf.scale.max = 30000.0f;

		conf.line_thickness = 2.f;

		ImGui::Plot("Combined plots", conf);
	}

	ImGui::End();
	
}
