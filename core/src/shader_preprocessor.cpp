#include <okami/shader_preprocessor.hpp>

#include <sstream>
#include <algorithm>
#include <fstream>

#include <regex>

using namespace okami;

std::string okami::ShaderPreprocessorConfig::Stringify(const ShaderPreprocessorConfig* overrides) const {
    std::stringstream ss;

    for (auto& it : headerItems) {
        ss << it << std::endl << std::endl;
    }

    for (auto& it : defines) {
        ss << "#define " << it.first << " " << it.second << std::endl;
    }
    if (overrides) {
        for (auto& it : overrides->defines) {
            ss << "#undef " << it.first << std::endl;
            ss << "#define " << it.first << " " << it.second << std::endl;
        }
    }

    ss << std::endl << std::endl;
    return ss.str();
}

Error okami::ShaderPreprocessor::Load(
    const std::filesystem::path& source,
    VirtualFileSystem const& fileLoader,
    const ShaderPreprocessorConfig& defaults,
    const ShaderPreprocessorConfig* overrides,
    std::stringstream& streamOut,
    std::stringstream* warningOut,
    ShaderPreprocessorOutput* output,
    bool bAddLineNumbers,
    std::unordered_set<std::filesystem::path, preprocessor::PathHash>& alreadyVisited) {

    Error err;

    if (alreadyVisited.find(source) != alreadyVisited.end()) {
        return {};
    }

    alreadyVisited.emplace(source);

    auto contents = OKAMI_ERR_UNWRAP(fileLoader.Find(source), err);
    output->sources.emplace_back(source);

    // Write preprocessed string
    std::stringstream& ss = streamOut;
    if (alreadyVisited.size() == 1) {
        ss << defaults.Stringify(overrides) << std::endl;
    }

    auto sourceStr = source.string();

    // Reset line numbers
    if (bAddLineNumbers)
        ss << std::endl << "#line 1 \"" << 
            sourceStr << "\"" << std::endl; 

    // Find all includes
    size_t include_pos = contents.find("#include");
    size_t last_include_pos = 0;
    size_t current_line = 0;
    while (include_pos != std::string::npos) {

        current_line += std::count(
            &contents.at(last_include_pos), 
            &contents.at(include_pos), '\n');

        size_t endLineIndex = contents.find('\n', include_pos);
        bool bGlobalSearch = false;

        auto quotesIt = std::find(
            &contents.at(include_pos),
            &contents.at(endLineIndex), '\"');

        ss << contents.substr(last_include_pos, include_pos - last_include_pos);

        if (quotesIt == &contents.at(endLineIndex)) {
            if (warningOut) {
                *warningOut << sourceStr << 
                    ": Warning: #include detected without include file!\n";
            }
        }
        else {
            size_t endIndx;
            size_t startIndx;

            startIndx = quotesIt - &contents.at(0) + 1;
            endIndx = contents.find('\"', startIndx);
            bGlobalSearch = false;
            
            if (endIndx == std::string::npos) {
                if (warningOut) {
                    *warningOut << sourceStr << 
                        ": Warning: unmatched quote in #include!\n";
                }
                return {};
            }

            last_include_pos = endIndx + 1;

            std::filesystem::path includeSource = contents.substr(startIndx, endIndx - startIndx);
            std::filesystem::path nextPath = source.parent_path();

            if (bGlobalSearch) {
                auto err = Load(includeSource, fileLoader, defaults, overrides, streamOut,
                    warningOut, output, bAddLineNumbers, alreadyVisited);

            } else {
                includeSource = nextPath / includeSource;

                auto err = Load(includeSource, fileLoader, defaults, overrides, streamOut,
                    warningOut, output, bAddLineNumbers, alreadyVisited);
            }

            if (bAddLineNumbers)
                ss << std::endl << "\n#line " << current_line + 1 
                    << " \"" << sourceStr << "\"\n"; // Reset line numbers
        }

        include_pos = contents.find("#include", include_pos + 1);
    }

    ss << contents.substr(last_include_pos);
    return {};
}

Expected<ShaderPreprocessorOutput> okami::ShaderPreprocessor::Load(
    const std::filesystem::path& source,
    VirtualFileSystem const& fileLoader,
    const ShaderPreprocessorConfig& defaults,
    std::stringstream* warningsOut,
    bool bAddLineNumbers,
    const ShaderPreprocessorConfig* overrides) {

    std::unordered_set<std::filesystem::path, preprocessor::PathHash> alreadyVisited;
    std::stringstream streamOut;

    ShaderPreprocessorOutput output;
    auto err = Load(source, fileLoader,
        defaults, overrides,
        streamOut, warningsOut,
        &output, bAddLineNumbers, alreadyVisited);
    OKAMI_EXP_RETURN(err);
    
    output.content = streamOut.str();
    output.content = std::regex_replace(
        output.content, std::regex("#pragma[^\n]*\n"), "\n");
    return output;
}