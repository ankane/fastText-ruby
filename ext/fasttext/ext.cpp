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

using fasttext::FastText;

using Rice::Array;
using Rice::Constructor;
using Rice::Hash;
using Rice::Module;
using Rice::Object;
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

fasttext::Args buildArgs(Hash h) {
  fasttext::Args a;

  for (const auto& it : h)
  {
    auto name = it.key.to_s().str();
    auto value = (it.value).value();

    if (name == "input") {
      a.input = Rice::detail::From_Ruby<std::string>().convert(value);
    } else if (name == "output") {
      a.output = Rice::detail::From_Ruby<std::string>().convert(value);
    } else if (name == "lr") {
      a.lr = Rice::detail::From_Ruby<double>().convert(value);
    } else if (name == "lr_update_rate") {
      a.lrUpdateRate = Rice::detail::From_Ruby<int>().convert(value);
    } else if (name == "dim") {
      a.dim = Rice::detail::From_Ruby<int>().convert(value);
    } else if (name == "ws") {
      a.ws = Rice::detail::From_Ruby<int>().convert(value);
    } else if (name == "epoch") {
      a.epoch = Rice::detail::From_Ruby<int>().convert(value);
    } else if (name == "min_count") {
      a.minCount = Rice::detail::From_Ruby<int>().convert(value);
    } else if (name == "min_count_label") {
      a.minCountLabel = Rice::detail::From_Ruby<int>().convert(value);
    } else if (name == "neg") {
      a.neg = Rice::detail::From_Ruby<int>().convert(value);
    } else if (name == "word_ngrams") {
      a.wordNgrams = Rice::detail::From_Ruby<int>().convert(value);
    } else if (name == "loss") {
      std::string str = Rice::detail::From_Ruby<std::string>().convert(value);
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
    } else if (name == "model") {
      std::string str = Rice::detail::From_Ruby<std::string>().convert(value);
      if (str == "supervised") {
        a.model = fasttext::model_name::sup;
      } else if (str == "skipgram") {
        a.model = fasttext::model_name::sg;
      } else if (str == "cbow") {
        a.model = fasttext::model_name::cbow;
      } else {
        throw std::invalid_argument("Unknown model: " + str);
      }
    } else if (name == "bucket") {
      a.bucket = Rice::detail::From_Ruby<int>().convert(value);
    } else if (name == "minn") {
      a.minn = Rice::detail::From_Ruby<int>().convert(value);
    } else if (name == "maxn") {
      a.maxn = Rice::detail::From_Ruby<int>().convert(value);
    } else if (name == "thread") {
      a.thread = Rice::detail::From_Ruby<int>().convert(value);
    } else if (name == "t") {
      a.t = Rice::detail::From_Ruby<double>().convert(value);
    } else if (name == "label_prefix") {
      a.label = Rice::detail::From_Ruby<std::string>().convert(value);
    } else if (name == "verbose") {
      a.verbose = Rice::detail::From_Ruby<int>().convert(value);
    } else if (name == "pretrained_vectors") {
      a.pretrainedVectors = Rice::detail::From_Ruby<std::string>().convert(value);
    } else if (name == "save_output") {
      a.saveOutput = Rice::detail::From_Ruby<bool>().convert(value);
    } else if (name == "seed") {
      a.seed = Rice::detail::From_Ruby<int>().convert(value);
    } else if (name == "autotune_validation_file") {
      a.autotuneValidationFile = Rice::detail::From_Ruby<std::string>().convert(value);
    } else if (name == "autotune_metric") {
      a.autotuneMetric = Rice::detail::From_Ruby<std::string>().convert(value);
    } else if (name == "autotune_predictions") {
      a.autotunePredictions = Rice::detail::From_Ruby<int>().convert(value);
    } else if (name == "autotune_duration") {
      a.autotuneDuration = Rice::detail::From_Ruby<int>().convert(value);
    } else if (name == "autotune_model_size") {
      a.autotuneModelSize = Rice::detail::From_Ruby<std::string>().convert(value);
    } else {
      throw std::invalid_argument("Unknown argument: " + name);
    }
  }
  return a;
}

extern "C"
void Init_ext()
{
  Module rb_mFastText = define_module("FastText");
  Module rb_mExt = define_module_under(rb_mFastText, "Ext");

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
        int dimension = m.getDimension();
        fasttext::Vector vec = fasttext::Vector(dimension);
        m.getWordVector(vec, word);
        float* data = vec.data();
        Array ret;
        for (int i = 0; i < dimension; i++) {
          ret.push(data[i]);
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
        int dimension = m.getDimension();
        fasttext::Vector vec = fasttext::Vector(dimension);
        m.getSentenceVector(in, vec);
        float* data = vec.data();
        Array ret;
        for (int i = 0; i < dimension; i++) {
          ret.push(data[i]);
        }
        return ret;
      })
    .define_method(
      "train",
      [](FastText& m, Hash h) {
        auto a = buildArgs(h);
        if (a.hasAutotune()) {
          fasttext::Autotune autotune(std::shared_ptr<fasttext::FastText>(&m, [](fasttext::FastText*) {}));
          autotune.train(a);
        } else {
          m.train(a);
        }
      })
    .define_method(
      "quantize",
      [](FastText& m, Hash h) {
        m.quantize(buildArgs(h));
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
