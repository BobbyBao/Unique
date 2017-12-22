#include "UniquePCH.h"
#include "Graphics/Shader.h"
#include "Serialize/JsonSerializer.h"
#include "Serialize/JsonDeserializer.h"
#include "Serialize/HjsonDeserializer.h"
#include <Timer.h>
#include "Serialize/DSL/AstParser.h"
#include "Serialize/DSL/ScriptLexer.h"
#include <iostream>

namespace Unique
{
	struct PerfBlock
	{
		const char* name_;
		Diligent::Timer timer_;
		PerfBlock(const char* name) : name_(name)
		{
			timer_.Restart();
		}

		~PerfBlock()
		{
			double t = timer_.GetElapsedTime()*1000;
			UNIQUE_LOGINFO(FormatString("%s, Time elapsed : %f", name_, t));
		}
	};

	void TestPerf();
	void TestParse();

	void Test()
	{
		TestParse();
		return;
		TestPerf();
		return;

		HashMap<String, int> a= { {"", 0} };

		SPtr<Shader> shader(new Shader());
		HjsonDeserializer jsonReader(true);
		jsonReader.Load("Shaders/Test.shader", shader);
#ifdef NO_VIRTUAL
		JsonWriter jsonWriter;
		jsonWriter.Save("test2.json", shader);
#endif
		{

			SPtr<Shader> newShader(new Shader());

			File file("test2.json");
			JsonDeserializer jsonDeser;
			jsonDeser.Load(file, newShader);

			JsonSerializer jsonSer;
			jsonSer.Save("test3.json", newShader);
		}

	}

	void TestParse()
	{
		auto& cache = GetSubsystem<ResourceCache>();
		SPtr<File> file = cache.GetFile("Shaders/Basic.shader");
		if (file)
		{
			AstParser tok;
			tok.Parse(file->ReadAllText());

		}
	}

	void TestPerf()
	{
		SPtr<Shader> shader(new Shader());
		shader->SetName("test_shader");

		const int COUNT = 100;
		{
			PerfBlock("Create");
			for (int i = 0; i < COUNT; i++)
			{
				Pass* pass = shader->AddPass(String("Test_") + String(i));
				pass->shaderStage_[0] = { ShaderType::SHADER_TYPE_VERTEX, "Assets/shader.hlsl", "VS", SHADER_PROFILE_DX_4_0 };
				pass->shaderStage_[1] = { ShaderType::SHADER_TYPE_PIXEL, "Assets/shader.hlsl", "PS", SHADER_PROFILE_DX_4_0 };
			}

		}

		{
			PerfBlock("Virtual");
			JsonSerializer jsonSer;
			jsonSer.Save("perf_2.json", shader);
		}

#ifdef NO_JSON
		{
			PerfBlock("No virtual");
			JsonWriter jsonWriter;
			jsonWriter.Save("perf_1.json", shader);
		}
#endif
		/*
		SPtr<Shader> s(new Shader());
		JsonReader jsonReader;
		jsonReader.Load("test.json", s);

		jsonWriter.Save("test1.json", s);*/
		

	}

}
