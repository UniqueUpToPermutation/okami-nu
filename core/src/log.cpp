#include <okami/log.hpp>

#include <plog/Initializers/ConsoleInitializer.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>

using namespace okami;
using namespace okami::log;

void okami::log::Init() {
    static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);
}

void okami::Log(Error const& err) {
    if (err.IsError()) {
        if (auto multi = std::get_if<MultipleErrors>(&err.details)) {
            for (auto const& subErr : multi->errors) {
                Log(subErr);
            }
        } else {
            PLOG_ERROR << err.ToString();
        }
    }
}
