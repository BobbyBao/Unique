#include "UniquePCH.h"
#include "Graphics/ShaderEffect.h"
#include "Serialize/JsonSerializer.h"
#include "Serialize/JsonDeserializer.h"
#include "Serialize/HjsonDeserializer.h"
#include <Timer.h>
#include "Serialize/DSL/ASTParser.h"
#include "Serialize/DslDeserializer.h"
#include <iostream>

namespace Unique
{

	struct PerfBlock
	{
		const char* name_;
		Diligent::Timer timer_;
	
		int test_;

		PerfBlock& Test(int val)
		{ 
			test_ = val;
			return *this;
		}

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

	uStruct(PerfBlock)
	{
		uMixedAttribute("Test", /*int,*/ test_, Test)
	}

	enum TestEnum : byte
	{
		AAA
	};

	enum TestEnum1 : int
	{
		BBB
	};
	void Test()
	{
		int sz = sizeof(TestEnum);
		int sz1 = sizeof(TestEnum1);

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
			SPtr<Shader> shader(new Shader());
			DslDeserializer reader;
			reader.Load(*file, shader);

			JsonSerializer jsonSer;
			jsonSer.Save("basic.json", shader);
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
				pass->shaderStage_[0] = { Diligent::SHADER_TYPE_VERTEX, "Assets/shader.hlsl", "VS", Diligent::SHADER_PROFILE_DX_4_0 };
				pass->shaderStage_[1] = { Diligent::SHADER_TYPE_PIXEL, "Assets/shader.hlsl", "PS", Diligent::SHADER_PROFILE_DX_4_0 };
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
