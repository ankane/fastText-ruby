// stdlib
#include <cmath>
#include <iterator>
#include <sstream>
#include <stdexcept>

// fasttext
#include <args.h>
#include <autotune.h>
#include <densematrix.h>
#include <fasttext.h>
#include <real.h>
#include <vector.h>

// rice
#include <rice/rice.hpp>
#include <rice/stl.hpp>

using fasttext::Args;
using fasttext::FastText;

using Rice::Array;
using Rice::Constructor;
using Rice::Module;
using Rice::define_class_under;
using Rice::define_module;
using Rice::define_module_under;

namespace Rice::detail
{
  template<>
  class To_Ruby<std::vector<std::pair<fasttext::real, std::string>>>
  {
  public:
    VALUE convert(std::vector<std::pair<fasttext::real, std::string>> const & x)
    {
      Array ret;
      for (const auto& v : x) {
        Array a;
        a.push(v.first);
        a.push(v.second);
        ret.push(a);
      }
      return ret;
    }
  };
}

extern "C"
void Init_ext()
{
  Module rb_mFastText = define_module("FastText");
  Module rb_mExt = define_module_under(rb_mFastText, "Ext");

  define_class_under<Args>(rb_mExt, "Args")
    .define_constructor(Constructor<Args>())
    .define_attr("input", &Args::input)
    .define_attr("output", &Args::output)
    .define_attr("lr", &Args::lr)
    .define_attr("lr_update_rate", &Args::lrUpdateRate)
    .define_attr("dim", &Args::dim)
    .define_attr("ws", &Args::ws)
    .define_attr("epoch", &Args::epoch)
    .define_attr("min_count", &Args::minCount)
    .define_attr("min_count_label", &Args::minCountLabel)
    .define_attr("neg", &Args::neg)
    .define_attr("word_ngrams", &Args::wordNgrams)
    .define_method(
      "loss=",
      [](Args& a, const std::string& str) {
        if (str == "softmax") {
          a.loss = fasttext::loss_name::softmax;
        } else if (str == "ns") {
          a.loss = fasttext::loss_name::ns;
        } else if (str == "hs") {
          a.loss = fasttext::loss_name::hs;
        } else if (str == "ova") {
          a.loss = fasttext::loss_name::ova;
        } else {
          throw std::invalid_argument("Unknown loss: " + str);
        }
      })
    .define_method(
      "model=",
      [](Args& a, const std::string& str) {
        if (str == "supervised") {
          a.model = fasttext::model_name::sup;
        } else if (str == "skipgram") {
          a.model = fasttext::model_name::sg;
        } else if (str == "cbow") {
          a.model = fasttext::model_name::cbow;
        } else {
          throw std::invalid_argument("Unknown model: " + str);
        }
      })
    .define_attr("bucket", &Args::bucket)
    .define_attr("minn", &Args::minn)
    .define_attr("maxn", &Args::maxn)
    .define_attr("thread", &Args::thread)
    .define_attr("t", &Args::t)
    .define_attr("label_prefix", &Args::label)
    .define_attr("verbose", &Args::verbose)
    .define_attr("pretrained_vectors", &Args::pretrainedVectors)
    .define_attr("save_output", &Args::saveOutput)
    .define_attr("seed", &Args::seed)
    .define_attr("autotune_validation_file", &Args::autotuneValidationFile)
    .define_attr("autotune_metric", &Args::autotuneMetric)
    .define_attr("autotune_predictions", &Args::autotunePredictions)
    .define_attr("autotune_duration", &Args::autotuneDuration)
    .define_attr("autotune_model_size", &Args::autotuneModelSize);

  define_class_under<FastText>(rb_mExt, "Model")
    .define_constructor(Constructor<FastText>())
    .define_method(
      "words",
      [](FastText& m) {
        std::shared_ptr<const fasttext::Dictionary> d = m.getDictionary();
        std::vector<int64_t> freq = d->getCounts(fasttext::entry_type::word);

        Array vocab_list;
        Array vocab_freq;
        for (int32_t i = 0; i < d->nwords(); i++) {
          vocab_list.push(d->getWord(i));
          vocab_freq.push(freq[i]);
        }

        Array ret;
        ret.push(vocab_list);
        ret.push(vocab_freq);
        return ret;
      })
    .define_method(
      "labels",
      [](FastText& m) {
        std::shared_ptr<const fasttext::Dictionary> d = m.getDictionary();
        std::vector<int64_t> freq = d->getCounts(fasttext::entry_type::label);

        Array vocab_list;
        Array vocab_freq;
        for (int32_t i = 0; i < d->nlabels(); i++) {
          vocab_list.push(d->getLabel(i));
          vocab_freq.push(freq[i]);
        }

        Array ret;
        ret.push(vocab_list);
        ret.push(vocab_freq);
        return ret;
      })
    .define_method(
      "test",
      [](FastText& m, const std::string& filename, int32_t k) {
        std::ifstream ifs(filename);
        if (!ifs.is_open()) {
          throw std::invalid_argument("Test file cannot be opened!");
        }
        fasttext::Meter meter(false);
        m.test(ifs, k, 0.0, meter);
        ifs.close();

        Array ret;
        ret.push(meter.nexamples());
        ret.push(meter.precision());
        ret.push(meter.recall());
        return ret;
      })
    .define_method(
      "load_model",
      [](FastText& m, const std::string& s) {
        m.loadModel(s);
      })
    .define_method(
      "save_model",
      [](FastText& m, const std::string& s) {
        m.saveModel(s);
      })
    .define_method("dimension", &FastText::getDimension)
    .define_method("quantized?", &FastText::isQuant)
    .define_method("word_id", &FastText::getWordId)
    .define_method("subword_id", &FastText::getSubwordId)
    .define_method(
      "predict",
      [](FastText& m, const std::string& text, int32_t k, float threshold) {
        std::stringstream ioss(text);
        std::vector<std::pair<fasttext::real, std::string>> predictions;
        m.predictLine(ioss, predictions, k, threshold);
        return predictions;
      })
    .define_method(
      "nearest_neighbors",
      [](FastText& m, const std::string& word, int32_t k) {
        return m.getNN(word, k);
      })
    .define_method("analogies", &FastText::getAnalogies)
    // .define_method("ngram_vectors", &FastText::getNgramVectors)
    .define_method(
      "word_vector",
      [](FastText& m, const std::string& word) {
        auto dimension = m.getDimension();
        fasttext::Vector vec = fasttext::Vector(dimension);
        m.getWordVector(vec, word);
        Array ret;
        for (size_t i = 0; i < vec.size(); i++) {
          ret.push(vec[i]);
        }
        return ret;
      })
    .define_method(
      "subwords",
      [](FastText& m, const std::string& word) {
        std::vector<std::string> subwords;
        std::vector<int32_t> ngrams;
        std::shared_ptr<const fasttext::Dictionary> d = m.getDictionary();
        d->getSubwords(word, ngrams, subwords);

        Array ret;
        for (const auto& subword : subwords) {
          ret.push(subword);
        }
        return ret;
      })
    .define_method(
      "sentence_vector",
      [](FastText& m, const std::string& text) {
        std::istringstream in(text);
        auto dimension = m.getDimension();
        fasttext::Vector vec = fasttext::Vector(dimension);
        m.getSentenceVector(in, vec);
        Array ret;
        for (size_t i = 0; i < vec.size(); i++) {
          ret.push(vec[i]);
        }
        return ret;
      })
    .define_method(
      "train",
      [](FastText& m, Args& a) {
        if (a.hasAutotune()) {
          fasttext::Autotune autotune(std::shared_ptr<fasttext::FastText>(&m, [](fasttext::FastText*) {}));
          autotune.train(a);
        } else {
          m.train(a);
        }
      })
    .define_method(
      "quantize",
      [](FastText& m, Args& a) {
        m.quantize(a);
      })
    .define_method(
      "supervised?",
      [](FastText& m) {
        return m.getArgs().model == fasttext::model_name::sup;
      })
    .define_method(
      "label_prefix",
      [](FastText& m) {
        return m.getArgs().label;
      });
}
