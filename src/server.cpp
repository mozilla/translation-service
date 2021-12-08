
#include <algorithm>
#include <cstdlib>
#include <future>
#include <iostream>
#include <sstream>
#include <map>
#include <filesystem>

#include "common/definitions.h"
#include "common/timer.h"
#include "common/utils.h"
#include "marian.h"
#include "translator/byte_array_util.h"
#include "translator/parser.h"
#include "translator/response.h"
#include "translator/response_options.h"
#include "translator/service.h"
#include "crow.h"

namespace marian {
    namespace bergamot {

        static bool endsWith(std::string_view str, std::string_view suffix) {
            return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
        }

        class TranslatorWrapper {

        public:
            TranslatorWrapper(size_t numWorkers=1)
                    : _service(AsyncService::Config{numWorkers}), _models(), _numWorkers(numWorkers) {};

            void loadModels(const std::string modelsDir) {
                for (const auto &entry: std::filesystem::directory_iterator(modelsDir)) {
                    auto basePath = entry.path();
                    auto pair = entry.path().filename().string();
                    std::string vocabPath = "";
                    std::string modelPath = "";
                    std::string shortlistPath = "";

                    for (const auto &entry2: std::filesystem::directory_iterator(basePath)) {
                        if (endsWith(entry2.path().filename().string(), ".spm"))
                            vocabPath = entry2.path().string();
                        else if (endsWith(entry2.path().filename().string(), ".intgemm.alphas.bin"))
                            modelPath = entry2.path().string();
                        else if (endsWith(entry2.path().filename().string(), ".s2t.bin"))
                            shortlistPath = entry2.path().string();
                    }

                    auto config = buildConfig(modelPath, vocabPath, shortlistPath);
                    auto options = parseOptionsFromString(config);
                    MemoryBundle memoryBundle;
                    auto translationModel = New<TranslationModel>(options, std::move(memoryBundle), _numWorkers);
                    _models[pair] = translationModel;
                }
            }

            int isSupported(const std::string from, const std::string to) {
                if (isSupportedInternal(from, to) ||
                    (isSupportedInternal(from, "en") && isSupportedInternal("en", to)))
                    return 1;
                else
                    return 0;
            }

            std::string translate(const std::string from, const std::string to, std::string input) {
                std::string result;

                if (isSupportedInternal(from, to)) {
                    result = translateInternal(from, to, input);
                } else if (isSupportedInternal(from, "en") && isSupportedInternal("en", to)) {
                    auto intermediateRes = translateInternal(from, "en", input);
                    result = translateInternal("en", to, intermediateRes);
                } else {
                    throw std::invalid_argument("Language pair is not supported");
                }

                return result;
            }

        private:
            std::map <std::string, Ptr<TranslationModel>> _models;
            AsyncService _service;
            size_t _numWorkers;

            const std::string
            buildConfig(const std::string &modelPath, const std::string &vocabPath, const std::string &shortlistPath) {
                std::string options =
                        "beam-size: 1"
                        "\nnormalize: 1.0"
                        "\nword-penalty: 0"
                        "\nmax-length-break: 128"
                        "\nmini-batch-words: 1024"
                        "\nworkspace: 128"
                        "\nmax-length-factor: 2.0"
                        "\nskip-costls: True"
                        "\nquiet: True"
                        "\nquiet-translation: True"
                        "\ngemm-precision: int8shift";

                options = options + "\nmodels: [" + modelPath + "]\nvocabs: [" + vocabPath + ", " + vocabPath +
                          "]\nshortlist: [" + shortlistPath + ", 50, 50]";

                return options;
            }

            std::string translateInternal(const std::string from, const std::string to, std::string input) {
                std::string pair = from + to;
                auto model = _models[pair];

                ResponseOptions responseOptions;
                // Wait on future until Response is complete
                std::promise <Response> responsePromise;
                std::future <Response> responseFuture = responsePromise.get_future();
                auto callback = [&responsePromise](Response &&response) {
                    responsePromise.set_value(std::move(response));
                };

                _service.translate(model, std::move(input), std::move(callback), responseOptions);
                responseFuture.wait();
                Response response = responseFuture.get();
                return response.target.text;
            }

            int isSupportedInternal(const std::string from, const std::string to) {
                std::string langPair = from + to;
                if (_models.find(langPair) == _models.end())
                    return 0;
                else
                    return 1;
            }


        };

        void run(int port, int workers, crow::LogLevel logLevel ) {

            TranslatorWrapper wrapper(workers);
            wrapper.loadModels("/models");

            crow::SimpleApp app;

            CROW_ROUTE(app, "/v1/translate")
                    .methods("POST"_method)
                            ([&wrapper](const crow::request &req) {
                                auto x = crow::json::load(req.body);
                                if (!x) {
                                    CROW_LOG_WARNING << "Bad json: " << x;
                                    return crow::response(400);
                                }

                                std::string from = (std::string) x["from"];
                                std::string to = (std::string) x["to"];
                                std::string input = (std::string) x["text"];

                                if (!wrapper.isSupported(from, to)) {
                                    CROW_LOG_WARNING << "Language pair is not supported: " << from << to;
                                    return crow::response(400);
                                }
                                CROW_LOG_DEBUG << "Starting translation from " << from << " to " << to << ": " << input;
                                auto result = wrapper.translate(from, to, input);
                                CROW_LOG_DEBUG << "Finished translation from " << from << " to " << to << ": " << result;

                                crow::json::wvalue jsonRes;
                                jsonRes["result"] = result;
                                return crow::response(jsonRes);
                            });

            CROW_ROUTE(app, "/status")
                    ([] {
                        return "Ready";
                    });

            app
            .port(port)
            .loglevel(logLevel)
            .multithreaded()
            .run();
        }
    }
}

std::string getEnvVar(const std::string& key, const std::string& defaultVal ) {
    char * val = getenv( key.c_str() );
    return val == NULL ? std::string(defaultVal) : std::string(val);
}

int main(int argc, char *argv[]) {
    auto port = std::stoi(getEnvVar("PORT", "8000"));

    auto logLevelVar = getEnvVar("LOG_LEVEL", "INFO");
    auto logLevel = crow::LogLevel::Info;
    if (logLevelVar == "WARNING")
        logLevel=crow::LogLevel::Warning;
    else if (logLevelVar == "ERROR")
        logLevel=crow::LogLevel::Error;
    else if (logLevelVar == "INFO")
        logLevel=crow::LogLevel::Info;
    else if (logLevelVar == "DEBUG")
        logLevel=crow::LogLevel::Debug;
    else {
        throw std::invalid_argument("Unknown logging level");
    }

    auto workers = std::stoi(getEnvVar("NUM_WORKERS", "1"));
    if (workers == 0)
        workers = std::thread::hardware_concurrency();

    marian::bergamot::run(port, workers, logLevel);

    return 0;
}
