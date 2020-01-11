#include <args.h>
#include <densematrix.h>
#include <fasttext.h>
#include <rice/Data_Type.hpp>
#include <rice/Constructor.hpp>
#include <rice/Array.hpp>
#include <rice/Hash.hpp>
#include <real.h>
#include <vector.h>
#include <cmath>
#include <iterator>
#include <sstream>
#include <stdexcept>

using namespace Rice;

template<>
inline
Object to_ruby<std::vector<std::pair<fasttext::real, std::string>>>(std::vector<std::pair<fasttext::real, std::string>> const & x)
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

fasttext::Args buildArgs(Hash h) {
  fasttext::Args a;

  std::vector<Hash::Entry> v;
  Hash::iterator it = h.begin();
  Hash::iterator end = h.end();

  for(; it != end; ++it)
  {
    std::string name = from_ruby<std::string>(it->key.to_s());
    Object value = it->value;

    if (name == "input") {
      a.input = from_ruby<std::string>(value);
    } else if (name == "output") {
      a.output = from_ruby<std::string>(value);
    } else if (name == "lr") {
      a.lr = from_ruby<double>(value);
    } else if (name == "lr_update_rate") {
      a.lrUpdateRate = from_ruby<int>(value);
    } else if (name == "dim") {
      a.dim = from_ruby<int>(value);
    } else if (name == "ws") {
      a.ws = from_ruby<int>(value);
    } else if (name == "epoch") {
      a.epoch = from_ruby<int>(value);
    } else if (name == "min_count") {
      a.minCount = from_ruby<int>(value);
    } else if (name == "min_count_label") {
      a.minCountLabel = from_ruby<int>(value);
    } else if (name == "neg") {
      a.neg = from_ruby<int>(value);
    } else if (name == "word_ngrams") {
      a.wordNgrams = from_ruby<int>(value);
    } else if (name == "loss") {
      std::string str = from_ruby<std::string>(value);
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
      std::string str = from_ruby<std::string>(value);
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
      a.bucket = from_ruby<int>(value);
    } else if (name == "minn") {
      a.minn = from_ruby<int>(value);
    } else if (name == "maxn") {
      a.maxn = from_ruby<int>(value);
    } else if (name == "thread") {
      a.thread = from_ruby<int>(value);
    } else if (name == "t") {
      a.t = from_ruby<double>(value);
    } else if (name == "label_prefix") {
      a.label = from_ruby<std::string>(value);
    } else if (name == "verbose") {
      a.verbose = from_ruby<int>(value);
    } else if (name == "pretrained_vectors") {
      a.pretrainedVectors = from_ruby<std::string>(value);
    } else if (name == "save_output") {
      a.saveOutput = from_ruby<bool>(value);
    // } else if (name == "seed") {
    //   a.seed = from_ruby<int>(value);
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

  define_class_under<fasttext::FastText>(rb_mExt, "Model")
    .define_constructor(Constructor<fasttext::FastText>())
    .define_method(
      "words",
      *[](fasttext::FastText& m) {
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
      *[](fasttext::FastText& m) {
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
      *[](fasttext::FastText& m, const std::string filename, int32_t k) {
        std::ifstream ifs(filename);
        if (!ifs.is_open()) {
          throw std::invalid_argument("Test file cannot be opened!");
        }
        fasttext::Meter meter;
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
      *[](fasttext::FastText& m, std::string s) { m.loadModel(s); })
    .define_method(
      "save_model",
      *[](fasttext::FastText& m, std::string s) { m.saveModel(s); })
    .define_method("dimension", &fasttext::FastText::getDimension)
    .define_method("quantized?", &fasttext::FastText::isQuant)
    .define_method("word_id", &fasttext::FastText::getWordId)
    .define_method("subword_id", &fasttext::FastText::getSubwordId)
    .define_method(
      "predict",
      *[](fasttext::FastText& m, const std::string text, int32_t k, float threshold) {
        std::stringstream ioss(text);
        std::vector<std::pair<fasttext::real, std::string>> predictions;
        m.predictLine(ioss, predictions, k, threshold);
        return predictions;
      })
    .define_method(
      "nearest_neighbors",
      *[](fasttext::FastText& m, const std::string& word, int32_t k) {
        return m.getNN(word, k);
      })
    .define_method("analogies", &fasttext::FastText::getAnalogies)
    .define_method("ngram_vectors", &fasttext::FastText::getNgramVectors)
    .define_method(
      "word_vector",
      *[](fasttext::FastText& m, const std::string word) {
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
      *[](fasttext::FastText& m, const std::string word) {
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
      *[](fasttext::FastText& m, const std::string text) {
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
      *[](fasttext::FastText& m, Hash h) {
        m.train(buildArgs(h));
      })
    .define_method(
      "quantize",
      *[](fasttext::FastText& m, Hash h) {
        m.quantize(buildArgs(h));
      })
    .define_method(
      "supervised?",
      *[](fasttext::FastText& m) {
        return m.getArgs().model == fasttext::model_name::sup;
      })
    .define_method(
      "label_prefix",
      *[](fasttext::FastText& m) {
        return m.getArgs().label;
      });
}
