#pragma once
#include <sstream>
#include <iomanip>

namespace Unique
{
	enum ShaderDef
	{
		SKINNED,
	};


	class ShaderUtil
	{
	public:

		static unsigned GetPassIndex(const String& passName);
		static String GetShaderPath(DeviceType deviceType);
		static Vector<String> SplitDef(const String& defs);

		static unsigned basePassIndex;
		static unsigned alphaPassIndex;
		static unsigned materialPassIndex;
		static unsigned deferredPassIndex;
		static unsigned lightPassIndex;
		static unsigned litBasePassIndex;
		static unsigned litAlphaPassIndex;
		static unsigned shadowPassIndex;

		static const Vector<String> interDefs;
		inline static uint64 InterMask() { return (uint64)((1 << (interDefs.size() + 1)) - 1) << 32; }
	private:
		/// Pass index assignments.
		static HashMap<String, unsigned> passIndices;
	};
	
	struct ShaderMacro
	{
		const char* Name;
		const char* Definition;
		ShaderMacro(const char* _Name, const char* _Def) : Name( _Name ), Definition( _Def ) {}
	};

	class ShaderMacroHelper
	{
	public:

		template<typename DefintionType>
		void AddShaderMacro( const char* Name, DefintionType Definition )
		{
			std::ostringstream ss;
			ss << Definition;
			AddShaderMacro<const char*>( Name, ss.str().c_str() );
		}
	
		void Finalize()
		{
			if (!m_bIsFinalized)
			{
				m_Macros.emplace_back(nullptr, nullptr);
				m_bIsFinalized = true;
			}
		}

		void Reopen()
		{
			if (m_bIsFinalized)
			{
				assert(m_Macros.size() > 0 && m_Macros.back().Name == nullptr && m_Macros.back().Definition == nullptr);
				m_Macros.pop_back();
				m_bIsFinalized = false;
			}
		}

		void Clear()
		{
			m_Macros.clear();
			m_DefinitionsPool.clear();
			m_bIsFinalized = false;
		}

		operator const ShaderMacro* ()
		{
			if (m_Macros.size() > 0 && !m_bIsFinalized)
				Finalize();
			return m_Macros.size() ? m_Macros.data() : nullptr;
		}
    
	private:

		Vector< ShaderMacro > m_Macros;
		std::set< std::string > m_DefinitionsPool;
		bool m_bIsFinalized = false;
	};

	template<>
	inline void ShaderMacroHelper::AddShaderMacro( const char* Name, const char* Definition )
	{
		Reopen();
		auto *PooledDefinition = m_DefinitionsPool.insert(Definition).first->c_str();
		m_Macros.emplace_back(Name, PooledDefinition);
	}

	template<>
	inline void ShaderMacroHelper::AddShaderMacro( const char* Name, bool Definition )
	{
		AddShaderMacro( Name, Definition ? "1" : "0");
	}

	template<>
	inline void ShaderMacroHelper::AddShaderMacro( const char* Name, float Definition )
	{
		std::ostringstream ss;
    
		// Make sure that when floating point represents integer, it is still
		// written as float: 1024.0, but not 1024. This is essnetial to
		// avoid type conversion issues in GLES.
		if( Definition == static_cast<float>(static_cast<int>(Definition)) )
			ss << std::fixed << std::setprecision( 1 );

		ss << Definition;
		AddShaderMacro<const char*>( Name, ss.str().c_str() );
	}
}
