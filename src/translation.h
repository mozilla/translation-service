
#ifndef TRANSLATION_SERVICE_TRANSLATION_H
#define TRANSLATION_SERVICE_TRANSLATION_H


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

                    std::cout << "Looking for models in " << basePath << std::endl;

                    for (const auto &entry2: std::filesystem::directory_iterator(basePath)) {
                        std::cout << "Adding file " << entry2.path().filename().string() << std::endl;
                        if (endsWith(entry2.path().filename().string(), ".spm"))
                            vocabPath = entry2.path().string();
                        else if (endsWith(entry2.path().filename().string(), ".intgemm.alphas.bin"))
                            modelPath = entry2.path().string();
                        else if (endsWith(entry2.path().filename().string(), ".s2t.bin"))
                            shortlistPath = entry2.path().string();
                    }

                    std::cout << "Building models config for " << pair << std::endl;

                    auto config = buildConfig(modelPath, vocabPath, shortlistPath);
                    auto options = parseOptionsFromString(config);
                    MemoryBundle memoryBundle;
                    std::cout << "Creating translation model for " << pair << std::endl;
                    auto translationModel = New<TranslationModel>(options, std::move(memoryBundle), _numWorkers);
                    _models[pair] = translationModel;
                    std::cout << "Model " << pair << " is loaded" << std::endl;
                }

                if (_models.empty())
                    throw std::logic_error("Models directory is empty");
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

    }
}


#endif //TRANSLATION_SERVICE_TRANSLATION_H
