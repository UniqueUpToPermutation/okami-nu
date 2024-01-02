#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <set>

#include <okami/embed.hpp>

namespace okami {
    namespace preprocessor {
        struct PathHash {
            std::size_t operator()(const std::filesystem::path& p) const noexcept {
                return std::filesystem::hash_value(p);
            }
        };
    }

	struct ShaderPreprocessorConfig {
		std::unordered_map<std::string, std::string> defines;
		std::vector<std::string> headerItems;

		std::string Stringify(const ShaderPreprocessorConfig* overrides) const;
	};

	struct ShaderPreprocessorOutput {
		std::vector<std::string> sources;
		std::string content;
	};
	
	class ShaderPreprocessor {
	private:
		static Error Load(const std::filesystem::path& source,
			VirtualFileSystem const& fileLoader,
			const ShaderPreprocessorConfig& defaults,
			const ShaderPreprocessorConfig* overrides,
			std::stringstream& streamOut,
            std::stringstream* warningsOut,
            ShaderPreprocessorOutput* output,
			bool bAddLineNumbers,
			std::unordered_set<std::filesystem::path, preprocessor::PathHash>& alreadyVisited);

	public:
		static Expected<ShaderPreprocessorOutput> Load(const std::filesystem::path& source,
			VirtualFileSystem const& fileLoader,
			const ShaderPreprocessorConfig& defaults, 
            std::stringstream* warningsOut,
			bool bAddLineNumbers,
			const ShaderPreprocessorConfig* overrides = nullptr);
	};
}