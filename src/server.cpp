
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


const std::string buildConfig(const std::string &modelPath, const std::string &vocabPath, const std::string &shortlistPath)
{
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

      options = options + "\nmodels: [" + modelPath + "]\nvocabs: [" + vocabPath + ", " + vocabPath + "]\nshortlist: [" + shortlistPath + ", 50, 50]";

  return options;
}

static bool endsWith(std::string_view str, std::string_view suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

void loadModels(const std::string modelsDir, std::map<std::string, Ptr<TranslationModel>>& models)
{
  for (const auto & entry : std::filesystem::directory_iterator(modelsDir))
  {
    auto basePath = entry.path();
    auto pair = entry.path().filename().string();
    std::string vocabPath = "";
    std::string modelPath = "";
    std::string shortlistPath = "";

    for (const auto & entry2 : std::filesystem::directory_iterator(basePath)) {
      if (endsWith(entry2.path().filename().string(), ".spm"))
        vocabPath = entry2.path().string();
      else if (endsWith(entry2.path().filename().string(), ".intgemm.alphas.bin"))
        modelPath = entry2.path().string();
      else if (endsWith(entry2.path().filename().string(), ".s2t.bin"))
        shortlistPath = entry2.path().string();
    }

    auto config = buildConfig(modelPath, vocabPath,  shortlistPath);
    auto options = parseOptionsFromString(config);
    MemoryBundle memoryBundle;
    Ptr<TranslationModel> translationModel = New<TranslationModel>(options, std::move(memoryBundle));
    models[pair] = translationModel;
    CROW_LOG_INFO << "Model " << pair << " is loaded";
  }
}

void run(int port) {

  std::map<std::string, Ptr<TranslationModel>> models;
  loadModels("/models", models);

  const int numWorkers = 1;
  AsyncService::Config asyncConfig{numWorkers};
  AsyncService service(asyncConfig);

  crow::SimpleApp app;

  CROW_ROUTE(app, "/v1/translate")
  .methods("POST"_method)
  ([&service, &models](const crow::request& req){
      auto x = crow::json::load(req.body);
      if (!x)
        return crow::response(400);

      std::string from = (std::string)x["from"];
      std::string to = (std::string)x["to"];
      std::string input = (std::string)x["text"];
      std::string pair = from + to;

      if (models.find( pair ) == models.end()) {
        CROW_LOG_INFO << "No language pair " << pair;
        return crow::response(400);
      }

      auto model = models[pair];

      ResponseOptions responseOptions;
      // Wait on future until Response is complete
      std::promise<Response> responsePromise;
      std::future<Response> responseFuture = responsePromise.get_future();
      auto callback = [&responsePromise](Response &&response) { responsePromise.set_value(std::move(response)); };
      CROW_LOG_INFO << "Starting translation from " << from << " to " << to << ": "<< input;
      service.translate(model, std::move(input), std::move(callback), responseOptions);
      responseFuture.wait();
      Response response = responseFuture.get();
      std::string result = response.target.text;
      CROW_LOG_INFO << "Finished translation from " << from << " to " << to << ": "<< result;

      crow::json::wvalue jsonRes;
      jsonRes["result"] = result;
      return crow::response(jsonRes);
  });

  CROW_ROUTE(app, "/status")
  ([]{
      return "Ready";
  });

  app.port(port).multithreaded().run();
 }
}}

int main(int argc, char *argv[]) {
  marian::bergamot::run(8080);

  return 0;
}
